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

    typedef std::pair<int, size_t> IndexedInt;

    std::vector<std::pair<int, size_t> > _mergeInsertSortVector(
        std::vector<IndexedInt> &indexedVec);
    void _binaryInsert(
        std::vector<IndexedInt> &vec, const IndexedInt &value, int end);
    void _binaryInsertOptimized(
        std::vector<IndexedInt> &vec, const IndexedInt &value, int maxPos);
    void _insertWithJacobsthalOrder(std::vector<IndexedInt> &result,
        std::vector<IndexedInt> const &smaller);
    std::vector<size_t> _generateJacobsthalSequence(size_t n);

    std::deque<std::pair<int, size_t> > _mergeInsertSortDeque(
        std::deque<IndexedInt> &indexedDeq);
    void _binaryInsert(
        std::deque<IndexedInt> &deq, const IndexedInt &value, int end);
    void _binaryInsertOptimized(
        std::deque<IndexedInt> &deq, const IndexedInt &value, int maxPos);
    void _insertWithJacobsthalOrder(
        std::deque<IndexedInt> &result, std::deque<IndexedInt> const &smaller);
    std::deque<size_t> _generateJacobsthalSequenceDeque(size_t n);
};

#endif /* PMERGEME_HPP */
