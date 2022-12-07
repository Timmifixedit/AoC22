//
// Created by tim on 07.12.22.
//

#include <iostream>
#include "util.hpp"

namespace util {
    auto getInputFile(int argc, char **argv) noexcept -> std::fstream {
        if (argc != 2) {
            std::cerr << "please specify a file to load" << std::endl;
            std::exit(1);
        }

        std::fstream file(argv[1]);
        if (not file.is_open()) {
            std::cerr << "unable to open file '" << argv[1] << "'" << std::endl;
            std::exit(1);
        }

        return file;
    }
}
