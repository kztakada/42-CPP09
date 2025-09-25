#include "PmergeMe.hpp"

#include <iostream>

PmergeMe::PmergeMe() : _unsortedVec(), _unsortedDeq() {}
PmergeMe::PmergeMe(std::vector<int> const &vec)
    : _unsortedVec(vec), _unsortedDeq(vec.begin(), vec.end()) {}

PmergeMe::~PmergeMe() {
#ifdef DEBUG
    std::cout << "Total Vector comparisons: " << _countVectorCompare()
              << std::endl;
    std::cout << "Total Deque comparisons: " << _countDequeCompare()
              << std::endl;
#endif
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

    // Step 1 Create pairs and recursively sort them
    std::vector<std::pair<IndexedInt, IndexedInt> > comparedPairs;
    for (size_t i = 0; i < n - 1; i += 2) {
        IndexedInt larger, smaller;
        _printCompare(indexedVec[i].first, indexedVec[i + 1].first, "Vector ");
        if (indexedVec[i].first > indexedVec[i + 1].first) {
            larger = indexedVec[i];
            smaller = indexedVec[i + 1];
        } else {
            larger = indexedVec[i + 1];
            smaller = indexedVec[i];
        }
        comparedPairs.push_back(
            std::pair<IndexedInt, IndexedInt>(smaller, larger));
    }

    if (comparedPairs.size() == 1 && !hasOddElement) {
        // Only one pair to sort
        std::vector<IndexedInt> mainChain;
        mainChain.push_back(comparedPairs[0].first);
        mainChain.push_back(comparedPairs[0].second);
        return mainChain;
    }

    // Step 2: Create mapping for recursive sorting of larger elements
    std::vector<IndexedInt> newLargers;
    for (size_t i = 0; i < comparedPairs.size(); ++i) {
        newLargers.push_back(IndexedInt(comparedPairs[i].second.first, i));
    }

    std::vector<IndexedInt> sortedLargers;
    if (newLargers.size() <= 1)
        sortedLargers = newLargers;
    else
        sortedLargers = _mergeInsertSortVector(newLargers);

    // Step 3: Create mainChain with all larger elements first and
    // prepare pending by smaller elements for insertion
    std::vector<IndexedInt> mainChain;
    std::vector<std::pair<IndexedInt, size_t> > pending;
    for (size_t i = 0; i < sortedLargers.size(); ++i) {
        size_t pairIdx = sortedLargers[i].second;
        mainChain.push_back(comparedPairs[pairIdx].second);
        // Extract smaller elements for insertion
        pending.push_back(
            std::pair<IndexedInt, size_t>(comparedPairs[pairIdx].first,
                comparedPairs[pairIdx].second.second));
    }
    _printMainChain(mainChain, "Vector ");

    if (hasOddElement) {
        pending.push_back(
            std::pair<IndexedInt, size_t>(oddElement, oddElement.second));
    }
    _printPending(pending, "Vector ");

    // Step 4: Insert smaller elements using Ford-Johnson order
    if (pending.size() > 0) {
        _insertWithJacobsthalOrder(mainChain, pending);
    }

    return mainChain;
}

