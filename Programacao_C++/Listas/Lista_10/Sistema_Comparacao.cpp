#include <iostream>
#include <string>
#include <functional>
using namespace std;

template<typename T>
class Item {
private:
    string nome;
    T valor;

public:
    Item(const string& nome, T valor) : nome(nome), valor(valor) {}

    bool operator<(const Item& outro) const {
        return std::less<T>()(this->valor, outro.valor);
    }

    void exibirItem() const {
        cout << "Item: " << nome << ", Valor: " << valor << endl;
    }
};

int main() {
    Item<int> item1("Item A", 10);
    Item<int> item2("Item B", 20);

    item1.exibirItem();
    item2.exibirItem();

    if (item1 < item2) {
        cout << "Item A é menor que Item B." << endl;
    } else {
        cout << "Item A não é menor que Item B." << endl;
    }

    return 0;
}
