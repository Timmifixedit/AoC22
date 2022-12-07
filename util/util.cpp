//
// Created by tim on 07.12.22.
//

#include <iostream>
#include "util.hpp"

namespace util {
    auto getInputFile() noexcept -> std::fstream {
        std::fstream file(__IFILE__);
        if (not file.is_open()) {
            std::cerr << "unable to open file '" << __IFILE__ << "'" << std::endl;
            std::exit(1);
        }

        return file;
    }
}
