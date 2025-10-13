/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 04
Aluna: Raissa C. Cavalcanti

Exercício QUATRO
*/

#include <iostream>
#include <string>

using namespace std;

class Carro {
private:
    string marca;
    string modelo;
    int ano;
    int quilometragem;

public:
    Carro(string m, string mod, int a, int km) : marca(m), modelo(mod), ano(a), quilometragem(km) {}

    void setMarca(string m) { marca = m; }
    string getMarca() const { return marca; }

    void setModelo(string mod) { modelo = mod; }
    string getModelo() const { return modelo; }

    void setAno(int a) { ano = a; }
    int getAno() const { return ano; }

    void setQuilometragem(int km) { quilometragem = km; }
    int getQuilometragem() const { return quilometragem; }

    void dirigir(int km) {
        if (km > 0) {
            quilometragem += km;
        } else {
            cout << "Quilometragem inválida." << endl;
        }
    }
    void mostrarDados() const {
        cout << "Marca: " << marca << ", Modelo: " << modelo
             << ", Ano: " << ano << ", Quilometragem: " << quilometragem << endl;
    }
};

int main() {
    Carro car1("Toyota", "Corolla", 2020, 15000);
    car1.mostrarDados(); // Marca: Toyota, Modelo: Corolla, Ano: 2020, Quilometragem: 15000

    car1.dirigir(500);
    car1.mostrarDados(); // Marca: Toyota, Modelo: Corolla, Ano: 2020, Quilometragem: 15500

    return 0;
}
