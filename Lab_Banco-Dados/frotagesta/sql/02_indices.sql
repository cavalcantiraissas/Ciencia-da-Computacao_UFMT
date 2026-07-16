-- =============================================================================
-- FrotaGesta | 02_indices.sql
-- Índices sobre FKs e colunas de status para acelerar filtros da interface.
-- Pré-requisito: 01_tabelas.sql
-- =============================================================================

CREATE INDEX IF NOT EXISTS idx_viagens_motorista ON viagens (motorista_id);
CREATE INDEX IF NOT EXISTS idx_viagens_veiculo   ON viagens (veiculo_id);
CREATE INDEX IF NOT EXISTS idx_viagens_status    ON viagens (status);

CREATE INDEX IF NOT EXISTS idx_alertas_veiculo   ON alertas_manutencao (veiculo_id);
CREATE INDEX IF NOT EXISTS idx_alertas_status    ON alertas_manutencao (status);

CREATE INDEX IF NOT EXISTS idx_plano_veiculo     ON plano_manutencao (veiculo_id);

-- Criação de índices sobre as chaves estrangeiras e sobre colunas de status
-- que são filtradas o tempo todo pela interace - por exemplo, toda vez que o 
-- Dashboard busca só os alertas "pendentes", ou a tela de Operação busca só viagens
-- "em andamento". Isso evita varredura sequencial na tabela toda conforme ela cresce