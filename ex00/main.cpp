#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "BitcoinExchange.hpp"

const std::string BC_EX_RATE_DB_PATH = "data.csv";
// Error Messages
const std::string ERR_FILE_OPEN = "Error: could not open file.";

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << ERR_FILE_OPEN << std::endl;
        return EXIT_FAILURE;
    }
    std::string filename = argv[1];

    std::ifstream inputFile(filename.c_str());
    if (!inputFile.is_open()) {
        std::cerr << ERR_FILE_OPEN << std::endl;
        return EXIT_FAILURE;
    }

    // initialize BitcoinExchange with the database path
    BitcoinExchange btc;
    try {
        btc = BitcoinExchange(BC_EX_RATE_DB_PATH);
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        inputFile.close();
        return EXIT_FAILURE;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        if (line == "date | value") {
            std::cout << line << std::endl;
            continue;  // skip header line
        }
        try {
            std::cout << btc.exchange(line) << std::endl;
        } catch (std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        std::cout << line << std::endl;
    }
    inputFile.close();

    return EXIT_SUCCESS;
}
