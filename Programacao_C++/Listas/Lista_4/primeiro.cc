/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 04
Aluna: Raissa C. Cavalcanti

Exercício UM 
*/

#include <iostream>
#include <string>

using namespace std;

class Produto {
private:
    string nome;
    double preco;
    int quantidade;

public:
    Produto(string nome, double preco, int quantidade) 
        : nome(nome), preco(preco), quantidade(quantidade) {}

    void setNome(string nome) {
        this->nome = nome;
    }

    string getNome() const {
        return nome;
    }

    void setPreco(double preco) {
        this->preco = preco;
    }

    double getPreco() const {
        return preco;
    }

    void setQuantidade(int quantidade) {
        this->quantidade = quantidade;
    }

    int getQuantidade() const {
        return quantidade;
    }

    void adicionarEstoque(int quantidade) {
        this->quantidade += quantidade;
    }

    void removerEstoque(int quantidade) {
        if (this->quantidade >= quantidade) {
            this->quantidade -= quantidade;
        } else {
            cout << "Quantidade insuficiente em estoque." << endl;
        }
    }

    void mostrarDados() const {
        cout << "Nome: " << nome << endl;
        cout << "Preço: " << preco << endl;
        cout << "Quantidade: " << quantidade << endl;
    }
};

int main() {
    Produto produto1("Notebook", 2500.0, 10);

    produto1.mostrarDados();

    produto1.adicionarEstoque(5);
    produto1.mostrarDados();

    produto1.removerEstoque(3);
    produto1.mostrarDados();

    produto1.removerEstoque(20);
    produto1.mostrarDados();

    return 0;
}
