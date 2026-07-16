-- =============================================================================
-- FrotaGesta | 03b_trigger_manutencao.sql
-- Trigger AFTER UPDATE em viagens.
-- Ao encerrar uma viagem (status → Concluída), percorre todos os planos do
-- veículo e insere alertas para os que atingiram km_proxima.
-- Também atualiza status do veículo para "Manutenção Urgente" quando necessário.
-- Pré-requisito: 01_tabelas.sql, 04_procedure.sql (executa antes do trigger disparar)
-- =============================================================================

CREATE OR REPLACE FUNCTION fn_verificar_manutencao()
RETURNS TRIGGER AS $$
DECLARE
    r_plano      plano_manutencao%ROWTYPE;
    v_km_atual   NUMERIC(10,1);
    v_tem_alerta BOOLEAN := FALSE;
BEGIN
    -- Age somente quando a viagem transita para Concluída
    IF NEW.status <> 'Concluída' OR OLD.status = 'Concluída' THEN
        RETURN NEW;
    END IF;

    -- km_atual já foi gravado no veículo pelo stored procedure antes deste trigger
    SELECT km_atual INTO v_km_atual
    FROM   veiculos
    WHERE  id = NEW.veiculo_id;

    -- Percorre todos os planos do veículo
    FOR r_plano IN
        SELECT * FROM plano_manutencao
        WHERE  veiculo_id = NEW.veiculo_id
    LOOP
        IF v_km_atual >= r_plano.km_proxima THEN
            -- Evita alerta duplicado para o mesmo plano ainda pendente
            IF NOT EXISTS (
                SELECT 1 FROM alertas_manutencao
                WHERE  plano_manutencao_id = r_plano.id
                AND    status = 'Pendente'
            ) THEN
                INSERT INTO alertas_manutencao
                    (veiculo_id, plano_manutencao_id, tipo_manutencao, km_alerta)
                VALUES
                    (NEW.veiculo_id, r_plano.id, r_plano.tipo_manutencao, v_km_atual);

                v_tem_alerta := TRUE;
            END IF;
        END IF;
    END LOOP;

    -- Atualiza status do veículo quando há pelo menos um alerta novo
    IF v_tem_alerta THEN
        UPDATE veiculos
        SET    status = 'Manutenção Urgente'
        WHERE  id = NEW.veiculo_id;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_verificar_manutencao
    AFTER UPDATE ON viagens
    FOR EACH ROW
    EXECUTE FUNCTION fn_verificar_manutencao();
