#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include <memory>
#include <deque>
#include <ranges>
#include <cassert>
#include <limits>
#include <Iterators.hpp>
#include <fmt/core.h>
#include "../util/util.hpp"

constexpr int sgn(int x) noexcept {
    if (x == 0) {
        return 0;
    }

    return x > 0 ? 1 : -1;
}

struct Vec {
    explicit Vec(const std::string &input) {
        auto parts = util::splitString<','>(input);
        assert(parts.size() == 2);
        x = std::stoi(parts.front());
        y = std::stoi(parts.back());
    }

    constexpr Vec(int x, int y) noexcept : x(x), y(y) {}

    constexpr Vec& operator+=(const Vec &other) noexcept {
        *this = *this + other;
        return *this;
    }

    constexpr Vec& operator-=(const Vec &other) noexcept {
        *this = *this - other;
        return *this;
    }

    constexpr Vec operator+(const Vec &other) const noexcept {
        return {x + other.x, y + other.y};
    }

    constexpr Vec operator-(const Vec &other) const noexcept {
        return {x - other.x, y - other.y};
    }

    constexpr bool operator==(const Vec &other) const noexcept {
        return x == other.x && y == other.y;
    }

    [[nodiscard]] constexpr Vec toDirVec() const {
        if (x != 0 && y != 0) {
            throw std::runtime_error("cannot create dir vec");
        }

        return {sgn(x), sgn(y)};
    }

    int x, y;
};

struct Path {
    explicit Path(const std::string &line) {
        auto parts = util::splitString<'-'>(line);
        for (auto &s : parts) {
            if (s.starts_with('>')) {
                s.replace(0, 1, 1, ' ');
            }

            vertices.emplace_back(s);
        }
    }

    std::vector<Vec> vertices;
};

struct Field {
    static constexpr auto Free = 0;
    static constexpr auto Sand = 1;
    static constexpr auto Blocked = 2;
    static constexpr Vec SandSource{500, 0};

    explicit Field(const std::vector<Path> &paths, int minX, int maxX, int maxY) :
            minX(minX), stride(maxX - minX + 1), field(stride * (maxY + 1), Free) {
        for (const auto &path : paths) {
            for (auto [from, to] : iterators::zip(path.vertices, path.vertices | std::views::drop(1))) {
                auto dir = (to - from).toDirVec();
                auto curr = from;
                while (curr != to) {
                    (*this)(curr) = Blocked;
                    curr += dir;
                }

                (*this)(to) = Blocked;
            }
        }
    }

    const int &operator()(const Vec &pos) const noexcept {
        return field[vecToIdx(pos)];
    }

    int &operator()(const Vec &pos) noexcept {
        return field[vecToIdx(pos)];
    }

    [[nodiscard]] constexpr Vec idxToVec(std::size_t idx) const noexcept{
        return {static_cast<int>(idx % stride), static_cast<int>(idx / stride)};
    }

    [[nodiscard]] auto percolate(const Vec &sandPos) const noexcept -> std::optional<Vec> {
        if (not contains(sandPos + Vec(0, 1))) {
            return {};
        }

        if (free(sandPos + Vec(0, 1))) {
            return sandPos + Vec(0, 1);
        }

        if (not contains(sandPos + Vec(-1, 1))) {
            return {};
        }

        if (free(sandPos + Vec(-1, 1))) {
            return sandPos + Vec(-1, 1);
        }

        if (not contains(sandPos + Vec(1, 1))) {
            return {};
        }

        if (free(sandPos + Vec(1, 1))) {
            return sandPos + Vec(1, 1);
        }


        return sandPos;
    }

    [[nodiscard]] bool contains(const Vec &pos) const noexcept {
        return vecToIdx(pos) < field.size();
    }

    [[nodiscard]] bool free(const Vec &pos) const noexcept {
        return (*this)(pos) == Free;
    }

    void placeSand(const Vec &pos) noexcept {
        (*this)(pos) = Sand;
    }

    void print() const {
        for (auto [idx, val] : iterators::enumerate(field)) {
            if (idx == vecToIdx(SandSource)) {
                std::cout << "+";
                continue;
            }

            if (idx % stride == 0) {
                std::cout << std::endl;
            }

            switch (val) {
                case Free:
                    std::cout << ".";
                    break;
                case Sand:
                    std::cout << "o";
                    break;
                case Blocked:
                    std::cout << "#";
                    break;
                default:
                    throw std::runtime_error("invalid field value");
            }
        }

        std::cout << std::endl;
    }

private:
    [[nodiscard]] constexpr std::size_t vecToIdx(const Vec &pos) const noexcept {
        return pos.y * stride + pos.x - minX;
    }

    int minX;
    std::size_t stride;
    std::vector<int> field;
};

void runSimulation(Field &field) {
    unsigned numSandUnitsRested = 0;
    while (true) {
        std::optional<Vec> sandPos = Field::SandSource;
        Vec oldPos = *sandPos;
        while (sandPos.has_value()) {
            oldPos = *sandPos;
            sandPos = field.percolate(*sandPos);
            if (sandPos == oldPos) {
                field.placeSand(oldPos);
                ++numSandUnitsRested;
                break;
            }
        }

        if (not sandPos.has_value() || sandPos == Field::SandSource) {
            break;
        }
    }

    field.print();
    std::cout << "number of sand units that have come to rest: " << numSandUnitsRested << std::endl;
}


int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    std::string line;
    std::vector<Path> paths;
    auto minX = std::numeric_limits<int>::max();
    auto maxX = 0;
    auto maxY = 0;
    while (std::getline(file, line)) {
        paths.emplace_back(line);
        for (const auto &vertex : paths.back().vertices) {
            minX = std::min(minX, vertex.x);
            maxX = std::max(maxX, vertex.x);
            maxY = std::max(maxY, vertex.y);
        }
    }

    Field field(paths, minX, maxX, maxY);
    field.print();
    runSimulation(field);
    const auto requiredWidth = 2 * maxY + 5;
    minX = Field::SandSource.x - requiredWidth / 2;
    maxX = Field::SandSource.x + requiredWidth / 2;
    paths.emplace_back(fmt::format("{0},{1} -> {2},{1}", minX, maxY + 2, maxX, maxY + 2));
    field = Field(paths, minX, maxX, maxY + 2);
    field.print();
    runSimulation(field);
}
