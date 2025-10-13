/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 04
Aluna: Raissa C. Cavalcanti

Exercício DOIS
*/

#include <iostream>
#include <string>

using namespace std;

class Livro {
private:
    string titulo;
    string autor;
    int anoPublicacao;
    int numPaginas;

public:
    Livro(string t, string a, int ano, int paginas)
        : titulo(t), autor(a), anoPublicacao(ano), numPaginas(paginas) {}

    void setTitulo(string t) { titulo = t; }
    string getTitulo() const { return titulo; }

    void setAutor(string a) { autor = a; }
    string getAutor() const { return autor; }

    void setAnoPublicacao(int ano) { anoPublicacao = ano; }
    int getAnoPublicacao() const { return anoPublicacao; }

    void setNumPaginas(int paginas) { numPaginas = paginas; }
    int getNumPaginas() const { return numPaginas; }

    void mostrarDados() const {
        cout << "Titulo: " << titulo << ", Autor: " << autor
             << ", Ano: " << anoPublicacao << ", Paginas: " << numPaginas << endl;
    }
};

int main() {
    Livro l1("1984", "George Orwell", 1949, 328);
    l1.mostrarDados(); // Titulo: 1984, Autor: George Orwell, Ano: 1949, Paginas: 328

    Livro l2("Fogo Morto", "Graciliano Ramos", 1933, 229);
    l2.mostrarDados(); // Titulo: Fogo Morto, Autor: Graciliano Ramos, Ano: 1933, Paginas: 229

    l2.setNumPaginas(299);
    l2.mostrarDados(); // Titulo: Fogo Morto, Autor: Graciliano Ramos, Ano: 1933, Paginas: 299

    return 0;
}
