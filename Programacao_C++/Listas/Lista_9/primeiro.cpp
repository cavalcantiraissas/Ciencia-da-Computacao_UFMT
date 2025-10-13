/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 09
Aluna: Raissa C. Cavalcanti

Primeiro Exercício
*/

#include <iostream>
using namespace std;

enum TipoPlanta { Cactos, Samambaias, Orquideas, TotalPlantas };

int main() {
    int niveisUmidade[TotalPlantas] = {0};
    int umidadeTotal = 0;
    
    cout << "Digite os níveis de umidade do solo para diferentes tipos de plantas:" << endl;
    
    for(int i = 0; i < TotalPlantas; ++i) {
        cout << "Digite o nível de umidade para ";
        switch(i) {
            case Cactos: cout << "Cactos: "; break;
            case Samambaias: cout << "Samambaias: "; break;
            case Orquideas: cout << "Orquídeas: "; break;
        }
        cin >> niveisUmidade[i];
        umidadeTotal += niveisUmidade[i];
    }
    
    double mediaUmidade = static_cast<double>(umidadeTotal) / TotalPlantas;
    cout << "Média de umidade de todas as plantas monitoradas: " << mediaUmidade << endl;
    
    return 0;
}
