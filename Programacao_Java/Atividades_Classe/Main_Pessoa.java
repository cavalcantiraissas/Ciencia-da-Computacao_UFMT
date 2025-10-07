//Crie uma classe Pessoa com os atributos nome e idade. Depois, crie a subclasse Aluno que herda de Pessoa e adiciona o atributo matricula. Crie um construtor para Aluno que receba todos os dados e utilize super() para inicializar nome e idade. Instancie um objeto de Aluno e mostre suas informações.

class Pessoa {
    private String nome;
    private int idade;
    
    public Pessoa() {
        this.nome = "Sem nome";
        this.idade = 0;
    }
    
    public Pessoa(String nome, int idade) {
        this.nome = nome;
        this.idade = idade;
    }
    
    public String getNome() {
        return nome;
    }
    
    public int getIdade() {
        return idade;
    }
    
    public void exibirInformacoes() {
        System.out.println("Nome: " + nome);
        System.out.println("Idade: " + idade);
    }
}

class Aluno extends Pessoa {
    private String matricula;
    
    public Aluno(String nome, int idade, String matricula) {
        super(nome, idade);
        this.matricula = matricula;
    }
    
    public String getMatricula() {
        return matricula;
    }
    
    @Override
    public void exibirInformacoes() {
        super.exibirInformacoes();
        System.out.println("Matrícula: " + matricula);
        System.out.println("-------------------");
    }
}

public class Main_Pessoa {
    public static void main(String[] args) {
        Aluno aluno1 = new Aluno("Maria Silva", 20, "2023001234");
        
        System.out.println("INFORMAÇÕES DO ALUNO");
        aluno1.exibirInformacoes();
        
        System.out.println("EXIBIÇÃO COM GETTERS");
        System.out.println("Nome: " + aluno1.getNome());
        System.out.println("Idade: " + aluno1.getIdade());
        System.out.println("Matrícula: " + aluno1.getMatricula());
        System.out.println("-------------------");
        
        Aluno aluno2 = new Aluno("João Santos", 22, "2023005678");
        
        System.out.println("SEGUNDO ALUNO");
        aluno2.exibirInformacoes();
    }
}
