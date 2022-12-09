#include <string>
#include <cassert>
#include <iostream>
#include <ranges>
#include "../util/util.hpp"


constexpr unsigned getPrio(char c) noexcept {
    auto tmp = c - 'a' + 1;
    return tmp < 0 ? tmp + 52 + 'a' - 'Z' - 1 : tmp;
}

constexpr char findDuplicate(std::string_view items) {
    using namespace std::views;
    const std::size_t half = items.size() / 2;
    auto comp1 = items | take(half);
    auto comp2 = items | drop(half);
    for(auto c : comp2) {
        auto res = std::ranges::find(comp1, c);
        if (res != comp1.end()) {
            return *res;
        }
    }

    std::abort();
}

char findBadge(const std::array<std::string, 3> &team) {
    std::array<short, 52> itemCounts = {0};
    for (const auto &backpack : team) {
        std::array<bool, 52> visited = {false};
        for (auto item : backpack) {
            unsigned index = getPrio(item) - 1;
            if (not visited[index]) {
                visited[index] = true;
                ++itemCounts[index];
                if (itemCounts[index] == 3) {
                    return item;
                }
            }
        }
    }

    abort();
}


int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    std::string line;
    unsigned sum = 0;
    unsigned badgeSum = 0;
    std::array<std::string, 3> team;
    unsigned teamIdx = 0;
    while (std::getline(file, line)) {
        char dup = findDuplicate(line);
        unsigned prio = getPrio(dup);
        std::cout << line << "\t";
        std::cout << "duplicate: " << dup << "(" << prio << ")" << std::endl;
        sum += prio;
        team[teamIdx] = std::move(line);
        teamIdx = (teamIdx + 1) % 3;
        if (teamIdx == 0) {
            char badge = findBadge(team);
            unsigned badgePrio = getPrio(badge);
            std::cout << "the teams badge is " << badge << "(" << badgePrio << ")" << std::endl;
            badgeSum += badgePrio;
        }
    }

    std::cout << "sum of priorities " << sum << std::endl;
    std::cout << "sum of badges " << badgeSum << std::endl;
}
