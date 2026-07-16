"""FrotaGesta — db/viagens.py"""
from .conexao import executar, chamar_procedure

def historico():
    return executar("SELECT * FROM vw_historico_viagens", fetch=True)

def em_andamento():
    return executar("""
        SELECT v.id, m.nome AS motorista, ve.placa, ve.modelo, v.dt_saida, v.km_inicial
        FROM viagens v
        JOIN motoristas m  ON m.id  = v.motorista_id
        JOIN veiculos   ve ON ve.id = v.veiculo_id
        WHERE v.status = 'Em andamento'
        ORDER BY v.dt_saida DESC
    """, fetch=True)

def iniciar(motorista_id, veiculo_id, km_inicial):
    executar("INSERT INTO viagens (motorista_id, veiculo_id, km_inicial, status) VALUES (%s,%s,%s,'Em andamento')",
             (motorista_id, veiculo_id, km_inicial))
    executar("UPDATE veiculos SET status='Em rota' WHERE id=%s", (veiculo_id,))

def cancelar(viagem_id, veiculo_id):
    executar("UPDATE viagens SET status='Cancelada' WHERE id=%s", (viagem_id,))
    executar("UPDATE veiculos SET status='Disponível' WHERE id=%s", (veiculo_id,))

def encerrar(viagem_id, km_final, observacoes=None):
    chamar_procedure("sp_encerrar_viagem", (viagem_id, km_final, observacoes))

def alertas_pendentes():
    return executar("""
        SELECT am.id, ve.placa, ve.modelo, am.tipo_manutencao,
               am.km_alerta, am.dt_alerta, am.status
        FROM alertas_manutencao am
        JOIN veiculos ve ON ve.id = am.veiculo_id
        WHERE am.status = 'Pendente'
        ORDER BY am.dt_alerta DESC
    """, fetch=True)

def resolver_alerta(alerta_id):
    executar("UPDATE alertas_manutencao SET status='Resolvido', resolvido_em=NOW() WHERE id=%s",
             (alerta_id,))

def buscar_veiculo_da_viagem(viagem_id):
    rows = executar("SELECT veiculo_id FROM viagens WHERE id=%s", (viagem_id,), fetch=True)
    return rows[0]["veiculo_id"] if rows else None