// Desenvolva um programa que: Leia múltiplas linhas do teclado usando Scanner. Grave todas as linhas digitadas em um arquivo chamado saida.txt usando PrintStream. O programa deve encerrar quando o usuário digitar a palavra FIM (sem gravá-la no arquivo).

import java.io.*;
import java.util.Scanner;

public class GravadorArquivo {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        PrintStream printStream = null;
        
        try {
            printStream = new PrintStream("saida.txt");
            
            System.out.println("Digite as linhas de texto (digite 'FIM' para encerrar):");
            System.out.println("======================================================");
            
            String linha;
            int contadorLinhas = 0;
            
            while (true) {
                System.out.print("Linha " + (contadorLinhas + 1) + ": ");
                linha = scanner.nextLine();
                
                if (linha.equalsIgnoreCase("FIM")) {
                    break;
                }
                
                printStream.println(linha);
                contadorLinhas++;
            }
            
            System.out.println("======================================================");
            System.out.println(contadorLinhas + " linhas gravadas com sucesso no arquivo saida.txt!");
            
        } catch (FileNotFoundException e) {
            System.err.println("Erro: Não foi possível criar o arquivo saida.txt");
            System.err.println("Detalhes: " + e.getMessage());
        } catch (SecurityException e) {
            System.err.println("Erro de segurança: Permissão negada para criar o arquivo");
            System.err.println("Detalhes: " + e.getMessage());
        } catch (Exception e) {
            System.err.println("Erro inesperado: " + e.getMessage());
        } finally {
            if (printStream != null) {
                printStream.close();
            }
            scanner.close();
        }
    }
}
