/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 09
Aluna: Raissa C. Cavalcanti

Terceiro Exercício
*/

#include <iostream>
#include <algorithm> 
using namespace std;

enum class PeriodoDoDia {
    Manha,
    Tarde,
    Noite
};

string periodoDoDiaParaString(PeriodoDoDia periodo) {
    switch (periodo) {
        case PeriodoDoDia::Manha: return "Manhã";
        case PeriodoDoDia::Tarde: return "Tarde";
        case PeriodoDoDia::Noite: return "Noite";
        default: return "Desconhecido";
    }
}

int main() {
    float temperaturas[3];

    cout << "Digite as medições de temperatura para Manhã, Tarde e Noite:\n";
    for (int i = 0; i < 3; ++i) {
        cout << periodoDoDiaParaString(static_cast<PeriodoDoDia>(i)) << ": ";
        cin >> temperaturas[i];
    }

    float tempMax = *max_element(temperaturas, temperaturas + 3);
    float tempMin = *min_element(temperaturas, temperaturas + 3);
    float variacao = tempMax - tempMin;

    cout << "Variação de temperatura ao longo do dia: " << variacao << " graus.\n";

    return 0;
}
