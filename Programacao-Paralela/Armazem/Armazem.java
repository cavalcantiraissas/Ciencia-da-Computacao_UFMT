import java.util.LinkedList;
import java.util.List;

public class Armazem {

    private static final int CAPACIDADE_MAXIMA = 10;
    private final List<Integer> pacotes = new LinkedList<>();

    public synchronized void depositar(int pacote) throws InterruptedException {
        while (pacotes.size() == CAPACIDADE_MAXIMA) {
            System.out.println("Armazém cheio! Produtor aguardando...");
            wait();
        }

        pacotes.add(pacote);
        System.out.println("Depositado: pacote " + pacote + " | Ocupação: " + pacotes.size() + "/" + CAPACIDADE_MAXIMA);

        notifyAll();
    }

    public synchronized int retirar() throws InterruptedException {
        while (pacotes.isEmpty()) {
            System.out.println("Armazém vazio! Consumidor aguardando...");
            wait();
        }

        int pacote = pacotes.remove(0);
        System.out.println("Retirado: pacote " + pacote + " | Ocupação: " + pacotes.size() + "/" + CAPACIDADE_MAXIMA);

        notifyAll();

        return pacote;
    }
}