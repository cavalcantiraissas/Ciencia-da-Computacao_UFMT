#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include "Livro.h"
#include "Membro.h"
#include <map>
#include <string>

using namespace std;

class Biblioteca {
private:
    map<string, Livro> livros;
    map<int, Membro> membros;

public:
    void carregarLivros(const string& nomeArquivo);
    void carregarMembros(const string& nomeArquivo);
    void registrarLivro(const Livro& livro);
    void registrarMembro(const Membro& membro);
    void gravarLivros(const string& nomeArquivo) const;
    void gravarMembros(const string& nomeArquivo) const;
    void listarLivros() const;
    void listarMembros() const;
    void emprestarLivro(const string& isbn, int idMembro);
    void devolverLivro(const string& isbn, int idMembro);
};

#endif // BIBLIOTECA_H
