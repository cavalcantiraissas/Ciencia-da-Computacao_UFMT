// Crie um programa que: Peça ao usuário o nome de um arquivo de texto. Utilize FileInputStream, InputStreamReader e BufferedReader para ler o conteúdo linha a linha. Exiba o conteúdo completo no console.
//Dica: Trate exceções com try-catch e feche os streams corretamente.

import java.io.*;

public class LeitorArquivoTexto {
    public static void main(String[] args) {
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        
        FileInputStream fileInputStream = null;
        InputStreamReader inputStreamReader = null;
        BufferedReader bufferedReader = null;
        
        try {
            System.out.print("Digite o nome do arquivo de texto: ");
            String nomeArquivo = reader.readLine();
            
            fileInputStream = new FileInputStream(nomeArquivo);
            inputStreamReader = new InputStreamReader(fileInputStream);
            bufferedReader = new BufferedReader(inputStreamReader);
            
            System.out.println("\nCONTEÚDO DO ARQUIVO");
            
            String linha;
            int numeroLinha = 1;
            
            while ((linha = bufferedReader.readLine()) != null) {
                System.out.printf("%03d: %s\n", numeroLinha, linha);
                numeroLinha++;
            }
            
            System.out.println("FIM DO ARQUIVO");
            
        } catch (FileNotFoundException e) {
            System.err.println("Erro: Arquivo não encontrado!");
            System.err.println("Verifique se o nome do arquivo está correto e se ele existe.");
        } catch (IOException e) {
            System.err.println("Erro de E/S (IO): " + e.getMessage());
        } catch (Exception e) {
            System.err.println("Erro inesperado: " + e.getMessage());
        } finally {
            try {
                if (bufferedReader != null) {
                    bufferedReader.close();
                }
                if (inputStreamReader != null) {
                    inputStreamReader.close();
                }
                if (fileInputStream != null) {
                    fileInputStream.close();
                }
                if (reader != null) {
                    reader.close();
                }
            } catch (IOException e) {
                System.err.println("Erro ao fechar os streams: " + e.getMessage());
            }
        }
    }
}
