// Crie uma ArrayList<String> para armazenar nomes de alunos. Adicione pelo menos 5 nomes. Use um Iterator para percorrer e imprimir cada nome da lista. Em seguida, remova um nome e exiba novamente a lista atualizada.

import java.util.ArrayList;
import java.util.Iterator;

public class ListaAlunos {
    public static void main(String[] args) {
        ArrayList<String> alunos = new ArrayList<>();
        
        alunos.add("Ana Silva");
        alunos.add("Luiz Henrique");
        alunos.add("Pedro Pazeto");
        alunos.add("Daniel Costa");
        alunos.add("Raissa Cavalcanti");
        
        System.out.println("LISTA ORIGINAL DE ALUNOS");
        Iterator<String> iterator = alunos.iterator();
        while (iterator.hasNext()) {
            String nome = iterator.next();
            System.out.println("Aluno: " + nome);
        }
        
        System.out.println("\nRemovendo 'Daniel Costa' da lista...");
        alunos.remove("Daniel Costa");
        
        System.out.println("\nLISTA ATUALIZADA DE ALUNOS");
        iterator = alunos.iterator();
        while (iterator.hasNext()) {
            String nome = iterator.next();
            System.out.println("Aluno: " + nome);
        }
        
        System.out.println("\nINFORMAÇÕES DA LISTA");
        System.out.println("Total de alunos: " + alunos.size());
        System.out.println("Lista vazia? " + alunos.isEmpty());
        
        System.out.println("Ana Silva está na lista? " + alunos.contains("Ana Silva"));
        System.out.println("Daniel Costa está na lista? " + alunos.contains("Daniel Costa"));
    }
}
