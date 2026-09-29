public class Consumidor implements Runnable {

    private final Armazem armazem;

    public Consumidor(Armazem armazem) {
        this.armazem = armazem;
    }

    @Override
    public void run() {
        while (true) {
            try {
                armazem.retirar();
                Thread.sleep(500);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
    }
}