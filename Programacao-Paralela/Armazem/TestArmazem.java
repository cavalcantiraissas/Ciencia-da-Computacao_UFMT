public class TestArmazem {

    public static void main(String[] args) {
        Armazem armazem = new Armazem();

        Thread produtor1 = new Thread(new Produtor(armazem), "Produtor-1");
        Thread produtor2 = new Thread(new Produtor(armazem), "Produtor-2");
        Thread consumidor1 = new Thread(new Consumidor(armazem), "Consumidor-1");

        produtor1.start();
        produtor2.start();
        consumidor1.start();
    }
}