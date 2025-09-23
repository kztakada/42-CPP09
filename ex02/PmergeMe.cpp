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

    // Create indexed vector from unsorted vector
    std::vector<std::pair<int, size_t> > indexedVec;
    for (size_t i = 0; i < _unsortedVec.size(); ++i) {
        indexedVec.push_back(std::pair<int, size_t>(_unsortedVec[i], i));
    }

    // Sort the indexed vector
    std::vector<std::pair<int, size_t> > sortedIndexedVec =
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
    std::deque<std::pair<int, size_t> > indexedDeq;
    for (size_t i = 0; i < _unsortedDeq.size(); ++i) {
        indexedDeq.push_back(std::pair<int, size_t>(_unsortedDeq[i], i));
    }

    // Sort the indexed deque
    std::deque<std::pair<int, size_t> > sortedIndexedDeq =
        _mergeInsertSortDeque(indexedDeq);

    // Extract the sorted values
    std::deque<int> result;
    for (size_t i = 0; i < sortedIndexedDeq.size(); ++i) {
        result.push_back(sortedIndexedDeq[i].first);
    }

    return result;
}

// ----------------------------------------------------------------
// private member functions

std::vector<std::pair<int, size_t> > PmergeMe::_mergeInsertSortVector(
    std::vector<std::pair<int, size_t> > &indexedVec) {
    size_t n = indexedVec.size();
    if (n <= 1)
        return indexedVec;

    // Handle odd element
    bool hasOddElement = false;
    std::pair<int, size_t> oddElement;
    if (n % 2 == 1) {
        hasOddElement = true;
        oddElement = indexedVec[n - 1];
    }

    // Step 1&2 Combined: Create pairs and recursively sort them
    std::vector<std::pair<std::pair<int, size_t>, std::pair<int, size_t> > >
        pairs;
    for (size_t i = 0; i < n - 1; i += 2) {
        if (indexedVec[i].first > indexedVec[i + 1].first) {
            pairs.push_back(
                std::pair<std::pair<int, size_t>, std::pair<int, size_t> >(
                    indexedVec[i], indexedVec[i + 1]));
        } else {
            pairs.push_back(
                std::pair<std::pair<int, size_t>, std::pair<int, size_t> >(
                    indexedVec[i + 1], indexedVec[i]));
        }
    }

    // Create mapping for recursive sorting of pairs
    std::vector<std::pair<int, size_t> > pairLarger;
    for (size_t i = 0; i < pairs.size(); ++i) {
        pairLarger.push_back(std::pair<int, size_t>(pairs[i].first.first, i));
    }

    // Recursively sort pairs if more than one
    std::vector<std::pair<int, size_t> > sortedPairLarger;
    if (pairLarger.size() > 1) {
        sortedPairLarger = _mergeInsertSortVector(pairLarger);
    } else {
        sortedPairLarger = pairLarger;
    }

    // Step 3: Create result with all larger elements first
    std::vector<std::pair<int, size_t> > result;
    for (size_t i = 0; i < sortedPairLarger.size(); ++i) {
        size_t pairIdx = sortedPairLarger[i].second;
        result.push_back(pairs[pairIdx].first);
    }

    // Step 4: Insert smaller elements using Ford-Johnson order
    if (!pairs.empty()) {
        // Extract smaller elements for insertion
        std::vector<std::pair<int, size_t> > smaller;
        for (size_t i = 0; i < sortedPairLarger.size(); ++i) {
            size_t pairIdx = sortedPairLarger[i].second;
            smaller.push_back(pairs[pairIdx].second);
        }

        // smaller[0] is inserted first (unconditionally at the front)
        result.insert(result.begin(), smaller[0]);

        // Insert remaining elements in Ford-Johnson order using binary
        // insertion
        if (smaller.size() > 1) {
            std::vector<size_t> endpoints = _generateJacobsthalSequence(
                smaller.size() -
                1);  // -1 because smaller[0] is already inserted
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
                        // Binary search to find insertion position
                        int left = 0;
                        int right = static_cast<int>(result.size());
                        int value = smaller[j].first;

                        while (left < right) {
                            int mid = left + (right - left) / 2;
                            if (result[mid].first < value)
                                left = mid + 1;
                            else
                                right = mid;
                        }

                        result.insert(result.begin() + left, smaller[j]);
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
                    // Binary search to find insertion position
                    int left = 0;
                    int right = static_cast<int>(result.size());
                    int value = smaller[i].first;

                    while (left < right) {
                        int mid = left + (right - left) / 2;
                        if (result[mid].first < value)
                            left = mid + 1;
                        else
                            right = mid;
                    }

                    result.insert(result.begin() + left, smaller[i]);
                }
            }
        }
    }

    // Step 5: Insert odd element if it exists
    if (hasOddElement) {
        // Binary search to find insertion position
        int left = 0;
        int right = static_cast<int>(result.size());
        int value = oddElement.first;

        while (left < right) {
            int mid = left + (right - left) / 2;
            if (result[mid].first < value)
                left = mid + 1;
            else
                right = mid;
        }

        result.insert(result.begin() + left, oddElement);
    }

    return result;
}

