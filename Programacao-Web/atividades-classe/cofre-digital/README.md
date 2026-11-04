# Cofre Digital: estado privado com módulo e closures

**Módulo JavaScript que guarda saldo e senha num estado inacessível de fora, exposto só por uma API pública com depósito, saque, consulta de saldo e troca de senha.**

[![JavaScript](https://img.shields.io/badge/JavaScript-ES6-F7DF1E?logo=javascript&logoColor=black)](https://developer.mozilla.org/pt-BR/docs/Web/JavaScript)
[![Disciplina](https://img.shields.io/badge/disciplina-Programa%C3%A7%C3%A3o_Web-6f42c1)](../../)

Atividade de Programação Web (UFMT). O objetivo é aplicar o **padrão de módulo**: uma função executada imediatamente (IIFE) cria variáveis locais, e os métodos retornados as acessam por *closure*. Como `saldo` e `senha` não são propriedades do objeto, não há como lê-los ou alterá-los diretamente.

---

## Como funciona

| Método | Regra |
|---|---|
| `depositar(valor)` | Aceita só números positivos; não exige senha |
| `sacar(senha, valor)` | Confere a senha, o valor e o saldo disponível |
| `consultarSaldo(senha)` | Confere a senha e mostra o saldo |
| `alterarSenha(antiga, nova)` | Confere a senha atual antes de trocar |

Três senhas erradas seguidas **bloqueiam o cofre**: a partir daí, nenhuma operação é aceita, nem com a senha correta.

## Como executar

O [`teste_cofre.js`](teste_cofre.js) usa o objeto `meuCofre` criado em [`cofre.js`](cofre.js). No Node.js, execute os dois em sequência:

```bash
cat cofre.js teste_cofre.js | node
```

O teste provoca o bloqueio de propósito, com três senhas erradas, e mostra que as operações seguintes são recusadas:

```text
Saldo após depósito: Senha incorreta.
Saldo após depósito: Senha incorreta.
Saldo após depósito: Cofre bloqueado após 3 tentativas falhas.
Saldo após depósito: Cofre bloqueado. Acesso negado.
```

---

[← Programação Web](../../) · [Repositório principal](../../../)
