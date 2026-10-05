"""O vsfs montado — a camada que fala com o kernel.

O mkfs e o dumpfs abrem a imagem e leem o que querem, na ordem que querem. Aqui
a iniciativa troca de lado: quem pergunta é o kernel, e as perguntas chegam como
chamadas de método, uma por chamada de sistema que algum processo fez.

    ls /tmp/mnt   ->   getattr("/")  e  readdir("/")

O mfusepy entrega o **caminho inteiro** em cada chamada, e não um componente por
vez. A travessia que resolve esse caminho é código nosso, e é o mesmo
percurso que o dumpfs já fazia: superbloco, tabela de inodes, inode, ponteiro,
bloco de dados.

Cria arquivos vazios, mas ainda não grava conteúdo neles.
"""

import argparse
import errno
import os
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
