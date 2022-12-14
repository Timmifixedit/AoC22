#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include <memory>
#include <deque>
#include <ranges>
#include <cassert>
#include <Iterators.hpp>
#include "../util/util.hpp"

using LitType = int;
struct ListEntry;
using cEntryPtr = std::shared_ptr<const ListEntry>;

struct ListEntry {
    [[nodiscard]] virtual bool isLiteral() const = 0;

    [[nodiscard]] virtual auto getItems() const -> const std::vector<cEntryPtr> & = 0;

    virtual void print(bool end) const = 0;
};

struct Literal : ListEntry {
    explicit Literal(LitType lit) noexcept: value(lit) {}

    auto getItems() const noexcept -> const std::vector<cEntryPtr> & override {
        if (items.empty()) {
            items.emplace_back(std::make_shared<Literal>(value));
        }

        return items;
    }

    [[nodiscard]] constexpr bool isLiteral() const noexcept override {
        return true;
    }

    void print(bool end) const override {
        std::cout << value;
        if (end) {
            std::cout << std::endl;
        }
    }

    virtual ~Literal() = default;

    LitType value;
    mutable std::vector<cEntryPtr> items;
};

struct List;

auto parse(const std::string &line, std::string::const_iterator start)
    -> std::pair<std::vector<cEntryPtr>, std::string::const_iterator> {
    std::vector<cEntryPtr> ret;
    for (auto curr = start + 1; curr != line.end(); ++curr) {
        if (*curr == ',') {
            continue;
        }

        if (*curr == ']') {
            return {ret,  curr};
        }

        if (*curr == '[') {
            auto [list, next] = parse(line, curr);
            ret.emplace_back(std::make_shared<List>(std::move(list)));
            curr = next;
            continue;
        }

        auto inner = curr;
        while (*inner != ']' && *inner != ',') {
            ++inner;
        }

        std::string part(curr, inner);
        ret.emplace_back(std::make_shared<Literal>(std::stoi(part)));
        curr = inner - 1;
    }

    return {ret, line.end()};
}

auto parse(const std::string &line) -> std::shared_ptr<List> {
    auto [list, _] = parse(line, line.begin());
    return std::make_shared<List>(std::move(list));
}

struct List : ListEntry {
    explicit List(std::vector<cEntryPtr> items) : items(std::move(items)) {}

    [[nodiscard]] constexpr bool isLiteral() const noexcept override {
        return false;
    }

    [[nodiscard]] auto getItems() const noexcept -> const std::vector<cEntryPtr> & override {
        return items;
    }

    void print(bool end = true) const override {
        std::cout << "[";
        for (auto [idx, item] : iterators::enumerate(items, 1u)) {
            item->print(false);
            if (idx < items.size()) {
                std::cout << ", ";
            }
        }

        std::cout << "]";
        if (end) {
            std::cout << std::endl;
        }
    }

    virtual ~List() = default;

    std::vector<cEntryPtr> items;
};

int operator<=>(const ListEntry &lhs, const ListEntry &rhs) {
    if (lhs.isLiteral() && rhs.isLiteral()) {
        return dynamic_cast<const Literal&>(lhs).value - dynamic_cast<const Literal&>(rhs).value;
    }

    for (auto [l, r] : iterators::zip(lhs.getItems(), rhs.getItems())) {
        auto comp = *l <=> *r;
        if (comp != 0) {
            return comp;
        }
    }

    return static_cast<int>(lhs.getItems().size()) - static_cast<int>(rhs.getItems().size());
}

bool operator==(const ListEntry &lhs, const ListEntry &rhs) {
    return lhs <=> rhs == 0;
}

int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    std::string line;
    std::size_t i = 0;
    unsigned correctCount = 0;
    std::vector<cEntryPtr> packets;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        auto list = parse(line);
        list->print();
        packets.emplace_back(std::move(list));
        if (++i % 2 == 0) {
            auto res = **(packets.rbegin() + 1) < *packets.back();
            correctCount += res == 1 ? i / 2 : 0;
            std::cout << res << std::endl;
        }
    }

    std::cout << "sum of correct pairs " << correctCount << std::endl;
    packets.emplace_back(parse("[[2]]"));
    packets.emplace_back(parse("[[6]]"));
    std::ranges::sort(packets, [](const auto &a, const auto &b) { return *a < *b; });

    auto d1 = std::ranges::find_if(packets, [c = parse("[[2]]")](const auto &elem) {
        return *elem == static_cast<const ListEntry &>(*c);
    });

    auto d2 = std::ranges::find_if(packets, [c = parse("[[6]]")](const auto &elem) {
        return *elem == static_cast<const ListEntry &>(*c);
    });

    assert(d1 != packets.end() && d2 != packets.end());
    std::cout << "product of indices " << (d1 - packets.begin() + 1) * (d2 - packets.begin() + 1) << std::endl;
}
