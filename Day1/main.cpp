#include <iostream>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>
#include "../util/util.hpp"


int main() {
    auto file = util::getInputFile();
    std::string line;
    std::size_t sum = 0;
    std::size_t largest = 0;
    std::vector<std::size_t> sums;

    while (std::getline(file, line)) {
        if (line.empty()) {
            std::cout << "current sum: " << sum << std::endl;
            sums.emplace_back(sum);
            if (sum > largest) {
                largest = sum;
            }

            sum = 0;
            continue;
        }

        sum += std::stoi(line);
    }

    std::cout << largest << std::endl;
    std::ranges::sort(sums, std::greater{});
    for(auto s : sums | std::views::take(3)) {
        sum += s;
    }

    std::cout << "sum of the largest 3: " << sum << std::endl;
}