// void PmergeMe::_mergeInsertSortPairs(std::vector<std::pair<int, int> >
// &pairs) {
//     if (pairs.size() <= 1)
//         return;

//     // Create pairs with original indices to track them through sorting
//     std::vector<std::pair<std::pair<int, int>, size_t> > indexedPairs;
//     for (size_t i = 0; i < pairs.size(); ++i) {
//         indexedPairs.push_back(
//             std::pair<std::pair<int, int>, size_t>(pairs[i], i));
//     }

//     // Extract larger elements with their original indices
//     std::vector<std::pair<int, size_t> > largerWithIndex;
//     for (size_t i = 0; i < indexedPairs.size(); ++i) {
//         largerWithIndex.push_back(std::pair<int, size_t>(
//             indexedPairs[i].first.first, indexedPairs[i].second));
//     }

//     if (largerWithIndex.size() > 1) {
//         // Sort indexed larger elements by their values
//         _mergeInsertSortIndexedVector(largerWithIndex);

//         // Reorder pairs based on sorted indices
//         std::vector<std::pair<int, int> > sortedPairs;
//         for (size_t i = 0; i < largerWithIndex.size(); ++i) {
//             size_t originalIndex = largerWithIndex[i].second;
//             sortedPairs.push_back(pairs[originalIndex]);
//         }
//         pairs = sortedPairs;
//     }
// }

// void PmergeMe::_mergeInsertSortIndexedVector(
//     std::vector<std::pair<int, size_t> > &indexedVec) {
//     if (indexedVec.size() <= 1)
//         return;

//     // Use the new _mergeInsertSortVector function directly with the indexed
//     // vector
//     std::vector<std::pair<int, size_t> > sortedIndexedVec =
//         _mergeInsertSortVector(indexedVec);

//     // Update the original vector with the sorted result
//     indexedVec = sortedIndexedVec;
// }

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

