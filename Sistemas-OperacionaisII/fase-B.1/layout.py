"""Constantes e formatos on-disk do vsfs.

Este módulo é a tradução em código de docs/formato-do-disco.md. Quando os dois
divergirem, o documento é que está certo — corrija o código.

Só formato, sem I/O: aqui não se lê nem se grava nada. Empacotar e desempacotar
bytes é tudo o que este módulo faz.
"""

import math
import struct
from typing import NamedTuple

# --------------------------------------------------------------------------
# Geometria
# --------------------------------------------------------------------------

# Tamanho do bloco e da imagem. Escolhas do capítulo 40 do OSTEP.
BLOCK_SIZE = 4096
NUM_BLOCKS = 64
IMAGE_SIZE = NUM_BLOCKS * BLOCK_SIZE

MAGIC = b"VSFS"

NUM_INODES = 80
INODE_SIZE = 256
INODES_PER_BLOCK = BLOCK_SIZE // INODE_SIZE

# Inode 0 é reservado: ele é a lápide das entradas de diretório removidas, e
# por isso a raiz não pode ocupá-lo.
ROOT_INUM = 1

SUPERBLOCK_BLOCK = 0
INODE_BITMAP_START = 1
DATA_BITMAP_START = 2
INODE_TABLE_START = 3

# Derivados. O ceil não é decoração: com 100 inodes daria 6,25 blocos, e o
# último ficaria parcialmente usado.
INODE_TABLE_BLOCKS = math.ceil(NUM_INODES * INODE_SIZE / BLOCK_SIZE)
DATA_REGION_START = INODE_TABLE_START + INODE_TABLE_BLOCKS
NUM_DATA_BLOCKS = NUM_BLOCKS - DATA_REGION_START


def _verifica_geometria():
    """Invariantes da geometria, conferidos no import.

    Derivar DATA_REGION_START e NUM_DATA_BLOCKS já garante que as regiões
    ladrilham o disco sem sobra nem sobreposição. O que estas verificações
    pegam são os erros que a derivação não impede — e é a semente do fsck.
    """
    assert INODE_TABLE_BLOCKS * BLOCK_SIZE >= NUM_INODES * INODE_SIZE, (
        "a tabela de inodes não cabe nos blocos reservados para ela"
    )
    assert DATA_REGION_START + NUM_DATA_BLOCKS == NUM_BLOCKS, (
        "as regiões não ladrilham o disco"
    )
    assert NUM_DATA_BLOCKS > 0, "não sobrou espaço para dados"
    assert ROOT_INUM != 0, "o inode 0 é a lápide; a raiz não pode ocupá-lo"

    # Cada bitmap ocupa um bloco só. Aumentar NUM_BLOCKS sem rever isso é o
    # erro que esta verificação existe para pegar.
    assert NUM_INODES <= BLOCK_SIZE * 8, "o bitmap de inodes não cabe num bloco"
    assert NUM_DATA_BLOCKS <= BLOCK_SIZE * 8, "o bitmap de dados não cabe num bloco"


_verifica_geometria()


# --------------------------------------------------------------------------
# Superbloco — bloco 0
# --------------------------------------------------------------------------

# O '<' importa por dois motivos: fixa a ordem dos bytes (um formato on-disk é
# um contrato portátil, não "o que esta CPU resolver fazer") e desliga o
# alinhamento, que faria o tamanho da struct depender da ordem dos campos.
SUPERBLOCK_FORMAT = "<4s9I"
SUPERBLOCK_SIZE = struct.calcsize(SUPERBLOCK_FORMAT)  # 40 bytes


class Superblock(NamedTuple):
    """O que o sistema precisa saber ao montar, antes de ler qualquer coisa.

    Os quatro campos `*_start` são deriváveis da geometria, e são gravados
    assim mesmo: é o que torna o sistema de arquivos auto-descritivo. O código
    de montagem pergunta ao disco em vez de trazer a geometria embutida.
    """

    magic: bytes
    block_size: int
    num_inodes: int
    num_data_blocks: int
    inode_bitmap_start: int
    data_bitmap_start: int
    inode_table_start: int
    data_region_start: int
    inode_size: int
    root_inum: int


def superbloco_padrao():
    """O superbloco correspondente à geometria deste módulo."""
    return Superblock(
        magic=MAGIC,
        block_size=BLOCK_SIZE,
        num_inodes=NUM_INODES,
        num_data_blocks=NUM_DATA_BLOCKS,
        inode_bitmap_start=INODE_BITMAP_START,
        data_bitmap_start=DATA_BITMAP_START,
        inode_table_start=INODE_TABLE_START,
        data_region_start=DATA_REGION_START,
        inode_size=INODE_SIZE,
        root_inum=ROOT_INUM,
    )


