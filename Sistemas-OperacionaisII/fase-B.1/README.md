# Fase B.1: montar, ler e criar

Primeira fase do vsfs montado com FUSE. O sistema de arquivos passa a responder ao kernel: lista diretórios, lê arquivos e cria arquivos novos, ainda vazios.

## Operações

| Operação | Chamada de sistema | O que faz |
|---|---|---|
| `getattr` | `stat` | Resolve o caminho até o inode e devolve os metadados dele |
| `readdir` | `ls` | Lista as entradas vivas do diretório, `.` e `..` incluídos, porque no vsfs elas ficam gravadas no disco |
| `read` | `read` | Traduz um deslocamento em bytes em ponteiro direto e posição dentro do bloco |
| `create` | `open(O_CREAT)` | Aloca um inode, grava-o e só então grava o nome no diretório pai |

## Decisões

- **Travessia própria.** O mfusepy entrega o caminho inteiro em cada chamada. `_resolve` parte da raiz e procura cada componente no diretório atual, o mesmo percurso que o `dumpfs` faz.
- **Ordem das gravações no `create`.** O inode é gravado antes da entrada no diretório. Se a gravação parar no meio, sobra um inode sem nome, que o fsck detecta, e nunca um nome apontando para um inode que não existe.
- **Arquivo nasce sem blocos.** `create` não aloca blocos de dados: o arquivo começa com `size` 0 e todos os ponteiros zerados.
- **Sem `open`.** Sem esse método, a libfuse considera o arquivo aberto, e não há estado a guardar entre uma leitura e outra.

## Limitações

- Não grava conteúdo (vem na [fase B.2](../fase-B.2)).
- Não apaga arquivos nem cria diretórios.

## Como rodar

```bash
python3 mkfs.py disco.img
python3 vsfs.py disco.img /tmp/mnt -f
```
