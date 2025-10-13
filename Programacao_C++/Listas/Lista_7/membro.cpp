#include "Membro.h"
#include <iostream>
#include <algorithm>

using namespace std;

int Membro::getID() const {
    return ID;
}

string Membro::getNome() const {
    return nome;
}

void Membro::exibirDados() const {
    cout << "ID: " << ID << "\nNome: " << nome << "\nLivros Emprestados: ";
    if (livrosEmprestados.empty()) {
        cout << "Nenhum livro emprestado.";
    } else {
        for (const auto& livro : livrosEmprestados) {
            cout << livro << " ";
        }
    }
    cout << endl;
}

const vector<string>& Membro::getLivrosEmprestados() const {
    return livrosEmprestados;
}

void Membro::emprestarLivro(const string& isbn) {
    livrosEmprestados.push_back(isbn);
}

void Membro::devolverLivro(const string& isbn) {
    auto it = remove(livrosEmprestados.begin(), livrosEmprestados.end(), isbn);
    livrosEmprestados.erase(it, livrosEmprestados.end());
}
