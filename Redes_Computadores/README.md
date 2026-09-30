# Redes de Computadores

Repositório de atividades e trabalhos desenvolvidos durante a disciplina de **Redes de Computadores**, do curso de Ciência da Computação.

---

## Informações Gerais

| | |
|---|---|
| Instituição | Universidade Federal de Mato Grosso (UFMT) |
| Linguagem | Python 3 |

---

## Conteúdo do repositório

| Arquivo | Descrição |
|---|---|
| [quem.py](quem.py) | Cliente WHOIS via socket TCP: consulta um domínio no servidor `whois.registro.br` (porta 43) e exibe a resposta |
| [servidor_eco_tcp.py](servidor_eco_tcp.py) | Servidor de eco TCP: aceita conexões e devolve ao cliente cada mensagem recebida, até receber `sair` |
| [cliente_eco_tcp.py](cliente_eco_tcp.py) | Cliente de eco TCP: envia mensagens digitadas ao servidor e exibe o eco retornado |

---

## Como executar

### Consulta WHOIS

```bash
python3 quem.py ufmt.br
```

### Eco TCP

Em um terminal, inicie o servidor (padrão `127.0.0.1:7000`):

```bash
python3 servidor_eco_tcp.py [host] [porta]
```

Em outro terminal, conecte o cliente:

```bash
python3 cliente_eco_tcp.py [host] [porta]
```

Digite `sair`, `exit` ou `quit` no cliente para encerrar a conexão. O servidor continua aguardando novos clientes até ser interrompido com `Ctrl+C`.

---

[Voltar ao repositório principal](https://github.com/cavalcantiraissas/Ciencia-da-Computacao_UFMT)
