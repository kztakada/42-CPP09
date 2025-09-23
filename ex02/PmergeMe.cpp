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

    // Step 2: Recursively sort the larger elements with paired smaller elements
    if (larger.size() > 1) {
        std::vector<std::pair<int, int> > pairs;
        for (size_t i = 0; i < larger.size(); ++i) {
            pairs.push_back(std::pair<int, int>(larger[i], smaller[i]));
        }
        _mergeInsertSortPairs(pairs);

        // Update both larger and smaller arrays based on sorted pairs
        for (size_t i = 0; i < pairs.size(); ++i) {
            larger[i] = pairs[i].first;
            smaller[i] = pairs[i].second;
        }
    }

    // Step 3: Create result with all larger elements first
    std::vector<int> result;
    for (size_t i = 0; i < larger.size(); ++i) {
        result.push_back(larger[i]);
    }

    // Step 4: Insert smaller elements - first element (smaller[0]) goes to
    // front unconditionally
    if (!smaller.empty()) {
        result.insert(result.begin(), smaller[0]);

        // Insert remaining smaller elements using Jacobsthal order
        if (smaller.size() > 1) {
            std::vector<int> remainingSmaller(
                smaller.begin() + 1, smaller.end());
            _insertWithJacobsthalOrder(result, remainingSmaller);
        }
    }

    // Step 5: Insert odd element if it exists
    if (hasOddElement) {
        _binaryInsert(result, oddElement, result.size());
    }

    return result;
}

void PmergeMe::_mergeInsertSortPairs(std::vector<std::pair<int, int> > &pairs) {
    if (pairs.size() <= 1)
        return;

    // Extract larger elements and recursively sort by them
    std::vector<int> largerElements;
    for (size_t i = 0; i < pairs.size(); ++i) {
        largerElements.push_back(pairs[i].first);
    }

    if (largerElements.size() > 1) {
        largerElements = _mergeInsertSortVector(largerElements);

        // Reorder pairs based on sorted larger elements
        std::vector<std::pair<int, int> > sortedPairs;
        for (size_t i = 0; i < largerElements.size(); ++i) {
            for (size_t j = 0; j < pairs.size(); ++j) {
                if (pairs[j].first == largerElements[i]) {
                    sortedPairs.push_back(pairs[j]);
                    break;
                }
            }
        }
        pairs = sortedPairs;
    }
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

void PmergeMe::_binaryInsertOptimized(
    std::vector<int> &vec, int value, int maxPos) {
    int left = 0;
    int right = (maxPos < static_cast<int>(vec.size()))
                    ? maxPos
                    : static_cast<int>(vec.size());

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

    // Step 2: Recursively sort the larger elements with paired smaller elements
    if (larger.size() > 1) {
        std::deque<std::pair<int, int> > pairs;
        for (size_t i = 0; i < larger.size(); ++i) {
            pairs.push_back(std::pair<int, int>(larger[i], smaller[i]));
        }
        _mergeInsertSortPairsDeque(pairs);

        // Update both larger and smaller arrays based on sorted pairs
        for (size_t i = 0; i < pairs.size(); ++i) {
            larger[i] = pairs[i].first;
            smaller[i] = pairs[i].second;
        }
    }

    // Step 3: Create result with all larger elements first
    std::deque<int> result;
    for (size_t i = 0; i < larger.size(); ++i) {
        result.push_back(larger[i]);
    }

    // Step 4: Insert smaller elements - first element (smaller[0]) goes to
    // front unconditionally
    if (!smaller.empty()) {
        result.push_front(smaller[0]);

        // Insert remaining smaller elements using Jacobsthal order
        if (smaller.size() > 1) {
            std::deque<int> remainingSmaller(
                smaller.begin() + 1, smaller.end());
            _insertWithJacobsthalOrder(result, remainingSmaller);
        }
    }

    // Step 5: Insert odd element if it exists
    if (hasOddElement) {
        _binaryInsert(result, oddElement, result.size());
    }

    return result;
}

void PmergeMe::_mergeInsertSortPairsDeque(
    std::deque<std::pair<int, int> > &pairs) {
    if (pairs.size() <= 1)
        return;

    // Extract larger elements and recursively sort by them
    std::deque<int> largerElements;
    for (size_t i = 0; i < pairs.size(); ++i) {
        largerElements.push_back(pairs[i].first);
    }

    if (largerElements.size() > 1) {
        largerElements = _mergeInsertSortDeque(largerElements);

        // Reorder pairs based on sorted larger elements
        std::deque<std::pair<int, int> > sortedPairs;
        for (size_t i = 0; i < largerElements.size(); ++i) {
            for (size_t j = 0; j < pairs.size(); ++j) {
                if (pairs[j].first == largerElements[i]) {
                    sortedPairs.push_back(pairs[j]);
                    break;
                }
            }
        }
        pairs = sortedPairs;
    }
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

void PmergeMe::_binaryInsertOptimized(
    std::deque<int> &deq, int value, int maxPos) {
    int left = 0;
    int right = (maxPos < static_cast<int>(deq.size()))
                    ? maxPos
                    : static_cast<int>(deq.size());

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

    size_t insertedCount = 0;

    // Insert elements according to Jacobsthal sequence
    for (size_t i = 0; i < jacobsthal.size(); ++i) {
        size_t groupEnd = jacobsthal[i];
        if (groupEnd > smaller.size())
            groupEnd = smaller.size();

        size_t groupStart = (i == 0) ? 1 : jacobsthal[i - 1] + 1;

        // Insert elements in reverse order within each group
        for (size_t j = groupEnd; j >= groupStart; --j) {
            size_t idx = j - 1;  // Convert to 0-based index
            if (idx < smaller.size() && !inserted[idx]) {
                // Calculate maximum search position for this element
                int maxPos = static_cast<int>(insertedCount + idx + 2);
                _binaryInsertOptimized(result, smaller[idx], maxPos);
                inserted[idx] = true;
                insertedCount++;
            }
            if (j == groupStart)
                break;  // Prevent underflow
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

    size_t insertedCount = 0;

    // Insert elements according to Jacobsthal sequence
    for (size_t i = 0; i < jacobsthal.size(); ++i) {
        size_t groupEnd = jacobsthal[i];
        if (groupEnd > smaller.size())
            groupEnd = smaller.size();

        size_t groupStart = (i == 0) ? 1 : jacobsthal[i - 1] + 1;

        // Insert elements in reverse order within each group
        for (size_t j = groupEnd; j >= groupStart; --j) {
            size_t idx = j - 1;  // Convert to 0-based index
            if (idx < smaller.size() && !inserted[idx]) {
                // Calculate maximum search position for this element
                int maxPos = static_cast<int>(insertedCount + idx + 2);
                _binaryInsertOptimized(result, smaller[idx], maxPos);
                inserted[idx] = true;
                insertedCount++;
            }
            if (j == groupStart)
                break;  // Prevent underflow
        }
    }

    // Insert any remaining elements
    for (size_t i = 0; i < smaller.size(); ++i) {
        if (!inserted[i]) {
            _binaryInsert(result, smaller[i], result.size());
        }
    }
}
