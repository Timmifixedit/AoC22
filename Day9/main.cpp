#include <string>
#include <iostream>
#include <cassert>
#include <unordered_set>
#include <concepts>
#include <Iterators.hpp>
#include <ranges>
#include "../util/util.hpp"


struct Pos {
    constexpr Pos(int x, int y) noexcept : x(x), y(y) {}

    constexpr Pos() noexcept : Pos(0, 0) {}

    [[nodiscard]] std::string toString() const {
        return std::to_string(x) + "," + std::to_string(y);
    }

    [[nodiscard]] constexpr int manhattanDist(const Pos &other) const noexcept {
        return std::abs(x - other.x) + std::abs(y - other.y);
    }

    constexpr Pos operator+(const Pos &other) const noexcept {
        return {x + other.x, y + other.y};
    }

    constexpr Pos operator-(const Pos &other) const noexcept {
        return {x - other.x, y - other.y};
    }

    constexpr Pos operator/(std::integral auto factor) const noexcept {
        return {x / factor, y / factor};
    }

    int x, y;
};

constexpr Pos moveTail(const Pos& head, const Pos &tail) noexcept {
    auto dist = tail.manhattanDist(head);
    auto dirVec = head - tail;
    if (dist >= 3) {
        dirVec.x += dirVec.x % 2;
        dirVec.y += dirVec.y % 2;
    }

    return tail + dirVec / 2;
}

constexpr Pos moveHead(const Pos &head, char dir) noexcept {
    switch (dir) {
        case 'R':
            return head + Pos(1, 0);
        case 'U':
            return head + Pos(0, 1);
        case 'L':
            return head + Pos(-1, 0);
        case 'D':
            return head + Pos(0, -1);
        default:
            abort();
    }
}

int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    std::string line;
    std::array<Pos, 10> rope;
    std::unordered_set<std::string> positionsShort;
    std::unordered_set<std::string> positionsLong;
    unsigned numVisitedShort = 0;
    unsigned numVisitedLong = 0;
    while (std::getline(file, line)) {
        char dir;
        unsigned steps;
        std::stringstream ss(line);
        ss >> dir >> steps;
        while (steps-- > 0) {
            rope[0] = moveHead(rope[0], dir);
            for (auto [tmpHead, tmpTail] : iterators::zip(rope, rope | std::views::drop(1))) {
                tmpTail = moveTail(tmpHead, tmpTail);
            }

            auto [_, inserted] = positionsShort.emplace(rope[1].toString());
            numVisitedShort += inserted;
            std::tie(_, inserted) = positionsLong.emplace(rope.back().toString());
            numVisitedLong += inserted;
        }
    }

    std::cout << "number of visited positions a1 " << numVisitedShort << std::endl;
    std::cout << "number of visited positions a2 " << numVisitedLong << std::endl;
}
