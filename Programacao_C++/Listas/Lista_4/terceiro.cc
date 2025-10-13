/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 04
Aluna: Raissa C. Cavalcanti

Exercício TRÊS
*/

#include <iostream>
#include <string>

using namespace std;

class ContaBancaria {
private:
    string numeroConta;
    double saldo;
    string titular;

public:
    ContaBancaria(string n, double s, string t) : numeroConta(n), saldo(s), titular(t) {}

    void setNumeroConta(string n) { numeroConta = n; }
    string getNumeroConta() const { return numeroConta; }

    void setSaldo(double s) { saldo = s; }
    double getSaldo() const { return saldo; }

    void setTitular(string t) { titular = t; }
    string getTitular() const { return titular; }

    void depositar(double valor) {
        if (valor > 0) {
            saldo += valor;
        } else {
            cout << "Valor de depósito inválido." << endl;
        }
    }

    void sacar(double valor) {
        if (valor > 0 && valor <= saldo) {
            saldo -= valor;
        } else {
            cout << "Saldo insuficiente ou valor de saque inválido." << endl;
        }
    }

    void mostrarDados() const {
        cout << "Conta: " << numeroConta << ", Titular: " << titular
             << ", Saldo: " << saldo << endl;
    }
};

int main() {
    ContaBancaria c1("12345-6", 1000.0, "Joao Silva");
    c1.mostrarDados(); // Conta: 12345-6, Titular: Joao Silva, Saldo: 1000

    c1.depositar(500.0);
    c1.sacar(300.0);
    c1.mostrarDados(); // Conta: 12345-6, Titular: Joao Silva, Saldo: 1200

    return 0;
}
