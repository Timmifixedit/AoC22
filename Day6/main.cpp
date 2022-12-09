#include <string>
#include <iostream>
#include <Iterators.hpp>
#include <bidirectional_map.hpp>
#include <cassert>
#include "../util/util.hpp"


int main(int argc, char **argv) {
    auto file = util::getInputFile(argc, argv);
    std::string line;
    std::getline(file, line);
    bimap::bidirectional_map<char, std::size_t, std::unordered_map, std::map> markerChars;
    auto &inverse = markerChars.inverse();
    bool packageStarted = false;
    for (auto [pos, c] : iterators::const_enumerate(line, 1)) {
        if (markerChars.contains(c)) {
            auto it = inverse.find(markerChars.at(c));
            assert(it != inverse.end());
            inverse.erase(inverse.begin(), ++it);
        }

        markerChars.emplace(c, pos);
        if (markerChars.size() == 4 && !packageStarted) {
            std::cout << "package marker at " << pos << std::endl;
            packageStarted = true;
        }

        if (markerChars.size() == 14) {
            std::cout << "message marker at " << pos << std::endl;
            break;
        }
    }
}
