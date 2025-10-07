//Crie um programa que: Leia diversas palavras do teclado (até o usuário digitar “fim”). Armazene essas palavras em um HashSet<String>. Exiba todas as palavras únicas digitadas (sem repetições). Verifique se a palavra “Java” foi digitada.
//Dica: use contains() e loops para percorrer o conjunto

import java.util.HashSet;
import java.util.Scanner;

public class GerenciadorPalavras {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        HashSet<String> palavras = new HashSet<>();
        
        System.out.println("Digite palavras (digite 'fim' para encerrar):");
        
        while (true) {
            System.out.print("Palavra: ");
            String palavra = scanner.nextLine().trim();
            
            if (palavra.equalsIgnoreCase("fim")) {
                break;
            }
            
            palavras.add(palavra);
        }
        
        System.out.println("\nPalavras únicas digitadas");
        if (palavras.isEmpty()) {
            System.out.println("Nenhuma palavra foi digitada.");
        } else {
            System.out.println("Total de palavras únicas: " + palavras.size());
            System.out.println("Lista de palavras:");
            
            int contador = 1;
            for (String palavra : palavras) {
                System.out.println(contador + ". " + palavra);
                contador++;
            }
        }
        
        System.out.println("\nVerificação da palavra 'Java'");
        if (palavras.contains("Java")) {
            System.out.println("A palavra 'Java' foi encontrada no conjunto!");
        } else {
            System.out.println("A palavra 'Java' NÃO foi encontrada no conjunto.");
        }
        
        scanner.close();
    }
}
