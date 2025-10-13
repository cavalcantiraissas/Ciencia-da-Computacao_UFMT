#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    for (auto it = vec.begin(); it != vec.end(); ++it)
        std::cout << *it << " ";
    
    std::cout << std::endl;

    for (auto it = vec.begin(); it != vec.end(); ++it)
        *it *= 2;

    for (const auto &val : vec)
        std::cout << val << " ";

    return 0;
}
