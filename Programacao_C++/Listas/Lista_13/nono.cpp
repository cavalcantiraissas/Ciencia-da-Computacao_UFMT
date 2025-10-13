#include <iostream>
#include <unordered_map>
#include <string>

int main() {
    std::unordered_map<char, int> ocorrencias;
    std::string palavra = "abracadabra";

    for (const auto &ch : palavra)
        ocorrencias[ch]++;

    for (const auto &par : ocorrencias)
        std::cout << par.first << ": " << par.second << std::endl;

    return 0;
}
