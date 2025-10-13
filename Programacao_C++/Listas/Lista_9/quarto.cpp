/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 09
Aluna: Raissa C. Cavalcanti

Quarto Exercício
*/


#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum class Nutriente {
    Calorias,
    Proteinas,
    Carboidratos,
    TotalNutrientes 
};

enum class Alimento {
    Maca,
    Banana,
    PeitoDeFrango,
    TotalAlimentos 
};

string obterNomeNutriente(Nutriente nutriente) {
    switch (nutriente) {
        case Nutriente::Calorias: return "Calorias";
        case Nutriente::Proteinas: return "Proteínas";
        case Nutriente::Carboidratos: return "Carboidratos";
        default: return "Desconhecido";
    }
}

string obterNomeAlimento(Alimento alimento) {
    switch (alimento) {
        case Alimento::Maca: return "Maçã";
        case Alimento::Banana: return "Banana";
        case Alimento::PeitoDeFrango: return "Peito de Frango";
        default: return "Desconhecido";
    }
}

int main() {
    vector<vector<int>> infoNutricional(static_cast<int>(Alimento::TotalAlimentos), vector<int>(static_cast<int>(Nutriente::TotalNutrientes)));

    infoNutricional[static_cast<int>(Alimento::Maca)] = {95, 0, 25};         
    infoNutricional[static_cast<int>(Alimento::Banana)] = {105, 1, 27};     
    infoNutricional[static_cast<int>(Alimento::PeitoDeFrango)] = {165, 31, 0}; 

    int escolhaAlimento;
    cout << "Escolha um alimento para consultar (0 - Maçã, 1 - Banana, 2 - Peito de Frango): ";
    cin >> escolhaAlimento;

    if (escolhaAlimento < 0 || escolhaAlimento >= static_cast<int>(Alimento::TotalAlimentos)) {
        cout << "Alimento inválido!" << endl;
        return 1;
    }

    Alimento alimentoEscolhido = static_cast<Alimento>(escolhaAlimento);

    cout << "Informações nutricionais para " << obterNomeAlimento(alimentoEscolhido) << ":" << endl;
    for (int i = 0; i < static_cast<int>(Nutriente::TotalNutrientes); ++i) {
        cout << obterNomeNutriente(static_cast<Nutriente>(i)) << ": " << infoNutricional[escolhaAlimento][i] << endl;
    }

    int totalCalorias = 0;
    char maisAlimentos;
    do {
        cout << "Deseja adicionar mais alimentos à refeição? (S/N): ";
        cin >> maisAlimentos;
        if (maisAlimentos == 'S' || maisAlimentos == 's') {
            cout << "Escolha outro alimento (0 - Maçã, 1 - Banana, 2 - Peito de Frango): ";
            cin >> escolhaAlimento;

            if (escolhaAlimento >= 0 && escolhaAlimento < static_cast<int>(Alimento::TotalAlimentos)) {
                totalCalorias += infoNutricional[escolhaAlimento][static_cast<int>(Nutriente::Calorias)];
            } else {
                cout << "Alimento inválido!" << endl;
            }
        }
    } while (maisAlimentos == 'S' || maisAlimentos == 's');

    cout << "Total de calorias consumidas na refeição: " << totalCalorias << " calorias." << endl;

    return 0;
}
