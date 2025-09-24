#include "PmergeMe.hpp"

#include <iostream>

PmergeMe::PmergeMe() : _unsortedVec(), _unsortedDeq() {}
PmergeMe::PmergeMe(std::vector<int> const &vec)
    : _unsortedVec(vec), _unsortedDeq(vec.begin(), vec.end()) {}

PmergeMe::~PmergeMe() {
    std::cout << "Total Vector comparisons: " << _countVectorCompare()
              << std::endl;
}

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
    IndexedInt oddElement;  // uninitialized until n is odd
    if (n % 2 == 1) {
        hasOddElement = true;
        oddElement = indexedVec[n - 1];
    }

    // Step 1&2 Combined: Create pairs and recursively sort them
    std::vector<std::pair<IndexedInt, IndexedInt> > pairs;
    for (size_t i = 0; i < n - 1; i += 2) {
        if (indexedVec[i].first > indexedVec[i + 1].first) {
            pairs.push_back(std::pair<IndexedInt, IndexedInt>(
                indexedVec[i], indexedVec[i + 1]));
        } else {
            pairs.push_back(std::pair<IndexedInt, IndexedInt>(
                indexedVec[i + 1], indexedVec[i]));
        }
        _printCompare(indexedVec[i].first, indexedVec[i + 1].first, "Vector ");
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

    std::vector<IndexedInt> result;
    if (!pairs.empty()) {
        // Step 3: Create result with all larger elements first
        std::vector<std::pair<IndexedInt, size_t> > smaller;
        for (size_t i = 0; i < sortedPairLarger.size(); ++i) {
            size_t pairIdx = sortedPairLarger[i].second;
            result.push_back(pairs[pairIdx].first);
            // Extract smaller elements for insertion
            smaller.push_back(std::pair<IndexedInt, size_t>(
                pairs[pairIdx].second, pairs[pairIdx].first.second));
        }
        _printMainChain(result, "Vector ");

        // Step 4: Insert smaller elements using Ford-Johnson order

        if (hasOddElement) {
            smaller.push_back(
                std::pair<IndexedInt, size_t>(oddElement, oddElement.second));
        }
        _printPending(smaller, "Vector ");
        // smaller[0] is inserted first (unconditionally at the front)
        result.insert(result.begin(), smaller[0].first);

        _printMainChain(result, "Vector ");
        // _printPending(smaller, "Vector ");

        // Insert remaining elements in Ford-Johnson order using binary
        // insertion
        if (smaller.size() > 1) {
            _insertWithJacobsthalOrder(result, smaller);
        }
    }

    return result;
}

void PmergeMe::_insertWithJacobsthalOrder(std::vector<IndexedInt> &result,
    std::vector<std::pair<IndexedInt, size_t> > const &smaller) {
    std::vector<size_t> endpoints = _generateJacobsthalSequence(smaller.size());
    std::vector<bool> inserted(smaller.size(), false);
    inserted[0] = true;  // smaller[0] is already inserted

    // Insert elements according to Ford-Johnson group endpoints
    for (size_t i = 3; i < endpoints.size(); ++i) {
        size_t groupStart = endpoints[i] - 1;
        size_t groupEnd = endpoints[i - 1];
        if (groupStart >= smaller.size())
            groupStart = smaller.size() - 1;
        // Insert elements in reverse order within each group
        for (size_t j = groupStart; j >= groupEnd; --j) {
            if (j < smaller.size() && !inserted[j]) {
                // Calculate maximum search position for this element
                int maxPos = static_cast<int>(result.size());
                for (size_t k = 0; k < result.size(); ++k) {
                    if (result[k].second == smaller[j].second &&
                        result[k].first >= smaller[j].first.first) {
                        maxPos = k;
                        break;
                    }
                }
                _binaryInsertOptimized(result, smaller[j].first, maxPos);
                _printMainChain(result, "Vector ");
                inserted[j] = true;
            }
        }
    }

    // Insert any remaining elements
    for (size_t i = 1; i < smaller.size();
        ++i) {  // Start from 1, skip smaller[0]
        if (!inserted[i]) {
            _binaryInsertOptimized(
                result, smaller[i].first, static_cast<int>(result.size()) - 1);
        }
    }
}

