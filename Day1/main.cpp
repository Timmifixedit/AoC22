#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "please specify a file to load" << std::endl;
        return 1;
    }

    std::fstream file(argv[1]);
    if (not file.is_open()) {
        std::cerr << "unable to open file '" << argv[1] << "'" << std::endl;
        return 1;
    }

    std::string line;
    std::size_t sum = 0;
    std::size_t largest = 0;
    while (std::getline(file, line)) {
        if (line.empty()) {
            std::cout << "current sum: " << sum << std::endl;
            if (sum > largest) {
                largest = sum;
            }

            sum = 0;
            continue;
        }

        sum += std::stoi(line);
    }

    std::cout << largest << std::endl;
}
