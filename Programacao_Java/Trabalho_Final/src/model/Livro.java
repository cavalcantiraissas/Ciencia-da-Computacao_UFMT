// Livro.java
package model;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

/**
 * Classe que representa um Livro, herda de Item
 * Armazena informações específicas de livros
 */
public class Livro extends Item {
    private String autor;
    private int numeroPaginas;
    
    public Livro(String titulo, String descricao, String autor, int numeroPaginas) {
        super(titulo, descricao);
        this.autor = autor;
        this.numeroPaginas = numeroPaginas;
    }
    
    public Livro(String titulo, String descricao, LocalDate dataCadastro, String autor, int numeroPaginas) {
        super(titulo, descricao, dataCadastro);
        this.autor = autor;
        this.numeroPaginas = numeroPaginas;
    }
    
    // Getters e Setters
    public String getAutor() { return autor; }
    public void setAutor(String autor) { this.autor = autor; }
    
    public int getNumeroPaginas() { return numeroPaginas; }
    public void setNumeroPaginas(int numeroPaginas) { 
        if (numeroPaginas <= 0) {
            throw new IllegalArgumentException("Número de páginas deve ser maior que zero");
        }
        this.numeroPaginas = numeroPaginas; 
    }
    
    /**
     * Exibe os detalhes formatados do livro
     * @return String formatada com todos os detalhes
     */
    @Override
    public String exibirDetalhes() {
        return String.format("LIVRO: %s\nAutor: %s\nPáginas: %d\nDescrição: %s\nCadastrado em: %s",
            titulo, autor, numeroPaginas, descricao, dataCadastro);
    }
    
    /**
     * Converte o livro para formato de exportação
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
            autor,
            numeroPaginas);
    }
}
