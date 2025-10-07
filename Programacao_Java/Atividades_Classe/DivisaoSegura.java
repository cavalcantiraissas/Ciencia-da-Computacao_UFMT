//Escreva um programa que leia dois números inteiros e divida o primeiro pelo segundo. Use try-catch para tratar ArithmeticException (divisão por zero). Exiba uma mensagem amigável caso ocorra erro.

import java.util.Scanner;

public class DivisaoSegura {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        
        try {
            System.out.print("Digite o primeiro número (dividendo): ");
            int dividendo = scanner.nextInt();
            
            System.out.print("Digite o segundo número (divisor): ");
            int divisor = scanner.nextInt();
            
            int resultado = dividendo / divisor;
            
            System.out.println("Resultado da divisão: " + resultado);
            
        } catch (ArithmeticException e) {
            System.out.println("Erro: Não é possível dividir por zero!");
            System.out.println("Por favor, digite um divisor diferente de zero.");
            
        } catch (Exception e) {
            System.out.println("Erro: Entrada inválida!");
            System.out.println("Por favor, digite apenas números inteiros.");
            
        } finally {
            scanner.close();
            System.out.println("Programa finalizado.");
        }
    }
}
