#include "PmergeMe.hpp"

PmergeMe::PmergeMe() : _unsortedVec(), _unsortedDeq() {}
PmergeMe::PmergeMe(std::vector<int> const &vec)
    : _unsortedVec(vec), _unsortedDeq(vec.begin(), vec.end()) {}

PmergeMe::~PmergeMe() {}

PmergeMe::PmergeMe(PmergeMe const &other)
    : _unsortedVec(other._unsortedVec), _unsortedDeq(other._unsortedDeq) {}

PmergeMe &PmergeMe::operator=(PmergeMe const &other) {
    if (this != &other) {
        _unsortedVec = other._unsortedVec;
        _unsortedDeq = other._unsortedDeq;
    }
    return *this;
}

// ----------------------------------------------------------------
// public member functions

std::vector<int> PmergeMe::mergeInsertSortByVector() {
    if (_unsortedVec.empty())
        return _unsortedVec;
    return _mergeInsertSortVector(_unsortedVec);
}

std::deque<int> PmergeMe::mergeInsertSortByDeque() {
    if (_unsortedDeq.empty())
        return _unsortedDeq;
    return _mergeInsertSortDeque(_unsortedDeq);
}

// ----------------------------------------------------------------
// private member functions

std::vector<int> PmergeMe::_mergeInsertSortVector(std::vector<int> &vec) {
    size_t n = vec.size();
    if (n <= 1)
        return vec;

    // Step 1: Create pairs and ensure larger element is first in each pair
    std::vector<int> larger;
    std::vector<int> smaller;
    bool hasOddElement = false;
    int oddElement = 0;

    // Process pairs
    for (size_t i = 0; i < n - 1; i += 2) {
        if (vec[i] > vec[i + 1]) {
            larger.push_back(vec[i]);
            smaller.push_back(vec[i + 1]);
        } else {
            larger.push_back(vec[i + 1]);
            smaller.push_back(vec[i]);
        }
    }

    // Handle odd element
    if (n % 2 == 1) {
        hasOddElement = true;
        oddElement = vec[n - 1];
    }

    // Step 2: Recursively sort the larger elements
    if (larger.size() > 1)
        larger = _mergeInsertSortVector(larger);

    // Step 3: Create result starting with the first smaller element and all
    // larger elements
    std::vector<int> result;
    if (!smaller.empty())
        result.push_back(smaller[0]);

    for (size_t i = 0; i < larger.size(); ++i) {
        result.push_back(larger[i]);
    }

    // Step 4: Insert remaining smaller elements using binary insertion
    for (size_t i = 1; i < smaller.size(); ++i) {
        _binaryInsert(result, smaller[i], result.size());
    }

    // Step 5: Insert odd element if it exists
    if (hasOddElement) {
        _binaryInsert(result, oddElement, result.size());
    }

    return result;
}

void PmergeMe::_binaryInsert(std::vector<int> &vec, int value, int end) {
    int left = 0;
    int right = end;

    while (left < right) {
        int mid = left + (right - left) / 2;
        if (vec[mid] < value)
            left = mid + 1;
        else
            right = mid;
    }

    vec.insert(vec.begin() + left, value);
}

std::deque<int> PmergeMe::_mergeInsertSortDeque(std::deque<int> &deq) {
    size_t n = deq.size();
    if (n <= 1)
        return deq;

    // Step 1: Create pairs and ensure larger element is first in each pair
    std::deque<int> larger;
    std::deque<int> smaller;
    bool hasOddElement = false;
    int oddElement = 0;

    // Process pairs
    for (size_t i = 0; i < n - 1; i += 2) {
        if (deq[i] > deq[i + 1]) {
            larger.push_back(deq[i]);
            smaller.push_back(deq[i + 1]);
        } else {
            larger.push_back(deq[i + 1]);
            smaller.push_back(deq[i]);
        }
    }

    // Handle odd element
    if (n % 2 == 1) {
        hasOddElement = true;
        oddElement = deq[n - 1];
    }

    // Step 2: Recursively sort the larger elements
    if (larger.size() > 1)
        larger = _mergeInsertSortDeque(larger);

    // Step 3: Create result starting with the first smaller element and all
    // larger elements
    std::deque<int> result;
    if (!smaller.empty())
        result.push_back(smaller[0]);

    for (size_t i = 0; i < larger.size(); ++i) {
        result.push_back(larger[i]);
    }

    // Step 4: Insert remaining smaller elements using binary insertion
    for (size_t i = 1; i < smaller.size(); ++i) {
        _binaryInsert(result, smaller[i], result.size());
    }

    // Step 5: Insert odd element if it exists
    if (hasOddElement) {
        _binaryInsert(result, oddElement, result.size());
    }

    return result;
}

void PmergeMe::_binaryInsert(std::deque<int> &deq, int value, int end) {
    int left = 0;
    int right = end;

    while (left < right) {
        int mid = left + (right - left) / 2;
        if (deq[mid] < value)
            left = mid + 1;
        else
            right = mid;
    }

    deq.insert(deq.begin() + left, value);
}
