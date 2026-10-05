"""Inspetor de imagens vsfs — lê e imprime, sem montar nada.

Cresce junto com o mkfs: todo pedaço novo que o mkfs aprende a gravar, o dumpfs
aprende a ler. Se o dumpfs lê de volta o que o mkfs gravou, o formato fecha; se
não lê, o erro está nas últimas linhas escritas.

Um formato de disco é um contrato entre um escritor e um leitor. Desenvolver os
dois como espelhos torna isso concreto.

O análogo real existe: é o debugfs do e2fsprogs, que lê um ext4 sem montá-lo. A
conferência do fim tem o dela: o e2fsck compara as mesmas estruturas entre si,
numa escala em que a varredura leva minutos.
"""

import argparse
import stat
import sys
import time

import layout
from disk import Disk
from layout import BLOCK_SIZE, MAGIC, SUPERBLOCK_BLOCK


def le_superbloco(disco):
    """Lê e valida o bloco 0. É o primeiro passo de qualquer montagem."""
    sb = layout.unpack_superblock(disco.read_block(SUPERBLOCK_BLOCK))

    if sb.magic != MAGIC:
        raise ValueError(
            f"magic {sb.magic!r}, esperava {MAGIC!r}: "
            f"isto não é uma imagem vsfs (ou o mkfs não rodou)"
        )
    if sb.block_size != disco.block_size:
        raise ValueError(
            f"superbloco diz bloco de {sb.block_size} B, "
            f"a imagem foi aberta com {disco.block_size} B"
        )
    return sb


def imprime_superbloco(sb):
    print("=== superbloco ===")
    print(f"  magic              : {sb.magic.decode()}")
    print(f"  block_size         : {sb.block_size}")
    print(f"  num_inodes         : {sb.num_inodes}")
    print(f"  num_data_blocks    : {sb.num_data_blocks}")
    print(f"  inode_bitmap_start : {sb.inode_bitmap_start}")
    print(f"  data_bitmap_start  : {sb.data_bitmap_start}")
    print(f"  inode_table_start  : {sb.inode_table_start}")
    print(f"  data_region_start  : {sb.data_region_start}")
    print(f"  inode_size         : {sb.inode_size}")
    print(f"  root_inum          : {sb.root_inum}")


def imprime_mapa(sb):
    """As regiões, derivadas do superbloco e não de constantes.

    É a prova de que o sistema de arquivos é auto-descritivo: este mapa sai do
    que está gravado no disco, não do que o layout.py acha que deveria ser.
    """
    regioes = [
        ("superbloco", SUPERBLOCK_BLOCK, 1),
        ("bitmap de inodes", sb.inode_bitmap_start, 1),
        ("bitmap de dados", sb.data_bitmap_start, 1),
        ("tabela de inodes", sb.inode_table_start,
         sb.data_region_start - sb.inode_table_start),
        ("região de dados", sb.data_region_start, sb.num_data_blocks),
    ]

    print("=== mapa ===")
    for nome, inicio, quantos in regioes:
        fim = inicio + quantos - 1
        faixa = f"{inicio}" if quantos == 1 else f"{inicio}-{fim}"
        print(f"  bloco {faixa:<7} {nome:<18} ({quantos} bloco(s))")


def _imprime_bitmap(nome, bloco, quantos, legenda=""):
    mapa = "".join("#" if layout.testa_bit(bloco, i) else "." for i in range(quantos))
    usados = layout.conta_bits(bloco, quantos)
    print(f"  {nome:<7} {usados}/{quantos} em uso{legenda}")
    print(f"  {'':<7} [{mapa}]")


def imprime_bitmaps(disco, sb):
    """A ocupação, bit a bit.

    Imprime apenas os bits que significam alguma coisa. O bloco tem 4096 bytes,
    ou 32768 bits, e só os primeiros num_inodes / num_data_blocks correspondem a
    algo que existe — a estrutura comporta muito mais estados do que os válidos.
    Um bit ligado lá no meio do resto seria corrupção real, e conferir isso é
    trabalho de um fsck.
    """
    print("=== bitmaps ===")
    _imprime_bitmap("inodes", disco.read_block(sb.inode_bitmap_start), sb.num_inodes)
    _imprime_bitmap(
        "dados",
        disco.read_block(sb.data_bitmap_start),
        sb.num_data_blocks,
        legenda=f"  (bit 0 = bloco {sb.data_region_start})",
    )


