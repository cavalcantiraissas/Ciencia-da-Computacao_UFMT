# NotificationSystem

Sistema de notificações simulado em Java, utilizando `ThreadPoolExecutor` para o processamento paralelo das tarefas e `ScheduledExecutorService` para monitoramento periódico das métricas do pool.

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

## Arquivos

| Arquivo | Descrição |
|---|---|
| `NotificationSystem.java` | Código-fonte do sistema de notificações |

---

[Voltar para Programação Paralela](../README.md)
