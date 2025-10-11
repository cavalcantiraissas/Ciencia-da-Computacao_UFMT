// Item.java
package model;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;

/**
 * Classe abstrata que representa um Item genérico
 * Serve como base para Livro e Filme
 */
public abstract class Item {
    protected String titulo;
    protected String descricao;
    protected LocalDate dataCadastro;
    
    public Item(String titulo, String descricao) {
        this.titulo = titulo;
        this.descricao = descricao;
        this.dataCadastro = LocalDate.now();
    }
    
    public Item(String titulo, String descricao, LocalDate dataCadastro) {
        this.titulo = titulo;
        this.descricao = descricao;
        this.dataCadastro = dataCadastro;
    }
    
    // Getters e Setters
    public String getTitulo() { return titulo; }
    public void setTitulo(String titulo) { this.titulo = titulo; }
    
    public String getDescricao() { return descricao; }
    public void setDescricao(String descricao) { this.descricao = descricao; }
    
    public LocalDate getDataCadastro() { return dataCadastro; }
    public void setDataCadastro(LocalDate dataCadastro) { this.dataCadastro = dataCadastro; }
    
    /**
     * Método abstrato para exibir detalhes específicos de cada tipo de item
     * @return String com detalhes formatados
     */
    public abstract String exibirDetalhes();
    
    /**
     * Converte o item para formato de exportação
     * @return String no formato para exportação
     */
    public String toExportString() {
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd");
        return String.format("%s|%s|%s|%s", 
            this.getClass().getSimpleName(),
            titulo,
            descricao,
            dataCadastro.format(formatter));
    }
    
    @Override
    public String toString() {
        return exibirDetalhes();
    }
}
