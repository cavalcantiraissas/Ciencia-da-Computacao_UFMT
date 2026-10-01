public class OperarioA implements Runnable {

    private final EstacaoDeTrabalho estacaoA;

    public OperarioA(EstacaoDeTrabalho estacaoA) {
        this.estacaoA = estacaoA;
    }

    @Override
    public void run() {
        int parteBruta = 0;
        while (true) {
            try {
                Thread.sleep(300); // tempo para criar a parte bruta
                estacaoA.depositar(parteBruta);
                parteBruta++;
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
    }
}
