# FrotaGesta

Sistema desktop de controle de frota e manutenção preventiva, desenvolvido para a disciplina de **Laboratório de Banco de Dados** — Universidade Federal de Mato Grosso, Campus Araguaia.

- **Professor:** Prof. Dr. Anthony Ferreira La Marca
- **Alunos:** Raissa Caroline Cavalcanti da Silva e Luis Henrique Avlis Ferraz
- **Local/Data:** Barra do Garças – MT, julho de 2026

## Sobre o projeto

Uma transportadora local possui uma frota de caminhões, vans e carros e precisa controlar o uso dos veículos, registrar viagens e ser alertada automaticamente sobre manutenções preventivas (troca de óleo, pastilhas etc.) antes que os veículos quebrem em rota. O FrotaGesta resolve esse problema com um banco de dados PostgreSQL que concentra as regras de negócio críticas (restrições, *triggers* e um procedimento armazenado) e uma interface desktop em Python para uso no setor operacional e na oficina.

Documentação completa disponível em:
- `relatorio_academico.pdf` — relatório em formato ABNT (contexto, objetivos, metodologia e resultados).
- `relatorio_tecnico.pdf` — documentação técnica detalhada de todo o código SQL e Python, com análise do DER e evidências de execução.

## Estrutura do projeto

```
frotagesta/
├── master.sql                  # executa todos os scripts sql/*.sql em ordem
├── sql/
│   ├── 00_criar_banco.sql       # cria o banco de dados
│   ├── 01_tabelas.sql           # cria as 5 tabelas com constraints e FKs
│   ├── 02_indices.sql           # índices sobre FKs e colunas de status
│   ├── 03a_trigger_cnh.sql      # trigger de restrição de CNH x tipo de veículo
│   ├── 03b_trigger_manutencao.sql # trigger de geração automática de alertas
│   ├── 04_procedure.sql         # procedure sp_encerrar_viagem (transação atômica)
│   ├── 05_views.sql             # views vw_painel_veiculos e vw_historico_viagens
│   ├── 06_seed.sql              # dados de teste iniciais
│   └── 07_testes.sql            # testes funcionais de todas as regras de negócio
├── diagrama.png                 # DER do banco de dados
└── app/
    ├── main.py                  # ponto de entrada da aplicação desktop
    ├── db/
    │   ├── conexao.py            # conexão psycopg2 com o PostgreSQL
    │   ├── motoristas.py         # CRUD de motoristas
    │   ├── veiculos.py           # CRUD de veículos + consulta ao painel
    │   └── viagens.py            # iniciar/encerrar viagens, alertas
    └── ui/
        ├── tela_dashboard.py      # painel do gerente (status da frota + alertas)
        ├── tela_motoristas.py     # cadastro de motoristas com validação de CPF
        ├── tela_veiculos.py       # cadastro de veículos com validação de placa
        ├── tela_operacao.py       # iniciar/concluir viagens
        ├── componentes.py         # widgets reutilizáveis (botões, tabelas, cores)
        └── validacoes.py          # validação de CPF e placa
```

> **Atenção:** garanta que os scripts `.sql` numerados estejam dentro de uma pasta `sql/` na raiz do projeto (ao lado do `master.sql`), pois é esse o caminho relativo que o `master.sql` espera (`\i sql/01_tabelas.sql` etc.).

## Requisitos

- PostgreSQL 16 (ou compatível)
- Python 3.12
- Bibliotecas Python: `customtkinter`, `psycopg2-binary`

## Como rodar (macOS)

### 1. Instalar o PostgreSQL

```bash
brew install postgresql@16
brew services start postgresql@16
```

### 2. Definir uma senha para o seu usuário (necessário em instalações recentes)

```bash
psql postgres
```
```sql
ALTER USER seu_usuario WITH PASSWORD 'sua_senha';
```

### 3. Criar o banco de dados

Ainda no `psql`:
```sql
CREATE DATABASE frotagesta ENCODING='UTF8' TEMPLATE=template0;
\q
```
> Se o seu sistema tiver o locale `pt_BR.UTF-8` instalado, pode usar o script `sql/00_criar_banco.sql`, que já inclui `LC_COLLATE`/`LC_CTYPE` em português.

### 4. Executar os scripts SQL

A partir da **raiz do projeto** (onde está o `master.sql`):
```bash
cd frotagesta
psql -d frotagesta -f master.sql
```

Para rodar os testes funcionais das regras de negócio:
```bash
psql -d frotagesta -f sql/07_testes.sql
```

### 5. Configurar a conexão da aplicação

Edite `app/db/conexao.py`:
```python
CONFIG = {
    "dbname":   "frotagesta",
    "user":     "seu_usuario",
    "password": "sua_senha",
    "host":     "localhost",
    "port":     5432,
}
```

### 6. Instalar dependências Python e rodar

```bash
cd app
python3 -m venv venv
source venv/bin/activate
pip3 install customtkinter psycopg2-binary
PYTHONPATH=. python3 main.py
```

## Como inspecionar o banco no DBeaver

1. Nova conexão → PostgreSQL
2. Host: `localhost` · Porta: `5432` · Database: `frotagesta`
3. Usuário e senha: os mesmos configurados em `conexao.py`
4. Teste a conexão e navegue pelas tabelas, views, *triggers* e pela *stored procedure* `sp_encerrar_viagem`.

## Principais regras de negócio implementadas

| Regra | Onde está implementada |
|---|---|
| Quilometragem final ≥ inicial | `CHECK constraint` em `viagens.km_final` |
| Motorista CNH B não dirige caminhão | Trigger `trg_validar_cnh_veiculo` (`BEFORE INSERT/UPDATE` em `viagens`) |
| Alerta automático de manutenção preventiva | Trigger `trg_verificar_manutencao` (`AFTER UPDATE` em `viagens`) |
| Encerramento de viagem (transação atômica) | Procedure `sp_encerrar_viagem` |
| Validação de CPF/placa antes do envio ao banco | `app/ui/validacoes.py` |

## Funcionalidades da interface

- **Dashboard:** contadores de veículos disponíveis, em rota e em manutenção urgente, tabela colorida por status e lista de alertas pendentes com opção de marcar como resolvido.
- **Motoristas:** cadastro com validação de CPF (dígitos verificadores) e categoria de CNH.
- **Veículos:** cadastro com validação de placa (padrão antigo e Mercosul).
- **Operação:** iniciar e concluir viagens, com tratamento amigável de erros retornados pelo banco (CNH inválida, quilometragem incoerente etc.).

## Solução de problemas comuns

- **`connection ... failed: fe_sendauth: no password supplied`** → defina uma senha para o usuário do PostgreSQL e preencha `user`/`password` em `conexao.py`.
- **`connection to server on socket ".../.s.PGSQL.5432" failed`** → o serviço do PostgreSQL não está rodando; use `brew services start postgresql@16`.
- **Erro ao executar `master.sql`** → confirme que você está executando o `psql` a partir da raiz do projeto, e que os scripts numerados estão dentro da pasta `sql/`.
