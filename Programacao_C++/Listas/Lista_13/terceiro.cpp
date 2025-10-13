#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> frutas{{"maçã", 5}, {"banana", 3}, {"laranja", 8}};

    for (const auto &par : frutas)
        std::cout << par.first << ": " << par.second << std::endl;

    for (auto &par : frutas)
        par.second += 2;

    for (const auto &par : frutas)
        std::cout << par.first << ": " << par.second << std::endl;

    return 0;
}
