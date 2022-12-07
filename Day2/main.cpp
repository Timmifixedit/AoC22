#include <string>
#include <cassert>
#include <iostream>
#include "../util/util.hpp"


struct RPSMove {
    explicit constexpr RPSMove(char c) noexcept :  id(c), score(c - 'A' + 1) {
        if (score > 3) {
            score -= 23;
        }
    }

    [[nodiscard]] constexpr char normalized() const noexcept {
        if (id <= 'C') {
            return id;
        }

        return static_cast<char>(id - 'X' + 'A');
    }

    [[nodiscard]] constexpr unsigned outcome(const RPSMove &other) const noexcept {
        const char myId = normalized();
        const char oId = other.normalized();
        if (myId == oId) {
            return 3;
        }

        if ((myId == 'A' && oId == 'C') || (myId == 'B' && oId == 'A') || (myId == 'C' && oId == 'B')) {
            return 6;
        }

        return 0;
    }

    [[nodiscard]] constexpr RPSMove getResponse(const RPSMove &winCondition) const noexcept {
        switch (winCondition.id) {
            case 'X': {
                int tmp = normalized() - 'A' - 1;
                if (tmp < 0) {
                    tmp += 3;
                }
                return RPSMove(static_cast<char>(tmp + 'A'));
                break;
            }
            case 'Y':
                return *this;
            case 'Z':
                return RPSMove(static_cast<char>((normalized() - 'A' + 1) % 3 + 'A'));
            default:
                abort();
        }
    }

    [[nodiscard]] constexpr unsigned toOutcome() const noexcept {
        return (id - 'X') * 3;
    }

    char id;
    unsigned score;
};


int main() {
    auto file = util::getInputFile();
    std::string line;
    unsigned sumA1 = 0;
    unsigned sumA2 = 0;
    while (std::getline(file, line)) {
        auto ret = util::splitString<' '>(line);
        assert(ret.size() == 2);
        RPSMove opponent(ret.front().front());
        RPSMove me(ret.back().front());
        auto resA1 = me.score + me.outcome(opponent);
        auto response = opponent.getResponse(me);
        auto resA2 = me.toOutcome() + response.score;
        std::cout << ret.front() << " vs " << ret.back() << " = " << resA1
                  << " | expected response: " << response.id << "(" << resA2 << ")" << std::endl;
        sumA1 += resA1;
        sumA2 += resA2;
    }

    std::cout << "total sum A1: " << sumA1 << std::endl;
    std::cout << "total sum A2: " << sumA2 << std::endl;
}
