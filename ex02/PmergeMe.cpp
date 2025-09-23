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

    // Step 4: Insert remaining smaller elements using Jacobsthal order
    if (smaller.size() > 1) {
        std::vector<int> remainingSmaller(smaller.begin() + 1, smaller.end());
        _insertWithJacobsthalOrder(result, remainingSmaller);
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

    // Step 4: Insert remaining smaller elements using Jacobsthal order
    if (smaller.size() > 1) {
        std::deque<int> remainingSmaller(smaller.begin() + 1, smaller.end());
        _insertWithJacobsthalOrder(result, remainingSmaller);
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

// Generate Jacobsthal sequence for insertion order: 1, 3, 5, 11, 21, 43, ...
std::vector<size_t> PmergeMe::_generateJacobsthalSequence(size_t n) {
    std::vector<size_t> jacobsthal;
    if (n == 0)
        return jacobsthal;

    size_t j1 = 1;  // t_1 = 1
    size_t j2 = 1;  // t_0 = 1
    jacobsthal.push_back(j1);

    while (j1 < n) {
        size_t next = 2 * j1 + j2;  // t_k = 2 * t_{k-1} + t_{k-2}
        if (next > n)
            break;
        jacobsthal.push_back(next);
        j2 = j1;
        j1 = next;
    }

    return jacobsthal;
}

std::deque<size_t> PmergeMe::_generateJacobsthalSequenceDeque(size_t n) {
    std::deque<size_t> jacobsthal;
    if (n == 0)
        return jacobsthal;

    size_t j1 = 1;  // t_1 = 1
    size_t j2 = 1;  // t_0 = 1
    jacobsthal.push_back(j1);

    while (j1 < n) {
        size_t next = 2 * j1 + j2;  // t_k = 2 * t_{k-1} + t_{k-2}
        if (next > n)
            break;
        jacobsthal.push_back(next);
        j2 = j1;
        j1 = next;
    }

    return jacobsthal;
}

void PmergeMe::_insertWithJacobsthalOrder(
    std::vector<int> &result, std::vector<int> const &smaller) {
    if (smaller.empty())
        return;

    std::vector<size_t> jacobsthal =
        _generateJacobsthalSequence(smaller.size());
    std::vector<bool> inserted(smaller.size(), false);

    // Insert elements according to Jacobsthal sequence
    for (size_t i = 0; i < jacobsthal.size(); ++i) {
        size_t groupEnd = jacobsthal[i];
        size_t groupStart = (i == 0) ? 1 : jacobsthal[i - 1] + 1;

        // Insert elements in reverse order within each group
        for (size_t j = groupEnd; j >= groupStart && j <= smaller.size(); --j) {
            size_t idx = j - 1;  // Convert to 0-based index
            if (!inserted[idx]) {
                _binaryInsert(result, smaller[idx], result.size());
                inserted[idx] = true;
            }
        }
    }

    // Insert any remaining elements
    for (size_t i = 0; i < smaller.size(); ++i) {
        if (!inserted[i]) {
            _binaryInsert(result, smaller[i], result.size());
        }
    }
}

void PmergeMe::_insertWithJacobsthalOrder(
    std::deque<int> &result, std::deque<int> const &smaller) {
    if (smaller.empty())
        return;

    std::deque<size_t> jacobsthal =
        _generateJacobsthalSequenceDeque(smaller.size());
    std::deque<bool> inserted(smaller.size(), false);

    // Insert elements according to Jacobsthal sequence
    for (size_t i = 0; i < jacobsthal.size(); ++i) {
        size_t groupEnd = jacobsthal[i];
        size_t groupStart = (i == 0) ? 1 : jacobsthal[i - 1] + 1;

        // Insert elements in reverse order within each group
        for (size_t j = groupEnd; j >= groupStart && j <= smaller.size(); --j) {
            size_t idx = j - 1;  // Convert to 0-based index
            if (!inserted[idx]) {
                _binaryInsert(result, smaller[idx], result.size());
                inserted[idx] = true;
            }
        }
    }

    // Insert any remaining elements
    for (size_t i = 0; i < smaller.size(); ++i) {
        if (!inserted[i]) {
            _binaryInsert(result, smaller[i], result.size());
        }
    }
}
