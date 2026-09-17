/*
Universidade Federal de Mato Grosso
Disciplina: Programação Paralela
Aluna: Raissa Cavalcanti
*/

// Sistema de Notificações com ThreadPoolExecutor e monitoramento via ScheduledExecutorService

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Executors;
import java.util.concurrent.RejectedExecutionHandler;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class NotificationSystem {

    public static void main(String[] args) throws InterruptedException {

        // ThreadPoolExecutor com fila limitada e política de rejeição
        BlockingQueue<Runnable> filaDeTarefas = new ArrayBlockingQueue<>(20);

        RejectedExecutionHandler politicaDeRejeicao = new RejectedExecutionHandler() {
            @Override
            public void rejectedExecution(Runnable tarefaRejeitada, ThreadPoolExecutor executor) {
                System.out.println("[ALERTA] Notificação rejeitada! Pool e fila estão no limite máximo. "
                        + "Ativas: " + executor.getActiveCount()
                        + " | Fila: " + executor.getQueue().size());
            }
        };

        ThreadPoolExecutor workerPool = new ThreadPoolExecutor(
                4,                          // corePoolSize
                8,                          // maximumPoolSize
                30,                         // keepAliveTime
                TimeUnit.SECONDS,
                filaDeTarefas,
                politicaDeRejeicao
        );

        //Configurar o ScheduledExecutorService para telemetria
        ScheduledExecutorService monitorService = Executors.newScheduledThreadPool(1);

        //Agendar o monitoramento a cada 2 segundos
        monitorService.scheduleAtFixedRate(() -> {
            System.out.println("--- MÉTRICAS ---"
                    + " Threads ativas: " + workerPool.getActiveCount()
                    + " | Tarefas completadas: " + workerPool.getCompletedTaskCount()
                    + " | Tamanho da fila: " + workerPool.getQueue().size());
        }, 2, 2, TimeUnit.SECONDS);

        //Submeter 40 tarefas de notificação (simulando envio)
        for (int i = 1; i <= 40; i++) {
            final int taskId = i;
            workerPool.execute(() -> {
                try {
                    int atraso = 300 + (int) (Math.random() * 500); // 300 a 800ms
                    Thread.sleep(atraso);
                    System.out.println("Notificação " + taskId + " enviada por "
                            + Thread.currentThread().getName() + " (atraso: " + atraso + "ms)");
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            });
        }

        //Shutdown
        workerPool.shutdown();
        workerPool.awaitTermination(1, TimeUnit.MINUTES);

        monitorService.shutdown();
        monitorService.awaitTermination(1, TimeUnit.MINUTES);

        System.out.println("Todas as notificações foram processadas. Sistema encerrado sem vazamento de threads.");
    }
}