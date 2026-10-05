"""O vsfs montado — a camada que fala com o kernel.

O mkfs e o dumpfs abrem a imagem e leem o que querem, na ordem que querem. Aqui
a iniciativa troca de lado: quem pergunta é o kernel, e as perguntas chegam como
chamadas de método, uma por chamada de sistema que algum processo fez.

    ls /tmp/mnt   ->   getattr("/")  e  readdir("/")

O mfusepy entrega o **caminho inteiro** em cada chamada, e não um componente por
vez. A travessia que resolve esse caminho é código nosso, e é o mesmo
percurso que o dumpfs já fazia: superbloco, tabela de inodes, inode, ponteiro,
bloco de dados.

Cria arquivos, grava conteúdo neles e os apaga, mas ainda não os encolhe.
"""

import argparse
import errno
import os
import stat
import sys
import time

from mfusepy import FUSE, FuseOSError, Operations

import layout
from disk import Disk
from layout import BLOCK_SIZE

NS_POR_SEGUNDO = 10**9


class VSFS(Operations):
    """Traduz caminhos em inodes, e inodes no que o kernel espera receber."""

    # O mfusepy troca tempos com o kernel em nanossegundos, e o inode guarda
    # segundos em 4 bytes. A conversão mora na fronteira entre os dois, e
    # é a mesma ideia de bloco_para_bit: quando duas camadas medem a mesma
    # coisa em unidades diferentes, a tradução tem um lugar só.
    use_ns = True

    def __init__(self, caminho):
        self.disco = Disk(caminho, BLOCK_SIZE)
        self.sb = layout.unpack_superblock(self.disco.read_block(0))

        if self.sb.magic != layout.MAGIC:
            raise ValueError(f"{caminho}: não é uma imagem vsfs; rode o mkfs")

    # ------------------------------------------------------------------
    # Travessia
    # ------------------------------------------------------------------

    def _le_inode(self, inum):
        bloco, offset = layout.localiza_inode(inum)
        return layout.unpack_inode(self.disco.read_block(bloco), offset)

    def _entradas(self, ino):
        """As entradas vivas de um diretório, seguindo os ponteiros do inode."""
        for bloco in ino.direct:
            if bloco == 0:  # 0 é sentinela de "sem bloco"
                continue
            for entrada in layout.entradas_do_bloco(self.disco.read_block(bloco)):
                if entrada.inum != layout.FREE_INUM:
                    yield entrada

    def _resolve(self, path):
        """Caminho -> número do inode.

        O laço é a travessia inteira: parte da raiz e, para cada componente,
        lê o diretório atual e procura o nome ali dentro. É o que o VFS do
        kernel faria por nós se a API fosse a de baixo nível.

        O `path` vem sempre absoluto e já normalizado pelo kernel — sem `.`,
        sem `//`, sem caminho relativo.
        """
        inum = self.sb.root_inum

        for componente in path.strip("/").split("/"):
            if not componente:  # a raiz, cujo split devolve uma string vazia
                continue

            alvo = componente.encode()
            for entrada in self._entradas(self._le_inode(inum)):
                if entrada.name == alvo:
                    inum = entrada.inum
                    break
            else:
                raise FuseOSError(errno.ENOENT)

        return inum

    # ------------------------------------------------------------------
    # Escrita
    # ------------------------------------------------------------------

    def _grava_inode(self, inum, ino):
        """Grava um inode na tabela.

        O bloco guarda dezesseis inodes e o disco só grava blocos inteiros:
        ler, trocar a fatia de 256 bytes e gravar de volta.
        """
        bloco, offset = layout.localiza_inode(inum)
        tabela = bytearray(self.disco.read_block(bloco))
        tabela[offset:offset + layout.INODE_SIZE] = layout.pack_inode(ino)
        self.disco.write_block(bloco, bytes(tabela))

    def _aloca_inode(self):
        """Liga o primeiro bit livre do bitmap de inodes e devolve o número.

        A lápide e a raiz já nascem marcadas, então a busca começa do zero sem
        risco de entregar uma delas.
        """
        bitmap = bytearray(self.disco.read_block(self.sb.inode_bitmap_start))
        inum = layout.primeiro_livre(bitmap, self.sb.num_inodes)
        if inum is None:
            raise FuseOSError(errno.ENOSPC)

        layout.marca_bit(bitmap, inum)
        self.disco.write_block(self.sb.inode_bitmap_start, bytes(bitmap))
        return inum

    def _aloca_bloco(self):
        """Liga o primeiro bit livre do bitmap de dados e devolve o bloco.

        O bit é relativo e o bloco devolvido é absoluto: é o número que
        vai para o `direct[]`.
        """
        bitmap = bytearray(self.disco.read_block(self.sb.data_bitmap_start))
        i = layout.primeiro_livre(bitmap, self.sb.num_data_blocks)
        if i is None:
            raise FuseOSError(errno.ENOSPC)

        layout.marca_bit(bitmap, i)
        self.disco.write_block(self.sb.data_bitmap_start, bytes(bitmap))
        return layout.bit_para_bloco(i)

    def _grava_entrada(self, dir_ino, entrada):
        """Grava a entrada na primeira vaga livre do diretório."""
        for bloco in dir_ino.direct:
            if bloco == 0:
                continue
            dados = bytearray(self.disco.read_block(bloco))
            for i, atual in enumerate(layout.entradas_do_bloco(dados)):
                if atual.inum == layout.FREE_INUM:
                    inicio = i * layout.DIRENT_SIZE
                    dados[inicio:inicio + layout.DIRENT_SIZE] = layout.pack_dirent(entrada)
                    self.disco.write_block(bloco, bytes(dados))
                    return

        # Com 80 inodes e 126 vagas no primeiro bloco, não acontece; fica para
        # o caso de a geometria mudar.
        raise FuseOSError(errno.ENOSPC)

    def _remove_entrada(self, dir_ino, nome):
        """Grava a lápide por cima da entrada `nome` e devolve o inode que ela
        apontava.

        O slot não é compactado: fica livre no lugar, e o próximo
        `_grava_entrada` o reaproveita.
        """
        for bloco in dir_ino.direct:
            if bloco == 0:
                continue
            dados = bytearray(self.disco.read_block(bloco))
            for i, atual in enumerate(layout.entradas_do_bloco(dados)):
                if atual.inum != layout.FREE_INUM and atual.name == nome:
                    inicio = i * layout.DIRENT_SIZE
                    dados[inicio:inicio + layout.DIRENT_SIZE] = bytes(layout.DIRENT_SIZE)
                    self.disco.write_block(bloco, bytes(dados))
                    return atual.inum

        raise FuseOSError(errno.ENOENT)

    def _libera_blocos(self, ino):
        """Desliga no bitmap de dados os bits dos blocos que o inode aponta.

        O conteúdo dos blocos fica onde está: liberar é só desmarcar.
        """
        bitmap = bytearray(self.disco.read_block(self.sb.data_bitmap_start))
        for bloco in ino.direct + (ino.indirect,):
            if bloco != 0:
                layout.limpa_bit(bitmap, layout.bloco_para_bit(bloco))
        self.disco.write_block(self.sb.data_bitmap_start, bytes(bitmap))

    def _libera_inode(self, inum):
        """Desliga o bit do inode no bitmap de inodes."""
        bitmap = bytearray(self.disco.read_block(self.sb.inode_bitmap_start))
        layout.limpa_bit(bitmap, inum)
        self.disco.write_block(self.sb.inode_bitmap_start, bytes(bitmap))

    # ------------------------------------------------------------------
    # Operações
    # ------------------------------------------------------------------

    def getattr(self, path, fh=None):
        """Os metadados de um caminho — o que o `stat` devolve.

        É a chamada mais frequente de todas: o kernel a faz antes de quase
        qualquer outra coisa, para saber se o alvo existe e o que ele é.

        Os campos saem direto do inode, menos o st_ino, que o kernel usa para
        distinguir arquivos entre si.
        """
        inum = self._resolve(path)
        ino = self._le_inode(inum)

        return {
            "st_ino": inum,
            "st_mode": ino.mode,
            "st_nlink": ino.nlink,
            "st_uid": ino.uid,
            "st_gid": ino.gid,
            "st_size": ino.size,
            "st_atime": ino.atime * NS_POR_SEGUNDO,
            "st_mtime": ino.mtime * NS_POR_SEGUNDO,
            "st_ctime": ino.ctime * NS_POR_SEGUNDO,
        }

    def readdir(self, path, fh):
        """Os nomes dentro de um diretório.

        O `.` e o `..` saem do disco como qualquer outro nome, porque no vsfs
        eles são entradas de verdade, gravadas pelo mkfs. Muitos sistemas de
        arquivos os sintetizam aqui em vez de guardá-los.
        """
        return [entrada.name.decode() for entrada in self._entradas(
            self._le_inode(self._resolve(path))
        )]

    def read(self, path, size, offset, fh):
        """Os bytes de um arquivo, de `offset` até `offset + size`.

        O kernel pede fatias arbitrárias e o disco só entrega blocos inteiros,
        então o laço traduz uma coisa na outra: divide o deslocamento pelo
        tamanho do bloco para achar qual ponteiro usar, e o resto da divisão
        para achar onde começar dentro dele.

        Não existe método `open` aqui: sem ele, a libfuse responde que abriu.
        Sem nada para guardar entre uma leitura e outra, o descritor não
        precisa carregar estado nenhum.
        """
        ino = self._le_inode(self._resolve(path))
        fim = min(offset + size, ino.size)  # ler além do fim devolve o que há
        dados = bytearray()

        while offset < fim:
            indice = offset // BLOCK_SIZE
            if indice >= layout.NUM_DIRECT:
                break  # ainda sem ponteiro indireto

            dentro = offset % BLOCK_SIZE
            quanto = min(BLOCK_SIZE - dentro, fim - offset)
            bloco = ino.direct[indice]

            # Ponteiro zerado dentro do tamanho declarado não deveria existir,
            # já que buracos não são suportados. Devolver zeros mantém a
            # leitura previsível em vez de deixar o processo pendurado.
            if bloco == 0:
                dados += b"\x00" * quanto
            else:
                dados += self.disco.read_block(bloco)[dentro:dentro + quanto]

            offset += quanto

        return bytes(dados)

    def create(self, path, mode):
        """Um arquivo novo e vazio: um inode e um nome no diretório pai.

        Nenhum bloco de dados é alocado. O arquivo nasce com `size` 0 e sem
        ponteiros, e quem ganha conteúdo é o diretório pai, que passa a ter
        uma entrada a mais.

        O `mode` chega pronto do kernel: tipo e permissões, com o umask do
        processo já aplicado.

        A entrada é gravada por último. Se a gravação parar no meio, sobra um
        inode marcado que nenhum nome aponta, e o fsck acusa; o contrário
        deixaria um nome apontando para um inode que não existe.
        """
        pai, _, nome = path.rpartition("/")
        nome = nome.encode()
        # Conferido antes de alocar, para não deixar um inode marcado sem nome.
        if len(nome) > layout.NAME_MAX:
            raise FuseOSError(errno.ENAMETOOLONG)

        dir_ino = self._le_inode(self._resolve(pai or "/"))
        inum = self._aloca_inode()

        agora = int(time.time())
        self._grava_inode(inum, layout.Inode(
            mode=mode,
            nlink=1,
            uid=os.getuid(),  # quem montou é dono de tudo, como no mkfs
            gid=os.getgid(),
            size=0,
            atime=agora,
            mtime=agora,
            ctime=agora,
            direct=(0,) * layout.NUM_DIRECT,
            indirect=0,
        ))

        self._grava_entrada(dir_ino, layout.Dirent(inum, nome))
        return 0

    def write(self, path, data, offset, fh):
        """Grava `data` a partir de `offset` e devolve quantos bytes gravou.

        O laço é o do `read`, com a tradução de deslocamento em ponteiro e
        posição dentro do bloco. A diferença é que aqui o bloco pode ainda não
        existir, e então é alocado.

        Bloco recém-alocado começa zerado na memória, sem ser lido: o `rm` não
        apaga dados, e o que estivesse lá seria de um arquivo antigo.

        Os blocos de dados são gravados antes do inode. Se a gravação parar no
        meio, sobram blocos marcados que nenhum ponteiro aponta, e o fsck
        acusa.
        """
        inum = self._resolve(path)
        ino = self._le_inode(inum)
        direct = list(ino.direct)
        fim = offset + len(data)
        # Recusado antes do laço, para não alocar blocos que o inode não
        # chegaria a apontar.
        if fim > layout.NUM_DIRECT * BLOCK_SIZE:
            raise FuseOSError(errno.EFBIG)  # ainda sem ponteiro indireto

        pos = offset
        while pos < fim:
            indice = pos // BLOCK_SIZE
            dentro = pos % BLOCK_SIZE
            quanto = min(BLOCK_SIZE - dentro, fim - pos)

            if direct[indice] == 0:
                direct[indice] = self._aloca_bloco()
                bloco = bytearray(BLOCK_SIZE)
            elif quanto < BLOCK_SIZE:
                # Escrita parcial: o resto do bloco precisa sobreviver.
                bloco = bytearray(self.disco.read_block(direct[indice]))
            else:
                bloco = bytearray(BLOCK_SIZE)

            inicio = pos - offset
            bloco[dentro:dentro + quanto] = data[inicio:inicio + quanto]
            self.disco.write_block(direct[indice], bytes(bloco))
            pos += quanto

        agora = int(time.time())
        self._grava_inode(inum, ino._replace(
            size=max(ino.size, fim),
            mtime=agora,
            ctime=agora,
            direct=tuple(direct),
        ))
        return len(data)

    def unlink(self, path):
        """Remove um nome. O arquivo só some quando o último nome sumir.

        O nome é apagado do diretório pai e o `nlink` do inode desce um. Se
        ainda sobrar nome, o inode só registra a mudança no `ctime`; se não
        sobrar, o inode e os blocos dele voltam para os bitmaps.

        A ordem das gravações é a inversa da do `create` e do `write`, pelo
        mesmo motivo: a cada passo, uma parada no meio deixa só espaço
        perdido, nunca um ponteiro para algo já livre.

            1. a entrada no pai     -> sem nome, o inode fica órfão
            2. o inode zerado       -> os blocos ficam marcados sem dono
            3. o bitmap de dados    -> o inode fica marcado e zerado
            4. o bitmap de inodes

        Zerar o inode antes de desmarcar os blocos é o que importa: a ordem
        contrária deixaria um inode apontando blocos que o alocador já pode
        entregar a outro arquivo. E o inode zerado, e não só desmarcado, é o
        que o fsck espera de um inode livre.

        Sem `open` e `release`, não há como saber se algum processo ainda tem
        o arquivo aberto: os blocos são liberados na hora, e quem continuar
        usando o descritor recebe ENOENT.
        """
        pai, _, nome = path.rpartition("/")
        dir_ino = self._le_inode(self._resolve(pai or "/"))

        # O kernel manda diretório para o rmdir, mas a imagem não pode confiar
        # nisso: apagar o nome de um diretório deixaria tudo dentro dele órfão.
        if stat.S_ISDIR(self._le_inode(self._resolve(path)).mode):
            raise FuseOSError(errno.EISDIR)

        inum = self._remove_entrada(dir_ino, nome.encode())
        ino = self._le_inode(inum)

        if ino.nlink > 1:
            self._grava_inode(inum, ino._replace(
                nlink=ino.nlink - 1,
                ctime=int(time.time()),
            ))
            return 0

        self._grava_inode(inum, layout.Inode(
            mode=0, nlink=0, uid=0, gid=0, size=0,
            atime=0, mtime=0, ctime=0,
            direct=(0,) * layout.NUM_DIRECT,
            indirect=0,
        ))
        self._libera_blocos(ino)
        self._libera_inode(inum)
        return 0

    def destroy(self, path):
        self.disco.close()


def main(argv):
    parser = argparse.ArgumentParser(description="Monta uma imagem vsfs.")
    parser.add_argument("imagem", help="imagem formatada pelo mkfs")
    parser.add_argument("ponto", help="diretório onde montar")
    parser.add_argument(
        "-f",
        "--foreground",
        action="store_true",
        help="não vai para segundo plano; os erros aparecem no terminal",
    )
    args = parser.parse_args(argv[1:])

    try:
        fs = VSFS(args.imagem)
    except (ValueError, OSError) as e:
        print(f"{parser.prog}: {e}", file=sys.stderr)
        return 1

    # nothreads desliga o paralelismo do FUSE. Uma thread só torna a ordem das
    # operações previsível, e o preço — uma chamada de cada vez — não se nota
    # numa imagem de 256 KiB.
    FUSE(fs, args.ponto, foreground=args.foreground, nothreads=True)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
