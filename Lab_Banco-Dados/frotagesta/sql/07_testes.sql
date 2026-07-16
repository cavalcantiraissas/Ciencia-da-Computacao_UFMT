-- =============================================================================
-- FrotaGesta | 07_testes.sql
-- Testes funcionais para validar todas as regras de negócio.
-- Execute após o seed (06_seed.sql).
-- Cada bloco pode ser executado individualmente.
-- Resultados esperados estão documentados nos comentários.
-- =============================================================================

-- =============================================================================
-- TESTE 1 — Consultas de verificação do estado inicial
-- Esperado: 6 motoristas, 6 veículos, 10 planos, 6 viagens, 2 alertas
-- =============================================================================
SELECT 'motoristas'        AS tabela, COUNT(*) AS total FROM motoristas
UNION ALL
SELECT 'veiculos',                    COUNT(*)           FROM veiculos
UNION ALL
SELECT 'viagens',                     COUNT(*)           FROM viagens
UNION ALL
SELECT 'plano_manutencao',            COUNT(*)           FROM plano_manutencao
UNION ALL
SELECT 'alertas_manutencao',          COUNT(*)           FROM alertas_manutencao;

-- =============================================================================
-- TESTE 2 — Painel de veículos (view)
-- Esperado: JKL-3456 aparece primeiro (Manutenção Urgente),
--           ABC-1234 aparece segundo (Em rota),
--           demais como Disponível
-- =============================================================================
SELECT placa, modelo, tipo, km_atual, status, alertas_pendentes, manutencoes_pendentes
FROM   vw_painel_veiculos;

-- =============================================================================
-- TESTE 3 — Histórico de viagens (view)
-- Esperado: 6 linhas ordenadas por dt_saida DESC,
--           viagem em andamento com km_percorrida NULL
-- =============================================================================
SELECT id, status, motorista, placa, dt_saida, km_inicial, km_final, km_percorrida
FROM   vw_historico_viagens;

-- =============================================================================
-- TESTE 4 — Trigger de CNH: tentativa VÁLIDA
-- Carlos Eduardo (CNH D) + ABC-1234 (Caminhão) → deve ser inserida sem erro
-- =============================================================================
INSERT INTO viagens (motorista_id, veiculo_id, dt_saida, km_inicial, status)
VALUES (1, 2, NOW(), 9800.0, 'Em andamento');
-- Limpa o insert de teste
DELETE FROM viagens WHERE motorista_id = 1 AND veiculo_id = 2 AND status = 'Em andamento'
  AND dt_saida >= NOW() - INTERVAL '1 minute';

-- =============================================================================
-- TESTE 5 — Trigger de CNH: tentativa INVÁLIDA
-- Ana Paula (CNH B) + ABC-1234 (Caminhão) → deve lançar EXCEPTION
-- Esperado: ERROR: Motorista com CNH categoria B não está habilitado...
-- =============================================================================
DO $$
BEGIN
    INSERT INTO viagens (motorista_id, veiculo_id, dt_saida, km_inicial, status)
    VALUES (2, 1, NOW(), 48500.0, 'Em andamento');
    RAISE NOTICE 'FALHOU: insert deveria ter sido bloqueado pela trigger de CNH.';
EXCEPTION
    WHEN OTHERS THEN
        RAISE NOTICE 'PASSOU — Trigger CNH bloqueou corretamente: %', SQLERRM;
END;
$$;

-- =============================================================================
-- TESTE 6 — CHECK de odômetro: km_final < km_inicial
-- Esperado: ERROR: new row for relation "viagens" violates check constraint
-- =============================================================================
DO $$
BEGIN
    INSERT INTO viagens
        (motorista_id, veiculo_id, dt_saida, dt_chegada, km_inicial, km_final, status)
    VALUES (1, 2, NOW() - INTERVAL '1 hour', NOW(), 9800.0, 9000.0, 'Concluída');
    RAISE NOTICE 'FALHOU: insert deveria ter sido bloqueado pelo CHECK de km_final.';
EXCEPTION
    WHEN OTHERS THEN
        RAISE NOTICE 'PASSOU — CHECK km_final bloqueou corretamente: %', SQLERRM;
END;
$$;

