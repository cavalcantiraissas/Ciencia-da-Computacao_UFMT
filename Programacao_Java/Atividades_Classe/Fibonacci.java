// Faça um programa que imprima os trinta primeiros elementos da série de Fibonacci. A série é a seguinte: 1, 1, 2, 3, 5, 8,13 etc. Para calculá-la, o primeiro e segundo elementos valem 1; daí por diante, cada elemento vale a soma dos dois elementos anteriores.

public class Fibonacci {
    public static void main(String[] args) {
        int n = 30; 
        long primeiro = 1;
        long segundo = 1;
        
        System.out.println("Os 30 primeiros números da sequência de Fibonacci:");
        
        for (int i = 1; i <= n; i++) {
            if (i == 1 || i == 2) {
                System.out.print("1");
            } else {
                long proximo = primeiro + segundo;
                System.out.print(proximo);
                primeiro = segundo;
                segundo = proximo;
            }
                        if (i < n) {
                System.out.print(", ");
            }
        }
    }
}
