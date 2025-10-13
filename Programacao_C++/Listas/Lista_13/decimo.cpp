#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto it = vec.begin() + 2;
    std::cout << "Terceiro elemento: " << *it << std::endl;

    *it = 99;

    for (const auto &val : vec)
        std::cout << val << " ";

    return 0;
}
