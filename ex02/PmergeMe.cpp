#include "PmergeMe.hpp"

#include <iostream>

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

    // Create indexed vector from unsorted vector
    std::vector<IndexedInt> indexedVec;
    for (size_t i = 0; i < _unsortedVec.size(); ++i) {
        indexedVec.push_back(IndexedInt(_unsortedVec[i], i));
    }

    // Sort the indexed vector
    std::vector<IndexedInt> sortedIndexedVec =
        _mergeInsertSortVector(indexedVec);

    // Extract the sorted values
    std::vector<int> result;
    for (size_t i = 0; i < sortedIndexedVec.size(); ++i) {
        result.push_back(sortedIndexedVec[i].first);
    }

    return result;
}

std::deque<int> PmergeMe::mergeInsertSortByDeque() {
    if (_unsortedDeq.empty())
        return _unsortedDeq;

    // Create indexed deque from unsorted deque
    std::deque<IndexedInt> indexedDeq;
    for (size_t i = 0; i < _unsortedDeq.size(); ++i) {
        indexedDeq.push_back(IndexedInt(_unsortedDeq[i], i));
    }

    // Sort the indexed deque
    std::deque<IndexedInt> sortedIndexedDeq = _mergeInsertSortDeque(indexedDeq);

    // Extract the sorted values
    std::deque<int> result;
    for (size_t i = 0; i < sortedIndexedDeq.size(); ++i) {
        result.push_back(sortedIndexedDeq[i].first);
    }

    return result;
}

// ----------------------------------------------------------------
// private member functions

std::vector<PmergeMe::IndexedInt> PmergeMe::_mergeInsertSortVector(
    std::vector<IndexedInt> &indexedVec) {
    size_t n = indexedVec.size();
    if (n <= 1)
        return indexedVec;

    // Handle odd element
    bool hasOddElement = false;
    IndexedInt oddElement;
    if (n % 2 == 1) {
        hasOddElement = true;
        oddElement = indexedVec[n - 1];
    }

    // Step 1&2 Combined: Create pairs and recursively sort them
    std::vector<std::pair<IndexedInt, IndexedInt> > pairs;
    for (size_t i = 0; i < n - 1; i += 2) {
#ifdef DEBUG
        std::cout << "Vector Compare: (" << indexedVec[i].first << ", "
                  << indexedVec[i + 1].first << ")\n";
#endif
        if (indexedVec[i].first > indexedVec[i + 1].first) {
            pairs.push_back(std::pair<IndexedInt, IndexedInt>(
                indexedVec[i], indexedVec[i + 1]));
        } else {
            pairs.push_back(std::pair<IndexedInt, IndexedInt>(
                indexedVec[i + 1], indexedVec[i]));
        }
    }

    // Create mapping for recursive sorting of pairs
    std::vector<IndexedInt> pairLarger;
    for (size_t i = 0; i < pairs.size(); ++i) {
        pairLarger.push_back(IndexedInt(pairs[i].first.first, i));
    }

    // Recursively sort pairs if more than one
    std::vector<IndexedInt> sortedPairLarger;
    if (pairLarger.size() > 1) {
        sortedPairLarger = _mergeInsertSortVector(pairLarger);
    } else {
        sortedPairLarger = pairLarger;
    }

    // Step 3: Create result with all larger elements first
    std::vector<IndexedInt> result;
    for (size_t i = 0; i < sortedPairLarger.size(); ++i) {
        size_t pairIdx = sortedPairLarger[i].second;
        result.push_back(pairs[pairIdx].first);
    }

    // Step 4: Insert smaller elements using Ford-Johnson order
    if (!pairs.empty()) {
        // Extract smaller elements for insertion
        std::vector<IndexedInt> smaller;
        for (size_t i = 0; i < sortedPairLarger.size(); ++i) {
            size_t pairIdx = sortedPairLarger[i].second;
            smaller.push_back(pairs[pairIdx].second);
        }

        // smaller[0] is inserted first (unconditionally at the front)
        result.insert(result.begin(), smaller[0]);

        // Insert remaining elements in Ford-Johnson order using binary
        // insertion
        if (smaller.size() > 1) {
            _insertWithJacobsthalOrder(result, smaller);
        }
    }

    // Step 5: Insert odd element if it exists using _binaryInsert
    if (hasOddElement) {
        _binaryInsert(result, oddElement, static_cast<int>(result.size()));
    }

    return result;
}

void PmergeMe::_binaryInsert(
    std::vector<IndexedInt> &vec, const IndexedInt &value, int end) {
    int left = 0;
    int right = end;

    while (left < right) {
        int mid = left + (right - left) / 2;
#ifdef DEBUG
        std::cout << "Vector Compare: (" << vec[mid].first << ", "
                  << value.first << ")\n";
#endif
        if (vec[mid].first < value.first)
            left = mid + 1;
        else
            right = mid;
    }

    vec.insert(vec.begin() + left, value);
}

