
#include "Arithmetic.hpp"

#include <iostream>
#include <stack>
//#include <stdexcept>

namespace arithmetic {

int opValue(const std::string& op)
{
    if (op == "^")
        return arithmetic::Exponentiation;
    else if (op == "*")
        return arithmetic::Multiplication;
    else if (op == "/")
        return arithmetic::Division;
    else if (op == "+")
        return arithmetic::Addition;
    else if (op == "-")
        return arithmetic::Subtraction;
    else {
        return arithmetic::Unknown;
    }
}

bool isOperator(const std::string& op)
{
    return op == "+" || op == "*" || op == "-" || op == "/" || op == "^";
}

int precedence(const std::string& op)
{
    if (op == "^")
        return 3;
    else if (op == "*")
        return 2;
    else if (op == "/")
        return 2;
    else if (op == "+")
        return 1;
    else if (op == "-")
        return 1;
    else {
        std::cout << "ERROR: Unknown precedence " << op << "(f : precedence(string))" << std::endl;
        return 1000;
    }
}

int precedence(const unsigned char op)
{
    switch(op) {
    case arithmetic::Exponentiation:
      return 3;
    case arithmetic::Multiplication:
      return 2;
    case arithmetic::Division:
      return 2;
    case arithmetic::Addition:
      return 1;
    case arithmetic::Subtraction:
      return 1;
    default:
      std::cout << "ERROR: Unknown operator " << (int)op << std::endl;
      return 1000;
    }
}

/**
 * Very basic Shunting Yard algorithm implementation.
 * http://en.wikipedia.org/wiki/Shunting-yard_algorithm
 *
 * Only works for very basic expressions right now,
 * but enough to test it with the program
 * TODO: parenthesis, exponentiation
 */
std::vector<std::string> shuntingYard(const std::vector<std::string>& expression)
{
    std::vector<std::string> postfix_exp;
    std::stack<std::string> operators_stack;

    /****
     * Very basic Shunting Yard algorithm.
     *  Did NOT incorporate parentheses yet
     *  or exponentiation
     ****/

    postfix_exp.push_back(expression[0]);
    operators_stack.push(expression[1]);
    postfix_exp.push_back(expression[2]);

    for (std::size_t i = 3; i < expression.size(); i++)
    {
        if (isOperator(expression[i]))
        {
            if (precedence(expression[i]) <= precedence(operators_stack.top()))
            {
                std::string op = operators_stack.top();
                operators_stack.pop();
                postfix_exp.push_back(op);
                operators_stack.push(expression[i]);
            }
            else
            {
                operators_stack.push(expression[i]);
            }
        }
        else
        {
            postfix_exp.push_back(expression[i]);
        }
    }

    while(!operators_stack.empty())
    {
        postfix_exp.push_back(operators_stack.top());
        operators_stack.pop();
    }
    return postfix_exp;

    /// /// /// /// /// ///
    /*
    std::stack<T> numbers; // change to T
    std::stack<unsigned char> operators;
    std::cout << "Stacks made." << std::endl;

    //Read first [number, operator, number]
    numbers.push(equation[0]);
    operators.push(equation[1]);
    numbers.push(equation[2]);

    // Starting with an operator every iteration
    for (std::size_t i = 3; i < equation.size(); i+=2)
    {
        int current_op = equation[i];
        if (precedence(current_op) <= precedence(operators.top()))
        {
            std::cout << "precedence equal or lower." << std::endl;
            while (numbers.size() >= 2 && precedence(current_op) <= precedence(operators.top()))
            {
                std::cout << "i = " << i << ", equation[i] = " << equation[i] << std::endl;
                std::cout << "Lower precedence -> simplify a bit..." << std::endl;
                simplifyEquationTop(numbers, operators.top());
                operators.pop();
                if (operators.empty())  //operators checking down here, numbers checking at for loop
                    break;
                //else // Should never be a time when I'm comparing the same thing...
                //    current_op = operators.top();
            }
            operators.push(equation[i]);
            numbers.push(equation[i+1]); //testing, hope it won't break code
        }
        else // At Higher Precedence Operator, keep pushing
        {
            std::cout << "Higher precedence, keep pushing" << std::endl;
            operators.push(equation[i]);
            numbers.push(equation[i+1]);
        }
    }
    // Final Operations:
    while (!operators.empty())
    {
        simplifyEquationTop(numbers, operators.top());
        operators.pop();
    }
    */
}

void printEquation(const std::vector<std::string>& equ)
{
    using namespace std;
    for (const auto& token : equ)
    {
        cout << token << " ";
    }   cout << endl;
}

} // namespace arithmetic
