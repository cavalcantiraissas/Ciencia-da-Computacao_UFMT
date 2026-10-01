public class Embalador implements Runnable {

    private final EstacaoDeTrabalho estacaoB;

    public Embalador(EstacaoDeTrabalho estacaoB) {
        this.estacaoB = estacaoB;
    }

    @Override
    public void run() {
        while (true) {
            try {
                int componenteMontado = estacaoB.retirar();
                Thread.sleep(400); // tempo de embalagem
                System.out.println("Produto finalizado: " + componenteMontado);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
    }
}
