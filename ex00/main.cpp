#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "BitcoinExchange.hpp"

const std::string BC_EX_RATE_DB_PATH = "data.csv";
// Error Messages
const std::string ERR_FILE_OPEN = "Error: could not open file.";

void testBitcoinExchange();
void testBCExchangeCases(BitcoinExchange bc);

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

#if defined(DEBUG)
    std::cout << "Debug mode enabled" << std::endl;
    testBitcoinExchange();
    testBCExchangeCases(btc);
    std::cout << "All tests passed!" << std::endl;
    std::cout << "----------------------------" << std::endl;
#endif

    std::string line;
    std::getline(inputFile, line);
    if (!(line == "date | value")) {
        std::cerr << "Error: first line is not a header." << std::endl;
        inputFile.close();
        return EXIT_FAILURE;
    }
    while (std::getline(inputFile, line)) {
        if (line == "") {
            // std::cout << line << std::endl;
            continue;  // skip header line
        }
        try {
            std::cout << btc.exchange(line) << std::endl;
        } catch (std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        // std::cout << line << std::endl;
    }
    inputFile.close();

    return EXIT_SUCCESS;
}

void testBitcoinExchange() {
    // Test default constructor
    BitcoinExchange btc1;
    assert(btc1.getRateDBSize() == 0);
    try {
        btc1.exchange("2021-01-02 | 100");
        assert(false);  // Should not reach here
    } catch (std::runtime_error &e) {
        assert(std::string(e.what()) == "master database is empty.");
    }

    // Test parameterized constructor with valid database path
    BitcoinExchange btc2(BC_EX_RATE_DB_PATH);
    assert(btc2.getRateDBSize() == 1612);
    testBCExchangeCases(btc2);

    // Test parameterized constructor with invalid database path
    try {
        BitcoinExchange btc3("invalid_path.csv");
        assert(false);  // Should not reach here
    } catch (std::runtime_error &e) {
        assert(std::string(e.what()) == "could not open master database file.");
    }

    // Test copy constructor
    BitcoinExchange btc4 = btc2;
    assert(btc4.getRateDBSize() == 1612);
    testBCExchangeCases(btc4);

    // Test assignment operator
    BitcoinExchange btc5;
    btc5 = btc2;
    assert(btc5.getRateDBSize() == 1612);
    testBCExchangeCases(btc5);
}

