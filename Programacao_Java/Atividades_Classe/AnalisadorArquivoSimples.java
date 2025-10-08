// Crie um programa que: Solicite ao usuário um caminho para um arquivo. Utilize a classe File para: Verificar se o arquivo existe. Exibir se ele é um arquivo ou um diretório. Mostrar o tamanho do arquivo em bytes e o caminho absoluto.
//Só para os pro: Se for um diretório, liste os arquivos contidos nele.

import java.io.File;
import java.util.Scanner;

public class AnalisadorArquivoSimples {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        
        System.out.print("Digite o caminho do arquivo/diretório: ");
        String caminho = scanner.nextLine();
        
        File arquivo = new File(caminho);
        
        if (!arquivo.exists()) {
            System.out.println("❌ Arquivo/diretório não encontrado!");
            return;
        }
        
        System.out.println("📍 Caminho absoluto: " + arquivo.getAbsolutePath());
        System.out.println("📝 É arquivo? " + arquivo.isFile());
        System.out.println("📁 É diretório? " + arquivo.isDirectory());
        
        if (arquivo.isFile()) {
            System.out.println("📊 Tamanho: " + arquivo.length() + " bytes");
        }
        
        if (arquivo.isDirectory()) {
            System.out.println("\n📂 Conteúdo do diretório:");
            File[] arquivos = arquivo.listFiles();
            
            if (arquivos != null) {
                for (File file : arquivos) {
                    String tipo = file.isDirectory() ? "[DIR] " : "[ARQ] ";
                    System.out.println(tipo + file.getName());
                }
            }
        }
        
        scanner.close();
    }
}
