/*
Universidade Federal de Mato Grosso
Programação II = Lista de Revisão
Aluna: Raissa Cavalcanti

-> Exercício Dois -> Anagramas

*/

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

bool Anagrama(const string& str1, const string& str2) {
    if (str1.length() != str2.length()) {
        return false;
    }
    
    string sorted_str1 = str1;
    string sorted_str2 = str2;
    sort(sorted_str1.begin(), sorted_str1.end());
    sort(sorted_str2.begin(), sorted_str2.end());
    
    return sorted_str1 == sorted_str2;
}

int main() {
    string str1, str2;
    
    cout << "Digite a primeira string: ";
    cin >> str1;
    cout << "Digite a segunda string: ";
    cin >> str2;
    
    if (Anagrama(str1, str2)) {
        cout << "As strings sao anagramas." << endl;
    } else {
        cout << "As strings nao sao anagramas." << endl;
    }
    
    return 0;
}
