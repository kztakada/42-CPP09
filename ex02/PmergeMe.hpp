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

    std::vector<std::pair<int, size_t> > _mergeInsertSortVector(
        std::vector<std::pair<int, size_t> > &indexedVec);
    void _binaryInsert(std::vector<std::pair<int, size_t> > &vec,
        const std::pair<int, size_t> &value, int end);
    void _binaryInsertOptimized(std::vector<std::pair<int, size_t> > &vec,
        const std::pair<int, size_t> &value, int maxPos);
    void _insertWithJacobsthalOrder(
        std::vector<std::pair<int, size_t> > &result,
        std::vector<std::pair<int, size_t> > const &smaller);
    std::vector<size_t> _generateJacobsthalSequence(size_t n);

    std::deque<std::pair<int, size_t> > _mergeInsertSortDeque(
        std::deque<std::pair<int, size_t> > &indexedDeq);
    void _binaryInsert(std::deque<std::pair<int, size_t> > &deq,
        const std::pair<int, size_t> &value, int end);
    void _binaryInsertOptimized(std::deque<std::pair<int, size_t> > &deq,
        const std::pair<int, size_t> &value, int maxPos);
    void _insertWithJacobsthalOrder(std::deque<std::pair<int, size_t> > &result,
        std::deque<std::pair<int, size_t> > const &smaller);
    std::deque<size_t> _generateJacobsthalSequenceDeque(size_t n);
};

#endif /* PMERGEME_HPP */
