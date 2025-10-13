#include <iostream>
#include <deque>

int main() {
    std::deque<int> dq;

    dq.push_front(3);
    dq.push_front(2);
    dq.push_front(1);
    dq.push_back(10);
    dq.push_back(20);
    dq.push_back(30);

    for (const auto &val : dq)
        std::cout << val << " ";
    
    std::cout << std::endl;

    dq.pop_front();
    dq.pop_back();

    for (const auto &val : dq)
        std::cout << val << " ";

    return 0;
}
