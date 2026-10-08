#include <iostream>

#include "forward_list.hpp"

int main() {
    List<std::string> list = {"abc", "cde", "fgh"};
    List<std::string> list2 = std::move(list);
    auto it = list.begin();
    auto it2 = list2.begin();
    while (it != list.end()) {
        std::cout << *it << ' ';
        ++it;
    }
    std::cout << '\n';
    while (it2 != list2.end()) {
        std::cout << *it2 << ' ';
        ++it2;
    }
    std::cout << '\n';

    List<int> list3;
    for (int i = 0; i < 10; ++i) {
        list3.PushBack(i);
    }
    List<int> list4;
    list4 = list3;
    auto it4 = list4.crbegin();
    while (it4 != list4.rend()) {
        std::cout << *it4 << ' ';
        ++it4;
    }

    std::cout << '\n';

    list4.PopBack();
    list4.PopFront();
    list4.PopBack();

    auto it5 = list4.begin();
    while (it5 != list4.end()) {
        std::cout << *it5 << ' ';
        ++it5;
    }
}
