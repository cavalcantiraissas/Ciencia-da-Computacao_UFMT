-- SISTEMA DE GERENCIAMENTO DE BIBLIOTECA DIGITAL
-- Script de Consultas SQL - RC001 a RC010
-- Ambiente: MySQL 8.0+ | MySQL Workbench

USE biblioteca_digital;


-- RC001 - LIVROS EM ATRASO
-- Lista: Nome do Usuário, Título do Livro, Dias de Atraso
-- Critério: data_prevista_devolucao < CURDATE() e não devolvido
SELECT
    u.nome                                              AS nome_usuario,
    l.titulo                                            AS titulo_livro,
    ex.numero_tombamento                                AS tombamento,
    e.data_emprestimo,
    e.data_prevista_devolucao,
    DATEDIFF(CURDATE(), e.data_prevista_devolucao)      AS dias_atraso
FROM
    Emprestimo  e
    INNER JOIN Usuario  u   ON e.id_usuario  = u.id_usuario
    INNER JOIN Exemplar ex  ON e.id_exemplar = ex.id_exemplar
    INNER JOIN Livro    l   ON ex.isbn        = l.isbn
WHERE
    e.data_real_devolucao IS NULL                        -- ainda não devolvido
    AND e.data_prevista_devolucao < CURDATE()            -- prazo vencido
ORDER BY
    dias_atraso DESC;


-- RC002 - ANÁLISE DE ACERVO
-- Retorna: Nome da Editora e Total de Livros (títulos únicos)
-- Ordenado do maior para o menor
SELECT
    ed.nome                  AS nome_editora,
    COUNT(l.isbn)            AS total_livros
FROM
    Editora ed
    LEFT JOIN Livro l ON ed.id_editora = l.id_editora
GROUP BY
    ed.id_editora, ed.nome
ORDER BY
    total_livros DESC;


-- RC003 - DISPONIBILIDADE
-- Lista todos os títulos de livros NÃO emprestados atualmente
-- (considera: todos os exemplares disponíveis)
-- Versão 1: usando NOT IN (subquery)
SELECT DISTINCT
    l.isbn,
    l.titulo,
    l.ano_publicacao,
    l.edicao
FROM
    Livro l
    INNER JOIN Exemplar ex ON l.isbn = ex.isbn
WHERE
    ex.id_exemplar NOT IN (
        SELECT id_exemplar
        FROM   Emprestimo
        WHERE  data_real_devolucao IS NULL    -- empréstimos ativos
    )
    AND ex.status = 'Disponivel'
ORDER BY
    l.titulo;

-- RC004 - TRANSAÇÕES POR USUÁRIO
-- Lista livros emprestados por um determinado usuário
-- incluindo status: 'Devolvido' ou 'Emprestado'
SELECT
    u.nome                      AS nome_usuario,
    l.titulo                    AS titulo_livro,
    ex.numero_tombamento        AS tombamento,
    e.data_emprestimo,
    e.data_prevista_devolucao,
    e.data_real_devolucao,
    CASE
        WHEN e.data_real_devolucao IS NOT NULL THEN 'Devolvido'
        ELSE 'Emprestado'
    END                         AS status_emprestimo
FROM
    Emprestimo  e
    INNER JOIN Usuario  u   ON e.id_usuario  = u.id_usuario
    INNER JOIN Exemplar ex  ON e.id_exemplar = ex.id_exemplar
    INNER JOIN Livro    l   ON ex.isbn        = l.isbn
WHERE
    u.id_usuario = 1           -- <<< parâmetro: alterar para o usuário desejado
ORDER BY
    e.data_emprestimo DESC;

-- RC005 - CRIAÇÃO DE VISÃO (VIEW)
-- V_EmprestimosAtivos: exibe apenas empréstimos não devolvidos
DROP VIEW IF EXISTS V_EmprestimosAtivos;

CREATE VIEW V_EmprestimosAtivos AS
SELECT
    e.id_emprestimo,
    u.id_usuario,
    u.nome                                          AS nome_usuario,
    u.tipo                                          AS tipo_usuario,
    l.isbn,
    l.titulo                                        AS titulo_livro,
    ex.numero_tombamento,
    e.data_emprestimo,
    e.data_prevista_devolucao,
    DATEDIFF(CURDATE(), e.data_prevista_devolucao)  AS dias_atraso,
    CASE
        WHEN e.data_prevista_devolucao < CURDATE() THEN 'Em Atraso'
        ELSE 'No Prazo'
    END                                             AS situacao
