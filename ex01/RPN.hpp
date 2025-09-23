#ifndef RPN_HPP
#define RPN_HPP

#include <exception>
#include <stack>
#include <stdexcept>
#include <string>

class RPN {
   public:
    static int evaluate(const std::string &expression);

   private:
    static std::stack<int> _stack;

    static bool _isOperator(char c);
    static int _applyOperator(char op, int a, int b);
    static bool _willOverflow(int a, int b, char op);

    RPN();                             // = delete;
    ~RPN();                            // = delete;
    RPN(RPN const &other);             // = delete;
    RPN &operator=(RPN const &other);  // = delete;
};

#endif /* RPN_HPP */
