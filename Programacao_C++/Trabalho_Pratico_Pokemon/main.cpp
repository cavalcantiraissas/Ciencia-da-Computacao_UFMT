#include "jogo.h"
#include <iostream>

int main() {
    std::string nickname;
    std::cout << "Digite seu nickname: ";
    std::cin >> nickname;

    Jogo jogo(nickname);  
    jogo.exibirMenu();

    return 0;
}
