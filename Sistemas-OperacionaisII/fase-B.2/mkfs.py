"""Formata uma imagem vsfs.

Formatar é escrever, não apagar: um sistema de arquivos vazio não é um disco
zerado, é um disco com superbloco, bitmaps e o inode da raiz no lugar certo.

Este programa não tem nada a ver com FUSE. Ele abre um arquivo e escreve bytes,
exatamente como o mkfs.ext4 faz num /dev/sda1 — a única diferença é qual arquivo
foi aberto.
"""

import argparse
import os
import stat
import sys
import time

import layout
from disk import Disk
from layout import BLOCK_SIZE, IMAGE_SIZE, SUPERBLOCK_BLOCK


def em_bloco(dados):
    """Completa com zeros até um bloco inteiro.

    write_block exige exatamente BLOCK_SIZE bytes, de propósito: uma escrita
    curta teria sucesso e deixaria o resto do bloco com o conteúdo velho.
    """
    if len(dados) > BLOCK_SIZE:
        raise ValueError(f"{len(dados)} bytes não cabem num bloco")
    return dados.ljust(BLOCK_SIZE, b"\x00")


def cria_imagem(caminho, sobrescreve):
    """Cria o arquivo com o tamanho exato, todo zerado.

    Zeros de verdade, e não f.truncate(IMAGE_SIZE), que produziria um arquivo
    esparso: tamanho declarado de 256 KiB sem bloco nenhum alocado no disco.
    Compare com `ls -l` e `du -h` para ver a diferença — é o mesmo conceito de
    buraco que decidimos não suportar no vsfs.

    O modo "xb" falha se o arquivo já existe, e é o próprio open que falha
   . Perguntar antes, com os.path.exists, abriria uma janela entre a
    pergunta e a resposta: outro processo cria o arquivo nesse intervalo e ele
    é destruído mesmo tendo sido conferido.
    """
    with open(caminho, "wb" if sobrescreve else "xb") as f:
        f.write(b"\x00" * IMAGE_SIZE)


def grava_superbloco(disco):
    """Bloco 0: o primeiro lugar onde qualquer leitor vai olhar."""
    sb = layout.superbloco_padrao()
    disco.write_block(SUPERBLOCK_BLOCK, em_bloco(layout.pack_superblock(sb)))
    return sb


def grava_bitmaps(disco, sb):
    """Escreve os dois bitmaps: inodes no bloco 1, dados no bloco 2.

    Numa imagem recém-formatada só três bits estão ligados: os inodes 0 e 1 e o
    bloco 8 — a lápide, a raiz, e o bloco onde a raiz vai guardar `.` e `..`.

    Os blocos 0 a 7 são metadado e nem aparecem no bitmap de dados, que começa a
    contar no bloco 8. O alocador não consegue nomeá-los, quanto mais
    entregá-los a um arquivo.

    Neste ponto o inode 1 e o bloco 8 ainda estão zerados: os bits dizem "em
    uso" antes de haver o que usar. A imagem só fecha quando a raiz for escrita.
    """
    inodes = layout.bitmap_vazio()
    layout.marca_bit(inodes, 0)             # a lápide das entradas vazias
    layout.marca_bit(inodes, sb.root_inum)  # a raiz
    disco.write_block(sb.inode_bitmap_start, bytes(inodes))

    dados = layout.bitmap_vazio()
    # Marcar pelo número do bloco, e não escrever 0 direto, mantém à vista que
    # índice de bitmap e número de bloco são coisas diferentes.
    layout.marca_bit(dados, layout.bloco_para_bit(sb.data_region_start))
    disco.write_block(sb.data_bitmap_start, bytes(dados))


