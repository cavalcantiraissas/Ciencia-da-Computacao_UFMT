// Crie uma classe Calculadora com métodos somar() sobrecarregados para: dois inteiros, dois doubles, três inteiros. No main, teste cada versão do método e exiba os resultados.

public class CalculadoraSimples {
    
    public int somar(int a, int b) {
        return a + b;
    }
    
    public double somar(double a, double b) {
        return a + b;
    }
    
    public int somar(int a, int b, int c) {
        return a + b + c;
    }
    
    public static void main(String[] args) {
        CalculadoraSimples calc = new CalculadoraSimples();
        
        System.out.println("Soma de inteiros: " + calc.somar(5, 3));
        System.out.println("Soma de doubles: " + calc.somar(5.5, 3.2));
        System.out.println("Soma de três inteiros: " + calc.somar(1, 2, 3));
        System.out.println("Soma com promoção: " + calc.somar(5, 3.5)); // int promovido para double
    }
}
