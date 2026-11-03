# Manipulação do DOM: tabela de montanhas

**Página que gera uma tabela HTML a partir de um array de objetos JavaScript e acrescenta novas linhas com os dados de um formulário.**

[![JavaScript](https://img.shields.io/badge/JavaScript-ES6-F7DF1E?logo=javascript&logoColor=black)](https://developer.mozilla.org/pt-BR/docs/Web/JavaScript)
[![HTML5](https://img.shields.io/badge/HTML5-E34F26?logo=html5&logoColor=white)](https://developer.mozilla.org/pt-BR/docs/Web/HTML)
[![CSS3](https://img.shields.io/badge/CSS3-1572B6?logo=css3&logoColor=white)](https://developer.mozilla.org/pt-BR/docs/Web/CSS)
[![Disciplina](https://img.shields.io/badge/disciplina-Programa%C3%A7%C3%A3o_Web-6f42c1)](../../)

Atividade de Programação Web (UFMT) para praticar a criação de elementos pelo DOM e o tratamento de eventos.

---

## O que a página faz

- **Tabela dinâmica:** cada montanha (nome, altura e local) vira uma linha, e os cabeçalhos (`<th>`) são gerados a partir das propriedades dos objetos.
- **Formulário:** campos de nome, altura e local e um botão que acrescenta uma linha com os valores digitados.
- **Eventos:** o clique do botão é tratado com `addEventListener`.

## Como executar

Abra o [`index.html`](index.html) no navegador.

## Estrutura do projeto

```text
mainupulacaoDOM/
├── index.html   # estrutura da página e formulário
├── script.js    # dados, geração da tabela e evento do botão
└── style.css    # estilos
```

---

[← Programação Web](../../) · [Repositório principal](../../../)
