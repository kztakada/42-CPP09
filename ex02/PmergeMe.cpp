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

    // Handle odd element
    bool hasOddElement = false;
    int oddElement = 0;
    if (n % 2 == 1) {
        hasOddElement = true;
        oddElement = vec[n - 1];
    }

    // Step 1&2 Combined: Create pairs and recursively sort them
    std::vector<std::pair<int, int> > pairs;
    for (size_t i = 0; i < n - 1; i += 2) {
        if (vec[i] > vec[i + 1]) {
            pairs.push_back(std::pair<int, int>(vec[i], vec[i + 1]));
        } else {
            pairs.push_back(std::pair<int, int>(vec[i + 1], vec[i]));
        }
    }

    // Recursively sort pairs if more than one
    if (pairs.size() > 1) {
        _mergeInsertSortPairs(pairs);
    }

    // Step 3: Create result with all larger elements first
    std::vector<int> result;
    for (size_t i = 0; i < pairs.size(); ++i) {
        result.push_back(pairs[i].first);
    }

    // Step 4: Insert smaller elements using Ford-Johnson order
    if (!pairs.empty()) {
        // Extract smaller elements for insertion
        std::vector<int> smaller;
        for (size_t i = 0; i < pairs.size(); ++i) {
            smaller.push_back(pairs[i].second);
        }

        // smaller[0] is inserted first (unconditionally at the front)
        result.insert(result.begin(), smaller[0]);

        // Insert remaining elements in Ford-Johnson order
        if (smaller.size() > 1) {
            _insertWithJacobsthalOrder(result, smaller);
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

    // Create pairs with original indices to track them through sorting
    std::vector<std::pair<std::pair<int, int>, size_t> > indexedPairs;
    for (size_t i = 0; i < pairs.size(); ++i) {
        indexedPairs.push_back(
            std::pair<std::pair<int, int>, size_t>(pairs[i], i));
    }

    // Extract larger elements with their original indices
    std::vector<std::pair<int, size_t> > largerWithIndex;
    for (size_t i = 0; i < indexedPairs.size(); ++i) {
        largerWithIndex.push_back(std::pair<int, size_t>(
            indexedPairs[i].first.first, indexedPairs[i].second));
    }

    if (largerWithIndex.size() > 1) {
        // Sort indexed larger elements by their values
        _mergeInsertSortIndexedVector(largerWithIndex);

        // Reorder pairs based on sorted indices
        std::vector<std::pair<int, int> > sortedPairs;
        for (size_t i = 0; i < largerWithIndex.size(); ++i) {
            size_t originalIndex = largerWithIndex[i].second;
            sortedPairs.push_back(pairs[originalIndex]);
        }
        pairs = sortedPairs;
    }
}

void PmergeMe::_mergeInsertSortIndexedVector(
    std::vector<std::pair<int, size_t> > &indexedVec) {
    if (indexedVec.size() <= 1)
        return;

    // Extract just the values for sorting
    std::vector<int> values;
    for (size_t i = 0; i < indexedVec.size(); ++i) {
        values.push_back(indexedVec[i].first);
    }

    // Get the sorted values
    std::vector<int> sortedValues = _mergeInsertSortVector(values);

    // Create a mapping from sorted values back to original indices
    // Handle duplicates by keeping track of used indices
    std::vector<bool> used(indexedVec.size(), false);
    std::vector<std::pair<int, size_t> > result;

    for (size_t i = 0; i < sortedValues.size(); ++i) {
        for (size_t j = 0; j < indexedVec.size(); ++j) {
            if (!used[j] && indexedVec[j].first == sortedValues[i]) {
                result.push_back(indexedVec[j]);
                used[j] = true;
                break;
            }
        }
    }

    indexedVec = result;
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

    // Handle odd element
    bool hasOddElement = false;
    int oddElement = 0;
    if (n % 2 == 1) {
        hasOddElement = true;
        oddElement = deq[n - 1];
    }

    // Step 1&2 Combined: Create pairs and recursively sort them
    std::deque<std::pair<int, int> > pairs;
    for (size_t i = 0; i < n - 1; i += 2) {
        if (deq[i] > deq[i + 1]) {
            pairs.push_back(std::pair<int, int>(deq[i], deq[i + 1]));
        } else {
            pairs.push_back(std::pair<int, int>(deq[i + 1], deq[i]));
        }
    }

    // Recursively sort pairs if more than one
    if (pairs.size() > 1) {
        _mergeInsertSortPairsDeque(pairs);
    }

    // Step 3: Create result with all larger elements first
    std::deque<int> result;
    for (size_t i = 0; i < pairs.size(); ++i) {
        result.push_back(pairs[i].first);
    }

    // Step 4: Insert smaller elements using Ford-Johnson order
    if (!pairs.empty()) {
        // Extract smaller elements for insertion
        std::deque<int> smaller;
        for (size_t i = 0; i < pairs.size(); ++i) {
            smaller.push_back(pairs[i].second);
        }

        // smaller[0] is inserted first (unconditionally at the front)
        result.push_front(smaller[0]);

        // Insert remaining elements in Ford-Johnson order
        if (smaller.size() > 1) {
            _insertWithJacobsthalOrder(result, smaller);
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

    // Create pairs with original indices to track them through sorting
    std::deque<std::pair<std::pair<int, int>, size_t> > indexedPairs;
    for (size_t i = 0; i < pairs.size(); ++i) {
        indexedPairs.push_back(
            std::pair<std::pair<int, int>, size_t>(pairs[i], i));
    }

    // Extract larger elements with their original indices
    std::deque<std::pair<int, size_t> > largerWithIndex;
    for (size_t i = 0; i < indexedPairs.size(); ++i) {
        largerWithIndex.push_back(std::pair<int, size_t>(
            indexedPairs[i].first.first, indexedPairs[i].second));
    }

    if (largerWithIndex.size() > 1) {
        // Sort indexed larger elements by their values
        _mergeInsertSortIndexedDeque(largerWithIndex);

        // Reorder pairs based on sorted indices
        std::deque<std::pair<int, int> > sortedPairs;
        for (size_t i = 0; i < largerWithIndex.size(); ++i) {
            size_t originalIndex = largerWithIndex[i].second;
            sortedPairs.push_back(pairs[originalIndex]);
        }
        pairs = sortedPairs;
    }
}

void PmergeMe::_mergeInsertSortIndexedDeque(
    std::deque<std::pair<int, size_t> > &indexedDeq) {
    if (indexedDeq.size() <= 1)
        return;

    // Extract just the values for sorting
    std::deque<int> values;
    for (size_t i = 0; i < indexedDeq.size(); ++i) {
        values.push_back(indexedDeq[i].first);
    }

    // Get the sorted values
    std::deque<int> sortedValues = _mergeInsertSortDeque(values);

    // Create a mapping from sorted values back to original indices
    // Handle duplicates by keeping track of used indices
    std::deque<bool> used(indexedDeq.size(), false);
    std::deque<std::pair<int, size_t> > result;

    for (size_t i = 0; i < sortedValues.size(); ++i) {
        for (size_t j = 0; j < indexedDeq.size(); ++j) {
            if (!used[j] && indexedDeq[j].first == sortedValues[i]) {
                result.push_back(indexedDeq[j]);
                used[j] = true;
                break;
            }
        }
    }

    indexedDeq = result;
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

// Generate Ford-Johnson group endpoints for insertion order
std::vector<size_t> PmergeMe::_generateJacobsthalSequence(size_t n) {
    std::vector<size_t> endpoints;
    if (n == 0)
        return endpoints;

    size_t power = 1;
    while (true) {
        // Calculate (3 * 2^k - 2 * (-1)^k) / 3 for group endpoints
        size_t endpoint;
        if (power % 2 == 1) {  // k is odd, (-1)^k = -1
            endpoint = (3 * power + 2) / 3;
        } else {  // k is even, (-1)^k = 1
            endpoint = (3 * power - 2) / 3;
        }

        if (endpoint > n)
            break;
        endpoints.push_back(endpoint);
        power *= 2;
    }

    return endpoints;
}

std::deque<size_t> PmergeMe::_generateJacobsthalSequenceDeque(size_t n) {
    std::deque<size_t> endpoints;
    if (n == 0)
        return endpoints;

    size_t power = 1;
    while (true) {
        // Calculate (3 * 2^k - 2 * (-1)^k) / 3 for group endpoints
        size_t endpoint;
        if (power % 2 == 1) {  // k is odd, (-1)^k = -1
            endpoint = (3 * power + 2) / 3;
        } else {  // k is even, (-1)^k = 1
            endpoint = (3 * power - 2) / 3;
        }

        if (endpoint > n)
            break;
        endpoints.push_back(endpoint);
        power *= 2;
    }

    return endpoints;
}

void PmergeMe::_insertWithJacobsthalOrder(
    std::vector<int> &result, std::vector<int> const &smaller) {
    if (smaller.size() <= 1)
        return;

    std::vector<size_t> endpoints = _generateJacobsthalSequence(
        smaller.size() - 1);  // -1 because smaller[0] is already inserted
    std::vector<bool> inserted(smaller.size(), false);
    inserted[0] = true;  // smaller[0] is already inserted

    // Insert elements according to Ford-Johnson group endpoints
    for (size_t i = 0; i < endpoints.size(); ++i) {
        size_t groupEnd =
            endpoints[i];  // 1-based index from smaller[1] onwards
        if (groupEnd >= smaller.size())
            groupEnd = smaller.size() - 1;

        size_t groupStart = (i == 0) ? 1 : endpoints[i - 1] + 1;

        // Insert elements in reverse order within each group
        for (size_t j = groupEnd; j >= groupStart; --j) {
            if (j < smaller.size() && !inserted[j]) {
                // Calculate maximum search position for this element
                int maxPos = static_cast<int>(result.size());
                _binaryInsertOptimized(result, smaller[j], maxPos);
                inserted[j] = true;
            }
            if (j == groupStart)
                break;  // Prevent underflow
        }
    }

    // Insert any remaining elements
    for (size_t i = 1; i < smaller.size();
        ++i) {  // Start from 1, skip smaller[0]
        if (!inserted[i]) {
            _binaryInsert(result, smaller[i], result.size());
        }
    }
}

void PmergeMe::_insertWithJacobsthalOrder(
    std::deque<int> &result, std::deque<int> const &smaller) {
    if (smaller.size() <= 1)
        return;

    std::deque<size_t> endpoints = _generateJacobsthalSequenceDeque(
        smaller.size() - 1);  // -1 because smaller[0] is already inserted
    std::deque<bool> inserted(smaller.size(), false);
    inserted[0] = true;  // smaller[0] is already inserted

    // Insert elements according to Ford-Johnson group endpoints
    for (size_t i = 0; i < endpoints.size(); ++i) {
        size_t groupEnd =
            endpoints[i];  // 1-based index from smaller[1] onwards
        if (groupEnd >= smaller.size())
            groupEnd = smaller.size() - 1;

        size_t groupStart = (i == 0) ? 1 : endpoints[i - 1] + 1;

        // Insert elements in reverse order within each group
        for (size_t j = groupEnd; j >= groupStart; --j) {
            if (j < smaller.size() && !inserted[j]) {
                // Calculate maximum search position for this element
                int maxPos = static_cast<int>(result.size());
                _binaryInsertOptimized(result, smaller[j], maxPos);
                inserted[j] = true;
            }
            if (j == groupStart)
                break;  // Prevent underflow
        }
    }

    // Insert any remaining elements
    for (size_t i = 1; i < smaller.size();
        ++i) {  // Start from 1, skip smaller[0]
        if (!inserted[i]) {
            _binaryInsert(result, smaller[i], result.size());
        }
    }
}