void PmergeMe::_binaryInsertOptimized(
    std::vector<IndexedInt> &vec, const IndexedInt &value, int maxPos) {
    if (vec.empty()) {
        vec.insert(vec.begin(), value);
        return;
    }

    int left = 0;
    int right = (maxPos < static_cast<int>(vec.size()))
                    ? maxPos
                    : static_cast<int>(vec.size()) - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (vec[mid].first < value.first) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
        _printCompare(vec[mid].first, value.first, "Vector ");
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
    IndexedInt oddElement;  // uninitialized until n is odd
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
        // #ifdef DEBUG
        //         std::cout << "Insert ODD element (deque): " <<
        //         oddElement.first
        //                   << std::endl;
        // #endif
        _binaryInsert(result, oddElement, static_cast<int>(result.size()));
    }

    return result;
}

void PmergeMe::_binaryInsert(
    std::deque<IndexedInt> &deq, const IndexedInt &value, int end) {
    // #ifdef DEBUG
    //     std::cout << "Binary insert (deque): " << value.first << std::endl;
    // #endif
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
    if (deq.empty()) {
        deq.insert(deq.begin(), value);
        return;
    }

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
// Returns 2, 2, 6, 10, 22, 42, ... (2 × Jacobsthal numbers)
std::vector<size_t> PmergeMe::_generateJacobsthalSequence(size_t n) {
    std::vector<size_t> jacobsthal_sequence;
    if (n == 0)
        return jacobsthal_sequence;

    jacobsthal_sequence.push_back(0);
    jacobsthal_sequence.push_back(1);

    int i = 2;
    while (true) {
        size_t next_val =
            jacobsthal_sequence[i - 1] + 2 * jacobsthal_sequence[i - 2];
        if (next_val > n) {
            break;
        }
        jacobsthal_sequence.push_back(next_val);
        i++;
    }
    return jacobsthal_sequence;
}

std::deque<size_t> PmergeMe::_generateJacobsthalSequenceDeque(size_t n) {
    std::deque<size_t> endpoints;
    if (n == 0)
        return endpoints;

    size_t jPrev2 = 0;  // J0
    size_t jPrev1 = 1;  // J1

    for (size_t k = 1; /* break internally */; ++k) {
        size_t jk;
        if (k == 1) {
            jk = 1;  // J1
        } else if (k == 2) {
            jk = 1;  // J2
        } else {
            size_t jCurr = jPrev1 + 2 * jPrev2;
            jk = jCurr;
            jPrev2 = jPrev1;
            jPrev1 = jCurr;
        }

        size_t endpoint = 2 * jk;
        if (endpoint > n)
            break;
        endpoints.push_back(endpoint);

        if (k == 1) {
            jPrev2 = 0;  // J0
            jPrev1 = 1;  // J1
        } else if (k == 2) {
            jPrev2 = 1;  // J1
            jPrev1 = 1;  // J2
        }
    }

    return endpoints;
}

void PmergeMe::_insertWithJacobsthalOrder(
    std::deque<IndexedInt> &result, std::deque<IndexedInt> const &smaller) {
    // if (smaller.size() <= 1)
    //     return;

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
            // Remaining smaller elements (not covered by Jacobsthal groups)
            // #ifdef DEBUG
            //             std::cout << "Insert REMAINING element (deque): "
            //                       << smaller[i].first << std::endl;
            // #endif
            _binaryInsertOptimized(
                result, smaller[i], static_cast<int>(result.size()));
        }
    }
}

// ----------------------------------------------------------------
// helper functions

int PmergeMe::_countVectorCompare() {
    static int v_count = 0;
    return v_count++;
}

void PmergeMe::_printCompare(int a, int b, const std::string &type) {
#ifdef DEBUG
    std::cout << type << "Compare: (" << a << ", " << b << ")\n";
    if (type == "Vector ")
        _countVectorCompare();
#endif
}

void PmergeMe::_printMainChain(
    const std::vector<IndexedInt> &vec, const std::string &type) {
#ifdef DEBUG
    std::cout << type << "MainChain: ";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i].first << " ";
    }
    std::cout << std::endl;
#endif
}

void PmergeMe::_printPending(
    const std::vector<std::pair<IndexedInt, size_t> > &pending,
    const std::string &type) {
#ifdef DEBUG
    std::cout << type << "Pending: ";
    for (size_t i = 0; i < pending.size(); ++i) {
        std::cout << pending[i].first.first << " ";
    }
    std::cout << std::endl;
#endif
}

// static void printResult(const std::deque<int> &deq, const std::string &type)
// { #ifdef DEBUG
//     std::cout << type << "Result: ";
//     for (size_t i = 0; i < deq.size(); ++i) {
//         std::cout << deq[i] << " ";
//     }
//     std::cout << std::endl;
// #endif
// }
