-- =============================================================================
-- FrotaGesta | 06_seed.sql
-- Dados iniciais de teste para que a interface desktop não inicie vazia.
-- Cobre todos os cenários de negócio: CNH inválida, alertas pendentes,
-- veículo em rota, viagens históricas concluídas.
-- Pré-requisito: todos os scripts anteriores (01 a 05).
-- =============================================================================

-- -----------------------------------------------------------------------------
-- Motoristas
-- Cobre categorias B, D, E e AB.
-- Fernanda Lima está inativa (teste de regra de exibição na interface).
-- Ana Paula e Juliana (CNH B) não podem ser associadas a Caminhões.
-- -----------------------------------------------------------------------------
INSERT INTO motoristas (cpf, nome, categoria_cnh, ativo) VALUES
    ('111.222.333-44', 'Carlos Eduardo Ferreira', 'D',  TRUE),
    ('222.333.444-55', 'Ana Paula Rodrigues',      'B',  TRUE),
    ('333.444.555-66', 'Roberto Alves Mendes',     'E',  TRUE),
    ('444.555.666-77', 'Juliana Costa Santana',    'B',  TRUE),
    ('555.666.777-88', 'Marcos Vinícius Pereira',  'D',  TRUE),
    ('666.777.888-99', 'Fernanda Lima Oliveira',   'AB', FALSE);

-- -----------------------------------------------------------------------------
-- Veículos
-- 2 Caminhões, 2 Vans, 2 Carros.
-- JKL-3456 (Scania) será colocado em Manutenção Urgente abaixo via alertas.
-- ABC-1234 ficará Em rota pela viagem em andamento inserida adiante.
-- -----------------------------------------------------------------------------
INSERT INTO veiculos (placa, modelo, ano, km_atual, tipo, status) VALUES
    ('ABC-1234', 'Volkswagen Delivery 11.180', 2021, 48500.0, 'Caminhão', 'Disponível'),
    ('DEF-5678', 'Mercedes-Benz Sprinter 415', 2022,  9800.0, 'Van',      'Disponível'),
    ('GHI-9012', 'Fiat Strada Ranch',          2023,  3200.0, 'Carro',    'Disponível'),
    ('JKL-3456', 'Scania R 450',               2020, 97300.0, 'Caminhão', 'Disponível'),
    ('MNO-7890', 'Ford Transit Custom',        2022, 22100.0, 'Van',      'Disponível'),
    ('PQR-1122', 'Renault Duster',             2023,  1500.0, 'Carro',    'Disponível');

-- -----------------------------------------------------------------------------
-- Planos de Manutenção
-- Cada veículo possui ao menos um plano de troca de óleo (10.000 km).
-- Caminhões possuem planos adicionais (pastilhas, filtros, alinhamento).
-- -----------------------------------------------------------------------------
INSERT INTO plano_manutencao
    (veiculo_id, tipo_manutencao, km_intervalo, km_proxima, ultima_km) VALUES
    -- Caminhão ABC-1234 (id=1)
    (1, 'Troca de óleo',               10000, 50000.0, 40000.0),
    (1, 'Troca de pastilhas de freio', 30000, 60000.0, 30000.0),
    (1, 'Alinhamento e balanceamento',  5000, 50000.0, 45000.0),
    -- Van DEF-5678 (id=2)
    (2, 'Troca de óleo',               10000, 10000.0,     0.0),
    (2, 'Revisão geral',               30000, 30000.0,     0.0),
    -- Carro GHI-9012 (id=3)
    (3, 'Troca de óleo',               10000, 10000.0,     0.0),
    -- Caminhão JKL-3456 (id=4)
    (4, 'Troca de óleo',               10000,100000.0, 90000.0),
    (4, 'Troca de filtros',            20000,100000.0, 80000.0),
    -- Van MNO-7890 (id=5)
    (5, 'Troca de óleo',               10000, 30000.0, 20000.0),
    -- Carro PQR-1122 (id=6)
    (6, 'Troca de óleo',               10000, 10000.0,     0.0);

-- -----------------------------------------------------------------------------
-- Viagens — histórico concluído
-- Inseridas diretamente (sem trigger de encerramento) para simular histórico.
-- -----------------------------------------------------------------------------
INSERT INTO viagens
    (motorista_id, veiculo_id, dt_saida, dt_chegada, km_inicial, km_final, status)
VALUES
    -- saída 10 dias atrás, chegada 6 horas depois (= 9 dias e 18 horas atrás)
    (1, 1, NOW() - INTERVAL '10 days',     NOW() - INTERVAL '9 days 18 hours',
        45000.0, 48500.0, 'Concluída'),
    -- saída 5 dias atrás, chegada 4 horas depois (= 4 dias e 20 horas atrás)
    (2, 3, NOW() - INTERVAL '5 days',      NOW() - INTERVAL '4 days 20 hours',
         3000.0,  3200.0, 'Concluída'),
    -- saída 3 dias atrás, chegada 12 horas depois (= 2 dias e 12 horas atrás)
    (3, 4, NOW() - INTERVAL '3 days',      NOW() - INTERVAL '2 days 12 hours',
        94000.0, 97300.0, 'Concluída'),
    -- saída 2 dias atrás, chegada 4 horas depois (= 1 dia e 20 horas atrás)
    (5, 5, NOW() - INTERVAL '2 days',      NOW() - INTERVAL '1 day 20 hours',
        20000.0, 22100.0, 'Concluída'),
    -- saída 1 dia atrás, chegada 4 horas depois (= 20 horas atrás)
    (4, 6, NOW() - INTERVAL '1 day',       NOW() - INTERVAL '20 hours',
         1200.0,  1500.0, 'Concluída');

-- -----------------------------------------------------------------------------
-- Viagem em andamento — ABC-1234 está Em rota agora
-- -----------------------------------------------------------------------------
INSERT INTO viagens
    (motorista_id, veiculo_id, dt_saida, km_inicial, status)
VALUES
    (1, 1, NOW() - INTERVAL '2 hours', 48500.0, 'Em andamento');

UPDATE veiculos SET status = 'Em rota' WHERE id = 1;

-- -----------------------------------------------------------------------------
-- Alertas pré-existentes — JKL-3456 (Scania) já ultrapassou os limites
-- Simula um veículo que já chegou em situação crítica antes do sistema entrar
-- em operação. plano_manutencao_id 7 = Troca de óleo, 8 = Troca de filtros.
-- -----------------------------------------------------------------------------
INSERT INTO alertas_manutencao
    (veiculo_id, plano_manutencao_id, tipo_manutencao, km_alerta, status)
VALUES
    (4, 7, 'Troca de óleo',    97300.0, 'Pendente'),
    (4, 8, 'Troca de filtros', 97300.0, 'Pendente');

UPDATE veiculos SET status = 'Manutenção Urgente' WHERE id = 4;
