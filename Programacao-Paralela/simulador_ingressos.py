import threading
import time
import random
import queue
from datetime import datetime

ESTOQUE_INICIAL = 20
NUM_COMPRADORES = 10

estoque = ESTOQUE_INICIAL
mutex_estoque = threading.Lock()
fila_auditoria = queue.Queue()

# Estatísticas acumuladas pela thread logger (para o relatório final)
total_vendido = 0
total_aprovadas = 0
total_rejeitadas = 0


def comprador(cliente_id):
    global estoque

    while True:
        qtd_solicitada = random.randint(1, 3)

        with mutex_estoque:
            if estoque <= 0:
                sem_estoque = True
            else:
                sem_estoque = False

                saldo_atual = estoque

                # Simula a latência de validação do cartão/gateway de pagamento
                time.sleep(random.uniform(0.01, 0.05))

                if saldo_atual >= qtd_solicitada:
                    estoque -= qtd_solicitada
                    sucesso = True
                else:
                    sucesso = False

                saldo_restante = estoque

        if sem_estoque:
            break

        # Nenhum print/log é feito aqui dentro do mutex: o evento é apenas
        # montado e enfileirado para a thread logger processar depois
        evento = {
            "timestamp": datetime.now().strftime("%H:%M:%S.%f")[:-3],
            "cliente_id": cliente_id,
            "qtd_solicitada": qtd_solicitada,
            "sucesso": sucesso,
            "saldo_restante": saldo_restante,
        }
        fila_auditoria.put(evento)


def logger():
    global total_vendido, total_aprovadas, total_rejeitadas

    while True:
        evento = fila_auditoria.get()

        if evento is None:
            fila_auditoria.task_done()
            break

        status = "APROVADA " if evento["sucesso"] else "REJEITADA"
        print(f"[{evento['timestamp']}] Cliente {evento['cliente_id']:02d} | "
              f"Qtd solicitada: {evento['qtd_solicitada']} | {status} | "
              f"Estoque restante: {evento['saldo_restante']}")

        if evento["sucesso"]:
            total_vendido += evento["qtd_solicitada"]
            total_aprovadas += 1
        else:
            total_rejeitadas += 1

        fila_auditoria.task_done()


def main():
    thread_logger = threading.Thread(target=logger, daemon=True, name="Logger")
    thread_logger.start()

    compradores = []
    for i in range(1, NUM_COMPRADORES + 1):
        t = threading.Thread(target=comprador, args=(i,), name=f"Comprador-{i}")
        compradores.append(t)
        t.start()

    # Garante que todas as threads compradoras terminaram
    for t in compradores:
        t.join()

    # Garante que todos os logs pendentes na fila foram devidamente processados
    fila_auditoria.join()

    # Sinaliza término para a thread logger e aguarda ela encerrar
    fila_auditoria.put(None)
    thread_logger.join()

    print("\n--- RELATÓRIO FINAL ---")
    print(f"Estoque inicial:        {ESTOQUE_INICIAL}")
    print(f"Estoque final:          {estoque}")
    print(f"Total vendido:          {total_vendido}")
    print(f"Compras aprovadas:      {total_aprovadas}")
    print(f"Compras rejeitadas:     {total_rejeitadas}")
    print(f"Consistência (vendido == deduzido): "
          f"{total_vendido == ESTOQUE_INICIAL - estoque}")
    print(f"Estoque final não-negativo: {estoque >= 0}")


if __name__ == "__main__":
    main()