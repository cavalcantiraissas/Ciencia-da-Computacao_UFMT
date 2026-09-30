import socket
import sys


def main():
    host = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 7000

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as cliente:
            cliente.connect((host, port))
            print(f"Conectado ao servidor {host}:{port}")

            while True:
                mensagem = input("Digite uma mensagem ('sair' para encerrar): ")

                if mensagem.lower() in ("sair", "exit", "quit"):
                    cliente.sendall(mensagem.encode("utf-8"))
                    break

                cliente.sendall(mensagem.encode("utf-8"))
                eco = cliente.recv(4096)
                print(f"Eco do servidor: {eco.decode('utf-8')}")

            print("Conexão encerrada.")

    except ConnectionRefusedError:
        print(f"Erro: não foi possível conectar em {host}:{port}.")
    except OSError as erro:
        print(f"Erro de socket: {erro}")


if __name__ == "__main__":
    main()
