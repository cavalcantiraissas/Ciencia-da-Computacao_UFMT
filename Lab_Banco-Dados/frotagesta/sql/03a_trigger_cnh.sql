-- =============================================================================
-- FrotaGesta | 03a_trigger_cnh.sql
-- Trigger BEFORE INSERT OR UPDATE em viagens.
-- Impede que motorista com CNH categoria B conduza Caminhão (exige D, E, AD ou AE).
-- Pré-requisito: 01_tabelas.sql
-- =============================================================================

CREATE OR REPLACE FUNCTION fn_validar_cnh_veiculo()
RETURNS TRIGGER AS $$
DECLARE
    v_categoria VARCHAR(2);
    v_tipo      VARCHAR(20);
BEGIN
    SELECT categoria_cnh INTO v_categoria
    FROM   motoristas
    WHERE  id = NEW.motorista_id;

    SELECT tipo INTO v_tipo
    FROM   veiculos
    WHERE  id = NEW.veiculo_id;

    IF v_tipo = 'Caminhão' AND v_categoria NOT IN ('D','E','AD','AE') THEN
        RAISE EXCEPTION
            'Motorista com CNH categoria % não está habilitado para conduzir Caminhão. '
            'Exige categoria D ou E.',
            v_categoria;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_validar_cnh_veiculo
    BEFORE INSERT OR UPDATE ON viagens
    FOR EACH ROW
    EXECUTE FUNCTION fn_validar_cnh_veiculo();

-- Postgres não permite subquery dentro de check, e aqui a gente precisa comparar dados
-- de duas tabelas diferentes, motoristas e veículos, no momento em que uma viagem é criada
-- É uma trigger BEFORE INSERT OR UPDATE: ela roda antes da linha ser gravada, busca a categoria
-- da CNH DO motorista e o tipo do veículo, e se for caminhão com categoria fora de D, E, AD ou AE
-- ele dá um RAISE EXCEPTION e cancela a operacap inteira