-- =============================================================================
-- TESTE 7 — Stored procedure: encerramento válido de viagem
-- Viagem 6 (ABC-1234, em andamento) com km_final = 49200.0
-- Esperado:
--   - viagem 6 status = Concluída, km_final = 49200.0
--   - veículo 1 km_atual = 49200.0
--   - alerta de 'Troca de óleo' inserido (49200 >= 50000? NÃO — não deve gerar alerta)
--   - veículo 1 status = Disponível
-- =============================================================================
CALL sp_encerrar_viagem(6, 49200.0, 'Teste de encerramento válido');

SELECT id, status, km_final, dt_chegada FROM viagens  WHERE id = 6;
SELECT id, placa, km_atual, status       FROM veiculos WHERE id = 1;
SELECT *                                 FROM alertas_manutencao WHERE veiculo_id = 1;

-- =============================================================================
-- TESTE 8 — Stored procedure: encerramento que GERA ALERTA
-- Cria uma nova viagem para o veículo DEF-5678 (Van, plano troca de óleo em 10000 km)
-- e encerra com km_final = 10500 (ultrapassa limite de 10000)
-- Esperado: alerta inserido + veículo status = Manutenção Urgente
-- =============================================================================
INSERT INTO viagens (motorista_id, veiculo_id, dt_saida, km_inicial, status)
VALUES (2, 2, NOW() - INTERVAL '3 hours', 9800.0, 'Em andamento')
RETURNING id;  -- anote o id retornado e substitua abaixo se necessário

-- Substitua <ID> pelo id retornado acima se executar manualmente
-- CALL sp_encerrar_viagem(<ID>, 10500.0, 'Teste de geração de alerta');

-- Versão automatizada (captura o último id inserido na sessão):
DO $$
DECLARE v_id INTEGER;
BEGIN
    SELECT MAX(id) INTO v_id FROM viagens WHERE veiculo_id = 2 AND status = 'Em andamento';
    CALL sp_encerrar_viagem(v_id, 10500.0, 'Teste de geração de alerta automático');
    RAISE NOTICE 'Viagem % encerrada.', v_id;
END;
$$;

SELECT id, placa, km_atual, status FROM veiculos WHERE id = 2;
SELECT tipo_manutencao, km_alerta, status FROM alertas_manutencao WHERE veiculo_id = 2;

-- =============================================================================
-- TESTE 9 — Stored procedure: tentativa de encerrar viagem já concluída
-- Esperado: EXCEPTION 'Viagem % já está com status "Concluída"...'
-- =============================================================================
DO $$
BEGIN
    CALL sp_encerrar_viagem(6, 50000.0);
    RAISE NOTICE 'FALHOU: procedure deveria ter lançado exceção.';
EXCEPTION
    WHEN OTHERS THEN
        RAISE NOTICE 'PASSOU — Procedure bloqueou corretamente: %', SQLERRM;
END;
$$;

-- =============================================================================
-- TESTE 10 — Stored procedure: km_final menor que km_inicial
-- Esperado: EXCEPTION 'Quilometragem final ... não pode ser menor...'
-- =============================================================================
DO $$
DECLARE v_id INTEGER;
BEGIN
    -- Cria viagem de teste
    INSERT INTO viagens (motorista_id, veiculo_id, dt_saida, km_inicial, status)
    VALUES (2, 3, NOW() - INTERVAL '1 hour', 3200.0, 'Em andamento')
    RETURNING id INTO v_id;

    -- Tenta encerrar com km menor
    CALL sp_encerrar_viagem(v_id, 3000.0);
    RAISE NOTICE 'FALHOU: procedure deveria ter lançado exceção de km_final.';
EXCEPTION
    WHEN OTHERS THEN
        RAISE NOTICE 'PASSOU — Procedure bloqueou km incoerente: %', SQLERRM;
        -- Remove a viagem de teste que não foi encerrada
        DELETE FROM viagens WHERE id = v_id;
END;
$$;

-- =============================================================================
-- RESULTADO FINAL — resumo do estado das tabelas após os testes
-- =============================================================================
SELECT '-- ESTADO FINAL APÓS TESTES --' AS info;

SELECT placa, modelo, km_atual, status FROM veiculos ORDER BY id;

SELECT v.id, m.nome AS motorista, ve.placa, v.status, v.km_inicial, v.km_final
FROM   viagens v
JOIN   motoristas m  ON m.id  = v.motorista_id
JOIN   veiculos   ve ON ve.id = v.veiculo_id
ORDER  BY v.id;

SELECT am.id, ve.placa, am.tipo_manutencao, am.km_alerta, am.status
FROM   alertas_manutencao am
JOIN   veiculos ve ON ve.id = am.veiculo_id
ORDER  BY am.id;
