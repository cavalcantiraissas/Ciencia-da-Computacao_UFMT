#include <iostream>
#include <vector>

bool compararIteradores(std::vector<int>::iterator it1, std::vector<int>::iterator it2) {
    return it1 == it2;
}

int main() {
    std::vector<int> vec{10, 20, 30, 40, 50};

    auto it1 = vec.begin();
    auto it2 = vec.begin() + 2;

    std::cout << "Iteradores apontam para o mesmo elemento? " 
              << (compararIteradores(it1, it2) ? "Sim" : "Não") << std::endl;

    it2 = vec.begin();

    std::cout << "Iteradores apontam para o mesmo elemento? " 
              << (compararIteradores(it1, it2) ? "Sim" : "Não") << std::endl;

    return 0;
}
