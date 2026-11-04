# NotificationSystem: pool de threads com monitoramento

**Simulação de um serviço de notificações em Java que processa 40 tarefas num `ThreadPoolExecutor` com fila limitada e política de rejeição própria, enquanto um `ScheduledExecutorService` publica as métricas do pool.**

[![Java](https://img.shields.io/badge/Java-17%2B-ED8B00?logo=openjdk&logoColor=white)](https://openjdk.org/)
[![Concorrência](https://img.shields.io/badge/concorr%C3%AAncia-ThreadPoolExecutor-555555)](#como-funciona)
[![Disciplina](https://img.shields.io/badge/disciplina-Programa%C3%A7%C3%A3o_Paralela-6f42c1)](../)

Atividade da disciplina de Programação Paralela (UFMT). O exercício mostra como um pool de threads cresce entre o número mínimo e o máximo de threads conforme a fila enche, o que acontece quando os dois limites são atingidos, e como encerrar os *executors* sem deixar threads vivas.

---

## Sumário

- [Como funciona](#como-funciona)
- [Sobre](#sobre)
- [Como compilar e executar](#como-compilar-e-executar)
- [Estrutura do projeto](#estrutura-do-projeto)

---

## Como funciona

```mermaid
flowchart LR
    T[40 notificações] --> Q[(ArrayBlockingQueue<br/>capacidade 20)]
    Q --> P[ThreadPoolExecutor<br/>4 a 8 threads<br/>keepAlive 30 s]
    Q -. fila e pool cheios .-> R[handler de rejeição<br/>emite alerta]
    M[ScheduledExecutorService<br/>a cada 2 s] -. lê ativas, concluídas<br/>e tamanho da fila .-> P
```

## Sobre

O programa simula o envio de 40 notificações, processadas por um pool de threads configurado com:

- **corePoolSize:** 4
- **maximumPoolSize:** 8
- **keepAliveTime:** 30 segundos
- **Fila de tarefas:** `ArrayBlockingQueue` com capacidade para 20 itens
- **Política de rejeição:** handler customizado que alerta quando o pool e a fila atingem o limite

Paralelamente, um `ScheduledExecutorService` monitora e imprime métricas (threads ativas, tarefas completadas e tamanho da fila) a cada 2 segundos.

Ao final da execução, o sistema aguarda a conclusão de todas as tarefas e encerra os executors de forma controlada, evitando vazamento de threads.

## Como compilar e executar

```bash
javac NotificationSystem.java
java NotificationSystem
```

## Estrutura do projeto

| Arquivo | Descrição |
|---|---|
| `NotificationSystem.java` | Código-fonte do sistema de notificações |

---

[← Programação Paralela](../) · [Repositório principal](../../)
