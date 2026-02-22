-- SISTEMA DE GERENCIAMENTO DE BIBLIOTECA DIGITAL
-- Script DDL - Definição das Estruturas de Dados
-- Ambiente: MySQL 8.0+ / MySQL Workbench


-- Criação e seleção do banco de dados
DROP DATABASE IF EXISTS biblioteca_digital;
CREATE DATABASE biblioteca_digital
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE biblioteca_digital;

-- TABELA: Editora
-- RF005 - Gestão de Editoras
CREATE TABLE Editora (
    id_editora   INT            NOT NULL AUTO_INCREMENT,
    nome         VARCHAR(150)   NOT NULL,
    localizacao  VARCHAR(200),
    CONSTRAINT pk_editora PRIMARY KEY (id_editora)
) ENGINE=InnoDB;

-- TABELA: Livro
-- RF001 - Gerenciamento de Acervo
-- RN001 - ISBN como identificador único
-- RN002 - Associado a uma única Editora

CREATE TABLE Livro (
    isbn             VARCHAR(20)   NOT NULL,
    titulo           VARCHAR(300)  NOT NULL,          -- RN003: NOT NULL
    ano_publicacao   YEAR,
    edicao           VARCHAR(20),
    id_editora       INT           NOT NULL,           -- RN002
    CONSTRAINT pk_livro      PRIMARY KEY (isbn),
    CONSTRAINT fk_livro_editora
        FOREIGN KEY (id_editora) REFERENCES Editora(id_editora)
        ON UPDATE CASCADE
        ON DELETE RESTRICT
) ENGINE=InnoDB;

-- TABELA: Autor
-- RF001 - Gerenciamento de Acervo (autores)
-- RN003 - Nome obrigatório
CREATE TABLE Autor (
    id_autor  INT           NOT NULL AUTO_INCREMENT,
    nome      VARCHAR(200)  NOT NULL,                  -- RN003: NOT NULL
    CONSTRAINT pk_autor PRIMARY KEY (id_autor)
) ENGINE=InnoDB;


-- TABELA: Livro_Autor  (tabela associativa)
-- RN004 - Relacionamento N:M entre Livro e Autor

CREATE TABLE Livro_Autor (
    isbn      VARCHAR(20)  NOT NULL,
    id_autor  INT          NOT NULL,
    CONSTRAINT pk_livro_autor  PRIMARY KEY (isbn, id_autor),
    CONSTRAINT fk_la_livro
        FOREIGN KEY (isbn)     REFERENCES Livro(isbn)
        ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT fk_la_autor
        FOREIGN KEY (id_autor) REFERENCES Autor(id_autor)
        ON UPDATE CASCADE ON DELETE RESTRICT
) ENGINE=InnoDB;


-- TABELA: Exemplar
-- RN103 - Rastreamento físico de cada exemplar do título
-- Empréstimos e Reservas são feitos no nível do Exemplar
CREATE TABLE Exemplar (
    id_exemplar        INT          NOT NULL AUTO_INCREMENT,
    isbn               VARCHAR(20)  NOT NULL,
    numero_tombamento  VARCHAR(50),
    status             ENUM('Disponivel','Emprestado','Reservado','Extraviado')
                       NOT NULL DEFAULT 'Disponivel',
    CONSTRAINT pk_exemplar PRIMARY KEY (id_exemplar),
    CONSTRAINT fk_exemplar_livro
        FOREIGN KEY (isbn) REFERENCES Livro(isbn)
        ON UPDATE CASCADE ON DELETE RESTRICT
) ENGINE=InnoDB;

-- TABELA: Usuario
-- RF002 - Registro de Usuários
-- RN001 - Código de identificação único
CREATE TABLE Usuario (
    id_usuario           INT           NOT NULL AUTO_INCREMENT,
    nome                 VARCHAR(200)  NOT NULL,       -- RN003: NOT NULL
    numero_identificacao VARCHAR(50)   NOT NULL,
    status               ENUM('Ativo','Inativo')   NOT NULL DEFAULT 'Ativo',
    tipo                 ENUM('Aluno','Professor') NOT NULL,
    CONSTRAINT pk_usuario          PRIMARY KEY (id_usuario),
    CONSTRAINT uq_usuario_numid    UNIQUE (numero_identificacao)
) ENGINE=InnoDB;

