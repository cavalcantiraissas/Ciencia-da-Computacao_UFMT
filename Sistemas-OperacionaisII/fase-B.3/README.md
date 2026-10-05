# Fase B.3: remoção

Acrescenta o `unlink` ao vsfs da [fase B.2](../fase-B.2). O `rm` passa a funcionar, e o inode e os blocos de um arquivo apagado voltam a ficar disponíveis para o alocador.

## O que mudou

| Arquivo | Mudança |
|---|---|
| `vsfs.py` | `unlink` e as funções auxiliares `_remove_entrada`, `_libera_blocos` e `_libera_inode` |
| `layout.py` | Só a docstring de `limpa_bit`, que agora tem quem a chame |

## Como o `unlink` funciona

1. Grava a lápide (`FREE_INUM`) por cima da entrada no diretório pai. O slot fica livre no lugar e é reaproveitado pelo próximo `create`.
2. Diminui o `nlink` do inode. Se ainda sobrar algum nome, só atualiza o `ctime` e termina.
3. Se não sobrar nome, zera o inode.
4. Desliga no bitmap de dados os bits dos blocos que o inode apontava.
5. Desliga o bit do inode no bitmap de inodes.

## Decisões

- **Ordem inversa à do `create` e do `write`.** Em qualquer ponto de parada sobra só espaço perdido (inode órfão ou blocos marcados sem dono), que o fsck detecta. Nunca sobra um ponteiro para algo já livre.
- **Inode zerado antes de liberar os blocos.** A ordem contrária deixaria um inode apontando para blocos que o alocador já pode entregar a outro arquivo, e dois arquivos passariam a dividir o mesmo bloco.
- **Inode zerado, e não só desmarcado.** O `dumpfs` considera corrupção um inode com conteúdo que o bitmap dá por livre.
- **Conteúdo não é apagado.** Liberar um bloco é só desligar o bit. É por isso que o `write` nunca lê um bloco recém-alocado.
- **Diretórios recusados.** `unlink` em diretório devolve `EISDIR`. O kernel já manda diretórios para o `rmdir`, mas a imagem não pode depender disso: apagar o nome de um diretório deixaria tudo dentro dele órfão.

## Limitações

- Sem `open` e `release`, não dá para saber se algum processo ainda está com o arquivo aberto. Os blocos são liberados na hora, e quem continuar usando o descritor recebe `ENOENT`, em vez de manter o arquivo vivo até o `close`, como no Unix.
- Ainda sem `truncate`, `mkdir`, `rmdir`, `link` e `rename`.

## Como rodar

```bash
python3 mkfs.py disco.img
python3 vsfs.py disco.img /tmp/mnt -f
echo "olá" > /tmp/mnt/a.txt && rm /tmp/mnt/a.txt
umount /tmp/mnt
python3 dumpfs.py disco.img   # bitmaps de volta ao estado inicial, "nenhum problema"
```