def grava_inode_raiz(disco, sb):
    """Escreve o inode 1: a raiz passa a existir como arquivo.

    Um bloco da tabela guarda dezesseis inodes, então gravar um deles é ler o
    bloco, trocar a fatia de 256 bytes e gravar o bloco de volta. Aqui os
    vizinhos são zeros e ninguém sentiria falta da leitura — mas o dia em que
    sentir é o dia em que ela apaga quinze inodes alheios.

    Os três tempos saem de uma chamada só a time.time(): numa formatação eles
    devem ser o mesmo instante, e duas chamadas podem cair em segundos
    diferentes.
    """
    agora = int(time.time())

    raiz = layout.Inode(
        mode=stat.S_IFDIR | 0o755,
        nlink=2,  # a raiz é apontada pelo `.` dela e por ela mesma
        uid=os.getuid(),  # quem formata vira dono da raiz
        gid=os.getgid(),
        size=sb.block_size,  # diretório mede os blocos que tem, não as entradas
        atime=agora,
        mtime=agora,
        ctime=agora,
        # O primeiro bloco de dados, pelo número **absoluto** dele. É o
        # mesmo bloco que grava_bitmaps marcou, e lá o número era relativo.
        direct=(sb.data_region_start,) + (0,) * (layout.NUM_DIRECT - 1),
        indirect=0,  # sem ponteiro indireto até a fase 2
    )

    bloco, offset = layout.localiza_inode(sb.root_inum)
    tabela = bytearray(disco.read_block(bloco))
    tabela[offset:offset + layout.INODE_SIZE] = layout.pack_inode(raiz)
    disco.write_block(bloco, bytes(tabela))

    return raiz


def grava_dados_raiz(disco, sb, raiz):
    """Escreve `.` e `..` no bloco de dados da raiz.

    As duas entradas apontam para o mesmo inode: `.` é o próprio diretório e
    `..` é o pai, que na raiz é ela mesma. É o que faz `cd /..` continuar em
    `/` em vez de cair fora do sistema de arquivos.

    Os outros 126 slots ficam zerados, e zerado já significa livre — inode 0
    numa entrada é a marca de vazio. O bloco nasce correto sem que seja preciso
    escrever coisa alguma neles.

    O bloco vem do ponteiro do inode, não de uma constante. Quem decidiu onde a
    raiz mora foi quem gravou o inode, e repetir a decisão aqui seria criar uma
    segunda fonte para a mesma verdade.
    """
    bloco = bytearray(BLOCK_SIZE)

    entradas = [
        layout.Dirent(sb.root_inum, b"."),
        layout.Dirent(sb.root_inum, b".."),
    ]
    for i, entrada in enumerate(entradas):
        inicio = i * layout.DIRENT_SIZE
        bloco[inicio:inicio + layout.DIRENT_SIZE] = layout.pack_dirent(entrada)

    disco.write_block(raiz.direct[0], bytes(bloco))


def formata(caminho, sobrescreve=False):
    cria_imagem(caminho, sobrescreve)

    with Disk(caminho, BLOCK_SIZE) as disco:
        sb = grava_superbloco(disco)
        grava_bitmaps(disco, sb)
        raiz = grava_inode_raiz(disco, sb)
        grava_dados_raiz(disco, sb, raiz)

        print(f"{caminho}: {disco.num_blocks} blocos de {sb.block_size} B")
        print(f"  {sb.num_inodes} inodes a partir do bloco {sb.inode_table_start}")
        print(
            f"  {sb.num_data_blocks} blocos de dados a partir do "
            f"bloco {sb.data_region_start}"
        )
        print(f"  {disco.writes} bloco(s) escrito(s)")


def main(argv):
    parser = argparse.ArgumentParser(description="Formata uma imagem vsfs.")
    parser.add_argument("imagem", help="arquivo a formatar")
    parser.add_argument(
        "-f",
        "--force",
        action="store_true",
        help="formata por cima de uma imagem que já existe, apagando o conteúdo",
    )
    args = parser.parse_args(argv[1:])

    try:
        formata(args.imagem, args.force)
    except FileExistsError:
        print(
            f"{args.imagem}: já existe; use --force para formatar por cima",
            file=sys.stderr,
        )
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
