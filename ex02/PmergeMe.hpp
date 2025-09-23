#ifndef PMERGEME_HPP
#define PMERGEME_HPP
#include <deque>
#include <iterator>
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
    void _binaryInsert(std::vector<int> &vec, int value, int end);

    std::deque<int> _mergeInsertSortDeque(std::deque<int> &deq);
    void _binaryInsert(std::deque<int> &deq, int value, int end);
};

#endif /* PMERGEME_HPP */
