// Crie uma classe ContaBancaria com o atributo numero. Sobrescreva os métodos toString() e equals(): Teste no main: Crie duas contas com números iguais e compare com equals(). Imprima um objeto diretamente para verificar o funcionamento do toString().

public class ContaBancariaDetalhada {
    private String numero;
    private double saldo;
    
    public ContaBancariaDetalhada(String numero, double saldo) {
        this.numero = numero;
        this.saldo = saldo;
    }
    
    @Override
    public String toString() {
        return String.format("Conta Bancária [Número: %s, Saldo: R$ %.2f]", numero, saldo);
    }
    
    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        
        ContaBancariaDetalhada that = (ContaBancariaDetalhada) obj;
        return numero.equals(that.numero);
    }
    
    @Override
    public int hashCode() {
        return numero.hashCode();
    }
    
    public static void main(String[] args) {
        ContaBancariaDetalhada contaA = new ContaBancariaDetalhada("99999-9", 1500.0);
        ContaBancariaDetalhada contaB = new ContaBancariaDetalhada("99999-9", 2000.0);
        
        System.out.println("contaA: " + contaA);
        System.out.println("contaB: " + contaB);
        System.out.println("São iguais? " + contaA.equals(contaB));
    }
}
