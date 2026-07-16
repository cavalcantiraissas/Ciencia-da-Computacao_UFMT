-- =============================================================================
-- FrotaGesta | 05_views.sql
-- Views auxiliares usadas diretamente pela interface desktop.
-- Pré-requisito: 01_tabelas.sql
-- =============================================================================

-- -----------------------------------------------------------------------------
-- vw_painel_veiculos
-- Usada pelo Dashboard do Gerente.
-- Retorna veículos ordenados por prioridade de status, com contagem de alertas
-- pendentes e lista das manutenções necessárias.
-- -----------------------------------------------------------------------------
CREATE OR REPLACE VIEW vw_painel_veiculos AS
SELECT
    v.id,
    v.placa,
    v.modelo,
    v.tipo,
    v.km_atual,
    v.status,
    COUNT(a.id)         FILTER (WHERE a.status = 'Pendente') AS alertas_pendentes,
    STRING_AGG(a.tipo_manutencao, ', ')
                        FILTER (WHERE a.status = 'Pendente') AS manutencoes_pendentes
FROM  veiculos v
LEFT  JOIN alertas_manutencao a ON a.veiculo_id = v.id
GROUP BY v.id, v.placa, v.modelo, v.tipo, v.km_atual, v.status
ORDER BY
    CASE v.status
        WHEN 'Manutenção Urgente' THEN 1
        WHEN 'Em rota'            THEN 2
        WHEN 'Em manutenção'      THEN 3
        ELSE                           4
    END,
    v.placa;

-- -----------------------------------------------------------------------------
-- vw_historico_viagens
-- Usada pela tela de histórico de viagens.
-- JOIN com motoristas e veículos; calcula km_percorrida.
-- Ordenada por dt_saida DESC (mais recentes primeiro).
-- -----------------------------------------------------------------------------
CREATE OR REPLACE VIEW vw_historico_viagens AS
SELECT
    vi.id,
    vi.status,
    m.nome             AS motorista,
    m.categoria_cnh,
    ve.placa,
    ve.modelo,
    ve.tipo,
    vi.dt_saida,
    vi.dt_chegada,
    vi.km_inicial,
    vi.km_final,
    (vi.km_final - vi.km_inicial) AS km_percorrida,
    vi.observacoes
FROM  viagens vi
JOIN  motoristas m  ON m.id  = vi.motorista_id
JOIN  veiculos   ve ON ve.id = vi.veiculo_id
ORDER BY vi.dt_saida DESC;
