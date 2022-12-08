#include <string>
#include <istream>
#include <cassert>
#include <iostream>
#include "../util/util.hpp"


class IdRange {
public:
    explicit IdRange(const std::string &input) {
        std::istringstream iss(input);
        char d;
        iss >> lower >> d >> upper;
    }

    [[nodiscard]] constexpr bool fullOverlap(const IdRange &other) const noexcept {
        return contains(other) || other.contains(*this);
    }


    [[nodiscard]] constexpr bool overlaps(const IdRange &other) const noexcept {
        return containsLower(other) || other.containsLower(*this);
    }

private:
    [[nodiscard]] constexpr bool contains(const IdRange &other) const noexcept {
        return other.lower >= lower && other.upper <= upper;
    }

    [[nodiscard]] constexpr bool containsLower(const IdRange &other) const noexcept {
        return lower <= other.lower && upper >= other.lower;
    }

    unsigned lower{}, upper{};
};

int main() {
    auto file = util::getInputFile();
    std::string line;
    unsigned numFullOverlap = 0;
    unsigned numPartialOverlap = 0;
    while (std::getline(file, line)) {
        auto res = util::splitString<','>(line);
        assert(res.size() == 2);
        IdRange e1(res.front());
        IdRange e2(res.back());
        bool overlap = e1.fullOverlap(e2);
        bool partialOverlap = e1.overlaps(e2);
        std::cout << line;
        if (overlap) {
            std::cout << "\t(full overlap)";
        } else if (partialOverlap) {
            std::cout << "\t(partial overlap)";
        }

        std::cout << std::endl;
        numFullOverlap += overlap;
        numPartialOverlap += partialOverlap;
    }

    std::cout << "num full overlaps " << numFullOverlap << std::endl;
    std::cout << "num partial overlaps " << numPartialOverlap << std::endl;
}
