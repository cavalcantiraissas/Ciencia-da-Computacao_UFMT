#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Produto {
private:
    float preco;
    string nome;
    string categoria;

public:
    Produto(float p, string n, string c) : preco(p), nome(n), categoria(c) {}

    float getPreco() const { return preco; }
    string getNome() const { return nome; }
    string getCategoria() const { return categoria; }

    bool operator<(const Produto& p) const {
        return this->preco < p.preco;
    }

    bool operator>(const Produto& p) const {
        return this->preco > p.preco;
    }
};

bool compararPorNome(const Produto& p1, const Produto& p2) {
    return p1.getNome() < p2.getNome();
}

bool compararPorCategoria(const Produto& p1, const Produto& p2) {
    return p1.getCategoria() < p2.getCategoria();
}

void mostrarMenu() {
    cout << "Escolha o critério de ordenação:\n";
    cout << "1: Ordenar por preço\n";
    cout << "2: Ordenar por nome\n";
    cout << "3: Ordenar por categoria\n";
    cout << "Escolha: ";
}

void adicionarProduto(vector<Produto>& produtos) {
    float preco;
    string nome, categoria;

    cout << "Digite o preço do produto: ";
    cin >> preco;
    cout << "Digite o nome do produto: ";
    cin >> nome;
    cout << "Digite a categoria do produto: ";
    cin >> categoria;

    produtos.push_back(Produto(preco, nome, categoria));
}

int main() {
    vector<Produto> produtos;
    int opcao;
    bool ordemCrescente;

    adicionarProduto(produtos);
    adicionarProduto(produtos);

    mostrarMenu();
    cin >> opcao;

    cout << "Deseja ordenar em ordem crescente (1) ou decrescente (0)? ";
    cin >> ordemCrescente;

    if (opcao == 1) {
        if (ordemCrescente)
            sort(produtos.begin(), produtos.end(), less<Produto>());
        else
            sort(produtos.begin(), produtos.end(), greater<Produto>());
    } else if (opcao == 2) {
        if (ordemCrescente)
            sort(produtos.begin(), produtos.end(), compararPorNome);
        else
            sort(produtos.rbegin(), produtos.rend(), compararPorNome);
    } else if (opcao == 3) {
        if (ordemCrescente)
            sort(produtos.begin(), produtos.end(), compararPorCategoria);
        else
            sort(produtos.rbegin(), produtos.rend(), compararPorCategoria);
    }

    for (const auto& p : produtos) {
        cout << "Produto: " << p.getNome() << ", Preço: " << p.getPreco() << ", Categoria: " << p.getCategoria() << endl;
    }

    return 0;
}
