// Crie uma classe Livro com os atributos titulo e autor. Implemente dois construtores: Um sem parâmetros que define valores padrão. Um com parâmetros para inicializar os atributos. No método main, instancie dois objetos utilizando os dois construtores diferentes e exiba os dados.

public class Livro {
    private String titulo;
    private String autor;
    
    public Livro() {
        this.titulo = "Sem título";
        this.autor = "Autor desconhecido";
    }
    
    public Livro(String titulo, String autor) {
        this.titulo = titulo;
        this.autor = autor;
    }
    
    public String getTitulo() {
        return titulo;
    }
    
    public String getAutor() {
        return autor;
    }
    
    public void exibirDados() {
        System.out.println("Título: " + titulo);
        System.out.println("Autor: " + autor);
        System.out.println("-------------------");
    }
    
    public static void main(String[] args) {
        Livro livro1 = new Livro();
        
        Livro livro2 = new Livro("Dom Casmurro", "Machado de Assis");
        
        System.out.println("DADOS DOS LIVROS");
        System.out.println("\nLivro 1 (construtor padrão):");
        livro1.exibirDados();
        
        System.out.println("Livro 2 (construtor com parâmetros):");
        livro2.exibirDados();
        
        System.out.println("EXIBIÇÃO COM GETTERS");
        System.out.println("Livro 1:");
        System.out.println("Título: " + livro1.getTitulo());
        System.out.println("Autor: " + livro1.getAutor());
        System.out.println("-------------------");
        
        System.out.println("Livro 2:");
        System.out.println("Título: " + livro2.getTitulo());
        System.out.println("Autor: " + livro2.getAutor());
        System.out.println("-------------------");
    }
}
