#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include <memory>
#include <deque>
#include <concepts>
#include <queue>
#include <limits>
#include <Iterators.hpp>
#include "../util/util.hpp"


class Field {
public:
    using HType = int;
    explicit Field(std::istream &in) {
        std::string line;
        std::size_t row = 0;
        while (std::getline(in, line)) {
            if (stride == 0) {
                stride = line.size();
            }

            for (auto [idx, c] : iterators::const_enumerate(line)) {
                auto val = c - 'a';
                if (c == 'S') {
                    start = row * stride + idx;
                    val = 0;
                } else if (c == 'E') {
                    goal = row * stride + idx;
                    val = 'z' - 'a';
                }

                if (val == 0) {
                    startingPoints.emplace_back(row * stride + idx);
                }

                terrain.emplace_back(val);
            }

            ++row;
        }
    }

    [[nodiscard]] constexpr unsigned dist(std::size_t a, std::size_t b) const noexcept {
        auto [xa, ya] = coordinates(a);
        auto [xb, yb] = coordinates(b);
        return std::abs(xa - xb) + std::abs(ya - yb);

    }

    [[nodiscard]] constexpr auto coordinates(std::size_t index) const noexcept -> std::pair<HType, HType>{
        return {index % stride, index / stride};
    }

    std::size_t stride{};
    std::size_t start;
    std::size_t goal;
    std::vector<HType> terrain{};
    std::vector<std::size_t> startingPoints;
};



template <typename T, std::totally_ordered P = unsigned>
class SearchNode {
public:
    using NodePtr = std::shared_ptr<SearchNode>;
    template<typename T_, typename P_>
    SearchNode(T_ state, NodePtr parent, P_ pathCost) : state(std::forward<T_>(state)), parent(std::move(parent)),
                                                        pathCost(std::forward<P_>(pathCost)) {}

    template<typename T_>
    SearchNode(T_ state, NodePtr parent) : SearchNode(std::forward<T_>(state), std::move(parent), P{0}) {}

    bool operator==(const SearchNode &other) const {
        return state == other.state;
    }

    const T state;
    NodePtr parent;
    P pathCost = 0;
};

template<typename T, typename P>
using NodePtr = typename SearchNode<T, P>::NodePtr;

template<typename T, typename P, typename H>
struct NodeCompare {
    template<typename H_>
    constexpr explicit NodeCompare(H_ && h) : h(std::forward<H_>(h)) {}

    constexpr auto operator()(const NodePtr<T, P> &lhs, const NodePtr<T, P> &rhs) const {
        return f(lhs) > f(rhs);
    }

private:
    [[nodiscard]] constexpr auto f(const NodePtr<T, P> &n) const {
        return n->pathCost + h(n->state);
    }

    H h;
};


template<typename T, typename P, typename ExpFun, typename HFun>
auto aStar(const NodePtr<T, P> &start, const NodePtr<T, P> &goal, const ExpFun &expand, HFun &&h,
           std::optional<P> upperBound) -> NodePtr<T, P> {
    using NPtr = NodePtr<T, P>;
    std::vector<NPtr> visited;
    NodeCompare<T, P, HFun> compare(std::forward<HFun>(h));
    std::priority_queue fringe(compare, std::vector{start});
    while (not fringe.empty()) {
        auto current = std::move(const_cast<NPtr &>(fringe.top()));
        fringe.pop();
        if (*current == *goal) {
            return current;
        }

        if (upperBound.has_value() && current->pathCost >= *upperBound) {
            return nullptr;
        }

        auto res = std::find_if(visited.begin(), visited.end(),
                                [current](const auto &elem) { return *current == *elem; });
        if (res != visited.end()) {
            continue;
        }

        visited.emplace_back(current);
        auto neighbours = expand(current->state);
        for (const auto &n : neighbours) {
            auto g = current->pathCost + 1;
            auto nPtr = std::make_shared<SearchNode<T, P>>(n, current, g);
            fringe.emplace(std::move(nPtr));
        }
    }

    return nullptr;
}

unsigned shortestPathToGoal(std::size_t start, const Field &field, std::optional<unsigned> upperBound = {}) {
    auto startNode = std::make_shared<SearchNode<Field::HType>>(start, nullptr);
    auto goalNode = std::make_shared<SearchNode<Field::HType>>(field.goal, nullptr);
    auto expand = [&field](std::size_t pos) {
        auto height = field.terrain[pos];
        std::vector<std::size_t> neighbors;
        neighbors.reserve(4);
        std::array candidates = {pos - 1, pos + 1, pos - field.stride, pos + field.stride};
        for (auto c : candidates) {
            if (c < field.terrain.size() && field.terrain[c] - height <= 1) {
                neighbors.emplace_back(c);
            }
        }

        return neighbors;
    };

    auto res = aStar<Field::HType, unsigned>(startNode, goalNode, expand,
                                             [&field](auto a) { return field.dist(a, field.goal); }, upperBound);
    if (res != nullptr) {
        return res->pathCost;
    }

    if (upperBound.has_value()) {
        return *upperBound;
    }

    std::cerr << "unsolvable" << std::endl;
    std::exit(1);
}


int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    const Field field(file);
    auto shortest = shortestPathToGoal(field.start, field);
    std::cout << "ex 1: " << shortest << std::endl;
    for (auto [idx, s] : iterators::enumerate(field.startingPoints)) {
        auto res = shortestPathToGoal(s, field, shortest);
        if (res < shortest) {
            std::cout << "new best " << res << std::endl;
            shortest = res;
        }
    }

    std::cout << "ex 2: " << shortest << std::endl;
}
