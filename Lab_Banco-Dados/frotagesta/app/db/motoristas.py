"""FrotaGesta — db/motoristas.py"""
from .conexao import executar

def listar():
    return executar(
        "SELECT id, cpf, nome, categoria_cnh, ativo FROM motoristas ORDER BY nome",
        fetch=True)

def listar_ativos():
    return executar(
        "SELECT id, nome, categoria_cnh FROM motoristas WHERE ativo=TRUE ORDER BY nome",
        fetch=True)

def cadastrar(cpf, nome, categoria_cnh):
    executar("INSERT INTO motoristas (cpf, nome, categoria_cnh) VALUES (%s,%s,%s)",
             (cpf, nome, categoria_cnh))

def atualizar(id, cpf, nome, categoria_cnh, ativo):
    executar("UPDATE motoristas SET cpf=%s, nome=%s, categoria_cnh=%s, ativo=%s WHERE id=%s",
             (cpf, nome, categoria_cnh, ativo, id))

def excluir(id):
    executar("DELETE FROM motoristas WHERE id=%s", (id,))

def buscar_por_id(id):
    rows = executar("SELECT * FROM motoristas WHERE id=%s", (id,), fetch=True)
    return rows[0] if rows else None