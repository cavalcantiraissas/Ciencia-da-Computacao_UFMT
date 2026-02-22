-- SISTEMA DE GERENCIAMENTO DE BIBLIOTECA DIGITAL
-- Script DML - Inserção de Dados de Teste
-- Ambiente: MySQL 8.0+ / MySQL Workbench

USE biblioteca_digital;


-- EDITORAS
INSERT INTO Editora (nome, localizacao) VALUES
('Pearson Education',         'São Paulo, SP'),
('Alta Books',                'Rio de Janeiro, RJ'),
('O\'Reilly Media',           'Sebastopol, CA - EUA'),
('Elsevier',                  'Amsterdam - Holanda'),
('Bookman',                   'Porto Alegre, RS'),
('Novatec Editora',           'São Paulo, SP'),
('Cengage Learning',          'São Paulo, SP'),
('MIT Press',                  'Cambridge, MA - EUA');

-- LIVROS
INSERT INTO Livro (isbn, titulo, ano_publicacao, edicao, id_editora) VALUES
('978-8543004792', 'Sistemas de Banco de Dados',                      2019, '7ª Edição', 1),
('978-8576050568', 'Introdução à Álgebra Relacional',                 2020, '1ª Edição', 2),
('978-1492056348', 'Learning SQL',                                     2020, '3ª Edição', 3),
('978-8535281064', 'MySQL: Guia do Desenvolvedor',                     2021, '2ª Edição', 4),
('978-8577808533', 'Fundamentos de Sistemas de Informação',           2018, '5ª Edição', 5),
('978-8575227244', 'SQL e Modelagem de Dados',                        2022, '1ª Edição', 6),
('978-0262033848', 'Introduction to Algorithms',                       2022, '4ª Edição', 8),
('978-8521637073', 'Análise e Projeto de Sistemas',                   2017, '3ª Edição', 7),
('978-8576057802', 'Engenharia de Software',                           2016, '8ª Edição', 5),
('978-1491950357', 'Python for Data Analysis',                         2022, '3ª Edição', 3);

-- AUTORES
INSERT INTO Autor (nome) VALUES
('Ramez Elmasri'),            -- 1
('Shamkant B. Navathe'),      -- 2
('Alan Beaulieu'),            -- 3
('Thomas H. Cormen'),         -- 4
('Charles E. Leiserson'),     -- 5
('Ronald L. Rivest'),         -- 6
('Clifford Stein'),           -- 7
('Ian Sommerville'),          -- 8
('Wes McKinney'),             -- 9
('Kenneth C. Laudon'),        -- 10
('Jane P. Laudon'),           -- 11
('Carlos Alberto Heuser'),    -- 12
('Robson Lima');              -- 13


-- LIVRO_AUTOR (relacionamento N:M)
INSERT INTO Livro_Autor (isbn, id_autor) VALUES
-- Sistemas de Banco de Dados - Elmasri & Navathe
('978-8543004792', 1),
('978-8543004792', 2),
-- Introdução à Álgebra Relacional
('978-8576050568', 12),
-- Learning SQL
('978-1492056348', 3),
-- MySQL: Guia do Desenvolvedor
('978-8535281064', 13),
-- Fundamentos de Sistemas de Informação - Laudon & Laudon
('978-8577808533', 10),
('978-8577808533', 11),
-- SQL e Modelagem de Dados
('978-8575227244', 12),
-- Introduction to Algorithms - Cormen et al.
('978-0262033848', 4),
('978-0262033848', 5),
('978-0262033848', 6),
('978-0262033848', 7),
-- Análise e Projeto de Sistemas
('978-8521637073', 13),
-- Engenharia de Software - Sommerville
('978-8576057802', 8),
-- Python for Data Analysis
('978-1491950357', 9);


-- EXEMPLARES (múltiplos exemplares por título - RN103)
INSERT INTO Exemplar (isbn, numero_tombamento, status) VALUES
-- 3 exemplares de Sistemas de Banco de Dados
('978-8543004792', 'TOMB-0001', 'Emprestado'),
('978-8543004792', 'TOMB-0002', 'Disponivel'),
('978-8543004792', 'TOMB-0003', 'Reservado'),
-- 2 exemplares de Introdução à Álgebra Relacional
('978-8576050568', 'TOMB-0004', 'Disponivel'),
('978-8576050568', 'TOMB-0005', 'Emprestado'),
-- 2 exemplares de Learning SQL
('978-1492056348', 'TOMB-0006', 'Disponivel'),
('978-1492056348', 'TOMB-0007', 'Emprestado'),
-- 1 exemplar de MySQL: Guia do Desenvolvedor
('978-8535281064', 'TOMB-0008', 'Emprestado'),
-- 2 exemplares de Fundamentos de Sistemas de Informação
('978-8577808533', 'TOMB-0009', 'Disponivel'),
('978-8577808533', 'TOMB-0010', 'Disponivel'),
-- 1 exemplar de SQL e Modelagem de Dados
('978-8575227244', 'TOMB-0011', 'Emprestado'),
-- 2 exemplares de Introduction to Algorithms
('978-0262033848', 'TOMB-0012', 'Disponivel'),
('978-0262033848', 'TOMB-0013', 'Disponivel'),
-- 1 exemplar de Análise e Projeto de Sistemas
('978-8521637073', 'TOMB-0014', 'Disponivel'),
-- 2 exemplares de Engenharia de Software
('978-8576057802', 'TOMB-0015', 'Emprestado'),
('978-8576057802', 'TOMB-0016', 'Disponivel'),
-- 1 exemplar de Python for Data Analysis
('978-1491950357', 'TOMB-0017', 'Emprestado');

