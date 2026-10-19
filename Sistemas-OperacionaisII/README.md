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
| [fase-B.3](./fase-B.3) | Apagar arquivos, diretórios, hard links, renomear, permissões e espaço livre | `unlink`, `mkdir`, `rmdir`, `link`, `rename`, `chmod`, `statfs` |

### Geometria do disco

| Blocos | Região |
|---|---|
| 0 | Superbloco |
| 1 | Bitmap de inodes |
| 2 | Bitmap de dados |
| 3-7 | Tabela de inodes (80 inodes de 256 B) |
| 8-63 | Região de dados (56 blocos de 4 KiB) |

O inode 0 é reservado como lápide das entradas de diretório livres, e a raiz é o inode 1. Cada inode tem 12 ponteiros diretos. O ponteiro indireto existe no formato, mas ainda não é usado, então um arquivo (ou diretório) tem no máximo 48 KiB.

### Arquivos comuns a todas as fases

| Arquivo | Papel |
|---|---|
| `disk.py` | Camada de blocos: o único acesso à imagem, sempre por bloco inteiro, com contadores de leituras e gravações |
| `layout.py` | Constantes e formatos on-disk (superbloco, bitmaps, inode, entrada de diretório). Só empacota e desempacota bytes, sem I/O |
| `mkfs.py` | Formata uma imagem: superbloco, bitmaps e a raiz com `.` e `..` |
| `dumpfs.py` | Inspeciona uma imagem sem montá-la e confere a consistência entre bitmaps, inodes e diretórios (na B.3, a árvore inteira e o `nlink` de cada inode), como um fsck |
| `vsfs.py` | O sistema de arquivos montado: traduz cada chamada do kernel em leituras e gravações de blocos |

## Instalação

O vsfs precisa de três coisas:

- **Python 3.9 ou mais novo**;
- **FUSE**, o módulo do kernel que deixa um programa comum atender chamadas de sistema;
- a biblioteca **mfusepy**.

O jeito mais simples de ter FUSE é o **Linux**. Por isso, no Windows e no macOS o caminho recomendado é rodar um Ubuntu dentro do próprio computador: o WSL 2 no Windows e uma máquina virtual Multipass no macOS. A partir do passo "Instalar as dependências", os comandos são os mesmos nos três sistemas.

### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install -y git python3 python3-venv fuse3
```

Em outras distribuições, instale os pacotes equivalentes. No Fedora, por exemplo: `sudo dnf install git python3 fuse3`.

### Windows (WSL 2)

O WSL 2 roda um kernel Linux de verdade, com suporte a FUSE.

1. Abra o **PowerShell como administrador** e instale o WSL com o Ubuntu:
   ```powershell
   wsl --install -d Ubuntu-24.04
   ```
2. Reinicie o computador quando o instalador pedir.
3. Abra o aplicativo **Ubuntu** pelo menu Iniciar e crie o usuário e a senha do Linux.
4. Dentro do Ubuntu, instale os pacotes:
   ```bash
   sudo apt update
   sudo apt install -y git python3 python3-venv fuse3
   ```

Clone e rode o projeto **dentro do sistema de arquivos do Linux** (em `~`), e não em `/mnt/c/...`. A pasta do Windows passa por uma camada de tradução lenta, e o arquivo de imagem do vsfs não deve depender dela.

### macOS (máquina virtual Multipass)

1. Instale o [Multipass](https://multipass.run/install), pelo instalador do site ou com o Homebrew:
   ```bash
   brew install --cask multipass
   ```
2. Crie a máquina virtual e entre nela:
   ```bash
   multipass launch 24.04 --name vsfs --cpus 2 --memory 2G --disk 10G
   multipass shell vsfs
   ```
3. Dentro da VM (o prompt vira `ubuntu@vsfs:~$`), instale os pacotes:
   ```bash
   sudo apt update
   sudo apt install -y git python3 python3-venv fuse3
   ```

Para abrir outro terminal na VM, rode `multipass shell vsfs` numa nova aba. Para desligá-la, use `multipass stop vsfs`. Para apagá-la de vez, use `multipass delete --purge vsfs`.

Também dá para rodar direto no macOS com o [macFUSE](https://osxfuse.github.io/). Mas ele instala uma extensão de kernel, que precisa ser autorizada nos Ajustes do Sistema, e nos Macs com Apple Silicon isso exige reduzir a segurança na inicialização. Por isso a VM é o caminho recomendado.

### Instalar as dependências (todos os sistemas)

Dentro do Linux (nativo, WSL ou VM):

```bash
git clone https://github.com/cavalcantiraissas/Ciencia-da-Computacao_UFMT.git
cd Ciencia-da-Computacao_UFMT/Sistemas-OperacionaisII

python3 -m venv ~/venv            # ambiente isolado; o Ubuntu 24.04 não deixa usar pip no Python do sistema
source ~/venv/bin/activate        # repita este comando em todo terminal novo
pip install mfusepy
```

## Como rodar

Você vai usar **dois terminais**. O primeiro mantém o sistema de arquivos montado e mostra os erros. O segundo usa o sistema de arquivos como qualquer outra pasta.

**Terminal 1: formatar e montar**

```bash
source ~/venv/bin/activate
cd ~/Ciencia-da-Computacao_UFMT/Sistemas-OperacionaisII/fase-B.3

