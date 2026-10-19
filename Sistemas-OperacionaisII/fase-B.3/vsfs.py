"""O vsfs montado — a camada que fala com o kernel.

O mkfs e o dumpfs abrem a imagem e leem o que querem, na ordem que querem. Aqui
a iniciativa troca de lado: quem pergunta é o kernel, e as perguntas chegam como
chamadas de método, uma por chamada de sistema que algum processo fez.

    ls /tmp/mnt   ->   getattr("/")  e  readdir("/")

O mfusepy entrega o **caminho inteiro** em cada chamada, e não um componente por
vez. A travessia que resolve esse caminho é código nosso, e é o mesmo
percurso que o dumpfs já fazia: superbloco, tabela de inodes, inode, ponteiro,
bloco de dados.

Operações atendidas, e o comando de shell que as dispara:

    getattr, readdir, read     ls, cat
    create, write              touch, echo >
    unlink                     rm
    mkdir, rmdir               mkdir, rmdir
    link                       ln       (hard link; o simbólico fica de fora)
    rename                     mv
    chmod                      chmod
    statfs                     df

Ainda não encolhe arquivos (sem `truncate`) nem muda o dono (sem `chown`).
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

# Os bits de permissão de um `mode`: rwx de dono, grupo e outros, mais setuid,
# setgid e sticky. O que sobra acima deles é o tipo (arquivo, diretório...),
# que o chmod não pode mexer.
BITS_DE_PERMISSAO = 0o7777

# Um inode livre é um inode todo zerado: é o que o fsck espera encontrar na
# tabela quando o bitmap dá o inode por livre.
INODE_ZERADO = layout.Inode(
    mode=0, nlink=0, uid=0, gid=0, size=0,
    atime=0, mtime=0, ctime=0,
    direct=(0,) * layout.NUM_DIRECT,
    indirect=0,
)


def _separa(path):
    """Caminho -> (caminho do pai, nome em bytes).

    "/a/b/c" vira ("/a/b", b"c"), e "/c" vira ("/", b"c"): o rpartition deixa
    o pai da raiz como string vazia, e ela é trocada por "/".
    """
    pai, _, nome = path.rpartition("/")
    return pai or "/", nome.encode()


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

    def _procura(self, dir_ino, nome):
        """O inode que atende por `nome` dentro do diretório, ou None."""
        for entrada in self._entradas(dir_ino):
            if entrada.name == nome:
                return entrada.inum
        return None

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

            ino = self._le_inode(inum)
            # Atravessar um arquivo comum como se fosse diretório leria o
            # conteúdo dele como entradas, e acharia nomes no meio dos dados.
            if not stat.S_ISDIR(ino.mode):
                raise FuseOSError(errno.ENOTDIR)

            inum = self._procura(ino, componente.encode())
            if inum is None:
                raise FuseOSError(errno.ENOENT)

        return inum

    def _resolve_dir(self, path):
        """Como o `_resolve`, mas exige que o alvo seja um diretório.

        É o que toda operação que grava um nome faz com o pai: devolve o
        número e o inode, que quase sempre são usados juntos.
        """
        inum = self._resolve(path)
        ino = self._le_inode(inum)
        if not stat.S_ISDIR(ino.mode):
            raise FuseOSError(errno.ENOTDIR)
        return inum, ino

    def _vazio(self, dir_ino):
        """Um diretório está vazio quando só lhe restam o `.` e o `..`."""
        return all(e.name in (b".", b"..") for e in self._entradas(dir_ino))

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

    def _ajusta_inode(self, inum, nlink=0, conteudo=False):
        """Soma `nlink` ao contador de nomes e carimba os tempos.

        O `ctime` muda sempre: ele registra qualquer mudança no inode. O
        `mtime` só quando o conteúdo mudou (`conteudo=True`), que num
        diretório é ganhar ou perder uma entrada.

        O inode é relido aqui, e não recebido de quem chama: entre a leitura
        de quem chama e esta gravação pode ter havido outra, e gravar uma
        cópia velha desfaria a mudança dela.
        """
        ino = self._le_inode(inum)
        agora = int(time.time())
        self._grava_inode(inum, ino._replace(
            nlink=ino.nlink + nlink,
            ctime=agora,
            mtime=agora if conteudo else ino.mtime,
        ))

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

    def _grava_entrada(self, dir_inum, entrada):
        """Grava a entrada na primeira vaga livre do diretório.

        Sem vaga nos blocos que o diretório já tem, ele cresce um bloco. Antes
        dos hard links isso não acontecia: com 80 inodes, um nome por inode
        nunca enchia os 128 slots do primeiro bloco. Com `ln`, um inode só pode
        ter quantos nomes quiser, e o diretório precisa crescer.

        O diretório pai também ganha `mtime` e `ctime` novos: o conteúdo dele
        é a lista de nomes, e ela acabou de mudar.
        """
        dir_ino = self._le_inode(dir_inum)

        for bloco in dir_ino.direct:
            if bloco == 0:
                continue
            dados = bytearray(self.disco.read_block(bloco))
            for i, atual in enumerate(layout.entradas_do_bloco(dados)):
                if atual.inum == layout.FREE_INUM:
                    inicio = i * layout.DIRENT_SIZE
                    dados[inicio:inicio + layout.DIRENT_SIZE] = layout.pack_dirent(entrada)
                    self.disco.write_block(bloco, bytes(dados))
                    self._ajusta_inode(dir_inum, conteudo=True)
                    return

        # Todos os blocos cheios: aloca mais um, se houver ponteiro livre.
        direct = list(dir_ino.direct)
        if 0 not in direct:
            raise FuseOSError(errno.ENOSPC)  # ainda sem ponteiro indireto

        # O bloco novo é gravado antes do inode que passa a apontá-lo, a mesma
        # ordem do `write`. Zerado, ele já nasce com todos os slots livres.
        bloco = self._aloca_bloco()
        dados = bytearray(BLOCK_SIZE)
        dados[:layout.DIRENT_SIZE] = layout.pack_dirent(entrada)
        self.disco.write_block(bloco, bytes(dados))

        direct[direct.index(0)] = bloco
        agora = int(time.time())
        self._grava_inode(dir_inum, dir_ino._replace(
            size=dir_ino.size + BLOCK_SIZE,  # diretório mede os blocos que tem
            mtime=agora,
            ctime=agora,
            direct=tuple(direct),
        ))

    def _reescreve_entrada(self, dir_ino, nome, novo_inum, novo_nome=None):
        """Troca, no lugar, o inode (e talvez o nome) de uma entrada existente.

        Uma entrada inteira cabe num bloco, e o bloco é gravado de uma vez só:
        a troca é atômica. Nenhuma parada no meio deixa o nome sem apontar
        para nada, nem apontando para os dois inodes. É o que permite ao
        `rename` substituir um destino que já existe sem que ele desapareça
        por um instante, e ao `rename` dentro do mesmo diretório trocar só o
        nome.

        Devolve o inode que a entrada apontava antes.
        """
        for bloco in dir_ino.direct:
            if bloco == 0:
                continue
            dados = bytearray(self.disco.read_block(bloco))
            for i, atual in enumerate(layout.entradas_do_bloco(dados)):
                if atual.inum != layout.FREE_INUM and atual.name == nome:
                    inicio = i * layout.DIRENT_SIZE
                    dados[inicio:inicio + layout.DIRENT_SIZE] = layout.pack_dirent(
                        layout.Dirent(novo_inum, novo_nome or nome)
                    )
                    self.disco.write_block(bloco, bytes(dados))
                    return atual.inum

        raise FuseOSError(errno.ENOENT)

    def _remove_entrada(self, dir_inum, nome):
        """Grava a lápide por cima da entrada `nome` e devolve o inode que ela
        apontava.

        O slot não é compactado: fica livre no lugar, e o próximo
        `_grava_entrada` o reaproveita. O diretório também não encolhe: um
        bloco que fica todo vazio continua sendo dele até o `rmdir`.
        """
        dir_ino = self._le_inode(dir_inum)
        for bloco in dir_ino.direct:
            if bloco == 0:
                continue
            dados = bytearray(self.disco.read_block(bloco))
            for i, atual in enumerate(layout.entradas_do_bloco(dados)):
                if atual.inum != layout.FREE_INUM and atual.name == nome:
                    inicio = i * layout.DIRENT_SIZE
                    dados[inicio:inicio + layout.DIRENT_SIZE] = bytes(layout.DIRENT_SIZE)
                    self.disco.write_block(bloco, bytes(dados))
                    self._ajusta_inode(dir_inum, conteudo=True)
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

    def _descarta(self, inum):
        """Devolve aos bitmaps um inode que perdeu o último nome.

        A ordem é a do `unlink`, descrita lá: inode zerado, bitmap de dados,
        bitmap de inodes. Serve para arquivos e diretórios, porque um
        diretório também é só um inode com blocos.
        """
        ino = self._le_inode(inum)
        self._grava_inode(inum, INODE_ZERADO)
        self._libera_blocos(ino)
        self._libera_inode(inum)

    def _solta_nome(self, inum):
        """Um nome que apontava `inum` acabou de sumir: desconta no `nlink`.

        Se era o último nome, o arquivo some junto. Vale para o `unlink` e para
        o `rename` que sobrescreve um arquivo comum.
        """
        if self._le_inode(inum).nlink > 1:
            self._ajusta_inode(inum, nlink=-1)
        else:
            self._descarta(inum)

    # ------------------------------------------------------------------
    # Operações — leitura
    # ------------------------------------------------------------------

    def getattr(self, path, fh=None):
        """Os metadados de um caminho — o que o `stat` devolve.

        É a chamada mais frequente de todas: o kernel a faz antes de quase
        qualquer outra coisa, para saber se o alvo existe e o que ele é.

        Os campos saem direto do inode, menos o st_ino, que o kernel usa para
        distinguir arquivos entre si. É pelo st_ino igual que o `ls -i` mostra
        que dois hard links são o mesmo arquivo.
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
        eles são entradas de verdade, gravadas pelo mkfs e pelo mkdir. Muitos
        sistemas de arquivos os sintetizam aqui em vez de guardá-los.
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

    def statfs(self, path):
        """O espaço livre do sistema de arquivos — o que o `df` mostra.

        Tudo sai dos dois bitmaps, contados na hora: o vsfs não guarda um
        contador de livres no superbloco (ver `layout.conta_bits`), e por isso
        nunca pode haver um contador discordando do bitmap.

        Só a região de dados entra no total de blocos. Superbloco, bitmaps e
        tabela de inodes são o custo fixo do formato, e nenhum arquivo pode
        usá-los; o ext4 faz a mesma conta ao descontar o "overhead".

        `bfree` e `bavail` são iguais porque o vsfs não reserva blocos para o
        root, como o ext4 faz com 5% do disco. Do lado dos inodes, a lápide
        e a raiz contam como usados: estão marcados no bitmap e nenhum
        arquivo novo pode recebê-los.
        """
        dados = self.disco.read_block(self.sb.data_bitmap_start)
        inodes = self.disco.read_block(self.sb.inode_bitmap_start)

        blocos_livres = self.sb.num_data_blocks - layout.conta_bits(
            dados, self.sb.num_data_blocks
        )
        inodes_livres = self.sb.num_inodes - layout.conta_bits(
            inodes, self.sb.num_inodes
        )

        return {
            "f_bsize": BLOCK_SIZE,   # tamanho preferido de I/O
            "f_frsize": BLOCK_SIZE,  # unidade em que f_blocks é contado
            "f_blocks": self.sb.num_data_blocks,
            "f_bfree": blocos_livres,
            "f_bavail": blocos_livres,
            "f_files": self.sb.num_inodes,
            "f_ffree": inodes_livres,
            "f_favail": inodes_livres,
            "f_namemax": layout.NAME_MAX,
        }

    # ------------------------------------------------------------------
    # Operações — arquivos
    # ------------------------------------------------------------------

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
        pai, nome = _separa(path)
        # Conferido antes de alocar, para não deixar um inode marcado sem nome.
        if len(nome) > layout.NAME_MAX:
            raise FuseOSError(errno.ENAMETOOLONG)

        dir_inum, _ = self._resolve_dir(pai)
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

        try:
            self._grava_entrada(dir_inum, layout.Dirent(inum, nome))
        except FuseOSError:
            # O pai estava cheio e não pôde crescer: o inode recém-criado
            # volta para o bitmap em vez de ficar órfão.
            self._descarta(inum)
            raise
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
        ainda sobrar nome (um hard link criado pelo `link`), o inode só
        registra a mudança no `ctime`; se não sobrar, o inode e os blocos dele
        voltam para os bitmaps.

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
        pai, nome = _separa(path)
        dir_inum, _ = self._resolve_dir(pai)

        # O kernel manda diretório para o rmdir, mas a imagem não pode confiar
        # nisso: apagar o nome de um diretório deixaria tudo dentro dele órfão.
        if stat.S_ISDIR(self._le_inode(self._resolve(path)).mode):
            raise FuseOSError(errno.EISDIR)

        inum = self._remove_entrada(dir_inum, nome)
        self._solta_nome(inum)
        return 0

    def link(self, target, source):
        """Um hard link: `target` passa a ser mais um nome do inode de `source`.

        É o `ln source target`. Nenhum dado é copiado e nenhum inode é criado:
        só uma entrada de diretório nova, apontando para um inode que já
        existe, e o `nlink` dele sobe um. Os dois nomes ficam indistinguíveis
        — não há "original" e "cópia" — e o arquivo só some quando o `unlink`
        apagar o último deles.

        O `nlink` sobe **antes** de a entrada ser gravada. Se a gravação parar
        no meio, sobra um contador maior que o número de nomes, e o arquivo
        nunca é liberado: espaço perdido, que o fsck acusa. A ordem contrária
        deixaria dois nomes com `nlink` 1, e o primeiro `rm` liberaria um
        inode que o segundo nome ainda aponta.

        Diretórios são recusados, como no Linux: um hard link para diretório
        poderia fazer um ciclo na árvore, e o `..` deixaria de ter um pai só.
        Links simbólicos ficam de fora desta fase.
        """
        pai, nome = _separa(target)
        if len(nome) > layout.NAME_MAX:
            raise FuseOSError(errno.ENAMETOOLONG)

        inum = self._resolve(source)
        ino = self._le_inode(inum)
        if stat.S_ISDIR(ino.mode):
            raise FuseOSError(errno.EPERM)

        dir_inum, dir_ino = self._resolve_dir(pai)
        if self._procura(dir_ino, nome) is not None:
            raise FuseOSError(errno.EEXIST)

        # O contador tem 2 bytes no inode; passar disso daria a volta para 0.
        if ino.nlink >= 0xFFFF:
            raise FuseOSError(errno.EMLINK)

        self._ajusta_inode(inum, nlink=+1)
        try:
            self._grava_entrada(dir_inum, layout.Dirent(inum, nome))
        except FuseOSError:
            self._ajusta_inode(inum, nlink=-1)  # o nome não chegou a existir
            raise
        return 0

    def chmod(self, path, mode):
        """Troca as permissões de um arquivo ou diretório.

        Só os 12 bits de permissão mudam (rwx de dono, grupo e outros, mais
        setuid, setgid e sticky). Os bits de tipo continuam os do inode: um
        `chmod` não pode transformar um arquivo em diretório, e um `mode`
        vindo do kernel sem os bits de tipo apagaria o tipo se fosse gravado
        inteiro.

        Mudar permissão não muda conteúdo: só o `ctime` anda, e o `mtime`
        fica. É por isso que o `ls -l` (que mostra o `mtime`) não percebe um
        chmod, e o `ls -lc` percebe.

        Quem confere se o processo tem direito de fazer o chmod (ser o dono ou
        ser root) é o kernel, graças à opção `default_permissions` da montagem.
        """
        inum = self._resolve(path)
        ino = self._le_inode(inum)
        self._grava_inode(inum, ino._replace(
            mode=stat.S_IFMT(ino.mode) | (mode & BITS_DE_PERMISSAO),
            ctime=int(time.time()),
        ))
        return 0

    # ------------------------------------------------------------------
    # Operações — diretórios
    # ------------------------------------------------------------------

    def mkdir(self, path, mode):
        """Um diretório novo: um inode, um bloco com `.` e `..`, e um nome no pai.

        É o `create` com duas diferenças. O diretório já nasce com um bloco,
        porque o `.` e o `..` são entradas gravadas de verdade (o mesmo que o
        mkfs faz com a raiz). E o diretório pai ganha um `nlink`, porque o `..`
        do filho é mais um nome que aponta para ele.

            novo  : nlink 2 = o nome no pai + o próprio `.`
            pai   : nlink + 1 = o `..` do novo

        O kernel nem sempre manda o bit de tipo no `mode` do mkdir, então ele é
        posto aqui; das permissões, só os 12 bits de baixo são aproveitados.

        A ordem é a do `create`: tudo o que o diretório precisa é gravado
        antes do nome que o torna visível. Uma parada no meio deixa só um
        inode e um bloco marcados sem dono, que o fsck acusa.

            1. bitmap de inodes e bitmap de dados (alocação)
            2. o bloco com `.` e `..`
            3. o inode do diretório novo
            4. o `nlink` do pai
            5. a entrada no pai
        """
        pai, nome = _separa(path)
        if len(nome) > layout.NAME_MAX:
            raise FuseOSError(errno.ENAMETOOLONG)

        dir_inum, dir_ino = self._resolve_dir(pai)
        if self._procura(dir_ino, nome) is not None:
            raise FuseOSError(errno.EEXIST)

        inum = self._aloca_inode()
        try:
            bloco = self._aloca_bloco()
        except FuseOSError:
            # Sem bloco para o `.` e o `..`, o inode recém-marcado volta para
            # o bitmap em vez de ficar perdido.
            self._libera_inode(inum)
            raise

        dados = bytearray(BLOCK_SIZE)
        entradas = [layout.Dirent(inum, b"."), layout.Dirent(dir_inum, b"..")]
        for i, entrada in enumerate(entradas):
            inicio = i * layout.DIRENT_SIZE
            dados[inicio:inicio + layout.DIRENT_SIZE] = layout.pack_dirent(entrada)
        self.disco.write_block(bloco, bytes(dados))

        agora = int(time.time())
        self._grava_inode(inum, layout.Inode(
            mode=stat.S_IFDIR | (mode & BITS_DE_PERMISSAO),
            nlink=2,
            uid=os.getuid(),
            gid=os.getgid(),
            size=BLOCK_SIZE,  # diretório mede os blocos que tem, não as entradas
            atime=agora,
            mtime=agora,
            ctime=agora,
            direct=(bloco,) + (0,) * (layout.NUM_DIRECT - 1),
            indirect=0,
        ))

        # Mesmo raciocínio do `link`: o contador sobe antes do nome aparecer.
        self._ajusta_inode(dir_inum, nlink=+1)
        try:
            self._grava_entrada(dir_inum, layout.Dirent(inum, nome))
        except FuseOSError:
            # O pai não pôde crescer: desfaz tudo, na ordem inversa.
            self._ajusta_inode(dir_inum, nlink=-1)
            self._descarta(inum)
            raise
        return 0

    def rmdir(self, path):
        """Remove um diretório vazio.

        Vazio quer dizer só com `.` e `..`. Com qualquer outra entrada a
        remoção é recusada com ENOTEMPTY: apagar o diretório deixaria tudo o
        que está dentro dele sem caminho até a raiz.

        É o `mkdir` ao contrário, e a ordem também se inverte: primeiro some o
        nome, depois o `..` deixa de contar no pai, e por fim o inode e o
        bloco voltam para os bitmaps (pela mesma `_descarta` do `unlink`).
        O `nlink` do diretório não é consultado: um diretório tem um nome só,
        já que hard link para diretório é proibido.

        A raiz não pode ser removida (EBUSY): é ela que segura a montagem.
        """
        inum = self._resolve(path)
        if inum == self.sb.root_inum:
            raise FuseOSError(errno.EBUSY)

        ino = self._le_inode(inum)
        if not stat.S_ISDIR(ino.mode):
            raise FuseOSError(errno.ENOTDIR)
        if not self._vazio(ino):
            raise FuseOSError(errno.ENOTEMPTY)

        pai, nome = _separa(path)
        dir_inum, _ = self._resolve_dir(pai)

        self._remove_entrada(dir_inum, nome)
        self._ajusta_inode(dir_inum, nlink=-1)  # o `..` do removido sumiu
        self._descarta(inum)
        return 0

    def rename(self, old, new):
        """Muda o nome e/ou o diretório de um arquivo — o `mv`.

        O inode não muda: nem número, nem dados, nem `nlink`. Só a entrada de
        diretório que o aponta sai de um lugar e vai para outro. Por isso o
        `mv` é instantâneo mesmo para um arquivo enorme, desde que fique no
        mesmo sistema de arquivos.

        Três casos, do mais simples ao mais delicado:

        1. Mesmo diretório e destino livre: a entrada é reescrita no lugar,
           só com o nome trocado. Uma gravação de bloco, e portanto atômica.

        2. Destino já existe: ele é substituído. A entrada do destino é
           reescrita no lugar para apontar o inode de origem, e só então o
           nome antigo some. O POSIX exige isso: em nenhum instante o caminho
           `new` deixa de existir. O inode que o destino apontava perde um
           nome e, se era o último, é liberado.

        3. Diretório que muda de pai: além das entradas, o `..` dele passa a
           apontar o pai novo, e os `nlink` dos dois pais são acertados
           (o antigo perde o `..` do filho, o novo ganha).

        Recusas, as mesmas do Linux:
            - mover um diretório para dentro dele mesmo (EINVAL), o que
              desligaria a subárvore inteira da raiz;
            - trocar arquivo por diretório (EISDIR) ou o contrário (ENOTDIR);
            - sobrescrever um diretório que não está vazio (ENOTEMPTY).
        """
        pai_velho, nome_velho = _separa(old)
        pai_novo, nome_novo = _separa(new)
        if len(nome_novo) > layout.NAME_MAX:
            raise FuseOSError(errno.ENAMETOOLONG)

        inum = self._resolve(old)
        eh_dir = stat.S_ISDIR(self._le_inode(inum).mode)

        # O caminho já vem normalizado, então "estar dentro de" é ter o
        # caminho antigo, seguido de uma barra, como prefixo.
        if eh_dir and new.startswith(old.rstrip("/") + "/"):
            raise FuseOSError(errno.EINVAL)

        velho_inum, _ = self._resolve_dir(pai_velho)
        novo_inum, novo_ino = self._resolve_dir(pai_novo)

        destino = self._procura(novo_ino, nome_novo)
        if destino == inum:
            return 0  # dois nomes do mesmo arquivo: o POSIX manda não fazer nada

        if destino is not None:
            # Caso 2: substitui o destino, depois de conferir se pode.
            dest_ino = self._le_inode(destino)
            dest_eh_dir = stat.S_ISDIR(dest_ino.mode)
            if dest_eh_dir and not eh_dir:
                raise FuseOSError(errno.EISDIR)
            if eh_dir and not dest_eh_dir:
                raise FuseOSError(errno.ENOTDIR)
            if dest_eh_dir and not self._vazio(dest_ino):
                raise FuseOSError(errno.ENOTEMPTY)

            self._reescreve_entrada(novo_ino, nome_novo, inum)
            self._ajusta_inode(novo_inum, conteudo=True)
            self._remove_entrada(velho_inum, nome_velho)

            if dest_eh_dir:
                # O diretório substituído some de vez, e o `..` dele deixa de
                # contar no pai novo.
                self._ajusta_inode(novo_inum, nlink=-1)
                self._descarta(destino)
            else:
                self._solta_nome(destino)

        elif velho_inum == novo_inum:
            # Caso 1: só troca o nome, no lugar, e encerra. Nenhum `..` muda.
            self._reescreve_entrada(novo_ino, nome_velho, inum, nome_novo)
            self._ajusta_inode(novo_inum, conteudo=True)
            self._ajusta_inode(inum)  # o `ctime` registra a mudança de nome
            return 0

        else:
            # Destino livre em outro diretório: nome novo primeiro, depois o
            # antigo some. Uma parada no meio deixa dois nomes para o mesmo
            # inode, com `nlink` 1 — o mesmo ponto fraco do ext2, que só o
            # journaling resolve.
            self._grava_entrada(novo_inum, layout.Dirent(inum, nome_novo))
            self._remove_entrada(velho_inum, nome_velho)

        if eh_dir and velho_inum != novo_inum:
            # Caso 3: o `..` acompanha a mudança de pai.
            self._reescreve_entrada(self._le_inode(inum), b"..", novo_inum)
            self._ajusta_inode(velho_inum, nlink=-1)
            self._ajusta_inode(novo_inum, nlink=+1)

        self._ajusta_inode(inum)
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
    #
    # default_permissions pede ao kernel que confira as permissões gravadas
    # nos inodes (as que o chmod muda) antes de cada acesso. Sem ela, o FUSE
    # deixa a conferência por conta do sistema de arquivos, e os bits rwx
    # seriam só decoração: um `chmod 000` não impediria ninguém de ler.
    FUSE(
        fs,
        args.ponto,
        foreground=args.foreground,
        nothreads=True,
        default_permissions=True,
    )
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
