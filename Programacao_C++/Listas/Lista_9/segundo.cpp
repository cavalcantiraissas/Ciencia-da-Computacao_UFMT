/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 09
Aluna: Raissa C. Cavalcanti

Segundo Exercício
*/

#include <iostream>
#include <vector>

using namespace std;

enum TipoProduto { TipoA, TipoB, TipoC, TotalTiposProduto };
enum NivelQualidade { Alta, Media, Baixa, TotalNiveisQualidade };

int main() {
    int produtos[TotalTiposProduto][TotalNiveisQualidade] = {};

    cout << "Digite as quantidades de produtos para cada tipo e nível de qualidade:\n";
    for (int tipo = 0; tipo < TotalTiposProduto; ++tipo) {
        for (int qualidade = 0; qualidade < TotalNiveisQualidade; ++qualidade) {
            cout << "Tipo " << tipo << ", Qualidade " << qualidade << ": ";
            cin >> produtos[tipo][qualidade];
        }
    }

    cout << "\nTotal de produtos de alta qualidade para cada tipo:\n";
    for (int tipo = 0; tipo < TotalTiposProduto; ++tipo) {
        cout << "Tipo " << tipo << ": " << produtos[tipo][Alta] << endl;
    }

    return 0;
}
