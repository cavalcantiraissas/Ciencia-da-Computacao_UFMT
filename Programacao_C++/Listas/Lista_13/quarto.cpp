#include <iostream>
#include <set>

int main() {
    std::set<int> valores{2, 4, 6, 8, 10, 12};

    for (const auto &val : valores)
        std::cout << val << " ";
    
    std::cout << std::endl;

    valores.insert(6);

    for (const auto &val : valores)
        std::cout << val << " ";

    return 0;
}