-- TABELA: Emprestimo
-- RF003 - Controle de Empréstimos
-- RF004 - Controle de Devoluções (data_real_devolucao)
-- RN002 - Integridade referencial com Usuário e Exemplar
-- RN006 - data_prevista_devolucao > data_emprestimo (CHECK)
CREATE TABLE Emprestimo (
    id_emprestimo            INT   NOT NULL AUTO_INCREMENT,
    id_exemplar              INT   NOT NULL,
    id_usuario               INT   NOT NULL,
    data_emprestimo          DATE  NOT NULL,
    data_prevista_devolucao  DATE  NOT NULL,
    data_real_devolucao      DATE  NULL DEFAULT NULL,  -- NULL = ainda não devolvido
    CONSTRAINT pk_emprestimo PRIMARY KEY (id_emprestimo),
    CONSTRAINT fk_emp_exemplar
        FOREIGN KEY (id_exemplar) REFERENCES Exemplar(id_exemplar)
        ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT fk_emp_usuario
        FOREIGN KEY (id_usuario)  REFERENCES Usuario(id_usuario)
        ON UPDATE CASCADE ON DELETE RESTRICT,
    -- RN006: data de devolução prevista sempre posterior à data do empréstimo
    CONSTRAINT chk_datas_emprestimo
        CHECK (data_prevista_devolucao > data_emprestimo),
    -- Consistência: data real de devolução >= data do empréstimo
    CONSTRAINT chk_data_real_devolucao
        CHECK (data_real_devolucao IS NULL OR data_real_devolucao >= data_emprestimo)
) ENGINE=InnoDB;

-- TABELA: Reserva
-- RN101 - Entidade Reserva com status
-- RN102 - Prioridade FIFO por data_reserva
-- RN103 - Reservas no nível do Exemplar
CREATE TABLE Reserva (
    id_reserva    INT       NOT NULL AUTO_INCREMENT,
    id_exemplar   INT       NOT NULL,
    id_usuario    INT       NOT NULL,
    data_reserva  DATETIME  NOT NULL DEFAULT CURRENT_TIMESTAMP,
    status        ENUM('Ativa','Cancelada','Atendida') NOT NULL DEFAULT 'Ativa',
    CONSTRAINT pk_reserva PRIMARY KEY (id_reserva),
    CONSTRAINT fk_res_exemplar
        FOREIGN KEY (id_exemplar) REFERENCES Exemplar(id_exemplar)
        ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT fk_res_usuario
        FOREIGN KEY (id_usuario)  REFERENCES Usuario(id_usuario)
        ON UPDATE CASCADE ON DELETE RESTRICT
) ENGINE=InnoDB;


-- TABELA: Multa
-- RC008 - Relatório de Inadimplência (multas acumuladas)
CREATE TABLE Multa (
    id_multa       INT             NOT NULL AUTO_INCREMENT,
    id_emprestimo  INT             NOT NULL,
    valor          DECIMAL(10, 2)  NOT NULL CHECK (valor >= 0),
    pago           TINYINT(1)      NOT NULL DEFAULT 0,  -- 0 = pendente, 1 = pago
    data_geracao   DATE            NOT NULL DEFAULT (CURRENT_DATE),
    CONSTRAINT pk_multa PRIMARY KEY (id_multa),
    CONSTRAINT fk_multa_emprestimo
        FOREIGN KEY (id_emprestimo) REFERENCES Emprestimo(id_emprestimo)
        ON UPDATE CASCADE ON DELETE RESTRICT
) ENGINE=InnoDB;

-- ÍNDICES auxiliares para otimização de consultas

CREATE INDEX idx_emprestimo_usuario   ON Emprestimo(id_usuario);
CREATE INDEX idx_emprestimo_exemplar  ON Emprestimo(id_exemplar);
CREATE INDEX idx_emprestimo_datas     ON Emprestimo(data_prevista_devolucao, data_real_devolucao);
CREATE INDEX idx_reserva_status       ON Reserva(status, data_reserva);
CREATE INDEX idx_reserva_exemplar     ON Reserva(id_exemplar);
CREATE INDEX idx_exemplar_isbn        ON Exemplar(isbn);
CREATE INDEX idx_exemplar_status      ON Exemplar(status);
CREATE INDEX idx_livro_editora        ON Livro(id_editora);
