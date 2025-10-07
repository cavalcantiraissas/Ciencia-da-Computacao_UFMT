// Crie uma classe Animal com um método emitirSom(). Crie duas subclasses: Cachorro e Gato. Sobrescreva o método emitirSom() para imprimir sons diferentes. No main, crie um array de Animal contendo um Cachorro e um Gato, e percorra chamando emitirSom() para cada um.

class Animal {
    public void emitirSom() {
        System.out.println("Animal emitindo um som genérico");
    }
}

class Cachorro extends Animal {
    @Override
    public void emitirSom() {
        System.out.println("Au au! Latido do cachorro");
    }
}

class Gato extends Animal {
    @Override
    public void emitirSom() {
        System.out.println("Miau! Miado do gato");
    }
}

public class Main_Animal {
    public static void main(String[] args) {
        Animal[] animais = new Animal[2];
        animais[0] = new Cachorro();
        animais[1] = new Gato();
        
        System.out.println("=== SONS DOS ANIMAIS ===");
        for (int i = 0; i < animais.length; i++) {
            System.out.print("Animal " + (i + 1) + ": ");
            animais[i].emitirSom();
        }
        
        System.out.println("-------------------");
        
        System.out.println("MAIS ANIMAIS");
        Animal[] maisAnimais = {
            new Cachorro(),
            new Gato(),
            new Cachorro(),
            new Gato()
        };
        
        for (int i = 0; i < maisAnimais.length; i++) {
            System.out.print("Animal " + (i + 1) + " - ");
            maisAnimais[i].emitirSom();
        }
        
        System.out.println("-------------------");
        
        System.out.println("DEMONSTRAÇÃO DO POLIMORFISMO");
        Animal animal1 = new Cachorro();
        Animal animal2 = new Gato();
        
        System.out.print("Animal1 (referência Animal, objeto Cachorro): ");
        animal1.emitirSom();
        
        System.out.print("Animal2 (referência Animal, objeto Gato): ");
        animal2.emitirSom();
        
        System.out.println("CHAMADAS DIRETAS");
        Cachorro cachorro = new Cachorro();
        Gato gato = new Gato();
        
        cachorro.emitirSom();
        gato.emitirSom();
    }
}
