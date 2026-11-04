# Programação Web

**Desenvolvimento web do front-end ao back-end: HTML, CSS, JavaScript moderno (módulos, closures, DOM e requisições assíncronas) e uma aplicação Flask com SQLite.**

[![HTML5](https://img.shields.io/badge/HTML5-E34F26?logo=html5&logoColor=white)](https://developer.mozilla.org/pt-BR/docs/Web/HTML)
[![CSS3](https://img.shields.io/badge/CSS3-1572B6?logo=css3&logoColor=white)](https://developer.mozilla.org/pt-BR/docs/Web/CSS)
[![JavaScript](https://img.shields.io/badge/JavaScript-ES6-F7DF1E?logo=javascript&logoColor=black)](https://developer.mozilla.org/pt-BR/docs/Web/JavaScript)
[![Flask](https://img.shields.io/badge/Flask-SQLite-000000?logo=flask&logoColor=white)](https://flask.palletsprojects.com/)
[![UFMT](https://img.shields.io/badge/UFMT-Ci%C3%AAncia_da_Computa%C3%A7%C3%A3o-003366)](../)

Atividades da disciplina de Programação IV (Programação Web) do curso de Ciência da Computação da UFMT. O foco é o desenvolvimento *full stack*: interfaces interativas no navegador integradas a uma lógica de servidor com persistência.

| | |
|---|---|
| **Docente** | Sandino Barros Jardim |
| **Período** | 2025/1 |
| **Carga horária** | 64 horas |

---

## Atividades

| Atividade | Descrição | Tecnologias |
|---|---|---|
| [Cofre Digital](./atividades-classe/cofre-digital) | Saldo e senha privados com padrão de módulo (IIFE) e *closures*; bloqueio após três senhas erradas | JavaScript |
| [Gestão de Inventário](./atividades-classe/gestao-de-inventario) | Refatoração em módulos ES6 (`import`/`export`) | JavaScript |
| [Manipulação do DOM](./atividades-classe/mainupulacaoDOM) | Tabela gerada a partir de um array e novas linhas por formulário | HTML, CSS, JavaScript |
| [Requisições assíncronas](./atividades-classe/requisicoes-assicronas) | Busca de universidades por país numa API pública com `fetch` e `async`/`await` | JavaScript, API REST |
| [Página Flask](./atividades-classe/Pagina-Flask) | Cadastro e listagem de produtos (nome, marca e valor) com Flask e SQLite | Python, Flask, SQLite, Jinja2 |
| [Primeira folha de estilos](./atividades-classe/primeira-folha-estilos) | Primeira página estilizada com CSS externo | HTML, CSS |
| [`formulario_evento.html`](./atividades-classe/formulario_evento.html) | Formulário de inscrição num evento da UFMT | HTML |

### Como executar a Página Flask

```bash
cd atividades-classe/Pagina-Flask
pip install flask
python3 app.py      # cria produtos.db na primeira execução; abra http://127.0.0.1:5000
```

As páginas ficam em `templates/` (`base.html` e as três que a estendem). A página inicial mostra o total de produtos cadastrados, calculado na rota `/` do `app.py`.

## Conteúdo da disciplina

- Arquitetura web: requisições HTTP, servidores e clientes
- Front-end: HTML5, CSS3 e JavaScript
- Back-end: programação orientada a objetos com PHP
- Versionamento com Git e fluxos de trabalho colaborativos
- Testes automatizados e garantia de qualidade
- Segurança em aplicações web

## Bibliografia

- DALL'OGLIO, Pablo. *PHP: Programando com Orientação a Objetos*.
- NIEDERAUER, Juliano. *Desenvolvendo Websites com PHP*.
- BIBEAULT, Bear; KATZ, Yehuda. *jQuery in Action*.

---

[← Repositório principal](../)
