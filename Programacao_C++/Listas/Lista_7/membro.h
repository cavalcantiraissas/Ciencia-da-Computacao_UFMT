#ifndef MEMBRO_H
#define MEMBRO_H

using namespace std;

#include <string>
#include <vector>

class Membro {
private:
    int ID;
    string nome;
    vector<string> livrosEmprestados;

public:
    Membro(int id = 0, const string& nom = "")
        : ID(id), nome(nom) {}

    int getID() const;
    string getNome() const;
    void exibirDados() const;
    const vector<string>& getLivrosEmprestados() const;
    void emprestarLivro(const string& isbn);
    void devolverLivro(const string& isbn);
};

#endif // MEMBRO_H
#ifndef MEMBRO_H
#define MEMBRO_H

using namespace std;

#include <string>
#include <vector>

class Membro {
private:
    int ID;
    string nome;
    vector<string> livrosEmprestados;

public:
    Membro(int id = 0, const string& nom = "")
        : ID(id), nome(nom) {}

    int getID() const;
    string getNome() const;
    void exibirDados() const;
    const vector<string>& getLivrosEmprestados() const;
    void emprestarLivro(const string& isbn);
    void devolverLivro(const string& isbn);
};

#endif // MEMBRO_H
