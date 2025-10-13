#include <iostream>
#include <queue>
using namespace std;

template<typename T>
class Fila {
private:
    queue<T> fila;

public:
    void enfileirar(T elemento) {
        fila.push(elemento);
    }

    void desenfileirar() {
        if (!fila.empty()) {
            fila.pop();
        } else {
            cout << "A fila está vazia!" << endl;
        }
    }

    bool estaVazia() const {
        return fila.empty();
    }

    void mostrarFila() {
        queue<T> temp = fila;
        while (!temp.empty()) {
            cout << temp.front() << " ";
            temp.pop();
        }
        cout << endl;
    }
};

int main() {
    Fila<int> filaInt;
    filaInt.enfileirar(1);
    filaInt.enfileirar(2);
    filaInt.mostrarFila();

    filaInt.desenfileirar();
    filaInt.mostrarFila();

    return 0;
}
