# Fase B.3: as rotinas restantes

Completa o vsfs da [fase B.2](../fase-B.2) com as operações que faltavam. Agora dá para apagar arquivos, criar e remover diretórios, criar hard links, renomear e mover, mudar permissões e consultar o espaço livre.

| Operação | Comando no shell | O que faz no disco |
|---|---|---|
| `unlink` | `rm` | Apaga um nome. Libera o inode e os blocos quando era o último nome |
| `mkdir` | `mkdir` | Cria um inode de diretório e um bloco com `.` e `..`, e soma 1 ao `nlink` do pai |
| `rmdir` | `rmdir` | Remove um diretório vazio e devolve o inode e o bloco aos bitmaps |
| `link` | `ln` | Cria mais uma entrada apontando para um inode existente, e o `nlink` sobe um |
| `rename` | `mv` | Move a entrada de diretório. O inode e os dados não mudam |
| `chmod` | `chmod` | Troca os 12 bits de permissão e mantém os bits de tipo |
| `statfs` | `df` | Conta os bits livres nos dois bitmaps |

Links simbólicos (`ln -s`) ficam de fora desta fase.

## O que mudou

| Arquivo | Mudança |
|---|---|
| `vsfs.py` | As sete operações acima e os auxiliares que elas compartilham: `_procura`, `_resolve_dir`, `_vazio`, `_ajusta_inode`, `_reescreve_entrada`, `_descarta` e `_solta_nome`. O `_grava_entrada` passou a aumentar o diretório quando ele fica cheio. A montagem usa `default_permissions` |
| `dumpfs.py` | Percorre a árvore inteira (`percorre_arvore`) e mostra todos os caminhos. A verificação também confere o `.` e o `..` de cada diretório e o `nlink` de cada inode |
| `layout.py` | Só a docstring de `limpa_bit`, que agora tem mais chamadores |

## Como cada operação funciona

### `unlink`
1. Grava a lápide (`FREE_INUM`) por cima da entrada no diretório pai.
2. Diminui o `nlink` do inode. Se ainda sobrar algum nome (um hard link), termina aqui.
3. Se não sobrar nenhum, zera o inode, desliga os bits dos blocos dele no bitmap de dados e depois o bit dele no bitmap de inodes.

### `mkdir`
1. Aloca um inode e um bloco de dados.
2. Grava no bloco as entradas `.` (o próprio diretório) e `..` (o pai).
3. Grava o inode com `S_IFDIR`, `nlink = 2` (o nome no pai e o `.`) e `size` de um bloco.
4. Soma 1 ao `nlink` do pai, por causa do `..` do filho, e por último grava o nome no pai.

### `rmdir`
- Recusa a raiz (`EBUSY`), quem não é diretório (`ENOTDIR`) e diretórios com algo além de `.` e `..` (`ENOTEMPTY`).
- Apaga o nome no pai, tira 1 do `nlink` do pai e libera o inode e os blocos, pela mesma rotina do `unlink`.

### `link` (hard link)
- Nenhum dado é copiado. Uma entrada nova passa a apontar para o mesmo inode, e os dois nomes ficam indistinguíveis (mesmo `st_ino` no `ls -i`).
- Hard link para diretório é recusado (`EPERM`). Ele poderia criar um ciclo, e o `..` deixaria de ter um único pai.

### `rename`
- **No mesmo diretório:** a entrada é reescrita no lugar, só com o nome trocado. É uma única gravação de bloco.
- **O destino já existe:** a entrada do destino passa a apontar o inode de origem, e só depois o nome antigo é apagado. Assim o caminho de destino nunca deixa de existir, como o POSIX exige. O arquivo substituído perde um nome e é liberado se era o último.
- **Diretório que troca de pai:** o `..` dele é atualizado, o pai antigo perde 1 no `nlink` e o pai novo ganha 1.
- Recusas: mover um diretório para dentro dele mesmo (`EINVAL`), trocar arquivo por diretório (`EISDIR`) ou o contrário (`ENOTDIR`), e sobrescrever um diretório que não está vazio (`ENOTEMPTY`).