void PmergeMe::_insertWithJacobsthalOrder(std::vector<IndexedInt> &mainChain,
    std::vector<std::pair<IndexedInt, size_t> > const &pending) {
    // pending[0] is inserted first (unconditionally at the front)
    mainChain.insert(mainChain.begin(), pending[0].first);
    _printMainChain(mainChain, "Vector ");
    if (pending.size() <= 1)
        return;

    // Generate Ford-Johnson group endpoints for insertion order
    std::vector<size_t> endpoints = _generateJacobsthalSequence(pending.size());

    // for insertion tracking
    std::vector<bool> inserted(pending.size(), false);
    inserted[0] = true;  // pending[0] is already inserted

    // Insert elements according to Ford-Johnson group endpoints
    for (size_t i = 3; i < endpoints.size(); ++i) {
        size_t groupStart = endpoints[i] - 1;
        size_t groupEnd = endpoints[i - 1];
        if (groupStart >= pending.size())
            groupStart = pending.size() - 1;
        // Insert elements in reverse order within each group
        for (size_t j = groupStart; j >= groupEnd; --j) {
            if (j < pending.size() && !inserted[j]) {
                // Calculate maximum search position for this element
                int maxPos = static_cast<int>(mainChain.size());
                for (size_t k = 0; k < mainChain.size(); ++k) {
                    if (mainChain[k].second == pending[j].second) {
                        maxPos = k;
                        break;
                    }
                }
                _binaryInsertOptimized(mainChain, pending[j].first, maxPos);
                _printMainChain(mainChain, "Vector ");
                inserted[j] = true;
            }
        }
    }

    // Insert any remaining elements
    for (size_t i = 1; i < pending.size();
        ++i) {  // Start from 1, skip pending[0]
        if (!inserted[i]) {
            _binaryInsertOptimized(mainChain, pending[i].first,
                static_cast<int>(mainChain.size()) - 1);
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

// Generate Ford-Johnson group endpoints for insertion order
// Returns 1, 3, 5, 11, 21, ... (Jacobsthal numbers)
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

std::deque<PmergeMe::IndexedInt> PmergeMe::_mergeInsertSortDeque(
    std::deque<IndexedInt> &indexedVec) {
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

    // Step 1 Create pairs and recursively sort them
    std::deque<std::pair<IndexedInt, IndexedInt> > comparedPairs;
    for (size_t i = 0; i < n - 1; i += 2) {
        IndexedInt larger, smaller;
        _printCompare(indexedVec[i].first, indexedVec[i + 1].first, "Deque ");
        if (indexedVec[i].first > indexedVec[i + 1].first) {
            larger = indexedVec[i];
            smaller = indexedVec[i + 1];
        } else {
            larger = indexedVec[i + 1];
            smaller = indexedVec[i];
        }
        comparedPairs.push_back(
            std::pair<IndexedInt, IndexedInt>(smaller, larger));
    }

    if (comparedPairs.size() == 1 && !hasOddElement) {
        // Only one pair to sort
        std::deque<IndexedInt> mainChain;
        mainChain.push_back(comparedPairs[0].first);
        mainChain.push_back(comparedPairs[0].second);
        return mainChain;
    }

    // Step 2: Create mapping for recursive sorting of larger elements
    std::deque<IndexedInt> newLargers;
    for (size_t i = 0; i < comparedPairs.size(); ++i) {
        newLargers.push_back(IndexedInt(comparedPairs[i].second.first, i));
    }

    std::deque<IndexedInt> sortedLargers;
    if (newLargers.size() <= 1)
        sortedLargers = newLargers;
    else
        sortedLargers = _mergeInsertSortDeque(newLargers);

    // Step 3: Create mainChain with all larger elements first and
    // prepare pending by smaller elements for insertion
    std::deque<IndexedInt> mainChain;
    std::deque<std::pair<IndexedInt, size_t> > pending;
    for (size_t i = 0; i < sortedLargers.size(); ++i) {
        size_t pairIdx = sortedLargers[i].second;
        mainChain.push_back(comparedPairs[pairIdx].second);
        // Extract smaller elements for insertion
        pending.push_back(
            std::pair<IndexedInt, size_t>(comparedPairs[pairIdx].first,
                comparedPairs[pairIdx].second.second));
    }
    _printMainChain(mainChain, "Vector ");

    if (hasOddElement) {
        pending.push_back(
            std::pair<IndexedInt, size_t>(oddElement, oddElement.second));
    }
    _printPending(pending, "Vector ");

    // Step 4: Insert smaller elements using Ford-Johnson order
    if (pending.size() > 0) {
        _insertWithJacobsthalOrder(mainChain, pending);
    }

    return mainChain;
}

void PmergeMe::_insertWithJacobsthalOrder(std::deque<IndexedInt> &mainChain,
    std::deque<std::pair<IndexedInt, size_t> > const &pending) {
    // pending[0] is inserted first (unconditionally at the front)
    mainChain.insert(mainChain.begin(), pending[0].first);
    _printMainChain(mainChain, "Deque ");
    if (pending.size() <= 1)
        return;

    // Generate Ford-Johnson group endpoints for insertion order
    std::deque<size_t> endpoints =
        _generateJacobsthalSequenceDeque(pending.size());

    // for insertion tracking
    std::deque<bool> inserted(pending.size(), false);
    inserted[0] = true;  // pending[0] is already inserted

    // Insert elements according to Ford-Johnson group endpoints
    for (size_t i = 3; i < endpoints.size(); ++i) {
        size_t groupStart = endpoints[i] - 1;
        size_t groupEnd = endpoints[i - 1];
        if (groupStart >= pending.size())
            groupStart = pending.size() - 1;
        // Insert elements in reverse order within each group
        for (size_t j = groupStart; j >= groupEnd; --j) {
            if (j < pending.size() && !inserted[j]) {
                // Calculate maximum search position for this element
                int maxPos = static_cast<int>(mainChain.size());
                for (size_t k = 0; k < mainChain.size(); ++k) {
                    if (mainChain[k].second == pending[j].second) {
                        maxPos = k;
                        break;
                    }
                }
                _binaryInsertOptimized(mainChain, pending[j].first, maxPos);
                _printMainChain(mainChain, "Deque ");
                inserted[j] = true;
            }
        }
    }

    // Insert any remaining elements
    for (size_t i = 1; i < pending.size();
        ++i) {  // Start from 1, skip pending[0]
        if (!inserted[i]) {
            _binaryInsertOptimized(mainChain, pending[i].first,
                static_cast<int>(mainChain.size()) - 1);
        }
    }
}

void PmergeMe::_binaryInsertOptimized(
    std::deque<IndexedInt> &vec, const IndexedInt &value, int maxPos) {
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
        _printCompare(vec[mid].first, value.first, "Deque ");
    }
    vec.insert(vec.begin() + left, value);
}

// Generate Ford-Johnson group endpoints for insertion order
// Returns 1, 3, 5, 11, 21, ... (Jacobsthal numbers)
std::deque<size_t> PmergeMe::_generateJacobsthalSequenceDeque(size_t n) {
    std::deque<size_t> jacobsthal_sequence;
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

// ----------------------------------------------------------------
// helper functions

int PmergeMe::_countVectorCompare() {
    static int v_count = 0;
    return v_count++;
}

int PmergeMe::_countDequeCompare() {
    static int d_count = 0;
    return d_count++;
}

void PmergeMe::_printCompare(int a, int b, const std::string &type) {
#ifdef DEBUG
    std::cout << type << "Compare: (" << a << ", " << b << ")\n";
    if (type == "Vector ")
        _countVectorCompare();
    else if (type == "Deque ")
        _countDequeCompare();
        #else
        (void)a;
        (void)b;
        (void)type;
#endif
}
