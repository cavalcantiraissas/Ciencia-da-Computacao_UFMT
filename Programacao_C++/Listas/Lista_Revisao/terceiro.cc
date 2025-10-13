/*
Universidade Federal de Mato Grosso
Programação II = Lista de Revisão
Aluna: Raissa Cavalcanti

-> Exercício Três -> Palíndromo

*/

#include <iostream>
#include <string>
#include <cctype>

using namespace std;

bool Palindromo(const string& str) {
    int esquerda = 0;
    int direita = str.length() - 1;
    
    while (esquerda < direita) {
        while (!isalnum(str[esquerda]) && esquerda < direita) {
            esquerda++;
        }
        while (!isalnum(str[direita]) && esquerda < direita) {
            direita--;
        }
        
        char ch1 = tolower(str[esquerda]);
        char ch2 = tolower(str[direita]);
        
        if (ch1 != ch2) {
            return false;
        }
        
        esquerda++;
        direita--;
    }
    
    return true;
}

int main() {
    string entrada;
    
    cout << "Digite uma string: ";
    getline(cin, entrada);
    
    if (Palindromo(entrada)) {
        cout << "A string '" << entrada << "' é um palindromo." << endl;
    } else {
        cout << "A string '" << entrada << "' nao é um palindromo." << endl;
    }
    
    return 0;
}
