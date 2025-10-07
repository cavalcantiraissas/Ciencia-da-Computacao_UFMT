//Solicite ao usuário um número inteiro e tente converter uma String para int. Utilize try-catch para tratar NumberFormatException (ex.: se o usuário digitar letras). Utilize um bloco finally para exibir uma mensagem do tipo: "Encerrando programa...", garantindo que essa linha sempre seja executada, independentemente de erros.

import java.util.Scanner;

public class ConversorStringParaInt {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        
        try {
            System.out.print("Digite um número inteiro: ");
            String entrada = scanner.nextLine();
            
            int numero = Integer.parseInt(entrada);
            
            System.out.println("Número convertido com sucesso: " + numero);
            System.out.println("O dobro do número é: " + (numero * 2));
            
        } catch (NumberFormatException e) {
            System.out.println("Erro: A entrada não é um número inteiro válido!");
            System.out.println("Por favor, digite apenas números inteiros (ex: 123, -45, 0)");
            System.out.println("Mensagem técnica: " + e.getMessage());
            
        } finally {
            System.out.println("Encerrando programa...");
            scanner.close();
        }
    }
}