### `chmod`
- Grava `tipo_do_inode | (mode & 0o7777)`. O tipo nunca muda.
- Só o `ctime` é atualizado, porque o conteúdo não mudou. Por isso o `ls -lc` mostra a mudança e o `ls -l` não mostra.
- Com `default_permissions`, o kernel confere os bits gravados antes de cada acesso. Ele também só deixa o dono (ou o root) fazer `chmod`. Sem essa opção, um `chmod 000` não impediria ninguém de ler o arquivo.

### `statfs`
- `f_blocks` é a região de dados (56 blocos). Os metadados não entram, porque nenhum arquivo pode usá-los.
- Os valores livres são contados direto nos bitmaps. O superbloco não guarda contador, então não existe uma segunda cópia que possa discordar do bitmap.

## Decisões

- **Ordem das gravações.** Tudo o que cria (`create`, `mkdir`, `link`) grava o nome por último. Tudo o que remove (`unlink`, `rmdir`) apaga o nome primeiro. Se o programa parar no meio, sobra no máximo espaço perdido (um inode órfão, um bloco sem dono ou um `nlink` alto demais), e o fsck detecta. Nunca sobra um nome apontando para algo já liberado.
- **No `link` e no `mkdir`, o `nlink` sobe antes do nome aparecer.** A ordem contrária deixaria, numa parada no meio, dois nomes com `nlink` 1. O primeiro `rm` liberaria um inode que o segundo nome ainda aponta.
- **Diretórios crescem.** Antes, um diretório nunca enchia: são 80 inodes e 126 vagas livres no primeiro bloco. Com hard links, um único inode pode ter quantos nomes quiser. Por isso o `_grava_entrada` aloca um bloco novo quando os que o diretório já tem estão cheios. Se não houver espaço para isso, o `create`, o `link` e o `mkdir` desfazem o que já tinham gravado.
- **Diretórios não encolhem.** Um bloco que fica vazio continua sendo do diretório até o `rmdir`.
- **`_reescreve_entrada` é atômico.** Uma entrada de 32 bytes fica inteira dentro de um bloco, e o bloco é gravado de uma vez. É isso que permite ao `rename` substituir o destino sem que ele desapareça por um instante.

## Limitações

- O `rename` entre diretórios diferentes grava o nome novo antes de apagar o antigo. Se o programa parar entre as duas gravações, ficam dois nomes com `nlink` 1. O ext2 tem o mesmo problema, e só o journaling resolve.
- Sem `open` e `release`, o vsfs não sabe se algum processo ainda está com o arquivo aberto. Os blocos são liberados na hora do `unlink`.
- Ainda não há `truncate`, `chown`, `utimens` nem links simbólicos.

## Como rodar

A instalação do Python, do FUSE e do `mfusepy` no Linux, no Windows (WSL 2) e no macOS está no [README geral](../README.md#instalação).

```bash
python3 mkfs.py disco.img --force
python3 vsfs.py disco.img /tmp/mnt -f

# em outro terminal
echo "olá" > /tmp/mnt/a.txt
mkdir -p /tmp/mnt/docs/sub              # mkdir
ln /tmp/mnt/a.txt /tmp/mnt/docs/b.txt   # link: ls -li mostra o mesmo inode e nlink 2
mv /tmp/mnt/docs/b.txt /tmp/mnt/c.txt   # rename entre diretórios
chmod 600 /tmp/mnt/c.txt                # chmod
df -h /tmp/mnt                          # statfs
rm /tmp/mnt/a.txt                       # unlink: c.txt continua com o conteúdo
rmdir /tmp/mnt/docs                     # ENOTEMPTY: ainda tem sub/
rmdir /tmp/mnt/docs/sub /tmp/mnt/docs   # rmdir

umount /tmp/mnt
python3 dumpfs.py disco.img   # mostra a árvore e deve terminar com "nenhum problema"
```