def le_inode(disco, sb, inum):
    """Lê um inode da tabela.

    A conta que acha a fatia mora no layout, e não aqui: é a mesma que o mkfs
    usa para gravar. Leitor e escritor discordarem sobre onde um inode fica é
    o tipo de erro que produz lixo plausível em vez de exceção.
    """
    bloco, offset = layout.localiza_inode(inum)
    return layout.unpack_inode(disco.read_block(bloco), offset)


def percorre_inodes(disco, sb):
    """Gera (número, inode) para a tabela inteira, lendo cada bloco uma vez.

    O le_inode serve para buscar um inode solto. Numa varredura ele traria o
    mesmo bloco dezesseis vezes, e o contador de I/O do Disk existe para deixar
    isso à vista. Como os números saem em ordem, o bloco só muda a cada
    dezesseis inodes — guardar o último lido basta.
    """
    buf = None
    ultimo = None

    for inum in range(sb.num_inodes):
        bloco, offset = layout.localiza_inode(inum)
        if bloco != ultimo:
            buf = disco.read_block(bloco)
            ultimo = bloco
        yield inum, layout.unpack_inode(buf, offset)


def imprime_inode(titulo, ino):
    ocupados = [b for b in ino.direct if b != layout.FREE_INUM]
    quando = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(ino.mtime))

    print(f"=== {titulo} ===")
    print(f"  mode     : {stat.filemode(ino.mode)}  ({ino.mode:#o})")
    print(f"  nlink    : {ino.nlink}")
    print(f"  uid/gid  : {ino.uid}/{ino.gid}")
    print(f"  size     : {ino.size}")
    print(f"  mtime    : {quando}")
    print(f"  direct   : {ocupados}  (+{layout.NUM_DIRECT - len(ocupados)} vazio(s))")
    print(f"  indirect : {ino.indirect}")


def imprime_diretorio(disco, ino):
    """Lista as entradas, seguindo os ponteiros do inode.

    É a primeira travessia de verdade da imagem: sai do inode, chega ao bloco e
    lê nomes. O readdir vai fazer exatamente isto, com a diferença de entregar o
    resultado ao kernel em vez de imprimir.
    """
    print("=== conteúdo ===")
    for bloco in (b for b in ino.direct if b != layout.FREE_INUM):
        print(f"  bloco {bloco}")
        livres = 0
        for i, entrada in enumerate(layout.entradas_do_bloco(disco.read_block(bloco))):
            if entrada.inum == layout.FREE_INUM:
                livres += 1
                continue
            nome = entrada.name.decode("utf-8", "replace")
            print(f"    slot {i:<4} inode {entrada.inum:<4} {nome}")
        print(f"    ({livres} slot(s) livre(s))")


def verifica(disco, sb):
    """Confere se as estruturas concordam entre si, e devolve os desacordos.

    Cada estrutura afirma algo sobre as outras: o bitmap diz quais inodes
    existem, o inode diz quais blocos são dele, a entrada de diretório diz qual
    inode atende por aquele nome. Num disco íntegro as três contam a mesma
    história, e conferir isso é o trabalho de um fsck.

    Nada é corrigido aqui, só relatado.
    """
    problemas = []

    # A geometria do superbloco tem de caber na imagem que a carrega. Um
    # superbloco de outro tamanho de disco descreve blocos que não existem.
    descritos = sb.data_region_start + sb.num_data_blocks
    if descritos != disco.num_blocks:
        problemas.append(
            f"o superbloco descreve {descritos} blocos, "
            f"mas a imagem tem {disco.num_blocks}"
        )

    bitmap_inodes = disco.read_block(sb.inode_bitmap_start)
    bitmap_dados = disco.read_block(sb.data_bitmap_start)

    if not layout.testa_bit(bitmap_inodes, layout.FREE_INUM):
        problemas.append("o inode 0 é a lápide e não está marcado em uso")
    if not layout.testa_bit(bitmap_inodes, sb.root_inum):
        problemas.append(f"o inode {sb.root_inum} é a raiz e não está marcado em uso")

    # O bitmap de dados refeito do zero, a partir dos ponteiros de quem está em
    # uso. Comparar o bitmap reconstruído com o gravado é o teste central, e
    # pega os dois desacordos possíveis de uma vez só.
    apontados = layout.bitmap_vazio()

    for inum, ino in percorre_inodes(disco, sb):
        # O inode 0 fica de fora: é a lápide, está zerado de propósito, e a
        # regra "marcado em uso tem conteúdo" não vale para ele.
        if inum == layout.FREE_INUM:
            continue
        em_uso = layout.testa_bit(bitmap_inodes, inum)

        # mode zerado é o que distingue um inode nunca escrito de um em uso:
        # nenhum arquivo tem tipo nenhum.
        if ino.mode == 0:
            if em_uso:
                problemas.append(f"inode {inum}: marcado em uso, mas está zerado")
            continue
        if not em_uso:
            problemas.append(f"inode {inum}: tem conteúdo, mas o bitmap o dá por livre")
            continue
        if ino.nlink == 0:
            problemas.append(f"inode {inum}: em uso com nlink 0; nome nenhum o alcança")

        for bloco in ino.direct + (ino.indirect,):
            if bloco == 0:
                continue
            if not sb.data_region_start <= bloco < disco.num_blocks:
                problemas.append(
                    f"inode {inum}: aponta para o bloco {bloco}, "
                    f"fora da região de dados"
                )
                continue
            bit = layout.bloco_para_bit(bloco)
            if layout.testa_bit(apontados, bit):
                problemas.append(f"bloco {bloco}: apontado mais de uma vez")
            layout.marca_bit(apontados, bit)

    # Os dois desacordos custam coisas diferentes: um bloco em uso que ninguém
    # aponta só desperdiça espaço, enquanto um bloco apontado que o bitmap dá
    # por livre vai ser entregue a um segundo arquivo e sobrescrito.
    for i in range(sb.num_data_blocks):
        no_bitmap = layout.testa_bit(bitmap_dados, i)
        apontado = layout.testa_bit(apontados, i)
        if no_bitmap == apontado:
            continue
        bloco = layout.bit_para_bloco(i)
        if no_bitmap:
            problemas.append(f"bloco {bloco}: em uso no bitmap, mas ninguém o aponta")
        else:
            problemas.append(f"bloco {bloco}: apontado por um inode, mas livre no bitmap")

    problemas += _verifica_raiz(disco, sb)
    return problemas


