-- =============================================================================
-- FrotaGesta | 01_tabelas.sql
-- Cria as cinco tabelas do sistema com todas as constraints e chaves.
-- Pré-requisito: banco frotagesta criado (00_criar_banco.sql).
-- =============================================================================

-- -----------------------------------------------------------------------------
-- 1.1 MOTORISTAS
-- Armazena os condutores habilitados da frota.
-- categoria_cnh é validada por CHECK e usada pela trigger de CNH.
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS motoristas (
    id            SERIAL       PRIMARY KEY,
    cpf           VARCHAR(14)  NOT NULL UNIQUE,      -- formato: 000.000.000-00
    nome          VARCHAR(120) NOT NULL,
    categoria_cnh VARCHAR(2)   NOT NULL
        CHECK (categoria_cnh IN ('A','B','C','D','E','AB','AC','AD','AE')),
    ativo         BOOLEAN      NOT NULL DEFAULT TRUE,
    criado_em     TIMESTAMP    NOT NULL DEFAULT NOW()
);

-- -----------------------------------------------------------------------------
-- 1.2 VEICULOS
-- Cadastro completo da frota.
-- tipo determina qual categoria de CNH é exigida.
-- status alimenta o painel de controle da interface desktop.
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS veiculos (
    id        SERIAL        PRIMARY KEY,
    placa     VARCHAR(8)    NOT NULL UNIQUE,          -- ABC-1234 ou ABC1D23 (Mercosul)
    modelo    VARCHAR(80)   NOT NULL,
    ano       SMALLINT      NOT NULL
        CHECK (ano >= 1950 AND ano <= EXTRACT(YEAR FROM NOW()) + 1),
    km_atual  NUMERIC(10,1) NOT NULL DEFAULT 0
        CHECK (km_atual >= 0),
    tipo      VARCHAR(20)   NOT NULL
        CHECK (tipo IN ('Carro','Van','Caminhão')),
    status    VARCHAR(30)   NOT NULL DEFAULT 'Disponível'
        CHECK (status IN ('Disponível','Em rota','Manutenção Urgente','Em manutenção')),
    criado_em TIMESTAMP     NOT NULL DEFAULT NOW()
);

-- -----------------------------------------------------------------------------
-- 1.3 VIAGENS
-- Cada saída e chegada de veículo.
-- Consistência do odômetro garantida por CHECK.
-- Encerramento deve ser feito via sp_encerrar_viagem (04_procedure.sql).
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS viagens (
    id           SERIAL        PRIMARY KEY,
    motorista_id INTEGER       NOT NULL REFERENCES motoristas(id),
    veiculo_id   INTEGER       NOT NULL REFERENCES veiculos(id),
    dt_saida     TIMESTAMP     NOT NULL DEFAULT NOW(),
    dt_chegada   TIMESTAMP,
    km_inicial   NUMERIC(10,1) NOT NULL CHECK (km_inicial >= 0),
    km_final     NUMERIC(10,1)
        CHECK (km_final IS NULL OR km_final >= km_inicial),
    status       VARCHAR(20)   NOT NULL DEFAULT 'Em andamento'
        CHECK (status IN ('Em andamento','Concluída','Cancelada')),
    observacoes  TEXT,
    criado_em    TIMESTAMP     NOT NULL DEFAULT NOW(),

    CONSTRAINT ck_datas -- Regra de consistencia
        CHECK (dt_chegada IS NULL OR dt_chegada >= dt_saida)
);

-- -----------------------------------------------------------------------------
-- 1.4 PLANO_MANUTENCAO
-- Regras de manutenção preventiva por veículo.
-- UNIQUE (veiculo_id, tipo_manutencao) impede planos duplicados do mesmo tipo.
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS plano_manutencao (
    id              SERIAL        PRIMARY KEY,
    veiculo_id      INTEGER       NOT NULL REFERENCES veiculos(id),
    tipo_manutencao VARCHAR(80)   NOT NULL,
    km_intervalo    NUMERIC(10,1) NOT NULL CHECK (km_intervalo > 0),
    km_proxima      NUMERIC(10,1) NOT NULL CHECK (km_proxima > 0),
    ultima_km       NUMERIC(10,1) NOT NULL DEFAULT 0 CHECK (ultima_km >= 0),
    criado_em       TIMESTAMP     NOT NULL DEFAULT NOW(),

    UNIQUE (veiculo_id, tipo_manutencao) -- não cadastrar dois planos de troca no mesmo veículo
);

-- -----------------------------------------------------------------------------
-- 1.5 ALERTAS_MANUTENCAO
-- Gerada automaticamente pela trigger (03_trigger_manutencao.sql).
-- plano_manutencao_id vincula cada alerta ao plano que o originou.
-- -----------------------------------------------------------------------------
CREATE TABLE IF NOT EXISTS alertas_manutencao (
    id                  SERIAL        PRIMARY KEY,
    veiculo_id          INTEGER       NOT NULL REFERENCES veiculos(id),
    plano_manutencao_id INTEGER       NOT NULL REFERENCES plano_manutencao(id),
    tipo_manutencao     VARCHAR(80)   NOT NULL,
    km_alerta           NUMERIC(10,1) NOT NULL,
    dt_alerta           TIMESTAMP     NOT NULL DEFAULT NOW(),
    status              VARCHAR(20)   NOT NULL DEFAULT 'Pendente'
        CHECK (status IN ('Pendente','Resolvido')),
    resolvido_em        TIMESTAMP
);
