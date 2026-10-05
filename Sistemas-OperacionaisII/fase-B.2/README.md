# Fase B.2: gravação

Acrescenta o `write` ao vsfs da [fase B.1](../fase-B.1). Os arquivos passam a ter conteúdo, e os blocos de dados são alocados sob demanda.

## O que mudou

Só o `vsfs.py`. Os demais arquivos são idênticos aos da fase anterior.

| Função | O que faz |
|---|---|
| `_aloca_bloco` | Liga o primeiro bit livre do bitmap de dados e devolve o número **absoluto** do bloco, que é o que vai para o `direct[]` |
| `write` | Grava `data` a partir de `offset`, alocando os blocos que ainda não existem, e atualiza `size`, `mtime` e `ctime` |

## Decisões

- **Mesmo laço do `read`.** O deslocamento é dividido pelo tamanho do bloco para achar o ponteiro, e o resto da divisão dá a posição dentro dele.
- **Bloco novo começa zerado na memória.** Ele não é lido do disco, porque o que estivesse lá pertenceria a um arquivo antigo.
- **Escrita parcial lê antes de gravar.** Quando a escrita não cobre o bloco inteiro, o bloco é lido para que o resto dele sobreviva.
- **Limite conferido antes do laço.** Escritas além de 12 × 4 KiB = 48 KiB dão `EFBIG` antes de alocar qualquer coisa, para não deixar blocos marcados que nenhum ponteiro aponta.
- **Dados antes do inode.** Os blocos são gravados antes do inode. Uma parada no meio deixa blocos marcados sem dono, que o fsck detecta, e nunca um ponteiro para um bloco com lixo.

## Limitações

- Sem ponteiro indireto: um arquivo tem no máximo 48 KiB.
- Não encolhe (`truncate`) nem apaga arquivos (vem na [fase B.3](../fase-B.3)).

## Como rodar

```bash
python3 mkfs.py disco.img
python3 vsfs.py disco.img /tmp/mnt -f
echo "olá" > /tmp/mnt/a.txt && cat /tmp/mnt/a.txt
```