FROM
    Emprestimo  e
    INNER JOIN Usuario  u   ON e.id_usuario  = u.id_usuario
    INNER JOIN Exemplar ex  ON e.id_exemplar = ex.id_exemplar
    INNER JOIN Livro    l   ON ex.isbn        = l.isbn
WHERE
    e.data_real_devolucao IS NULL;   -- critério principal da VIEW

-- Consultar a VIEW:
SELECT * FROM V_EmprestimosAtivos ORDER BY dias_atraso DESC;

-- RC006 - IMPLEMENTAÇÃO DE ÁLGEBRA RELACIONAL
-- Expressão:
--   Π_Nome,Titulo ( σ_Ano>2020 ( Usuário ⋈ Empréstimo ⋈ Livro ) )
--
-- Tradução: Projeção de Nome e Título sobre a seleção onde
-- o ano de publicação do livro é > 2020, após junção natural
-- das entidades Usuário, Empréstimo e Livro.
SELECT DISTINCT
    u.nome   AS nome_usuario,
    l.titulo AS titulo_livro
FROM
    Usuario  u
    INNER JOIN Emprestimo  e   ON u.id_usuario  = e.id_usuario
    INNER JOIN Exemplar    ex  ON e.id_exemplar = ex.id_exemplar
    INNER JOIN Livro       l   ON ex.isbn        = l.isbn
WHERE
    l.ano_publicacao > 2020
ORDER BY
    u.nome, l.titulo;

-- RC007 - RANKING DE LIVROS
-- Livros mais emprestados nos últimos 6 meses
SELECT
    l.isbn,
    l.titulo                            AS titulo_livro,
    COUNT(e.id_emprestimo)              AS total_emprestimos_6meses
FROM
    Emprestimo  e
    INNER JOIN Exemplar ex ON e.id_exemplar = ex.id_exemplar
    INNER JOIN Livro    l  ON ex.isbn        = l.isbn
WHERE
    e.data_emprestimo >= DATE_SUB(CURDATE(), INTERVAL 6 MONTH)
GROUP BY
    l.isbn, l.titulo
ORDER BY
    total_emprestimos_6meses DESC
LIMIT 10;


-- RC008 - RELATÓRIO DE INADIMPLÊNCIA
-- Nome do Usuário e Valor Total de Multas Acumuladas (pagas ou não)
SELECT
    u.id_usuario,
    u.nome                          AS nome_usuario,
    u.tipo                          AS tipo_usuario,
    SUM(m.valor)                    AS total_multas_acumuladas,
    SUM(CASE WHEN m.pago = 1 THEN m.valor ELSE 0 END) AS multas_pagas,
    SUM(CASE WHEN m.pago = 0 THEN m.valor ELSE 0 END) AS multas_pendentes
FROM
    Multa      m
    INNER JOIN Emprestimo e ON m.id_emprestimo = e.id_emprestimo
    INNER JOIN Usuario    u ON e.id_usuario    = u.id_usuario
GROUP BY
    u.id_usuario, u.nome, u.tipo
ORDER BY
    total_multas_acumuladas DESC;


-- RC009 - LISTAR RESERVAS PENDENTES
-- Livros com Reservas Ativas: Título e Quantidade na Fila
SELECT
    l.isbn,
    l.titulo                        AS titulo_livro,
    COUNT(r.id_reserva)             AS quantidade_na_fila
FROM
    Reserva    r
    INNER JOIN Exemplar ex ON r.id_exemplar = ex.id_exemplar
    INNER JOIN Livro    l  ON ex.isbn        = l.isbn
WHERE
    r.status = 'Ativa'
GROUP BY
    l.isbn, l.titulo
ORDER BY
    quantidade_na_fila DESC;


-- RC010 - PRÓXIMO NA FILA
-- Para um dado ISBN, o usuário em 1ª posição (FIFO por data_reserva)
-- Substitua o ISBN pelo desejado
SELECT
    u.nome                   AS nome_usuario,
    u.numero_identificacao,
    u.tipo                   AS tipo_usuario,
    l.titulo                 AS titulo_livro,
    r.data_reserva,
    r.status
FROM
    Reserva    r
    INNER JOIN Usuario  u   ON r.id_usuario  = u.id_usuario
    INNER JOIN Exemplar ex  ON r.id_exemplar = ex.id_exemplar
    INNER JOIN Livro    l   ON ex.isbn        = l.isbn
WHERE
    ex.isbn     = '978-8543004792'   -- <<< parâmetro: substituir pelo ISBN desejado
    AND r.status = 'Ativa'
ORDER BY
    r.data_reserva ASC
LIMIT 1;
