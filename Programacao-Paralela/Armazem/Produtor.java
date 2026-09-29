public class Produtor implements Runnable {

    private final Armazem armazem;

    public Produtor(Armazem armazem) {
        this.armazem = armazem;
    }

    @Override
    public void run() {
        int pacote = 0;
        while (true) {
            try {
                armazem.depositar(pacote);
                pacote++;
                Thread.sleep(300);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
    }
}