void PmergeMe::_binaryInsertOptimized(
    std::vector<IndexedInt> &vec, const IndexedInt &value, int maxPos) {
    int left = 0;
    int right = (maxPos < static_cast<int>(vec.size()))
                    ? maxPos
                    : static_cast<int>(vec.size());

    while (left < right) {
        int mid = left + (right - left) / 2;
#ifdef DEBUG
        std::cout << "Vector Compare: (" << vec[mid].first << ", "
                  << value.first << ")\n";
#endif
        if (vec[mid].first < value.first)
            left = mid + 1;
        else
            right = mid;
    }

    vec.insert(vec.begin() + left, value);
}

std::deque<PmergeMe::IndexedInt> PmergeMe::_mergeInsertSortDeque(
    std::deque<IndexedInt> &indexedDeq) {
    size_t n = indexedDeq.size();
    if (n <= 1)
        return indexedDeq;

    // Handle odd element
    bool hasOddElement = false;
    IndexedInt oddElement;
    if (n % 2 == 1) {
        hasOddElement = true;
        oddElement = indexedDeq[n - 1];
    }

    // Step 1&2 Combined: Create pairs and recursively sort them
    std::deque<std::pair<IndexedInt, IndexedInt> > pairs;
    for (size_t i = 0; i < n - 1; i += 2) {
#ifdef DEBUG
        std::cout << "Deque Compare: (" << indexedDeq[i].first << ", "
                  << indexedDeq[i + 1].first << ")\n";
#endif
        if (indexedDeq[i].first > indexedDeq[i + 1].first) {
            pairs.push_back(std::pair<IndexedInt, IndexedInt>(
                indexedDeq[i], indexedDeq[i + 1]));
        } else {
            pairs.push_back(std::pair<IndexedInt, IndexedInt>(
                indexedDeq[i + 1], indexedDeq[i]));
        }
    }

    // Create mapping for recursive sorting of pairs
    std::deque<IndexedInt> pairLarger;
    for (size_t i = 0; i < pairs.size(); ++i) {
        pairLarger.push_back(IndexedInt(pairs[i].first.first, i));
    }

    // Recursively sort pairs if more than one
    std::deque<IndexedInt> sortedPairLarger;
    if (pairLarger.size() > 1) {
        sortedPairLarger = _mergeInsertSortDeque(pairLarger);
    } else {
        sortedPairLarger = pairLarger;
    }

    // Step 3: Create result with all larger elements first
    std::deque<IndexedInt> result;
    for (size_t i = 0; i < sortedPairLarger.size(); ++i) {
        size_t pairIdx = sortedPairLarger[i].second;
        result.push_back(pairs[pairIdx].first);
    }

    // Step 4: Insert smaller elements using Ford-Johnson order
    if (!pairs.empty()) {
        // Extract smaller elements for insertion
        std::deque<IndexedInt> smaller;
        for (size_t i = 0; i < sortedPairLarger.size(); ++i) {
            size_t pairIdx = sortedPairLarger[i].second;
            smaller.push_back(pairs[pairIdx].second);
        }

        // smaller[0] is inserted first (unconditionally at the front)
        result.push_front(smaller[0]);

        // Insert remaining elements in Ford-Johnson order using binary
        // insertion
        if (smaller.size() > 1) {
            _insertWithJacobsthalOrder(result, smaller);
        }
    }

    // Step 5: Insert odd element if it exists
    if (hasOddElement) {
        _binaryInsert(result, oddElement, static_cast<int>(result.size()));
    }

    return result;
}

void PmergeMe::_binaryInsert(
    std::deque<IndexedInt> &deq, const IndexedInt &value, int end) {
    int left = 0;
    int right = end;

    while (left < right) {
        int mid = left + (right - left) / 2;
#ifdef DEBUG
        std::cout << "Deque Compare: (" << deq[mid].first << ", " << value.first
                  << ")\n";
#endif
        if (deq[mid].first < value.first)
            left = mid + 1;
        else
            right = mid;
    }

    deq.insert(deq.begin() + left, value);
}

void PmergeMe::_binaryInsertOptimized(
    std::deque<IndexedInt> &deq, const IndexedInt &value, int maxPos) {
    int left = 0;
    int right = (maxPos < static_cast<int>(deq.size()))
                    ? maxPos
                    : static_cast<int>(deq.size());

    while (left < right) {
        int mid = left + (right - left) / 2;
#ifdef DEBUG
        std::cout << "Deque Compare: (" << deq[mid].first << ", " << value.first
                  << ")\n";
#endif
        if (deq[mid].first < value.first)
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
    std::vector<IndexedInt> &result, std::vector<IndexedInt> const &smaller) {
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
    std::deque<IndexedInt> &result, std::deque<IndexedInt> const &smaller) {
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