def pack_superblock(sb):
    """Superblock -> 40 bytes."""
    return struct.pack(SUPERBLOCK_FORMAT, *sb)


def unpack_superblock(bloco):
    """Bytes -> Superblock. Aceita o bloco inteiro; lê só os 40 primeiros."""
    return Superblock(*struct.unpack_from(SUPERBLOCK_FORMAT, bloco))


# --------------------------------------------------------------------------
# Bitmaps — blocos 1 e 2
# --------------------------------------------------------------------------

# Toda a manipulação de bits mora aqui, e só aqui. Espalhá-la entre o mkfs e o
# vsfs seria garantir que as duas cópias divergissem com o tempo — e um bitmap
# divergente não trava: ele entrega um bloco que já é de outro arquivo, e a
# corrupção aparece longe da causa.
#
# As funções alteram o bytearray no lugar. A alternativa pura devolveria uma
# cópia de 4 KiB a cada bit mexido, para não ganhar nada: quem chama já é dono
# exclusivo do buffer, entre ler o bloco e gravá-lo de volta.


def bitmap_vazio():
    """Um bloco de bits zerados, pronto para ser marcado."""
    return bytearray(BLOCK_SIZE)


def marca_bit(bitmap, i):
    """Liga o bit i.

    O bit i é o `i % 8`-ésimo bit, contado a partir do **menos** significativo,
    do byte `i // 8`. É por isso que marcar os inodes 0 e 1 dá `0x03` no
    hexdump, e não `0xC0`.
    """
    bitmap[i // 8] |= 1 << (i % 8)


def limpa_bit(bitmap, i):
    """Desliga o bit i. Ainda sem chamador: nada é liberado antes de existir
    remoção."""
    bitmap[i // 8] &= ~(1 << (i % 8))


def testa_bit(bitmap, i):
    """O bit i está ligado? Serve tanto para bytes quanto para bytearray."""
    return bool(bitmap[i // 8] & (1 << (i % 8)))


def conta_bits(bitmap, quantos):
    """Quantos dos primeiros `quantos` bits estão ligados.

    Contar sob demanda foi escolhido em vez de um contador gravado no
    superbloco, e não é troca de espaço por tempo: quem pergunta já tem o bloco
    na memória, então contar não custa I/O — que é o único custo que conta aqui.

    O que se evita é a segunda cópia de uma verdade que o bitmap já guarda. Duas
    cópias podem discordar, e discordam exatamente no pior momento: uma falha
    entre gravar o bitmap e gravar o contador deixa os dois em desacordo, sem
    ninguém para notar.
    """
    return sum(1 for i in range(quantos) if testa_bit(bitmap, i))


def primeiro_livre(bitmap, quantos):
    """O índice do primeiro bit desligado entre os `quantos` primeiros, ou None
    se todos estão ligados."""
    for i in range(quantos):
        if not testa_bit(bitmap, i):
            return i
    return None


def bloco_para_bit(bloco):
    """Bloco físico -> índice no bitmap de dados.

    O bitmap de dados é indexado de forma relativa: o bit 0 é o bloco
    `DATA_REGION_START`, não o bloco 0. Esta função existe para que a subtração
    tenha um nome e um lugar, em vez de ser um `- 8` repetido em cada chamador
    — que é onde ele fatalmente vira um `- 8` esquecido.

    Passar um bloco de metadado é erro de programação, não entrada inválida: o
    alocador não deve sequer conseguir *nomear* o superbloco.
    """
    if bloco < DATA_REGION_START:
        raise ValueError(
            f"bloco {bloco} é metadado: não tem bit no bitmap de dados"
        )
    return bloco - DATA_REGION_START


def bit_para_bloco(i):
    """Índice no bitmap de dados -> bloco físico. A volta de bloco_para_bit."""
    return DATA_REGION_START + i


# --------------------------------------------------------------------------
# Inode — tabela a partir do bloco 3
# --------------------------------------------------------------------------

NUM_DIRECT = 12

# Os 176 bytes de padding entram no formato, não num ljust de quem chama. O
# inode tem 80 bytes de campos e ocupa uma fatia de 256; gravar só os 80
# deslocaria todos os inodes seguintes, e uma escrita curta aqui não dá erro
# nenhum — o estrago aparece no segundo arquivo criado, longe da causa. Padding
# que dá para esquecer em silêncio não pode ficar por conta do chamador.
_INODE_CAMPOS = f"<HHIIIIII{NUM_DIRECT}II"
_INODE_PADDING = INODE_SIZE - struct.calcsize(_INODE_CAMPOS)
# Preenche o restante do inode com o byte null (00).
INODE_FORMAT = _INODE_CAMPOS + f"{_INODE_PADDING}x"

assert _INODE_PADDING >= 0, "os campos do inode não cabem em INODE_SIZE"


class Inode(NamedTuple):
    """Tudo o que se sabe sobre um arquivo, menos o nome dele.

    O nome mora na entrada de diretório, não aqui — é o que permite que dois
    nomes apontem para o mesmo inode, e é por isso que existe o `nlink`.
    """

    mode: int
    nlink: int
    uid: int
    gid: int
    size: int
    atime: int
    mtime: int
    ctime: int
    direct: tuple  # NUM_DIRECT números de bloco absolutos; 0 = sem bloco
    indirect: int


def pack_inode(ino):
    """Inode -> 256 bytes, padding incluído."""
    if len(ino.direct) != NUM_DIRECT:
        raise ValueError(
            f"direct tem {len(ino.direct)} ponteiros, esperava {NUM_DIRECT}"
        )
    return struct.pack(INODE_FORMAT, *ino[:8], *ino.direct, ino.indirect)


def unpack_inode(buf, offset=0):
    """Bytes -> Inode. Lê 256 bytes a partir de `offset`.

    O `Inode(*campos)` que serviu ao superbloco não serve aqui: `12I` devolve
    doze valores soltos, e não um campo só. Os ponteiros diretos precisam ser
    reagrupados na volta, ou o `direct` ficaria valendo o primeiro deles.
    """
    campos = struct.unpack_from(INODE_FORMAT, buf, offset)
    return Inode(*campos[:8], campos[8:8 + NUM_DIRECT], campos[8 + NUM_DIRECT])


def localiza_inode(inum):
    """Número de inode -> (bloco, deslocamento dentro do bloco).

    A tabela não começa no bloco 0 e cabem INODES_PER_BLOCK inodes em cada
    bloco. As duas contas aparecem em toda leitura e toda escrita de inode, e
    ficam aqui pelo mesmo motivo de bloco_para_bit: repetidas em cada chamador,
    uma delas acaba errada.
    """
    if not 0 <= inum < NUM_INODES:
        raise ValueError(
            f"inode {inum} fora da tabela (válidos: 0 a {NUM_INODES - 1})"
        )
    bloco = INODE_TABLE_START + inum // INODES_PER_BLOCK
    return bloco, (inum % INODES_PER_BLOCK) * INODE_SIZE


# --------------------------------------------------------------------------
# Entrada de diretório — 32 bytes
# --------------------------------------------------------------------------

DIRENT_FORMAT = "<I28s"
DIRENT_SIZE = struct.calcsize(DIRENT_FORMAT)  # 32 bytes
NAME_MAX = DIRENT_SIZE - 4
DIRENTS_PER_BLOCK = BLOCK_SIZE // DIRENT_SIZE

# Inode 0 numa entrada quer dizer slot livre. É o outro lado da reserva do
# inode 0: como nenhum arquivo pode tê-lo, ele sobra para marcar o vazio.
# Remover um nome é gravar FREE_INUM por cima, e um bloco zerado já nasce com
# os 128 slots corretamente livres.
FREE_INUM = 0


class Dirent(NamedTuple):
    """Um nome e o inode que ele aponta. Nada mais.

    Todo o resto sobre o arquivo está no inode, e é essa separação que deixa
    dois nomes apontarem para o mesmo arquivo. Um diretório é um arquivo comum
    cujo conteúdo é uma sequência destas entradas.
    """

    inum: int
    name: bytes


def pack_dirent(ent):
    """Dirent -> 32 bytes, com o nome completado de NUL à direita.

    A conferência do comprimento não é zelo redundante: se o nome for maior que
    `NAME_MAX`, a função `struct.pack` vai cortar o nome e gravar um nome
    diferente.

    NUL e `/` ficam de fora do nome do arquivo porque o espaço restante dos 28
    bytes, se houver, é preenchido com NULs. Um nome que contivesse `/` seria
    interpretado como um caminho, não como um nome.
    """
    if len(ent.name) > NAME_MAX:
        raise ValueError(f"nome de {len(ent.name)} bytes, o máximo é {NAME_MAX}")
    if b"\x00" in ent.name or b"/" in ent.name:
        raise ValueError(f"nome inválido: {ent.name!r}")
    return struct.pack(DIRENT_FORMAT, ent.inum, ent.name)


def unpack_dirent(buf, offset=0):
    """Lê uma Dirent de 32 bytes a partir do offset no bloco (buf)."""
    inum, name = struct.unpack_from(DIRENT_FORMAT, buf, offset)
    return Dirent(inum, name.rstrip(b"\x00"))


def entradas_do_bloco(bloco):
    """Todas as entradas de um bloco de diretório, livres inclusive.

    Filtrar as livres é decisão de quem varre: o readdir vai pular, o alocador
    de nomes vai justamente procurar por elas, e o fsck precisa ver as duas
    coisas.
    """
    for i in range(DIRENTS_PER_BLOCK):
        yield unpack_dirent(bloco, i * DIRENT_SIZE)
