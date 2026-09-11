# Sistema de Gerenciamento de Biblioteca Digital

> Banco de Dados Relacional para gerenciamento completo de acervo, usuários, empréstimos, reservas e multas de uma biblioteca digital. Desenvolvido como projeto acadêmico de Banco de Dados.

![MySQL](https://img.shields.io/badge/MySQL-8.0%2B-blue?logo=mysql&logoColor=white)
![Status](https://img.shields.io/badge/status-concluído-brightgreen)
![Normalização](https://img.shields.io/badge/normalização-3FN-orange)
![Consultas](https://img.shields.io/badge/consultas%20SQL-10-purple)

**Autores:** Raissa Caroline Cavalcanti da Silva e Gabriel Nunes

---

## Índice

- [Sobre o Projeto](#sobre-o-projeto)
- [Modelo de Dados](#modelo-de-dados)
- [Tecnologias](#tecnologias)
- [Estrutura do Repositório](#estrutura-do-repositório)
- [Como Executar](#como-executar)
- [Consultas Implementadas](#consultas-implementadas)
- [Resultados Obtidos](#resultados-obtidos)
- [Regras de Negócio](#regras-de-negócio)
- [Normalização](#normalização)

---

## Sobre o Projeto

Este projeto implementa um **Banco de Dados Relacional** para gerenciar as operações de uma Biblioteca Digital. O sistema cobre o ciclo completo de uma biblioteca: cadastro do acervo físico, controle de usuários, empréstimos com prazos, sistema de reservas com fila de espera FIFO, geração automática de multas por atraso e 10 consultas analíticas.

O esquema foi projetado na **Terceira Forma Normal (3FN)**, garantindo integridade referencial e eliminação de redundâncias.

---

## Modelo de Dados

### Entidades

| Tabela | Descrição |
|---|---|
| `Editora` | Editoras cadastradas no sistema |
| `Livro` | Títulos do acervo identificados por ISBN |
| `Autor` | Autores dos livros |
| `Livro_Autor` | Relacionamento N:M entre Livro e Autor |
| `Exemplar` | Cada item físico de um título (tombamento, status) |
| `Usuario` | Alunos e professores cadastrados |
| `Emprestimo` | Transações de retirada e devolução |
| `Reserva` | Fila de espera para exemplares indisponíveis |
| `Multa` | Penalidades financeiras por atraso |

### Cardinalidades

```
Editora    1 ──── N    Livro
Livro      N ──── M    Autor       (via Livro_Autor)
Livro      1 ──── N    Exemplar
Exemplar   1 ──── N    Emprestimo
Exemplar   1 ──── N    Reserva
Usuario    1 ──── N    Emprestimo
Usuario    1 ──── N    Reserva
Emprestimo 1 ──── N    Multa
```

O DER completo está disponível no arquivo [`DER_v2.html`](./DER_v2.html) — abra no navegador para visualização interativa.

---

## Tecnologias

- **MySQL 8.0+** — SGBD relacional
- **MySQL Workbench** — IDE de modelagem e execução
- **SQL** — DDL, DML, DQL, Views, Constraints e Índices

---

## Estrutura do Repositório

```
 biblioteca-digital/
├── 📄 DDL.sql                  # CREATE TABLE, constraints e índices
├── 📄 DML.sql                  # INSERT INTO com dados de teste
├── 📄 Consultas.sql            # RC001 a RC010 — queries analíticas
├── 📊 DER_v2.html              # Diagrama Entidade-Relacionamento interativo
├── 📋 Esquema_Logico.html      # Esquema relacional lógico detalhado
├── 📊 resultados_1.csv         # RC001 — Livros em atraso
├── 📊 resultados_2.csv         # RC002 — Análise de acervo por editora
├── 📊 resultados_3.csv         # RC003 v1 — Títulos disponíveis
├── 📊 resultados_4.csv         # RC003 v2 — Títulos disponíveis (NOT EXISTS)
├── 📊 resultados_5.csv         # RC005 — View V_EmprestimosAtivos
├── 📊 resultados_6.csv         # RC006 — Álgebra relacional (Ano > 2020)
├── 📊 resultados_7.csv         # RC007 — Ranking últimos 6 meses
├── 📊 resultados_8.csv         # RC007 — Ranking últimos 6 meses (execução alternativa)
├── 📊 resultados_9.csv         # RC008 — Relatório de inadimplência
├── 📊 resultados_10.csv        # RC009 — Reservas pendentes
├── 📊 resultados_11.csv        # RC010 — Próximo na fila
├── 📕 relatório-código.pdf     # Explicação detalhada do código (DDL, DML e consultas)
└── 📖 README.md                # Este arquivo
```

---

## Como Executar

### Pré-requisitos

- MySQL 8.0 ou superior
- MySQL Workbench

### Instalação no macOS (via Homebrew)

```bash
brew install mysql
brew services start mysql
mysql_secure_installation
```

### Passo a passo

Execute os scripts **nesta ordem** no MySQL Workbench (`Cmd+Shift+Enter` para rodar o arquivo inteiro):

```
1. DDL.sql        →  Cria o banco e todas as tabelas
2. DML.sql        →  Popula com dados de teste
3. Consultas.sql  →  Executa as queries (Cmd+Enter para rodar uma por vez)
```

### Verificação rápida após execução

```sql
USE biblioteca_digital;
SHOW TABLES;

SELECT COUNT(*) AS livros    FROM Livro;       -- esperado: 10
SELECT COUNT(*) AS usuarios  FROM Usuario;     -- esperado: 10
SELECT COUNT(*) AS exemplares FROM Exemplar;  -- esperado: 17
SELECT COUNT(*) AS emprestimos FROM Emprestimo; -- esperado: 13
```

---

## Consultas Implementadas

| ID | Nome | Descrição | Resultado |
|---|---|---|---|
| RC001 | Livros em Atraso | Usuário, título e dias de atraso para empréstimos vencidos | 6 registros |
| RC002 | Análise de Acervo | Editoras ordenadas por número de títulos | 8 editoras |
| RC003 | Disponibilidade | Títulos sem nenhum exemplar emprestado no momento | 7 títulos |
| RC004 | Transações por Usuário | Histórico de um usuário com status de cada empréstimo | Paramétrico |
| RC005 | View Empréstimos Ativos | `CREATE VIEW V_EmprestimosAtivos` com coluna `situacao` | 6 registros |
| RC006 | Álgebra Relacional | Tradução de `ΠNome,Titulo(σAno>2020(Usuário ⋈ Empréstimo ⋈ Livro))` | 4 registros |
| RC007 | Ranking de Livros | Títulos mais emprestados nos últimos 6 meses | Dinâmico |
| RC008 | Inadimplência | Total de multas acumuladas por usuário (pagas + pendentes) | 6 usuários |
| RC009 | Reservas Pendentes | Títulos com fila ativa e quantidade de pessoas esperando | 3 títulos |
| RC010 | Próximo na Fila | Primeiro usuário da fila FIFO para um ISBN específico | 1 registro |

---

## Resultados Obtidos

### RC001 — Livros em Atraso
6 empréstimos identificados como vencidos. O maior atraso pertence a **Diego Rafael Souza** com **434 dias** no título *MySQL: Guia do Desenvolvedor* (TOMB-0008), emprestado desde 01/12/2024.

| Usuário | Título | Atraso |
|---|---|---|
| Diego Rafael Souza | MySQL: Guia do Desenvolvedor | 434 dias |
| Ana Paula Ferreira | Sistemas de Banco de Dados | 399 dias |
| Bruno Henrique Costa | Introdução à Álgebra Relacional | 394 dias |
| Carla Dias Mendonça | Learning SQL | 389 dias |
| Isabela Cristina Nunes | SQL e Modelagem de Dados | 384 dias |
| João Pedro Matos | Python for Data Analysis | 372 dias |

### RC002 — Análise de Acervo
O'Reilly Media e Bookman lideram com 2 títulos cada. As demais 6 editoras possuem 1 título cada no acervo.

### RC003 — Disponibilidade
7 títulos com pelo menos um exemplar disponível para empréstimo imediato, incluindo *Introduction to Algorithms* (4ª ed., 2022) e *Engenharia de Software* (8ª ed.).

### RC005 — View V_EmprestimosAtivos
A view retorna 6 empréstimos ativos, todos classificados como **"Em Atraso"**, evidenciando que não há empréstimos ativos dentro do prazo nos dados de teste.

### RC006 — Álgebra Relacional (Ano > 2020)
4 combinações usuário–livro resultantes da junção tripla com filtro `ano_publicacao > 2020`: Diego, Isabela, João Pedro e Prof. Fernando — todos com livros publicados após 2020.

### RC008 — Relatório de Inadimplência
Total de **R$ 190,00** em multas pendentes no sistema. Diego Rafael Souza acumula o maior valor: **R$ 72,00**.

| Usuário | Total Acumulado | Pendente |
|---|---|---|
| Diego Rafael Souza | R$ 72,00 | R$ 72,00 |
| Ana Paula Ferreira | R$ 37,00 | R$ 37,00 |
| Bruno Henrique Costa | R$ 32,00 | R$ 32,00 |
| Carla Dias Mendonça | R$ 27,00 | R$ 27,00 |
| Isabela Cristina Nunes | R$ 22,00 | R$ 22,00 |
| Prof. Fernando Alves | R$ 0,00 | R$ 0,00 |

### RC009 — Reservas Pendentes
3 títulos com fila ativa. *Sistemas de Banco de Dados* possui a maior fila: **2 pessoas** aguardando.

### RC010 — Próximo na Fila
Para o ISBN `978-8543004792` (*Sistemas de Banco de Dados*), a próxima usuária é **Elisa Tavares Lima** (ALU-2024-005), com reserva feita em 20/01/2025 às 09:00 — a mais antiga na fila.

---

## Regras de Negócio

| ID | Regra | Implementação |
|---|---|---|
| RN001 | ISBN único por livro | `PRIMARY KEY (isbn)` |
| RN002 | Integridade referencial | `FOREIGN KEY` com `ON DELETE RESTRICT` |
| RN003 | Campos obrigatórios | `NOT NULL` em titulo, nome, numero_identificacao |
| RN004 | Livro–Autor N:M | Tabela `Livro_Autor` com PK composta |
| RN005 | 3ª Forma Normal | Ausência de dependências transitivas |
| RN006 | Data devolução > data empréstimo | `CHECK (data_prevista_devolucao > data_emprestimo)` |
| RN101 | Entidade Reserva com status | `ENUM('Ativa','Cancelada','Atendida')` |
| RN102 | Fila FIFO | `ORDER BY data_reserva ASC` nas consultas |
| RN103 | Controle por exemplar | Entidade `Exemplar` com `id_exemplar` único |

---

## Normalização

**1FN** — Todos os atributos são atômicos. O relacionamento N:M Livro↔Autor foi decomposto na tabela associativa `Livro_Autor`.

**2FN** — Todos os atributos não-chave dependem funcionalmente da chave primária completa. Em `Livro_Autor`, não há atributos não-chave; em `Emprestimo`, tudo depende de `id_emprestimo`.

**3FN** — Não existem dependências transitivas. A localização da editora foi isolada na tabela `Editora` (não fica em `Livro`). Status e tipo do usuário dependem exclusivamente de `id_usuario`.
