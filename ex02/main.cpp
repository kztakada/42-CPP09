#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
#include <vector>

#include "PmergeMe.hpp"

const std::clock_t CLOCKS_PER_MS = CLOCKS_PER_SEC / 1000;
const std::clock_t CLOCKS_PER_US = CLOCKS_PER_SEC / 1000000;

void printTime(std::clock_t elapsed) {
    double elapsed_secs = static_cast<double>(elapsed) / CLOCKS_PER_SEC;
    double elapsed_ms = static_cast<double>(elapsed) / CLOCKS_PER_MS;
    double elapsed_us = static_cast<double>(elapsed) / CLOCKS_PER_US;
    if (elapsed > CLOCKS_PER_SEC) {
        std::cout << std::setw(8) << elapsed_secs << " s" << std::endl;
    } else if (elapsed > CLOCKS_PER_MS) {
        std::cout << std::setw(8) << elapsed_ms << " ms" << std::endl;
    } else {
        std::cout << std::setw(8) << elapsed_us << " us" << std::endl;
    }
}

bool invalidArgument(const std::string &str) {
    for (size_t i = 0; i < str.length(); ++i) {
        if (!isdigit(str[i]) && !(i == 0 && str[i] == '-')) {
            return true;
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Error: At least one integer argument is required."
                  << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<int> numbers;
    for (int i = 1; i < argc; ++i) {
        try {
            if (invalidArgument(argv[i])) {
                throw std::invalid_argument("Invalid argument");
            }
            int num = atoi(argv[i]);
            if (num < 0) {
                std::cerr << "Error: Negative numbers are not allowed."
                          << std::endl;
                return EXIT_FAILURE;
            }
            numbers.push_back(num);
        } catch (const std::invalid_argument &) {
            std::cerr << "Error: Invalid integer '" << argv[i] << "'."
                      << std::endl;
            return EXIT_FAILURE;
        } catch (const std::out_of_range &) {
            std::cerr << "Error: Integer '" << argv[i] << "' is out of range."
                      << std::endl;
            return EXIT_FAILURE;
        }
    }

    PmergeMe pmergeMe(numbers);

    // Print original sequence
    std::cout << "Before: ";
    for (size_t i = 0; i < numbers.size(); ++i) {
        std::cout << numbers[i];
        if (i != numbers.size() - 1)
            std::cout << " ";
    }
    std::cout << std::endl;

    std::clock_t start, end;
    // Sort by vector
    std::clock_t elapsedVec;
    start = std::clock();
    std::vector<int> sortedByVec = pmergeMe.mergeInsertSortByVector();
    end = std::clock();
    elapsedVec = end - start;

    // Sort by deque
    std::clock_t elapsedDeq;
    start = std::clock();
    std::deque<int> sortedByDeq = pmergeMe.mergeInsertSortByDeque();
    end = std::clock();
    elapsedDeq = end - start;

    // Print sorted sequence
    std::cout << "After:  ";
    for (size_t i = 0; i < sortedByVec.size(); ++i) {
        std::cout << sortedByVec[i];
        if (i != sortedByVec.size() - 1)
            std::cout << " ";
    }
    std::cout << std::endl;

    // Print time taken
    std::cout << "Time to process a range of " << std::setw(3) << numbers.size()
              << " elements with std::vector :";
    printTime(elapsedVec);
    std::cout << "Time to process a range of " << std::setw(3) << numbers.size()
              << " elements with std::deque  :";
    printTime(elapsedDeq);

#ifdef DEBUG
    // Verify both sorted results are the same
    std::vector<int> sortedByArgorithm = numbers;
    std::sort(sortedByArgorithm.begin(), sortedByArgorithm.end());
    for (size_t i = 0; i < sortedByArgorithm.size(); ++i) {
        if (sortedByArgorithm[i] != sortedByVec[i] ||
            sortedByArgorithm[i] != sortedByDeq[i]) {
            std::cout << "argorithm: " << sortedByArgorithm[i]
                      << ", vec: " << sortedByVec[i]
                      << ", deq: " << sortedByDeq[i] << std::endl;
        }

        assert(sortedByArgorithm[i] == sortedByVec[i]);
        assert(sortedByArgorithm[i] == sortedByDeq[i]);
    }

#endif

    return EXIT_SUCCESS;
}
