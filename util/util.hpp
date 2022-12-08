//
// Created by tim on 07.12.22.
//

#ifndef AOC22_UTIL_HPP
#define AOC22_UTIL_HPP

#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <sstream>


namespace util {
    namespace impl {
        template<char Delim>
        struct Word : std::string {};

        template<char Delim>
        std::istream &operator>>(std::istream &in, Word<Delim> &word) {
            std::getline(in, word, Delim);
            return in;
        }
    }
    auto getInputFile() noexcept -> std::fstream;

    template<char Delim>
    auto splitString(const std::string &string) -> std::vector<std::string> {
        std::istringstream iss(string);
        using iter = std::istream_iterator<impl::Word<Delim>>;
        std::vector<std::string> ret(iter{iss}, iter{});
        return ret;
    }

}

#endif //AOC22_UTIL_HPP
