"""
FrotaGesta — db/conexao.py
Gerencia a conexão com o PostgreSQL via psycopg2.
"""

import psycopg2
from psycopg2.extras import RealDictCursor

# Ajuste as credenciais se necessário
CONFIG = {
    "dbname":   "frotagesta",
    "user":     "raissacavalcanti",          # deixe vazio para usar o usuário do sistema (Mac/Homebrew)
    "password": "bancodados",
    "host":     "localhost",
    "port":     5432,
}


def get_conn():
    """Retorna uma nova conexão com o banco."""
    return psycopg2.connect(**CONFIG)


def executar(query: str, params=None, fetch=False):
    """
    Executa uma query e opcionalmente retorna os resultados.
    - fetch=False  → INSERT / UPDATE / DELETE / CALL
    - fetch=True   → SELECT (retorna lista de dicts)
    """
    with get_conn() as conn:
        with conn.cursor(cursor_factory=RealDictCursor) as cur:
            cur.execute(query, params)
            if fetch:
                return [dict(r) for r in cur.fetchall()]
    return []


def chamar_procedure(nome: str, params: tuple):
    """Chama uma stored procedure via CALL."""
    placeholders = ", ".join(["%s"] * len(params))
    with get_conn() as conn:
        with conn.cursor() as cur:
            cur.execute(f"CALL {nome}({placeholders})", params)
