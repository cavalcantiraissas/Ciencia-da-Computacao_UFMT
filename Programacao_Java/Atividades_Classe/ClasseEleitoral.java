// faça um programa que dada a idade de uma pessoa verifique sua classe eleitoral:
//menor que 16 nao pode votar, com 16 ou 17 ou mais que 65 anos votar é facultativo, entre 19 e 65 (inclsuive) votar é obrigatorio

import java.util.Scanner;

public class ClasseEleitoral {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        
        System.out.print("Digite a idade: ");
        int idade = scanner.nextInt();
        
        if (idade < 16) {
            System.out.println("NÃO PODE VOTAR");
        } else if (idade <= 17 || idade > 65) {
            System.out.println("VOTO FACULTATIVO");
        } else {
            System.out.println("VOTO OBRIGATÓRIO");
        }
        
        scanner.close();
    }
}
