#include <iostream>
#include <queue>
#include <string>

int main() {
    std::queue<std::string> fila;

    fila.push("primeiro");
    fila.push("segundo");
    fila.push("terceiro");

    while (!fila.empty()) {
        std::cout << fila.front() << " ";
        fila.pop();
    }

    return 0;
}
