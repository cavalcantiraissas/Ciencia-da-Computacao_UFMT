// Implemente um programa que: Utilize um HashMap<String, Integer> para armazenar pares nome → idade. Permita cadastrar pelo menos 5 pessoas. Peça um nome ao usuário e exiba a idade correspondente, se existir. Remova uma pessoa pelo nome e mostre o mapa atualizado.
//Dica: use métodos put(), get(), remove() e containsKey().
 
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

public class CadastroPessoas {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        HashMap<String, Integer> pessoas = new HashMap<>();
        
        System.out.println("CADASTRO DE PESSOAS ");
        
        System.out.println("\nCadastro de 5 pessoas");
        for (int i = 1; i <= 5; i++) {
            System.out.print("Digite o nome da pessoa " + i + ": ");
            String nome = scanner.nextLine();
            
            if (nome == null || nome.trim().isEmpty()) {
                System.out.println("Nome inválido! Digite novamente.");
                i--;
                continue;
            }
            
            System.out.print("Digite a idade de " + nome + ": ");
            int idade = scanner.nextInt();
            scanner.nextLine();
            
            pessoas.put(nome, idade);
            System.out.println("✅ " + nome + " cadastrado(a) com sucesso!\n");
        }
        
        System.out.println("Todas as pessoas cadastradas");
        exibirMapa(pessoas);
        
        System.out.println("\nBuscar idade por nome ");
        System.out.print("Digite um nome para buscar: ");
        String nomeBusca = scanner.nextLine();
        
        if (pessoas.containsKey(nomeBusca)) {
            int idade = pessoas.get(nomeBusca);
            System.out.println("✅ " + nomeBusca + " tem " + idade + " anos.");
        } else {
            System.out.println("❌ " + nomeBusca + " não encontrado.");
        }
        
        System.out.println("\nRemover pessoa");
        System.out.print("Digite o nome da pessoa a ser removida: ");
        String nomeRemover = scanner.nextLine();
        
        if (pessoas.containsKey(nomeRemover)) {
            int idadeRemovida = pessoas.remove(nomeRemover);
            System.out.println("✅ " + nomeRemover + " (" + idadeRemovida + " anos) foi removido(a).");
        } else {
            System.out.println("❌ " + nomeRemover + " não encontrado.");
        }
        
        System.out.println("\nCadastro atualizado");
        exibirMapa(pessoas);
        
        scanner.close();
    }
    
    public static void exibirMapa(HashMap<String, Integer> mapa) {
        System.out.println("Total: " + mapa.size());
        System.out.println("------------------------");
        
        for (Map.Entry<String, Integer> entry : mapa.entrySet()) {
            System.out.println("Nome: " + entry.getKey() + " | Idade: " + entry.getValue());
        }
    }
}
