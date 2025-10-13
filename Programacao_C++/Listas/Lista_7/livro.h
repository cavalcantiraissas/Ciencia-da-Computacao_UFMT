#ifndef LIVRO_H
#define LIVRO_H

#include <string>

using namespace std;

class Livro {
private:
    string ISBN;
    string titulo;
    bool disponivel;

public:
    Livro(const string& isbn = "", const string& tit = "", bool disp = true)
        : ISBN(isbn), titulo(tit), disponivel(disp) {}

    string getISBN() const;
    string getTitulo() const;
    bool isDisponivel() const;
    void emprestar();
    void devolver();
    void exibirDados() const;
};

#endif
