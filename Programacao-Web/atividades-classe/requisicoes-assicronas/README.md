# Requisições assíncronas: busca de universidades por país

**Página que consulta uma API pública com `fetch` e `async`/`await` e mostra as universidades do país digitado, com link para o site de cada uma.**

[![JavaScript](https://img.shields.io/badge/JavaScript-ES6-F7DF1E?logo=javascript&logoColor=black)](https://developer.mozilla.org/pt-BR/docs/Web/JavaScript)
[![API](https://img.shields.io/badge/API-Universities_(Hipolabs)-555555)](http://universities.hipolabs.com/)
[![Disciplina](https://img.shields.io/badge/disciplina-Programa%C3%A7%C3%A3o_Web-6f42c1)](../../)

Atividade de Programação Web (UFMT) para aplicar JavaScript assíncrono a uma API real: a requisição parte de uma entrada do usuário e o resultado é inserido dinamicamente na página.

---

## Como funciona

```mermaid
sequenceDiagram
    participant U as Usuário
    participant P as app.js
    participant A as universities.hipolabs.com
    U->>P: digita o país e busca
    P->>A: fetch /search?country=...
    A-->>P: lista em JSON
    P->>U: cartões com nome e site de cada universidade
```

## Como executar

Abra o [`index.html`](index.html) no navegador, com acesso à internet.

A API responde só em `http://`. Se a página for servida por `https://`, o navegador pode bloquear a requisição por conteúdo misto.

## Estrutura do projeto

```text
requisicoes-assicronas/
├── index.html   # campo de busca e área de resultados
├── app.js       # requisição com fetch e montagem dos resultados
└── style.css    # estilos
```

---

[← Programação Web](../../) · [Repositório principal](../../../)
