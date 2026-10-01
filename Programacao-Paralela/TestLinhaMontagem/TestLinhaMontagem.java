public class TestLinhaMontagem {

    public static void main(String[] args) {
        EstacaoDeTrabalho estacaoA = new EstacaoDeTrabalho("Estação A (Partes Brutas)", 4);
        EstacaoDeTrabalho estacaoB = new EstacaoDeTrabalho("Estação B (Componentes Montados)", 3);

        Thread operarioA = new Thread(new OperarioA(estacaoA), "OperarioA");
        Thread montador = new Thread(new Montador(estacaoA, estacaoB), "Montador");
        Thread embalador = new Thread(new Embalador(estacaoB), "Embalador");

        operarioA.start();
        montador.start();
        embalador.start();
    }
}
