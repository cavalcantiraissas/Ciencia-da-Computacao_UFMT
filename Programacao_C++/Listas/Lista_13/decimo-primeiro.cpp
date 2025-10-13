#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec{1, 2, 3, 4, 5};

    for (auto rit = vec.rbegin(); rit != vec.rend(); ++rit)
        std::cout << *rit << " ";

    return 0;
}
