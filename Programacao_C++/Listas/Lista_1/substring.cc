/*
Universidade Federal de Mato Grosso
Programacao II || Professor Sandino
Discente: Raissa Cavalcanti || 16 de Maio de 2024

Atividade: Função que busca por uma substring dentro de uma string - 
Retorna verdadeiro se a substring for encontrada e a posição onde a substring começa
*/
#include <iostream> 
#include <string> 
using namespace std;

int main() {
    string str, substr; 

    cout << "Digite a string: ";
    getline(cin, str); 

    cout << "Digite a substring a ser buscada: ";
    getline(cin, substr); 

    size_t pos = str.find(substr);

    if (pos != string::npos) { 
        cout << "Substring encontrada na posição: " << pos << endl;
    } else { 
        cout << "Substring não encontrada." << endl;
    }

    return 0; 
}
