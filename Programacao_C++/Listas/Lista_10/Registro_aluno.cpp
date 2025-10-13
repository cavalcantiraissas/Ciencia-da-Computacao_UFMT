#include <iostream>
#include <vector>
#include <string>
using namespace std;

template<typename T>
class Registro {
private:
    struct Aluno {
        string nome;
        T nota;
    };
    vector<Aluno> alunos;

public:
    void adicionarAluno(const string& nome, T nota) {
        alunos.push_back({nome, nota});
    }

    T calcularMedia() {
        T soma = 0;
        for (const auto& aluno : alunos) {
            soma += aluno.nota;
        }
        return alunos.empty() ? 0 : soma / alunos.size();
    }

    void exibirAlunos() {
        for (const auto& aluno : alunos) {
            cout << "Nome: " << aluno.nome << ", Nota: " << aluno.nota << endl;
        }
    }
};

int main() {
    Registro<int> registroInt;
    registroInt.adicionarAluno("Maria", 90);
    registroInt.adicionarAluno("João", 85);

    registroInt.exibirAlunos();
    cout << "Média: " << registroInt.calcularMedia() << endl;

    return 0;
}
