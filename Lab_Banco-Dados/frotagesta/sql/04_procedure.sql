-- =============================================================================
-- FrotaGesta | 04_procedure.sql
-- Stored procedure sp_encerrar_viagem.
-- Encapsula em uma única transação atômica:
--   1. Busca e trava a viagem (FOR UPDATE)
--   2. Valida status, km_final não nulo e coerência do odômetro
--   3. Atualiza viagem → status Concluída (dispara 03b_trigger_manutencao)
--   4. Atualiza odômetro e devolve veículo ao status Disponível
--   5. Avança km_proxima dos planos que foram ultrapassados
-- Pré-requisito: 01_tabelas.sql, 03b_trigger_manutencao.sql
-- =============================================================================

CREATE OR REPLACE PROCEDURE sp_encerrar_viagem(
    p_viagem_id   INTEGER,
    p_km_final    NUMERIC(10,1),
    p_observacoes TEXT DEFAULT NULL
)
LANGUAGE plpgsql AS $$
DECLARE
    v_viagem viagens%ROWTYPE;
BEGIN
    -- -------------------------------------------------------------------------
    -- 1. Busca e trava o registro (evita race condition em acessos concorrentes)
    -- -------------------------------------------------------------------------
    SELECT * INTO v_viagem
    FROM   viagens
    WHERE  id = p_viagem_id
    FOR UPDATE;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'Viagem % não encontrada.', p_viagem_id;
    END IF;

    -- -------------------------------------------------------------------------
    -- 2. Validações de negócio
    -- -------------------------------------------------------------------------
    IF v_viagem.status <> 'Em andamento' THEN
        RAISE EXCEPTION
            'Viagem % já está com status "%" e não pode ser encerrada.',
            p_viagem_id, v_viagem.status;
    END IF;

    IF p_km_final IS NULL THEN
        RAISE EXCEPTION 'A quilometragem final não pode ser nula.';
    END IF;

    IF p_km_final < v_viagem.km_inicial THEN
        RAISE EXCEPTION
            'Quilometragem final (% km) não pode ser menor que a inicial (% km).',
            p_km_final, v_viagem.km_inicial;
    END IF;

    -- -------------------------------------------------------------------------
    -- 3. Atualiza odômetro e devolve veículo ao status Disponível
    --    ANTES de encerrar a viagem, para que a trigger leia km_atual correto
    --    (a trigger pode sobrescrever para "Manutenção Urgente" em seguida)
    -- -------------------------------------------------------------------------
    UPDATE veiculos
    SET
        km_atual = p_km_final,
        status   = 'Disponível'
    WHERE id = v_viagem.veiculo_id;

    -- -------------------------------------------------------------------------
    -- 4. Atualiza a viagem → dispara trg_verificar_manutencao (AFTER UPDATE)
    --    Neste ponto km_atual já está correto no veículo
    -- -------------------------------------------------------------------------
    UPDATE viagens
    SET
        status      = 'Concluída',
        km_final    = p_km_final,
        dt_chegada  = NOW(),
        observacoes = COALESCE(p_observacoes, observacoes)
    WHERE id = p_viagem_id;

    -- -------------------------------------------------------------------------
    -- 5. Avança km_proxima e ultima_km nos planos ultrapassados
    -- -------------------------------------------------------------------------
    UPDATE plano_manutencao
    SET
        ultima_km  = p_km_final,
        km_proxima = p_km_final + km_intervalo
    WHERE veiculo_id = v_viagem.veiculo_id
      AND p_km_final >= km_proxima;

    RAISE NOTICE 'Viagem % encerrada com sucesso. KM final: % km', p_viagem_id, p_km_final;
END;
$$;

-- Exemplo de uso:
-- CALL sp_encerrar_viagem(1, 49200.0, 'Entrega realizada sem intercorrências');
