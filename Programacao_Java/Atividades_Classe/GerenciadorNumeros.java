//Crie um programa em Java que: Utilize um ArrayList<Integer> para armazenar 10 números inteiros inseridos pelo usuário. Exiba todos os números. Calcule e mostre a soma total e a média dos números inseridos. Remova os números pares e exiba a lista atualizada.
//Dica: use métodos como add(), removeIf() e loops for ou for-each:

import java.util.ArrayList;
import java.util.Scanner;

public class GerenciadorNumeros {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        ArrayList<Integer> numeros = new ArrayList<>();
        
        System.out.println("Digite 10 números inteiros:");
        for (int i = 0; i < 10; i++) {
            System.out.print("Número " + (i + 1) + ": ");
            int numero = scanner.nextInt();
            numeros.add(numero);
        }
        
        System.out.println("\n Todos os números digitados");
        System.out.println("Lista completa: " + numeros);
        
        int soma = 0;
        for (int numero : numeros) {
            soma += numero;
        }
        
        double media = (double) soma / numeros.size();
        
        System.out.println("\nEstatísticas");
        System.out.println("Soma total: " + soma);
        System.out.println("Média: " + media);
        
        numeros.removeIf(numero -> numero % 2 == 0);
        
        System.out.println("\nLista após remover números pares");
        System.out.println("Lista atualizada: " + numeros);
        
        scanner.close();
    }
}
