#include "Biblioteca.h"
#include <iostream>
#include <limits>

using namespace std;

void limparBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.clear();
}

void exibirMenu() {
    cout << "\nMenu:\n"
              << "1. Registrar livro\n"
              << "2. Registrar membro\n"
              << "3. Listar livros\n"
              << "4. Listar membros\n"
              << "5. Emprestar livro\n"
              << "6. Devolver livro\n"
              << "7. Sair\n\n"
              << "Escolha uma opção: ";
}

void registrarLivro(Biblioteca& biblioteca) {
    string isbn;
    string titulo;

    cout << "\nDigite o ISBN do livro:\n";
    cin >> isbn;
    limparBuffer();

    cout << "\nDigite o título do livro:\n";
    getline(cin, titulo); 

    biblioteca.registrarLivro(Livro(isbn, titulo));
    cout << "\nLivro registrado com sucesso!" << endl;
}

void registrarMembro(Biblioteca& biblioteca) {
    int id;
    string nome;

    cout << "\nDigite o ID do membro:\n";
    cin >> id;
    limparBuffer(); 

    cout << "\nDigite o nome do membro:\n";
    getline(cin, nome);

    biblioteca.registrarMembro(Membro(id, nome));
    cout << "\nMembro registrado com sucesso!" << endl;
}

void listarLivros(Biblioteca& biblioteca) {
    biblioteca.listarLivros();
}

void listarMembros(Biblioteca& biblioteca) {
    biblioteca.listarMembros();
}

void emprestarLivro(Biblioteca& biblioteca) {
    string isbn;
    int idMembro;

    cout << "Digite o ISBN do livro para emprestar:\n";
    getline(cin, isbn); 

    cout << "Digite o ID do membro:\n";
    cin >> idMembro;
    limparBuffer();

    biblioteca.emprestarLivro(isbn, idMembro);
    cout << "Empréstimo realizado com sucesso!" << endl;
}

void devolverLivro(Biblioteca& biblioteca) {
    string isbn;
    int idMembro;

    cout << "Digite o ISBN do livro para devolver:\n";
    getline(cin, isbn); 

    cout << "Digite o ID do membro:\n";
    cin >> idMembro;
    limparBuffer(); 

    biblioteca.devolverLivro(isbn, idMembro);
    cout << "Devolução realizada com sucesso!" << endl;
}

int main() {
    Biblioteca biblioteca;

    biblioteca.carregarLivros("livros.txt");
    biblioteca.carregarMembros("membros.txt");

    int opcao = 0;

    do {
        exibirMenu();
        cin >> opcao;
        limparBuffer();

        switch (opcao) {
            case 1:
                registrarLivro(biblioteca);
                break;
            case 2:
                registrarMembro(biblioteca);
                break;
            case 3:
                listarLivros(biblioteca);
                break;
            case 4:
                listarMembros(biblioteca);
                break;
            case 5:
                emprestarLivro(biblioteca);
                break;
            case 6:
                devolverLivro(biblioteca);
                break;
            case 7:
                biblioteca.gravarLivros("livros.txt");
                biblioteca.gravarMembros("membros.txt");
                cout << "Dados salvos. Saindo do programa." << endl;
                break;
            default:
                cout << "Opção inválida. Tente novamente." << endl;
                break;
        }
    } while (opcao != 7);

    return 0;
}