-- USUÁRIOS
INSERT INTO Usuario (nome, numero_identificacao, status, tipo) VALUES
('Ana Paula Ferreira',       'ALU-2024-001', 'Ativo',   'Aluno'),
('Bruno Henrique Costa',     'ALU-2024-002', 'Ativo',   'Aluno'),
('Carla Dias Mendonça',      'ALU-2024-003', 'Ativo',   'Aluno'),
('Diego Rafael Souza',       'ALU-2024-004', 'Ativo',   'Aluno'),
('Elisa Tavares Lima',       'ALU-2024-005', 'Inativo', 'Aluno'),
('Prof. Fernando Alves',     'PRF-2020-001', 'Ativo',   'Professor'),
('Profa. Gabriela Rocha',    'PRF-2020-002', 'Ativo',   'Professor'),
('Prof. Henrique Barros',    'PRF-2019-001', 'Ativo',   'Professor'),
('Isabela Cristina Nunes',   'ALU-2023-010', 'Ativo',   'Aluno'),
('João Pedro Matos',         'ALU-2023-011', 'Ativo',   'Aluno');

-- EMPRÉSTIMOS
-- Alguns em atraso (RC001), alguns devolvidos, alguns ativos
INSERT INTO Emprestimo (id_exemplar, id_usuario, data_emprestimo, data_prevista_devolucao, data_real_devolucao) VALUES
-- Empréstimo ativo em ATRASO (id_exemplar=1, Sistemas BD, Ana Paula)
(1,  1, '2025-01-05', '2025-01-19', NULL),
-- Empréstimo ativo em ATRASO (id_exemplar=5, Álgebra, Bruno)
(5,  2, '2025-01-10', '2025-01-24', NULL),
-- Empréstimo ativo em ATRASO (id_exemplar=7, Learning SQL, Carla)
(7,  3, '2025-01-15', '2025-01-29', NULL),
-- Empréstimo ativo em ATRASO (id_exemplar=8, MySQL, Diego)
(8,  4, '2024-12-01', '2024-12-15', NULL),
-- Empréstimo já devolvido no prazo (id_exemplar=11, SQL e Modelagem, Prof. Fernando)
(11, 6, '2025-01-02', '2025-01-16', '2025-01-14'),
-- Empréstimo já devolvido no prazo (id_exemplar=15, Engenharia SW, Profa. Gabriela)
(15, 7, '2025-01-08', '2025-01-22', '2025-01-20'),
-- Empréstimo ativo recente (id_exemplar=17, Python, João Pedro)
(17, 10,'2025-02-01', '2025-02-15', NULL),
-- Histórico de empréstimos anteriores (devolvidos) para estatísticas RC007
(2,  6, '2024-08-10', '2024-08-24', '2024-08-22'),  -- Sistemas BD
(2,  7, '2024-09-05', '2024-09-19', '2024-09-17'),  -- Sistemas BD
(4,  1, '2024-10-01', '2024-10-15', '2024-10-14'),  -- Álgebra Relacional
(6,  8, '2024-11-01', '2024-11-15', '2024-11-13'),  -- Learning SQL
(6,  9, '2024-12-05', '2024-12-19', '2024-12-18'),  -- Learning SQL
-- Empréstimo ativo em ATRASO (id_exemplar=11 reutilizado após devolução)
(11, 9, '2025-01-20', '2025-02-03', NULL);

-- RESERVAS
-- RN101: status, RN102: FIFO por data_reserva
INSERT INTO Reserva (id_exemplar, id_usuario, data_reserva, status) VALUES
-- Reserva ativa para exemplar 3 (Sistemas BD, Reservado)
(3, 5, '2025-01-20 09:00:00', 'Ativa'),
-- Reserva ativa (segunda na fila) para exemplar 3
(3, 9, '2025-01-21 14:30:00', 'Ativa'),
-- Reserva ativa para exemplar 8 (MySQL Guia, Emprestado)
(8, 1, '2025-01-16 10:00:00', 'Ativa'),
-- Reserva atendida (histórico)
(1, 2, '2024-12-20 08:00:00', 'Atendida'),
-- Reserva cancelada (histórico)
(5, 3, '2025-01-05 11:00:00', 'Cancelada'),
-- Reserva ativa para exemplar 11 (SQL e Modelagem)
(11, 2, '2025-01-18 16:00:00', 'Ativa');

-- MULTAS (geradas por empréstimos em atraso - RC008)
INSERT INTO Multa (id_emprestimo, valor, pago, data_geracao) VALUES
-- Multa do empréstimo 1 (Sistemas BD - Ana Paula) em atraso ~37 dias * R$1,00
(1, 37.00, 0, '2025-02-25'),
-- Multa do empréstimo 2 (Álgebra - Bruno) em atraso ~32 dias * R$1,00
(2, 32.00, 0, '2025-02-25'),
-- Multa do empréstimo 3 (Learning SQL - Carla) em atraso ~27 dias * R$1,00
(3, 27.00, 0, '2025-02-25'),
-- Multa do empréstimo 4 (MySQL - Diego) em atraso ~72 dias * R$1,00
(4, 72.00, 0, '2025-02-25'),
-- Multa paga por devolução anterior com atraso (Prof. Fernando - já paga)
(5,  0.00, 1, '2025-01-14'),
-- Multa do empréstimo 13 (SQL e Modelagem - Isabela) ativa
(13, 22.00, 0, '2025-02-25');
