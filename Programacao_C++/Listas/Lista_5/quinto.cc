/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 04
Aluna: Raissa C. Cavalcanti

Exercício CINCO
*/

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

class Transporte {
public:
    virtual void iniciar() const = 0; 
    virtual void parar() const = 0;  
    virtual ~Transporte() {}          
};

class Carro : public Transporte {
public:
    void iniciar() const override {
        cout << "O carro está ligando." << endl;
    }

    void parar() const override {
        cout << "O carro está desligando." << endl;
    }
};

class Bicicleta : public Transporte {
public:
    void iniciar() const override {
        cout << "A bicicleta está começando a pedalar." << endl;
    }

    void parar() const override {
        cout << "A bicicleta está parando de pedalar." << endl;
    }
};

class Aviao : public Transporte {
public:
    void iniciar() const override {
        cout << "O avião está decolando." << endl;
    }

    void parar() const override {
        cout << "O avião está aterrissando." << endl;
    }
};

int main() {
    vector<shared_ptr<Transporte>> transportes;
    transportes.push_back(make_shared<Carro>());
    transportes.push_back(make_shared<Bicicleta>());
    transportes.push_back(make_shared<Aviao>());

    for (const auto& transporte : transportes) {
        transporte->iniciar();
        transporte->parar();
        cout << endl;
    }

    return 0;
}
