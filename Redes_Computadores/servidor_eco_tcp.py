import socket
import sys


def main():
    host = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 7000

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as servidor:
            servidor.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            servidor.bind((host, port))
            servidor.listen()
            print(f"Servidor de eco escutando em {host}:{port}")

            while True:
                conexao, endereco = servidor.accept()
                with conexao:
                    print(f"Cliente conectado: {endereco[0]}:{endereco[1]}")

                    while True:
                        dados = conexao.recv(4096)
                        if not dados:
                            break

                        mensagem = dados.decode("utf-8")
                        if mensagem.lower() in ("sair", "exit", "quit"):
                            break

                        print(f"Recebido: {mensagem}")
                        conexao.sendall(dados)

                    print(f"Cliente desconectado: {endereco[0]}:{endereco[1]}")

    except KeyboardInterrupt:
        print("\nServidor encerrado.")
    except OSError as erro:
        print(f"Erro de socket: {erro}")


if __name__ == "__main__":
    main()
