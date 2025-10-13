#include <iostream>
#include <stack>

int main() {
    std::stack<int> stk;

    stk.push(5);
    stk.push(10);
    stk.push(15);
    stk.push(20);

    while (!stk.empty()) {
        std::cout << stk.top() << " ";
        stk.pop();
    }

    return 0;
}
