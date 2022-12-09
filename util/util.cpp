//
// Created by tim on 07.12.22.
//

#include <iostream>
#include "util.hpp"

namespace util {
    auto getInputFile(int argc, char **argv) noexcept -> std::fstream {
        std::string fLoc;
        if (argc == 2) {
            fLoc = argv[1];
        } else {
            std::cout << "No input specified. Trying to open default file " << __IFILE__ << std::endl;
            fLoc = __IFILE__;
        }

        std::fstream file(fLoc);
        if (not file.is_open()) {
            std::cerr << "unable to open file '" << fLoc << "'" << std::endl;
            std::exit(1);
        }

        return file;
    }
}
