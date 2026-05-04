# Trabalho - Aplicação de Listas, Pilha e Fila

Trabalho prático de aplicação integrada das estruturas de dados lineares estudadas na disciplina. O sistema simula um contexto real de gerenciamento de pedidos, utilizando pilha e fila como mecanismos de controle de fluxo, com separação clara entre interface (`main.c`), estruturas de dados (`.h`/`.c`) e entidade de domínio (`pedido`).

---

## Estrutura do Projeto

```
Trabalho_Aplicacao-de-listas-pilha-fila/
├── main.c          # Ponto de entrada e lógica da aplicação
├── pedido.h        # Definição da struct Pedido
├── pedido.c        # Implementação das operações sobre Pedido
├── fila.h          # Interface da estrutura Fila
├── fila.c          # Implementação da Fila
├── pilha.h         # Interface da estrutura Pilha
├── pilha.c         # Implementação da Pilha
└── Trab-Lista_Fila_Pilha-pratica.pdf   # Enunciado do trabalho
```

---

## Como Compilar e Executar

```bash
gcc main.c pedido.c fila.c pilha.c -o aplicacao
./aplicacao
```

---

[Voltar para Estrutura de Dados em C](../README.md)