def _verifica_raiz(disco, sb):
    """A raiz é um diretório e se acha pelo `.` e pelo `..`.

    Sem essas duas entradas nenhum caminho se resolve, porque toda travessia
    começa na raiz — e é o único diretório que ninguém pode recriar de fora.
    """
    problemas = []
    raiz = le_inode(disco, sb, sb.root_inum)

    if not stat.S_ISDIR(raiz.mode):
        return [f"o inode {sb.root_inum} é a raiz e não é um diretório"]

    # Ponteiro fora da região de dados já foi relatado; aqui ele é ignorado
    # para que a leitura não estoure em cima de uma imagem corrompida.
    nomes = {}
    for bloco in raiz.direct:
        if not sb.data_region_start <= bloco < disco.num_blocks:
            continue
        for entrada in layout.entradas_do_bloco(disco.read_block(bloco)):
            if entrada.inum != layout.FREE_INUM:
                nomes[entrada.name] = entrada.inum

    for nome in (b".", b".."):
        if nomes.get(nome) != sb.root_inum:
            problemas.append(
                f"a raiz não tem {nome.decode()} apontando para o inode {sb.root_inum}"
            )
    return problemas


def imprime_verificacao(problemas):
    print("=== verificação ===")
    if not problemas:
        print("  nenhum problema")
        return
    for problema in problemas:
        print(f"  {problema}")
    print(f"  {len(problemas)} problema(s)")


def despeja(caminho):
    """Imprime a imagem inteira e devolve os problemas encontrados."""
    with Disk(caminho, BLOCK_SIZE) as disco:
        sb = le_superbloco(disco)
        imprime_superbloco(sb)
        imprime_mapa(sb)
        imprime_bitmaps(disco, sb)

        raiz = le_inode(disco, sb, sb.root_inum)
        imprime_inode(f"inode {sb.root_inum} (raiz)", raiz)
        imprime_diretorio(disco, raiz)

        problemas = verifica(disco, sb)
        imprime_verificacao(problemas)

        print(f"\n({disco.reads} bloco(s) lido(s))")
        return problemas


def main(argv):
    parser = argparse.ArgumentParser(description="Inspeciona uma imagem vsfs.")
    parser.add_argument("imagem", help="arquivo a inspecionar")
    args = parser.parse_args(argv[1:])

    try:
        problemas = despeja(args.imagem)
    except ValueError as e:
        print(f"{parser.prog}: {e}", file=sys.stderr)
        return 1

    # Sair diferente de zero quando as estruturas discordam é o contrato de um
    # fsck: quem chamou reage sem precisar ler a saída.
    return 1 if problemas else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
