/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 04
Aluna: Raissa C. Cavalcanti

Exercício UM
*/

#include <iostream>
#include <vector>

using namespace std;

class Animal {
public:
    virtual void emitirSom() const = 0; 
    virtual ~Animal() {}
};

class Cachorro : public Animal {
public:
    void emitirSom() const override {
        cout << "Au Au" << endl;
    }
};

class Gato : public Animal {
public:
    void emitirSom() const override {
        cout << "Miau" << endl;
    }
};

class Vaca : public Animal {
public:
    void emitirSom() const override {
        cout << "Muuu" << endl;
    }
};

int main() {
    Cachorro cachorro;
    Gato gato;
    Vaca vaca;

    vector<Animal*> animais;
    animais.push_back(&cachorro);
    animais.push_back(&gato);
    animais.push_back(&vaca);

    for (Animal* animal : animais) {
        animal->emitirSom();
    }

    return 0;
}
