#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include <cassert>
#include <functional>
#include <algorithm>
#include "../util/util.hpp"


using Item = unsigned long;

class BinaryExpression {
    using operator_ = std::function<Item(Item, Item)>;
public:
    explicit BinaryExpression(const std::string &line) {
        auto parts = util::splitString<'='>(line);
        assert(parts.size() == 2);
        parts = util::splitString<' '>(parts.back());
        assert(parts.size() == 3);
        lhs = parseOperand(parts.front());
        op = parseOperator(parts[1]);
        rhs = parseOperand(parts.back());
    }

    [[nodiscard]] Item evaluate(Item input) const noexcept {
        return op(lhs.value_or(input), rhs.value_or(input));
    }

private:
    [[nodiscard]] static auto parseOperand(const std::string &input) noexcept -> std::optional<Item> {
        std::optional<Item> ret;
        try {
            ret = std::stoi(input);
        } catch (const std::invalid_argument &) {

        }

        return ret;
    }

    static operator_ parseOperator(const std::string &input) noexcept {
        if (input == "+") {
            return [](auto a, auto b) { return a + b; };
        } else if (input == "-") {
            return [](auto a, auto b) { return a - b; };
        } else if (input == "*") {
            return [](auto a, auto b) { return a * b; };
        } else if (input == "/") {
            return [](auto a, auto b) { return a / b; };
        } else {
            abort();
        }
    }

    operator_ op;
    std::optional<Item> lhs, rhs;
};

struct ThrownItem {
    constexpr ThrownItem(Item val, unsigned target) noexcept: val(val), target(target) {}
    Item val;
    unsigned target;
};

class Monkey {
    using decay = std::function<Item(Item)>;
public:
    explicit Monkey(const std::array<std::string, 6> &definition) : updateFunction(definition[2]), divTest(
            parseIntegerAtEnd(definition[3])), trueTarget(parseIntegerAtEnd(definition[4])), falseTarget(
            parseIntegerAtEnd(definition[5])) {
        std::string _;
        std::stringstream ss(definition[0]);
        ss >> _ >> id >> _;
        auto parts = util::splitString<':'>(definition[1]);
        assert(parts.size() == 2);
        parts = util::splitString<','>(parts.back());
        items.reserve(parts.size());
        for (const auto &itemDef: parts) {
            items.emplace_back(std::stoi(itemDef));
        }
    }

    auto throwStuff() -> std::vector<ThrownItem> {
        std::vector<ThrownItem> ret;
        ret.reserve(items.size());
        inspectedItems += items.size();
        for (auto item: items) {
            auto newRes = decayFunction.value_or(std::identity{})(updateFunction.evaluate(item));
            if (newRes % divTest == 0) {
                ret.emplace_back(newRes, trueTarget);
            } else {
                ret.emplace_back(newRes, falseTarget);
            }
        }

        items.clear();
        return ret;
    }

    void catchItem(Item val) {
        items.emplace_back(val);
    }

    [[nodiscard]] constexpr unsigned long getActivity() const noexcept {
        return inspectedItems;
    }

    [[nodiscard]] constexpr unsigned getModul() const noexcept {
        return divTest;
    }

    template<typename F>
    void setDecayFunction(F &&fun) {
        decayFunction = std::forward<F>(fun);
    }

private:
    [[nodiscard]] static int parseIntegerAtEnd(const std::string &input) {
        auto parts = util::splitString<' '>(input);
        assert(!parts.empty());
        return std::stoi(parts.back());
    }

    unsigned id{};
    std::vector<Item> items;
    BinaryExpression updateFunction;
    unsigned divTest, trueTarget, falseTarget;
    unsigned long inspectedItems = 0;
    std::optional<decay> decayFunction;
};

template<typename F>
void run(std::vector<Monkey> monkeys, const F &decayFun, unsigned numRounds) {
    for (auto  &m : monkeys) {
        m.setDecayFunction(decayFun);
    }

    for (unsigned round = 1; round <= numRounds; ++round) {
        for (auto &monkey: monkeys) {
            auto thrownItems = monkey.throwStuff();
            for (auto [val, target]: thrownItems) {
                monkeys[target].catchItem(val);
            }
        }
    }

    std::ranges::sort(monkeys, [](const auto &a, const auto &b) { return a.getActivity() > b.getActivity(); });
    std::cout << "monkey business " << monkeys[0].getActivity() * monkeys[1].getActivity() << std::endl;
}

int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    std::string line;
    std::array<std::string, 6> inputDef;
    auto defIt = inputDef.begin();
    std::vector<Monkey> monkeys;
    unsigned modul = 1;
    while (std::getline(file, line)) {
        if (line.empty()) {
            defIt = inputDef.begin();
            monkeys.emplace_back(inputDef);
            modul *= monkeys.back().getModul();
            continue;
        }

        *defIt = std::move(line);
        ++defIt;
    }

    if (defIt == inputDef.end()) {
        monkeys.emplace_back(inputDef);
        modul *= monkeys.back().getModul();
    }

    run(monkeys, [](auto val) { return val / 3; }, 20);
    run(monkeys, [modul](auto val) { return val % modul; }, 10000);
}
