# Gestão de Inventário com módulos ES6

**Sistema de inventário refatorado de um único arquivo para módulos JavaScript com `import` e `export`, separando dados, funções e ponto de entrada.**

[![JavaScript](https://img.shields.io/badge/JavaScript-ES6-F7DF1E?logo=javascript&logoColor=black)](https://developer.mozilla.org/pt-BR/docs/Web/JavaScript)
[![Disciplina](https://img.shields.io/badge/disciplina-Programa%C3%A7%C3%A3o_Web-6f42c1)](../../)

Atividade de Programação Web (UFMT). O objetivo é organizar o código por responsabilidade, tornando-o mais fácil de manter e reutilizar.

---

## Estrutura do projeto

```mermaid
flowchart LR
    D[dados.js<br/>inventarioLoja] --> M[main.js]
    F[funcoes.js<br/>exibirProduto<br/>listarProdutosEmEstoque<br/>calcularValorTotalInventario] --> M
    M --> H[index.html<br/>script type=module]
```

| Arquivo | Responsabilidade |
|---|---|
| [`dados.js`](dados.js) | Exporta a lista de produtos da loja |
| [`funcoes.js`](funcoes.js) | Exporta as funções que exibem um produto, listam os itens em estoque e somam o valor total |
| [`main.js`](main.js) | Importa dados e funções e executa o relatório |
| [`index.html`](index.html) | Carrega o `main.js` como módulo |

## Como executar

Navegadores bloqueiam módulos ES6 abertos direto do disco (`file://`). Sirva a pasta com um servidor local e abra o console do navegador:

```bash
python3 -m http.server 8000
# abra http://localhost:8000
```

---

[← Programação Web](../../) · [Repositório principal](../../../)
