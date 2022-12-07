//
// Created by tim on 07.12.22.
//

#ifndef AOC22_UTIL_HPP
#define AOC22_UTIL_HPP

#include <fstream>
namespace util {
    auto getInputFile(int argc, char** argv) noexcept -> std::fstream;

    namespace traits {
        template<typename Container, typename = std::void_t<>>
        struct value_type {};

        template<typename Container>
        struct value_type<Container, std::void_t<decltype(*std::ranges::begin(std::declval<std::remove_cvref_t<Container>&>()))>> {
            using type = std::remove_cvref_t<decltype(*std::ranges::begin(std::declval<std::remove_cvref_t<Container>&>()))>;
        };

        template<typename T>
        using value_type_t = typename value_type<T>::type;
    }
}

#endif //AOC22_UTIL_HPP
