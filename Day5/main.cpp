#include <string>
#include <cassert>
#include <iostream>
#include <stack>
#include <ranges>
#include <Iterators.hpp>
#include "../util/util.hpp"

using stack_vector = std::vector<std::stack<char>>;

void move(stack_vector &stacks, unsigned num, unsigned from, unsigned to) {
    while (num-- > 0) {
        stacks[to].emplace(stacks[from].top());
        stacks[from].pop();
    }
}

void moveStack(stack_vector &stacks, unsigned num, unsigned from, unsigned to) {
    std::vector<char> boxes;
    boxes.reserve(num);
    while (num-- > 0) {
        boxes.emplace_back(stacks[from].top());
        stacks[from].pop();
    }

    for (auto b : std::views::reverse(boxes)) {
        stacks[to].emplace(b);
    }
}


int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    std::string line;
    std::vector<std::string> boxes;
    while (std::getline(file, line) && line.find_first_of('[') != std::string::npos) {
        boxes.emplace_back(std::move(line));
    }

    std::vector<unsigned> stackIndices;
    for (std::size_t i = 0; i < line.size(); ++i) {
        if (line[i] != ' ') {
            stackIndices.emplace_back(i);
        }
    }

    stack_vector stacksA1(stackIndices.size());
    for (const auto &level : std::views::reverse(boxes)) {
        for (auto [stackId, lineIdx] : iterators::enumerate(stackIndices)) {
            if (level[lineIdx] != ' ') {
                stacksA1[stackId].emplace(level[lineIdx]);
            }
        }
    }

    stack_vector stacksA2 = stacksA1;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::string dummy;
        unsigned num, from, to;
        std::istringstream iss(line);
        iss >> dummy >> num >> dummy >> from >> dummy >> to;
        move(stacksA1, num, from - 1, to - 1);
        moveStack(stacksA2, num, from - 1, to - 1);
    }

    std::stringstream solA1;
    std::stringstream solA2;
    for (auto [s1, s2] : iterators::const_zip(stacksA1, stacksA2)) {
        solA1 << s1.top();
        solA2 << s2.top();
    }

    std::cout << "top boxes in A1 are " << solA1.str() << std::endl;
    std::cout << "top boxes in A2 are " << solA2.str() << std::endl;
}
