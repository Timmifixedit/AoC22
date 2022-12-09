#include <string>
#include <iostream>
#include <cassert>
#include <optional>
#include <unordered_map>
#include <memory>
#include <stack>
#include <ranges>
#include "../util/util.hpp"


struct Node;
using NodePtr = std::shared_ptr<Node>;
using cNodePtr = std::shared_ptr<const Node>;
constexpr inline auto MaxSize = 100000;
constexpr inline auto TotalSize = 70000000;
constexpr inline auto RequiredSpace = 30000000;

struct Node {
    explicit Node(const std::string &input) {
        auto parts = util::splitString<' '>(input);
        assert(parts.size() == 2);
        name = parts.back();
        if (parts.front() != "dir") {
            size = std::stoi(parts.front());
        }
    }

    [[nodiscard]] bool isFile() const noexcept {
        return children.empty();
    }

    template<typename ...Args>
    void addChild(Args &&...args) {
        auto node = std::make_shared<Node>(std::forward<Args>(args)...);
        children.emplace(node->name, std::move(node));
    }

    std::string name{};
    std::unordered_map<std::string, NodePtr> children{};
    std::optional<std::size_t> size{};
};

std::size_t nodeSize(const NodePtr &node, std::size_t &total) {
    if (node->size.has_value()) {
        return *node->size;
    }

    std::size_t size = 0;
    for (const auto &child : std::views::values(node->children)) {
        size += nodeSize(child, total);
    }

    node->size = size;
    if (size < MaxSize) {
        total += size;
        std::cout << "dir " << node->name << " is of size " << size << std::endl;
    }

    return size;
}

std::size_t accSizeOfSmallDirs(const NodePtr &node) {
    std::size_t size = 0;
    nodeSize(node, size);
    return size;
}

void sizeOfBestFit(const cNodePtr &node, std::size_t required, std::size_t &best) {
    if (node->isFile()) {
        return;
    }

    if (node->size.value() < required) {
        return;
    }

    best = std::min(best, node->size.value());
    for (const auto &child : std::views::values(node->children)) {
        sizeOfBestFit(child, required, best);
    }
}

std::size_t sizeOfBestFit(const cNodePtr &root, std::size_t required) {
    std::size_t best = root->size.value();
    sizeOfBestFit(root, required, best);
    return best;
}

int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    std::string line;
    std::stack<NodePtr> nodeStack;
    const auto root = std::make_shared<Node>("dir /");
    nodeStack.emplace(root);
    std::getline(file, line);
    while (std::getline(file, line)) {
        auto parts = util::splitString<' '>(line);
        if (parts.size() == 3) {
            if (parts.back() == "..") {
                nodeStack.pop();
            } else {
                nodeStack.emplace(nodeStack.top()->children.at(parts.back()));
            }

            continue;
        }

        if (parts.front() == "$") {
            continue;
        }

        nodeStack.top()->addChild(line);
    }

    auto sumOfSmallDirs = accSizeOfSmallDirs(root);
    std::cout << "sum of all directories smaller than " << MaxSize << ": " << sumOfSmallDirs << std::endl;
    auto freeSpace = TotalSize - root->size.value();
    auto diff = RequiredSpace - freeSpace;
    std::cout << "we need to free at least " << diff << " of space" << std::endl;
    auto bestFit = sizeOfBestFit(root, diff);
    std::cout << "best node to delete has size " << bestFit << std::endl;
}