void testBCExchangeCases(BitcoinExchange bc) {
    // Test exchange method with valid input
    // 2021-01-02,32195.46
    assert(bc.exchange("2021-01-02 | 100") == "2021-01-02 => 100 = 3219546");
    assert(bc.exchange("2021-01-02 | 1000") == "2021-01-02 => 1000 = 32195460");
    assert(bc.exchange("2021-01-02 | 0") == "2021-01-02 => 0 = 0");
    assert(bc.exchange("2021-01-02 | 0.1") ==
           "2021-01-02 => 0.1 = 3219.55");  // rounded
    assert(bc.exchange("2021-01-02 | 0.12345") ==
           "2021-01-02 => 0.12345 = 3974.53");  // rounded
    assert(bc.exchange("2021-01-02 | 425.67") ==
           "2021-01-02 => 425.67 = 13704641");  // rounded
    assert(bc.exchange("    2021-01-02   |   100    ") ==
           "2021-01-02 => 100 = 3219546");
    assert(bc.exchange("    2021-01-02   |   1000    ") ==
           "2021-01-02 => 1000 = 32195460");
    assert(bc.exchange("    2021-01-02   |   0    ") == "2021-01-02 => 0 = 0");
    assert(bc.exchange("    2021-01-02   |   0.12345    ") ==
           "2021-01-02 => 0.12345 = 3974.53");  // rounded
    assert(bc.exchange("    2021-01-02   |   425.67    ") ==
           "2021-01-02 => 425.67 = 13704641");
    assert(bc.exchange("2021-01-02 | 00100") == "2021-01-02 => 100 = 3219546");
    assert(bc.exchange("2021-01-02 | 000.100") ==
           "2021-01-02 => 0.1 = 3219.55");  // rounded
    assert(bc.exchange("2021-01-02 | 0000.100") ==
           "2021-01-02 => 0.1 = 3219.55");  // rounded
    assert(bc.exchange("2021-01-02 | 0000.000100") ==
           "2021-01-02 => 0.0001 = 3.21955");  // rounded
    assert(bc.exchange("2021-01-02 | 0000.00000100") ==
           "2021-01-02 => 0.000001 = 0.0321955");  // rounded
    // Test maximum range of float value that can be handledd
    assert(
        bc.exchange("2021-01-02 | 1000.00") == "2021-01-02 => 1000 = 32195460");
    assert(bc.exchange("2021-01-02 | 0.00001") ==
           "2021-01-02 => 0.00001 = 0.321955");  // rounded
    assert(bc.exchange("2021-01-02 | 999.999") ==
           "2021-01-02 => 999.999 = 32195428");  // rounded
    assert(bc.exchange("2021-01-02 | 0.0") == "2021-01-02 => 0 = 0");
    // Test iregular date formats that are still valid
    assert(bc.exchange("2021-1-2 | 100") == "2021-01-02 => 100 = 3219546");
    assert(bc.exchange("0002021-00001-00002 | 100") ==
           "2021-01-02 => 100 = 3219546");
    assert(bc.exchange("2021-01-02|100") == "2021-01-02 => 100 = 3219546");
    assert(bc.exchange("2021-01-02 |100") == "2021-01-02 => 100 = 3219546");
    assert(bc.exchange("2021-01-02 |  100") == "2021-01-02 => 100 = 3219546");
    assert(bc.exchange("2021-01-02 | 100  ") == "2021-01-02 => 100 = 3219546");
    // Test exchange method with date not in database
    // 2009-01-02,0
    assert(bc.exchange("0-01-01 | 100") == "0000-01-01 => 100 = 0");
    assert(bc.exchange("1900-01-01 | 100") == "1900-01-01 => 100 = 0");
    // 2022-03-29,47115.93
    assert(bc.exchange("2345-01-01 | 100") == "2345-01-01 => 100 = 4711593");

    // Test exchange method with invalid date format
    try {
        bc.exchange("20 21-01-02 | 100");
        assert(false);  // Should not reach here
    } catch (std::invalid_argument &e) {
        assert(std::string(e.what()) == "bad input => 20 21-01-02 ");
    }
    try {
        bc.exchange("2021-0 1-02 | 100");
        assert(false);  // Should not reach here
    } catch (std::invalid_argument &e) {
        assert(std::string(e.what()) == "bad input => 2021-0 1-02 ");
    }
    try {
        bc.exchange("2021-01-0 2 | 100");
        assert(false);  // Should not reach here
    } catch (std::invalid_argument &e) {
        assert(std::string(e.what()) == "bad input => 2021-01-0 2 ");
    }
    try {
        bc.exchange("2021 -01-02 | 100");
        assert(false);  // Should not reach here
    } catch (std::invalid_argument &e) {
        assert(std::string(e.what()) == "bad input => 2021 -01-02 ");
    }
    try {
        bc.exchange("2021-01 -02 | 100");
        assert(false);  // Should not reach here
    } catch (std::invalid_argument &e) {
        assert(std::string(e.what()) == "bad input => 2021-01 -02 ");
    }
    try {
        bc.exchange("2021-01- 02 | 100");
        assert(false);  // Should not reach here
    } catch (std::invalid_argument &e) {
        assert(std::string(e.what()) == "bad input => 2021-01- 02 ");
    }
    try {
        bc.exchange("2023/01/01 | 100");
        assert(false);  // Should not reach here
    } catch (std::invalid_argument &e) {
        assert(std::string(e.what()) == "bad input => 2023/01/01 ");
    }
    try {
        bc.exchange("-1-01-01 | 100");
        assert(false);  // Should not reach here
    } catch (std::invalid_argument &e) {
        assert(std::string(e.what()) == "bad input => -1-01-01 ");
    }

    // Test exchange method with negative value
    try {
        bc.exchange("2021-01-02 | -1");
        assert(false);  // Should not reach here
    } catch (std::invalid_argument &e) {
        assert(std::string(e.what()) == "not a positive number.");
    }

    // Test exchange method with value greater than 1000
    try {
        bc.exchange("2021-01-02 | 1001");
        assert(false);  // Should not reach here
    } catch (std::invalid_argument &e) {
        assert(std::string(e.what()) == "too large a number.");
    }
    // Test exchange method with invalid value format
    try {
        bc.exchange("2021-01-02 | abc");
        assert(false);  // Should not reach here
    } catch (std::invalid_argument &e) {
        assert(std::string(e.what()) == "bad input => abc");
    }
}
