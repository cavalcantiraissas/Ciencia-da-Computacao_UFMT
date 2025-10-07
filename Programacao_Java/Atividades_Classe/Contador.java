// Crie uma classe Contador que possui: Um atributo estático totalObjetos para contar quantos objetos foram criados; Um construtor que incremente esse contador sempre que um novo objeto for instanciado; Um método estático mostrarTotal() que exibe o total de objetos criados. No main, crie alguns objetos e depois exiba o total utilizando o método estático.

public class Contador {
    private static int totalObjetos = 0;
    
    public Contador() {
        totalObjetos++;
        System.out.println("Novo objeto criado. Total: " + totalObjetos);
    }
    
    public static void mostrarTotal() {
        System.out.println("Total de objetos criados: " + totalObjetos);
    }
    
    public static void main(String[] args) {
        System.out.println("Criando objetos...");
        
        Contador obj1 = new Contador();
        Contador obj2 = new Contador();
        Contador obj3 = new Contador();
        Contador obj4 = new Contador();
        
        System.out.println(); 
        
        Contador.mostrarTotal();
        
    
        System.out.println("\nChamando diretamente pela classe:");
        Contador.mostrarTotal();
        
        System.out.println("\nCriando mais objetos...");
        Contador obj5 = new Contador();
        Contador obj6 = new Contador();
        
        System.out.println("\nTotal atualizado");
        Contador.mostrarTotal();
    }
}
