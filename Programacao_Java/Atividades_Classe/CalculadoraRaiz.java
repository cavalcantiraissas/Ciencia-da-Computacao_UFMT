//Implemente um programa que contenha: Um método calcularRaiz(int numero) que lance uma exceção (IllegalArgumentException) caso o número seja negativo. A cláusula throws na assinatura do método para indicar a possibilidade da exceção. No método main, capture a exceção com try-catch e mostre uma mensagem adequada ao usuário

import java.util.Scanner;

public class CalculadoraRaiz {
    
    public static double calcularRaiz(int numero) throws IllegalArgumentException {
        if (numero < 0) {
            throw new IllegalArgumentException("Não é possível calcular a raiz quadrada de número negativo!");
        }
        return Math.sqrt(numero);
    }
    
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        
        try {
            System.out.print("Digite um número para calcular a raiz quadrada: ");
            int numero = scanner.nextInt();
            
            double resultado = calcularRaiz(numero);
            
            System.out.printf("A raiz quadrada de %d é: %.2f%n", numero, resultado);
            
        } catch (IllegalArgumentException e) {
            System.out.println("Erro: " + e.getMessage());
            System.out.println("Por favor, digite um número não negativo.");
            
        } catch (Exception e) {
            System.out.println("Erro: Entrada inválida!");
            System.out.println("Por favor, digite apenas números inteiros.");
            
        } finally {
            scanner.close();
            System.out.println("Programa finalizado.");
        }
    }
}
