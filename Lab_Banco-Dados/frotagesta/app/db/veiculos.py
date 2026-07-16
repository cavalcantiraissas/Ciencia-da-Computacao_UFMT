"""FrotaGesta — db/veiculos.py"""
from .conexao import executar

def listar():
    return executar(
        "SELECT id, placa, modelo, ano, km_atual, tipo, status FROM veiculos ORDER BY placa",
        fetch=True)

def listar_disponiveis():
    return executar(
        "SELECT id, placa, modelo, tipo FROM veiculos WHERE status='Disponível' ORDER BY placa",
        fetch=True)

def cadastrar(placa, modelo, ano, km_atual, tipo):
    executar("INSERT INTO veiculos (placa, modelo, ano, km_atual, tipo) VALUES (%s,%s,%s,%s,%s)",
             (placa, modelo, ano, km_atual, tipo))

def atualizar(id, placa, modelo, ano, km_atual, tipo, status):
    executar("UPDATE veiculos SET placa=%s, modelo=%s, ano=%s, km_atual=%s, tipo=%s, status=%s WHERE id=%s",
             (placa, modelo, ano, km_atual, tipo, status, id))

def excluir(id):
    executar("DELETE FROM veiculos WHERE id=%s", (id,))

def buscar_por_id(id):
    rows = executar("SELECT * FROM veiculos WHERE id=%s", (id,), fetch=True)
    return rows[0] if rows else None

def painel():
    return executar("SELECT * FROM vw_painel_veiculos", fetch=True)