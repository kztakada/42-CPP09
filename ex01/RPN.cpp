#include "RPN.hpp"

#include <climits>

// ----------------------------------------------------------------
// Public static members

int RPN::evaluate(const std::string &expression) {
    while (!_stack.empty())
        _stack.pop();

    for (size_t i = 0; i < expression.length(); ++i) {
        char c = expression[i];

        if (isspace(c))
            continue;

        if (isdigit(c)) {
            _stack.push(c - '0');
        } else if (_isOperator(c)) {
            if (_stack.size() < 2)
                throw std::invalid_argument("invalid expression");

            int b = _stack.top();
            _stack.pop();
            int a = _stack.top();
            _stack.pop();

            int result = _applyOperator(c, a, b);
            _stack.push(result);
        } else {
            throw std::invalid_argument("");
        }
    }

    if (_stack.size() != 1)
        throw std::invalid_argument("invalid expression");

    return _stack.top();
}

// ----------------------------------------------------------------
// Static private members

std::stack<int> RPN::_stack = std::stack<int>();

bool RPN::_isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int RPN::_applyOperator(char op, int a, int b) {
    switch (op) {
        case '+':
            if (_willOverflow(a, b, op))
                throw std::overflow_error("overflow");
            return a + b;
        case '-':
            if (_willOverflow(a, b, op))
                throw std::overflow_error("overflow");
            return a - b;
        case '*':
            if (_willOverflow(a, b, op))
                throw std::overflow_error("overflow");
            return a * b;
        case '/':
            if (b == 0)
                throw std::invalid_argument("division by zero");
            if (_willOverflow(a, b, op))
                throw std::overflow_error("overflow");
            return a / b;
        default:
            throw std::invalid_argument("invalid operator");
    }
}

bool RPN::_willOverflow(int a, int b, char op) {
    switch (op) {
        case '+':
            return (b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b);
        case '-':
            return (b < 0 && a > INT_MAX + b) || (b > 0 && a < INT_MIN + b);
        case '*':
            if (a == 0 || b == 0)
                return false;
            if (a == -1 && b == INT_MIN)
                return true;
            if (b == -1 && a == INT_MIN)
                return true;
            return a > INT_MAX / b || a < INT_MIN / b;
        case '/':  // Division by zero is handled separately
            return (a == INT_MIN && b == -1);
        default:
            return false;
    }
}
