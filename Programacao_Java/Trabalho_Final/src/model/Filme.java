// Filme.java
package model;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

/**
 * Classe que representa um Filme, herda de Item
 * Armazena informações específicas de filmes
 */

public class Filme extends Item {
    private String diretor;
    private int duracaoMinutos;
    
    public Filme(String titulo, String descricao, String diretor, int duracaoMinutos) {
        super(titulo, descricao);
        this.diretor = diretor;
        this.duracaoMinutos = duracaoMinutos;
    }
    
    public Filme(String titulo, String descricao, LocalDate dataCadastro, String diretor, int duracaoMinutos) {
        super(titulo, descricao, dataCadastro);
        this.diretor = diretor;
        this.duracaoMinutos = duracaoMinutos;
    }
    
    // Getters e Setters
    public String getDiretor() { return diretor; }
    public void setDiretor(String diretor) { this.diretor = diretor; }
    
    public int getDuracaoMinutos() { return duracaoMinutos; }
    public void setDuracaoMinutos(int duracaoMinutos) { 
        if (duracaoMinutos <= 0) {
            throw new IllegalArgumentException("Duração deve ser maior que zero");
        }
        this.duracaoMinutos = duracaoMinutos; 
    }
    
    /**
     * Exibe os detalhes formatados do filme
     * @return String formatada com todos os detalhes
     */
    @Override
    public String exibirDetalhes() {
        return String.format("FILME: %s\nDiretor: %s\nDuração: %d min\nDescrição: %s\nCadastrado em: %s",
            titulo, diretor, duracaoMinutos, descricao, dataCadastro);
    }
    
    /**
     * Converte o filme para formato de exportação
     * @return String no formato para exportação
     */

    @Override
    public String toExportString() {
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd");
        return String.format("%s|%s|%s|%s|%s|%d", 
            this.getClass().getSimpleName(),
            titulo,
            descricao,
            dataCadastro.format(formatter),
            diretor,
            duracaoMinutos);
    }
}
