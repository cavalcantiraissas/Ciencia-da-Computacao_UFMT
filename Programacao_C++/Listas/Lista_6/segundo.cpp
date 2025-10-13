/*
Universidade Federal de Mato Grosso
Disciplina: Programação II
Aluna: Raissa Cavalcanti
*/

// Segundo Exercício :  Vetores 2D

#include <iostream>

using namespace std;

class Vector2D {
private:
    double x, y;
public:
    Vector2D(double x = 0, double y = 0) : x(x), y(y) {}

    // Sobrecarga do operador +
    Vector2D operator+(const Vector2D &v) const {
        return Vector2D(x + v.x, y + v.y);
    }

    // Sobrecarga do operador -
    Vector2D operator-(const Vector2D &v) const {
        return Vector2D(x - v.x, y - v.y);
    }

    // Sobrecarga do operador ==
    bool operator==(const Vector2D &v) const {
        return (x == v.x && y == v.y);
    }

    // Sobrecarga do operador <<
    friend ostream& operator<<(ostream &out, const Vector2D &v) {
        out << "(" << v.x << ", " << v.y << ")";
        return out;
    }

    // Sobrecarga do operador >>
    friend istream& operator>>(istream &in, Vector2D &v) {
        in >> v.x >> v.y;
        return in;
    }
};

int main() {
    Vector2D v1(3.5, 2.0);
    Vector2D v2(1.5, 4.0);

    Vector2D v3 = v1 + v2;
    cout << "Soma de v1 e v2: " << v3 << endl;

    Vector2D v4 = v1 - v2;
    cout << "Diferença entre v1 e v2: " << v4 << endl;

    if (v1 == v2) {
        cout << "v1 e v2 são iguais." << endl;
    } else {
        cout << "v1 e v2 são diferentes." << endl;
    }

    Vector2D v5;
    cout << "Digite um vetor (formato: x y): ";
    cin >> v5;
    cout << "Vetor lido: " << v5 << endl;

    return 0;
}
