#ifndef PMERGEME_HPP
#define PMERGEME_HPP
#include <deque>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

class PmergeMe {
   public:
    PmergeMe();
    PmergeMe(std::vector<int> const &vec);
    ~PmergeMe();
    PmergeMe(PmergeMe const &other);
    PmergeMe &operator=(PmergeMe const &other);

    std::vector<int> mergeInsertSortByVector();
    std::deque<int> mergeInsertSortByDeque();

   private:
    std::vector<int> _unsortedVec;
    std::deque<int> _unsortedDeq;

    typedef std::pair<int, size_t> IndexedInt;

    // Vector implementations
    std::vector<IndexedInt> _mergeInsertSortVector(
        std::vector<IndexedInt> &indexedVec);
    void _insertWithJacobsthalOrder(std::vector<IndexedInt> &mainChain,
        std::vector<std::pair<IndexedInt, size_t> > const &pending);
    void _binaryInsertOptimized(
        std::vector<IndexedInt> &vec, const IndexedInt &value, int maxPos);
    std::vector<size_t> _generateJacobsthalSequence(size_t n);

    // Deque implementations
    std::deque<IndexedInt> _mergeInsertSortDeque(
        std::deque<IndexedInt> &indexedDeq);
    void _insertWithJacobsthalOrder(std::deque<IndexedInt> &mainChain,
        std::deque<std::pair<IndexedInt, size_t> > const &pending);
    void _binaryInsertOptimized(
        std::deque<IndexedInt> &deq, const IndexedInt &value, int maxPos);
    std::deque<size_t> _generateJacobsthalSequenceDeque(size_t n);

    // For debugging
    int _countVectorCompare();
    int _countDequeCompare();
    void _printCompare(int a, int b, const std::string &type);
    template <typename IndexedIntContainer>
    void _printMainChain(
        const IndexedIntContainer &container, const std::string &type) {
#ifdef DEBUG
        std::cout << type << "MainChain: ";
        for (size_t i = 0; i < container.size(); ++i) {
            std::cout << container[i].first << " ";
        }
        std::cout << std::endl;
#endif
    };

    template <typename PendingContainer>
    void _printPending(
        const PendingContainer &pending, const std::string &type) {
#ifdef DEBUG
        std::cout << type << "Pending: ";
        for (size_t i = 0; i < pending.size(); ++i) {
            std::cout << pending[i].first.first << " ";
        }
        std::cout << std::endl;
#endif
    };
};

#endif /* PMERGEME_HPP */
