from socket import *
import sys

SERVER = "whois.registro.br" # Servidor WHOIS do Registro.br
PORT = 43 # Porta padrão do protocolo WHOIS

if len(sys.argv) != 2:
    print("Usage: python3 quem.py <server_ip>")
    sys.exit(1)

domain = sys.argv[1] # Domínio a ser consultado
sock = socket(AF_INET, SOCK_STREAM) # Cria um socket TCP
sock.connect((SERVER, PORT)) # Conecta ao servidor WHOIS
m = (domain + "\r\n").encode() # Codifica o domínio em bytes
sock.send(m) # Envia o domínio para o servidor
data = sock.recv(4096) # Recebe a resposta do servidor
print("Resposta do servidor WHOIS:")
print(data.decode()) # Decodifica e imprime a resposta 
sock.close() # Fecha o socket