python3 mkfs.py ~/disco.img --force         # formata uma imagem de 256 KiB
mkdir -p /tmp/mnt
python3 vsfs.py ~/disco.img /tmp/mnt -f     # monta em primeiro plano (o terminal fica preso, é o esperado)
```

**Terminal 2: usar**

```bash
cd /tmp/mnt
echo "olá" > a.txt && cat a.txt
```

**Desmontar e verificar**

```bash
cd ~ && fusermount3 -u /tmp/mnt      # no terminal 2; libera o terminal 1
python3 dumpfs.py ~/disco.img        # no terminal 1: imprime a imagem e roda o fsck
```

O `dumpfs` deve terminar com **nenhum problema**. Para começar do zero, rode o `mkfs.py` de novo.

Para rodar as fases anteriores, troque `fase-B.3` por `fase-B.1` ou `fase-B.2`. Elas só atendem as operações listadas na tabela de fases.

## Comandos de teste (fase B.3)

Rode no **terminal 2**, dentro de `/tmp/mnt`. O comentário de cada linha diz o resultado esperado.

```bash
# Base: criar, gravar e ler
echo "olá mundo" > a.txt
cat a.txt                         # olá mundo

# mkdir
mkdir -p docs/sub/fundo
stat -c '%h %n' . docs            # 3 .   e   3 docs  (cada subdiretório soma 1 ao nlink do pai)
mkdir docs                        # erro: File exists

# link (hard link)
ln a.txt docs/b.txt
ls -li a.txt docs/b.txt           # mesmo inode nas duas linhas, nlink 2
echo "mais" >> docs/b.txt
cat a.txt                         # mostra "mais" também: é o mesmo arquivo
ln docs dir2                      # erro: hard link not allowed for directory

# unlink
rm a.txt
cat docs/b.txt                    # o conteúdo continua lá
stat -c '%h' docs/b.txt           # 1

# rename
mv docs/b.txt docs/c.txt          # troca o nome no mesmo diretório
mv docs/c.txt c.txt               # muda de diretório
echo novo > d.txt
mv c.txt d.txt                    # sobrescreve um destino que já existe
cat d.txt                         # olá mundo / mais
mv docs/sub outro                 # move um diretório; o ".." dele passa a apontar a raiz
mv outro outro/fundo/x            # erro: Invalid argument (dentro dele mesmo)

# chmod
chmod 600 d.txt && ls -l d.txt    # -rw-------
chmod 000 d.txt
cat d.txt                         # erro: Permission denied
chmod 644 d.txt && cat d.txt      # volta a funcionar
chmod 700 docs && ls -ld docs     # drwx------ (continua sendo diretório)

# rmdir
rmdir outro                       # erro: Directory not empty (ainda tem fundo/)
rmdir outro/fundo outro           # funciona
rmdir d.txt                       # erro: Not a directory

# statfs
df -h /tmp/mnt                    # tamanho 224K (56 blocos de dados de 4 KiB)
df -i /tmp/mnt                    # 80 inodes no total
head -c 40000 /dev/urandom > grande.bin
df -h /tmp/mnt                    # o espaço usado sobe cerca de 40K
rm grande.bin
df -h /tmp/mnt                    # volta ao valor anterior

# Diretório que cresce com muitos hard links
mkdir muitos
for i in $(seq 1 200); do ln d.txt muitos/l$i; done
stat -c '%s' muitos               # 8192: o diretório ganhou um segundo bloco
stat -c '%h' d.txt                # 201
rm muitos/* && rmdir muitos
stat -c '%h' d.txt                # 1
```

Depois dos testes, desmonte e rode o `dumpfs.py`, como em [Desmontar e verificar](#como-rodar). A seção `=== árvore ===` lista o que sobrou (`/docs/` e `/d.txt`), e a verificação deve terminar com **nenhum problema**.

### Problemas comuns

| Erro | Causa e solução |
|---|---|
| `OSError: Unable to find libfuse` | O FUSE não está instalado. Rode `sudo apt install fuse3` |
| `ModuleNotFoundError: No module named 'mfusepy'` | O ambiente virtual não está ativo. Rode `source ~/venv/bin/activate` |
| `fusermount3: failed to access mountpoint` ou `Transport endpoint is not connected` | Sobrou uma montagem de uma execução interrompida. Rode `fusermount3 -u /tmp/mnt` (ou `sudo umount -l /tmp/mnt`) e monte de novo |
| `/dev/fuse: No such file or directory` no WSL | O WSL está na versão 1. No PowerShell, rode `wsl --set-version Ubuntu-24.04 2` |
| `não é uma imagem vsfs; rode o mkfs` | A imagem não foi formatada. Rode `python3 mkfs.py ~/disco.img --force` |

## Bibliografia Principal
* ARPACI-DUSSEAU, R. H.; ARPACI-DUSSEAU, A. C. Operating Systems: Three Easy Pieces (OSTEP)
* TANENBAUM, Andrew S. Sistemas operacionais modernos
