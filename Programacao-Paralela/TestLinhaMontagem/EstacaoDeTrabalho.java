import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;

public class EstacaoDeTrabalho {

    private final Queue<Integer> fila = new LinkedList<>();
    private final String nome;

    private final Semaphore lock;   // Mutex (permits=1) - exclusão mútua no acesso à fila
    private final Semaphore empty;  // Contagem (permits=CAPACIDADE) - espaços livres para depósito
    private final Semaphore full;   // Contagem (permits=0) - itens disponíveis para retirada

    public EstacaoDeTrabalho(String nome, int capacidade) {
        this.nome = nome;
        this.lock = new Semaphore(1);
        this.empty = new Semaphore(capacidade);
        this.full = new Semaphore(0);
    }

    public void depositar(int item) throws InterruptedException {
        empty.acquire();   // aguarda um espaço vazio

        lock.acquire();    // trava de exclusão mútua
        fila.add(item);
        System.out.println(nome + " -> depositado: " + item + " | Ocupação: " + fila.size());
        lock.release();

        full.release();    // sinaliza que tem item disponível
    }

    public int retirar() throws InterruptedException {
        full.acquire();    // aguarda um item completo

        lock.acquire();    // trava de exclusão mútua
        int item = fila.poll();
        System.out.println(nome + " -> retirado: " + item + " | Ocupação: " + fila.size());
        lock.release();

        empty.release();   // sinaliza que tem espaço disponível

        return item;
    }
}
