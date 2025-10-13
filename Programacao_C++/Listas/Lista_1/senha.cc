/*
Universidade Federal de Mato Grosso
Programacao II || Professor Sandino
Discente: Raissa Cavalcanti || 16 de Maio de 2024

Atividade: Escreva um programa que solicite ao usuário a criação de uma senha que 
atenda os seguintes requisitos:

-> Pelo menos dois números
-> Um caractere especial
-> Uma letra maiúscula
->Uma letra minúscula

a saída deve dar se a senha escolhida foi válida ou, em caso negativo, quais dos
requisitos ainda faltam ser atendidos e nao deve encerrar enquanto nao for criada
uma senha adequada
*/
#include <iostream>
using namespace std;


bool validar(const string& senha) { 
    int c_n = 0; 
    bool temCaractere = false;
    bool temMinuscula = false;
    bool temMaiuscula = false;

    for (char c : senha) { 
        if (isdigit(c)) { 
            c_n++;
        } else if ((c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~')) { 
            temCaractere = true;
        } else if (c >= 'a' && c <= 'z') { 
            temMinuscula = true;
        } else if (c >= 'A' && c <= 'Z') { 
            temMaiuscula = true;
        }
    }
    if (c_n < 2) {
        cout << "Senha inválida. Você precisa ter no mínimo dois números." << endl;
    }
    if (!temCaractere) {
        cout << "Senha inválida. Você precisa ter no mínimo um caractere especial." << endl;
    }
    if (!temMinuscula) {
        cout << "Senha inválida. Você precisa ter no mínimo uma letra minúscula." << endl;
    }
    if (!temMaiuscula) {
        cout << "Senha inválida. Você precisa ter no mínimo uma letra maiúscula." << endl;
    }

    return c_n >= 2 && temCaractere && temMinuscula && temMaiuscula;
}

int main() {
    string senha;

    do {
        cout << "Digite uma senha: ";
        cin >> senha;
        cout << endl;  

        if (validar(senha)) {
            cout << "Você digitou uma senha segura. Senha válida!" << endl;
            break;
        } else {
            cout << "Senha inválida. Por favor, tente novamente digitando uma senha mais segura." << endl;
        }
    } while (true); 

    return 0;
}
