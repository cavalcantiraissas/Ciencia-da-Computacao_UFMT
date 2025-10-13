#include <iostream>
#include <list>

int main() {
    std::list<int> lst{10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    
    for (auto it = lst.begin(); it != lst.end(); ) {
        if (*it % 2 == 0)
            it = lst.erase(it);
        else
            ++it;
    }

    for (const auto &val : lst)
        std::cout << val << " ";

    return 0;
}