std::deque<std::pair<int, size_t> > PmergeMe::_mergeInsertSortDeque(
    std::deque<std::pair<int, size_t> > &indexedDeq) {
    size_t n = indexedDeq.size();
    if (n <= 1)
        return indexedDeq;

    // Handle odd element
    bool hasOddElement = false;
    std::pair<int, size_t> oddElement;
    if (n % 2 == 1) {
        hasOddElement = true;
        oddElement = indexedDeq[n - 1];
    }

    // Step 1&2 Combined: Create pairs and recursively sort them
    std::deque<std::pair<std::pair<int, size_t>, std::pair<int, size_t> > >
        pairs;
    for (size_t i = 0; i < n - 1; i += 2) {
        if (indexedDeq[i].first > indexedDeq[i + 1].first) {
            pairs.push_back(
                std::pair<std::pair<int, size_t>, std::pair<int, size_t> >(
                    indexedDeq[i], indexedDeq[i + 1]));
        } else {
            pairs.push_back(
                std::pair<std::pair<int, size_t>, std::pair<int, size_t> >(
                    indexedDeq[i + 1], indexedDeq[i]));
        }
    }

    // Create mapping for recursive sorting of pairs
    std::deque<std::pair<int, size_t> > pairLarger;
    for (size_t i = 0; i < pairs.size(); ++i) {
        pairLarger.push_back(std::pair<int, size_t>(pairs[i].first.first, i));
    }

    // Recursively sort pairs if more than one
    std::deque<std::pair<int, size_t> > sortedPairLarger;
    if (pairLarger.size() > 1) {
        sortedPairLarger = _mergeInsertSortDeque(pairLarger);
    } else {
        sortedPairLarger = pairLarger;
    }

    // Step 3: Create result with all larger elements first
    std::deque<std::pair<int, size_t> > result;
    for (size_t i = 0; i < sortedPairLarger.size(); ++i) {
        size_t pairIdx = sortedPairLarger[i].second;
        result.push_back(pairs[pairIdx].first);
    }

    // Step 4: Insert smaller elements using Ford-Johnson order
    if (!pairs.empty()) {
        // Extract smaller elements for insertion
        std::deque<std::pair<int, size_t> > smaller;
        for (size_t i = 0; i < sortedPairLarger.size(); ++i) {
            size_t pairIdx = sortedPairLarger[i].second;
            smaller.push_back(pairs[pairIdx].second);
        }

        // smaller[0] is inserted first (unconditionally at the front)
        result.push_front(smaller[0]);

        // Insert remaining elements in Ford-Johnson order using binary
        // insertion
        if (smaller.size() > 1) {
            std::deque<size_t> endpoints = _generateJacobsthalSequenceDeque(
                smaller.size() -
                1);  // -1 because smaller[0] is already inserted
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
                        // Binary search to find insertion position
                        int left = 0;
                        int right = static_cast<int>(result.size());
                        int value = smaller[j].first;

                        while (left < right) {
                            int mid = left + (right - left) / 2;
                            if (result[mid].first < value)
                                left = mid + 1;
                            else
                                right = mid;
                        }

                        result.insert(result.begin() + left, smaller[j]);
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
                    // Binary search to find insertion position
                    int left = 0;
                    int right = static_cast<int>(result.size());
                    int value = smaller[i].first;

                    while (left < right) {
                        int mid = left + (right - left) / 2;
                        if (result[mid].first < value)
                            left = mid + 1;
                        else
                            right = mid;
                    }

                    result.insert(result.begin() + left, smaller[i]);
                }
            }
        }
    }

    // Step 5: Insert odd element if it exists
    if (hasOddElement) {
        // Binary search to find insertion position
        int left = 0;
        int right = static_cast<int>(result.size());
        int value = oddElement.first;

        while (left < right) {
            int mid = left + (right - left) / 2;
            if (result[mid].first < value)
                left = mid + 1;
            else
                right = mid;
        }

        result.insert(result.begin() + left, oddElement);
    }

    return result;
}

// void PmergeMe::_mergeInsertSortPairsDeque(
//     std::deque<std::pair<int, int> > &pairs) {
//     if (pairs.size() <= 1)
//         return;

//     // Create pairs with original indices to track them through sorting
//     std::deque<std::pair<std::pair<int, int>, size_t> > indexedPairs;
//     for (size_t i = 0; i < pairs.size(); ++i) {
//         indexedPairs.push_back(
//             std::pair<std::pair<int, int>, size_t>(pairs[i], i));
//     }

//     // Extract larger elements with their original indices
//     std::deque<std::pair<int, size_t> > largerWithIndex;
//     for (size_t i = 0; i < indexedPairs.size(); ++i) {
//         largerWithIndex.push_back(std::pair<int, size_t>(
//             indexedPairs[i].first.first, indexedPairs[i].second));
//     }

//     if (largerWithIndex.size() > 1) {
//         // Sort indexed larger elements by their values
//         _mergeInsertSortIndexedDeque(largerWithIndex);

//         // Reorder pairs based on sorted indices
//         std::deque<std::pair<int, int> > sortedPairs;
//         for (size_t i = 0; i < largerWithIndex.size(); ++i) {
//             size_t originalIndex = largerWithIndex[i].second;
//             sortedPairs.push_back(pairs[originalIndex]);
//         }
//         pairs = sortedPairs;
//     }
// }

// void PmergeMe::_mergeInsertSortIndexedDeque(
//     std::deque<std::pair<int, size_t> > &indexedDeq) {
//     if (indexedDeq.size() <= 1)
//         return;

//     // Use the new _mergeInsertSortDeque function directly with the indexed
//     // deque
//     std::deque<std::pair<int, size_t> > sortedIndexedDeq =
//         _mergeInsertSortDeque(indexedDeq);

//     // Update the original deque with the sorted result
//     indexedDeq = sortedIndexedDeq;
// }

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
