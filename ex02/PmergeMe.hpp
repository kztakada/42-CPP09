#ifndef PMERGEME_HPP
#define PMERGEME_HPP
#include <deque>
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

    std::vector<int> _mergeInsertSortVector(std::vector<int> &vec);
    void _mergeInsertSortPairs(std::vector<std::pair<int, int> > &pairs);
    void _mergeInsertSortIndexedVector(
        std::vector<std::pair<int, size_t> > &indexedVec);
    void _binaryInsert(std::vector<int> &vec, int value, int end);
    void _binaryInsertOptimized(std::vector<int> &vec, int value, int maxPos);
    void _insertWithJacobsthalOrder(
        std::vector<int> &result, std::vector<int> const &smaller);
    std::vector<size_t> _generateJacobsthalSequence(size_t n);

    std::deque<int> _mergeInsertSortDeque(std::deque<int> &deq);
    void _mergeInsertSortPairsDeque(std::deque<std::pair<int, int> > &pairs);
    void _mergeInsertSortIndexedDeque(
        std::deque<std::pair<int, size_t> > &indexedDeq);
    void _binaryInsert(std::deque<int> &deq, int value, int end);
    void _binaryInsertOptimized(std::deque<int> &deq, int value, int maxPos);
    void _insertWithJacobsthalOrder(
        std::deque<int> &result, std::deque<int> const &smaller);
    std::deque<size_t> _generateJacobsthalSequenceDeque(size_t n);
};

#endif /* PMERGEME_HPP */
