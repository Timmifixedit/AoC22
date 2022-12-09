#include <string>
#include <iostream>
#include <cassert>
#include <vector>
#include <array>
#include "../util/util.hpp"

using tree_array = std::vector<int>;
using visited_array = std::vector<bool>;

constexpr std::size_t index(std::size_t row, std::size_t col, size_t stride) noexcept {
    return stride * row + col;
}

std::size_t visibleInLine(const tree_array &trees, visited_array &alreadyCounted,
                          std::size_t nCols, std::size_t idx, bool row) {
    assert(trees.size() == alreadyCounted.size());
    const std::size_t nRows = trees.size() / nCols;
    const std::size_t maxIdx = row ? nCols : nRows;
    int maxLeft = -1, maxRight = -1;
    std::size_t ret = 0;
    auto check = [&ret, &trees, &alreadyCounted](auto tidx, auto &max) {
        if (not alreadyCounted[tidx] and trees[tidx] > max) {
            alreadyCounted[tidx] = true;
            ++ret;
        }

        max = std::max(max, trees[tidx]);
    };
    for (std::size_t i = 0; i < maxIdx; ++i) {
        const auto inverse = maxIdx - 1 - i;
        const auto leftIdx = index(row ? idx : i, row ? i : idx, nCols);
        const auto rightIdx = index(row ? idx : inverse, row ? inverse : idx, nCols);
        check(leftIdx, maxLeft);
        check(rightIdx, maxRight);
    }

    return ret;
}

constexpr std::array<std::size_t, 4> sentinel(std::size_t index, std::size_t nCol, std::size_t nRow) noexcept {
    std::size_t row = index / nCol;
    std::size_t col = index % nCol;
    return {(row + 1) * nCol - 1, col, row * nCol, (nRow - 1) * nCol + col};
}

constexpr std::array<int, 4> increment(std::size_t nCol) noexcept {
    return {1, -static_cast<int>(nCol), -1, static_cast<int>(nCol)};
}

unsigned distance(const tree_array &trees, std::size_t index, std::size_t numCols, unsigned dir) noexcept {
    auto sen = sentinel(index, numCols, trees.size() / numCols)[dir];
    int inc = increment(numCols)[dir];
    int treeSize = trees[index];
    unsigned ret = 0;
    while (index != sen) {
        index += inc;
        ++ret;
        if (trees[index] >= treeSize) {
            break;
        }
    }

    return ret;
}

unsigned scenicScore(const tree_array &trees, std::size_t index, std::size_t numCols) noexcept {
    unsigned ret = 1;
    for (unsigned dir = 0; dir < 4; ++dir) {
        ret *= distance(trees, index, numCols, dir);
    }

    return ret;
}


int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    std::string line;
    tree_array trees;
    std::size_t numCols = 0;
    while (std::getline(file, line)) {
        if (numCols == 0) {
            numCols = line.size();
        }

        for (char c : line) {
            trees.emplace_back(c - '0');
        }
    }

    visited_array visible(trees.size(), false);
    const std::size_t numRows = trees.size() / numCols;
    std::size_t numVisible = 0;
    for (std::size_t row = 0; row < numRows; ++row) {
        numVisible += visibleInLine(trees, visible, numCols, row, true);
    }

    for (std::size_t col = 0; col < numCols; ++col ) {
        numVisible += visibleInLine(trees, visible, numCols, col, false);
    }

    std::cout << "num visible trees " << numVisible << std::endl;
    unsigned best = 0;
    for (std::size_t index = 0; index < trees.size(); ++index) {
        best = std::max(best, scenicScore(trees, index, numCols));
    }

    std::cout << "best scenic score " << best << std::endl;
}
