# Sistemas Operacionais II

Repositório técnico da disciplina, com foco na implementação de um sistema de arquivos de verdade: do formato on-disk até a camada que atende as chamadas de sistema do kernel.

## Informações Gerais
* Instituição: Universidade Federal de Mato Grosso
* Linguagem: Python 3
* Referência principal: OSTEP, capítulo 40 (*File System Implementation*)

## Projeto: vsfs

O **vsfs** (*Very Simple File System*) é o sistema de arquivos do capítulo 40 do OSTEP, implementado em Python sobre uma imagem de disco de 256 KiB e montado no sistema com FUSE (biblioteca [mfusepy](https://pypi.org/project/mfusepy/)).

O trabalho é construído em fases. Cada pasta é um retrato completo do código ao fim de uma fase, e cada fase acrescenta uma chamada de sistema à anterior:

| Fase | O que o vsfs montado sabe fazer | Operações novas |
|---|---|---|
| [fase-B.1](./fase-B.1) | Listar, ler e criar arquivos vazios | `getattr`, `readdir`, `read`, `create` |
| [fase-B.2](./fase-B.2) | Gravar conteúdo nos arquivos | `write` |
| [fase-B.3](./fase-B.3) | Apagar arquivos | `unlink` |

### Geometria do disco

| Blocos | Região |
|---|---|
| 0 | Superbloco |
| 1 | Bitmap de inodes |
| 2 | Bitmap de dados |
| 3-7 | Tabela de inodes (80 inodes de 256 B) |
| 8-63 | Região de dados (56 blocos de 4 KiB) |

O inode 0 é reservado como lápide das entradas de diretório livres, e a raiz é o inode 1. Cada inode tem 12 ponteiros diretos. O ponteiro indireto existe no formato, mas ainda não é usado, então um arquivo tem no máximo 48 KiB.

### Arquivos comuns a todas as fases

| Arquivo | Papel |
|---|---|
| `disk.py` | Camada de blocos: o único acesso à imagem, sempre por bloco inteiro, com contadores de leituras e gravações |
| `layout.py` | Constantes e formatos on-disk (superbloco, bitmaps, inode, entrada de diretório). Só empacota e desempacota bytes, sem I/O |
| `mkfs.py` | Formata uma imagem: superbloco, bitmaps e a raiz com `.` e `..` |
| `dumpfs.py` | Inspeciona uma imagem sem montá-la e confere a consistência entre bitmaps, inodes e diretórios, como um fsck |
| `vsfs.py` | O sistema de arquivos montado: traduz cada chamada do kernel em leituras e gravações de blocos |

### Como usar

```bash
pip install mfusepy                # exige FUSE no sistema (libfuse no Linux, macFUSE no macOS)

cd fase-B.3
python3 mkfs.py disco.img          # formata (use --force para sobrescrever)
mkdir -p /tmp/mnt
python3 vsfs.py disco.img /tmp/mnt -f   # monta em primeiro plano

# em outro terminal
echo "olá" > /tmp/mnt/a.txt
cat /tmp/mnt/a.txt
rm /tmp/mnt/a.txt

umount /tmp/mnt
python3 dumpfs.py disco.img        # imprime a imagem e verifica a consistência
```

## Bibliografia Principal
* ARPACI-DUSSEAU, R. H.; ARPACI-DUSSEAU, A. C. Operating Systems: Three Easy Pieces (OSTEP)
* TANENBAUM, Andrew S. Sistemas operacionais modernos
