// em java, Crie uma classe chamada Carro com os seguintes atributos: marca (String), modelo (String), ano (int). Adicione um método exibirInfo() que imprime as informações do carro. No método main, crie duas instâncias de Carro e chame o método exibirInfo() para cada uma.

public class Carro {
    private String marca;
    private String modelo;
    private int ano;
    
    public Carro(String marca, String modelo, int ano) {
        this.marca = marca;
        this.modelo = modelo;
        this.ano = ano;
    }
    
    public void exibirInfo() {
        System.out.println("Marca: " + marca);
        System.out.println("Modelo: " + modelo);
        System.out.println("Ano: " + ano);
        System.out.println("----------------------");
    }
    
    public static void main(String[] args) {
        Carro carro1 = new Carro("Toyota", "Corolla", 2022);
        Carro carro2 = new Carro("Honda", "Civic", 2023);
        
        System.out.println("Informações do Carro 1:");
        carro1.exibirInfo();
        
        System.out.println("Informações do Carro 2:");
        carro2.exibirInfo();
    }
}
