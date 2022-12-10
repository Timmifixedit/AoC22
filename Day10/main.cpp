#include <string>
#include <iostream>
#include <optional>
#include <deque>
#include <array>
#include "../util/util.hpp"


enum class InstructionType {
    Noop = 1, Add
};

struct Instruction {
    explicit Instruction(unsigned startingCycle, const std::string &input) {
        auto parts = util::splitString<' '>(input);
        if (parts.size() == 1) {
            type = InstructionType::Noop;
        } else {
            type = InstructionType::Add;
            value = std::stoi(parts.back());
        }

        finished = startingCycle + execTime();
    }

    [[nodiscard]] constexpr short execTime() const noexcept {
        return static_cast<short>(type);
    }

    constexpr void exec(int &reg) const noexcept {
        reg += value.value_or(0);
    }

    InstructionType type;
    std::optional<int> value{};
    unsigned finished;
};


int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    std::string line;
    constexpr std::array readouts{20u, 60u, 100u, 140u, 180u, 220u};
    auto nextReadout = readouts.begin();
    unsigned clockCycle = 0;
    int regX = 1;
    std::deque<Instruction> instructions;
    std::getline(file, line);
    instructions.emplace_back(0, line);
    unsigned sumSignalStrength = 0;
    constexpr unsigned ScreenWidth = 40;
    while (not instructions.empty()) {
        if (std::getline(file, line)) {
            instructions.emplace_back(instructions.back().finished, line);
        }

        auto crtPos = clockCycle % ScreenWidth;
        if (std::abs(static_cast<int>(crtPos) - regX) <= 1) {
            std::cout << "#";
        } else {
            std::cout << " ";
        }

        ++clockCycle;
        if (clockCycle % ScreenWidth == 0) {
            std::cout << std::endl;
        }

        if (nextReadout != readouts.end() && clockCycle == *nextReadout) {
            ++nextReadout;
            unsigned signalStrength = clockCycle * regX;
            sumSignalStrength += signalStrength;
        }

        if (clockCycle == instructions.front().finished) {
            instructions.front().exec(regX);
            instructions.pop_front();
        }
    }

    std::cout << std::endl << "sum of signal strengths " << sumSignalStrength << std::endl;
}
