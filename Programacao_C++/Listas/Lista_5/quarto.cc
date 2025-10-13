/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 04
Aluna: Raissa C. Cavalcanti

Exercício QUATRO
*/

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

class Midia {
public:
    virtual void exibirInfo() const = 0; 
    virtual void reproduzir() const = 0; 
    virtual ~Midia() {}                  
};

class Livro : public Midia {
private:
    string titulo;
    string autor;
    int numPaginas;
public:
    Livro(const string& t, const string& a, int n)
        : titulo(t), autor(a), numPaginas(n) {}

    void exibirInfo() const override {
        cout << "Livro: " << titulo << ", Autor: " << autor << ", Páginas: " << numPaginas << endl;
    }

    void reproduzir() const override {
        cout << "Lendo o livro \"" << titulo << "\" por " << autor << "." << endl;
    }
};

class CD : public Midia {
private:
    string titulo;
    string artista;
    int numFaixas;
public:
    CD(const string& t, const string& a, int n)
        : titulo(t), artista(a), numFaixas(n) {}

    void exibirInfo() const override {
        cout << "CD: " << titulo << ", Artista: " << artista << ", Faixas: " << numFaixas << endl;
    }

    void reproduzir() const override {
        cout << "Reproduzindo o CD \"" << titulo << "\" de " << artista << "." << endl;
    }
};


class DVD : public Midia {
private:
    string titulo;
    string diretor;
    int duracao; // em minutos
public:
    DVD(const string& t, const string& d, int dur)
        : titulo(t), diretor(d), duracao(dur) {}

    void exibirInfo() const override {
        cout << "DVD: " << titulo << ", Diretor: " << diretor << ", Duração: " << duracao << " minutos" << endl;
    }

    void reproduzir() const override {
        cout << "Reproduzindo o DVD \"" << titulo << "\", dirigido por " << diretor << "." << endl;
    }
};

int main() {
    vector<shared_ptr<Midia>> midias;
    midias.push_back(make_shared<Livro>("O Senhor dos Anéis", "J.R.R. Tolkien", 1200));
    midias.push_back(make_shared<CD>("Abbey Road", "The Beatles", 17));
    midias.push_back(make_shared<DVD>("Inception", "Christopher Nolan", 148));

    for (const auto& midia : midias) {
        midia->exibirInfo();
        midia->reproduzir();
        cout << endl;
    }

    return 0;
}
