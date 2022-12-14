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

            for (auto [i, c] : iterators::const_enumerate(line)) {
                auto val = c - 'a';
                auto idx = row * stride + i;
                if (c == 'S') {
                    start = idx;
                    val = 0;
                } else if (c == 'E') {
                    goal = idx;
                    val = 'z' - 'a';
                }

                if (val == 0) {
                    startingPoints.emplace_back(idx);
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

template <std::equality_comparable T>
class SearchNode {
public:
    using NodePtr = std::shared_ptr<SearchNode>;
    template<typename T_>
    SearchNode(T_ state, NodePtr parent, unsigned pathCost) : state(std::forward<T_>(state)), parent(std::move(parent)),
                                                              pathCost(pathCost) {}

    template<typename T_>
    SearchNode(T_ state, NodePtr parent) : SearchNode(std::forward<T_>(state), std::move(parent), 0) {}

    bool operator==(const SearchNode &other) const {
        return state == other.state;
    }

    const T state;
    NodePtr parent;
    unsigned pathCost = 0;
};

template<typename T>
using NodePtr = std::shared_ptr<SearchNode<T>>;

template<typename H, typename T>
concept HeuristicFunction = requires(H instance, SearchNode<T> node) {
    {instance(node.state)} -> std::totally_ordered;
};


template<typename T, HeuristicFunction<T> H>
struct NodeCompare {
    template<typename H_>
    constexpr explicit NodeCompare(H_ && h) : h(std::forward<H_>(h)) {}

    constexpr auto operator()(const NodePtr<T> &lhs, const NodePtr<T> &rhs) const {
        return f(lhs) > f(rhs);
    }

private:
    [[nodiscard]] constexpr auto f(const NodePtr<T> &n) const {
        return n->pathCost + h(n->state);
    }

    H h;
};

template<typename E, typename T>
concept ExpandFunction = requires(E instance, SearchNode<T> node) {
    requires std::convertible_to<std::ranges::range_value_t<decltype(instance(node.state))>, T>;
};

template<typename G, typename T>
concept GoalTest = requires(G instance, T state) {
    { instance(state) } -> std::convertible_to<bool>;
};

template<typename T, GoalTest<T> GoalFun, ExpandFunction<T> ExpFun, HeuristicFunction<T> HFun>
auto aStar(const NodePtr<T> &start, const GoalFun &goalTest, const ExpFun &expand, HFun &&h) -> NodePtr<T> {
    using NPtr = NodePtr<T>;
    std::vector<NPtr> visited;
    NodeCompare<T, HFun> compare(std::forward<HFun>(h));
    std::priority_queue fringe(compare, std::vector{start});
    while (not fringe.empty()) {
        auto current = std::move(const_cast<NPtr &>(fringe.top()));
        fringe.pop();
        if (goalTest(current->state)) {
            return current;
        }

        auto res = std::find_if(visited.begin(), visited.end(),
                                [current](const auto &elem) { return *current == *elem; });
        if (res != visited.end()) {
            continue;
        }

        visited.emplace_back(current);
        auto neighbours = expand(current->state);
        for (auto &n : neighbours) {
            auto g = current->pathCost + 1;
            auto nPtr = std::make_shared<SearchNode<T>>(std::move(n), current, g);
            fringe.emplace(std::move(nPtr));
        }
    }

    return nullptr;
}

template<typename E>
concept ElevationConstraint = requires(E instance, Field::HType height) {
    { instance(height, height) } -> std::convertible_to<bool>;
};

template<GoalTest<std::size_t> GoalFun, ElevationConstraint ElevationTest, HeuristicFunction<std::size_t> H>
unsigned shortestPath(std::size_t start, const Field &field, const GoalFun &goalTest, const ElevationTest &elevationTest, H &&h) {
    auto startNode = std::make_shared<SearchNode<std::size_t>>(start, nullptr);
    auto expand = [&field, &elevationTest](std::size_t pos) {
        auto height = field.terrain[pos];
        std::array candidates = {pos - 1, pos + 1, pos - field.stride, pos + field.stride};
        std::vector<std::size_t> neighbors;
        neighbors.reserve(candidates.size());
        for (auto c : candidates) {
            if (c < field.terrain.size() && elevationTest(field.terrain[c], height)) {
                neighbors.emplace_back(c);
            }
        }

        return neighbors;
    };

    auto res = aStar(startNode, goalTest, expand, std::forward<H>(h));
    if (res != nullptr) {
        return res->pathCost;
    }

    std::cerr << "unsolvable" << std::endl;
    std::exit(1);
}


int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    const Field field(file);
    auto shortest = shortestPath(field.start, field, [d = field.goal](auto idx) { return idx == d; },
                                 [](auto newHeight, auto oldHeight) { return newHeight - oldHeight <= 1; },
                                 [&field](auto idx) { return field.dist(idx, field.goal); });
    std::cout << "ex 1: " << shortest << std::endl;
    shortest = shortestPath(field.goal, field, [&field](auto idx) { return field.terrain[idx] == 0; },
                            [](auto newHeight, auto oldHeight) { return newHeight - oldHeight >= -1; },
                            [](auto) { return 0; });
    std::cout << "ex 2: " << shortest << std::endl;
}
