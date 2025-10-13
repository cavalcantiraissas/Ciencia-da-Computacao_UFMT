#include "Biblioteca.h"
#include "Livro.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

void Biblioteca::carregarLivros(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    string isbn, titulo, dispo;
    while (arquivo >> isbn) {
        getline(arquivo, titulo);
        bool disponivel = (titulo.find("Disponível") != string::npos);
        titulo = titulo.substr(0, titulo.find("Disponível") - 1);
        livros[isbn] = Livro(isbn, titulo, disponivel);
    }
}

void Biblioteca::carregarMembros(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    int id;
    string nome, isbn;
    while (arquivo >> id >> ws && getline(arquivo, nome)) {
        Membro membro(id, nome);
        while (arquivo >> isbn) {
            membro.emprestarLivro(isbn);
        }
        membros[id] = membro;
    }
}

void Biblioteca::registrarLivro(const Livro& livro) {
    livros[livro.getISBN()] = livro;
}

void Biblioteca::registrarMembro(const Membro& membro) {
    membros[membro.getID()] = membro;
}

void Biblioteca::gravarLivros(const string& nomeArquivo) const {
    ofstream arquivo(nomeArquivo);
    for (const auto& [isbn, livro] : livros) {
        arquivo << isbn << " " << livro.getTitulo() << " "
                << (livro.isDisponivel() ? "Disponível" : "Emprestado") << endl;
    }
}

void Biblioteca::gravarMembros(const string& nomeArquivo) const {
    ofstream arquivo(nomeArquivo);
    for (const auto& [id, membro] : membros) {
        arquivo << id << " " << membro.getNome();
        for (const auto& livro : membro.getLivrosEmprestados()) {
            arquivo << " " << livro;
        }
        arquivo << endl;
    }
}

void Biblioteca::listarLivros() const {
    for (const auto& [isbn, livro] : livros) {
        livro.exibirDados();
    }
}

void Biblioteca::listarMembros() const {
    for (const auto& [id, membro] : membros) {
        membro.exibirDados();
    }
}

void Biblioteca::emprestarLivro(const string& isbn, int idMembro) {
    if (livros.find(isbn) == livros.end()) {
        cerr << "Livro não encontrado." << endl;
        return;
    }
    if (membros.find(idMembro) == membros.end()) {
        cerr << "Membro não encontrado." << endl;
        return;
    }

    Livro& livro = livros[isbn];
    Membro& membro = membros[idMembro];

    if (!livro.isDisponivel()) {
        cerr << "Livro não está disponível." << endl;
        return;
    }

    livro.emprestar();
    membro.emprestarLivro(isbn);
}

void Biblioteca::devolverLivro(const string& isbn, int idMembro) {
    if (livros.find(isbn) == livros.end()) {
        cerr << "Livro não encontrado." << endl;
        return;
    }
    if (membros.find(idMembro) == membros.end()) {
        cerr << "Membro não encontrado." << endl;
        return;
    }

    Livro& livro = livros[isbn];
    Membro& membro = membros[idMembro];

    livro.devolver();
    membro.devolverLivro(isbn);
}
