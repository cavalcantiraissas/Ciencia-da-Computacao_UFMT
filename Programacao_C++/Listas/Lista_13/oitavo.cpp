#include <iostream>
#include <forward_list>

void removerSegundo(std::forward_list<int>& lst) {
    auto it = lst.begin();
    if (it != lst.end()) ++it; 
    if (it != lst.end()) lst.erase_after(lst.begin());
}

std::forward_list<int>::iterator encontrar(std::forward_list<int>& lst, int valor) {
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        if (*it == valor)
            return it;
    }
    return lst.end();
}

int main() {
    std::forward_list<int> lst{1, 2, 3, 4, 5};

    removerSegundo(lst);

    for (const auto &val : lst)
        std::cout << val << " ";

    std::cout << std::endl;

    auto it = encontrar(lst, 3);
    if (it != lst.end())
        std::cout << "Elemento encontrado: " << *it << std::endl;
    else
        std::cout << "Elemento não encontrado" << std::endl;

    return 0;
}
