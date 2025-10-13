/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 04
Aluna: Raissa C. Cavalcanti

Exercício CINCO
*/

#include <iostream>
#include <string>

using namespace std;

class Aluno {
private:
    string nome;
    int matricula;
    string curso;
    float nota;

public:
    Aluno(string n, int m, string c, float no) : nome(n), matricula(m), curso(c), nota(no) {}

    void setNome(string n) { nome = n; }
    string getNome() const { return nome; }

    void setMatricula(int m) { matricula = m; }
    int getMatricula() const { return matricula; }

    void setCurso(string c) { curso = c; }
    string getCurso() const { return curso; }

    void setNota(float no) { nota = no; }
    float getNota() const { return nota; }

    float calcularMedia() const {
        return nota;
    }

    bool verificarAprovacao() const {
        return nota >= 6.0;
    }

    void mostrarDados() const {
        cout << "Nome: " << nome << ", Matricula: " << matricula
             << ", Curso: " << curso << ", Nota: " << nota << endl;
    }
};

int main() {
    Aluno a1("Maria Silva", 202345, "Engenharia", 7.5);
    a1.mostrarDados(); // Nome: Maria Silva, Matricula: 202345, Curso: Engenharia, Nota: 7.5

    cout << "Media: " << a1.calcularMedia() << endl; // Media: 7.5
    cout << "Aprovado: " << (a1.verificarAprovacao() ? "Sim" : "Nao") << endl; // Aprovado: Sim

    return 0;
}
