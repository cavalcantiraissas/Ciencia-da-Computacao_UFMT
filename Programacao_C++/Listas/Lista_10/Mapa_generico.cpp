#include <iostream>
#include <map>
#include <string>
using namespace std;

template<typename K, typename V>
class Mapa {
private:
    map<K, V> mapa;

public:
    void adicionarPar(const K& chave, const V& valor) {
        mapa[chave] = valor;
    }

    V buscarValor(const K& chave) {
        if (mapa.find(chave) != mapa.end()) {
            return mapa[chave];
        } else {
            throw runtime_error("Chave não encontrada!");
        }
    }

    void listarChaves() const {
        for (const auto& par : mapa) {
            cout << "Chave: " << par.first << endl;
        }
    }
};

int main() {
    Mapa<string, int> mapa;
    mapa.adicionarPar("João", 85);
    mapa.adicionarPar("Maria", 90);

    mapa.listarChaves();

    try {
        cout << "Nota de João: " << mapa.buscarValor("João") << endl;
    } catch (const exception& e) {
        cout << e.what() << endl;
    }

    return 0;
}
