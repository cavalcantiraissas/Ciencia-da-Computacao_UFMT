public class Montador implements Runnable {

    private final EstacaoDeTrabalho estacaoA;
    private final EstacaoDeTrabalho estacaoB;

    public Montador(EstacaoDeTrabalho estacaoA, EstacaoDeTrabalho estacaoB) {
        this.estacaoA = estacaoA;
        this.estacaoB = estacaoB;
    }

    @Override
    public void run() {
        while (true) {
            try {
                int parteBruta = estacaoA.retirar();

                Thread.sleep(500); // tempo de montagem
                int componenteMontado = parteBruta; // transforma a parte bruta em componente montado

                estacaoB.depositar(componenteMontado);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
    }
}
