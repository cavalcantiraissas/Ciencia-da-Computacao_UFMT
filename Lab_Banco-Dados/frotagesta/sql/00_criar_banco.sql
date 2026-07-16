-- =============================================================================
-- FrotaGesta | 00_criar_banco.sql
-- Executar conectado ao postgres (superuser), ANTES dos demais scripts.
-- =============================================================================

CREATE DATABASE frotagesta
    ENCODING   = 'UTF8'
    LC_COLLATE = 'pt_BR.UTF-8'
    LC_CTYPE   = 'pt_BR.UTF-8'
    TEMPLATE   = template0;

-- Após criar o banco, conecte-se a ele:
-- \c frotagesta
-- e então execute os demais scripts na ordem numérica.
