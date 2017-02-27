
#ifndef ARITHMETIC_HPP
#define ARITHMETIC_HPP

#include <string>
#include <vector>

/**
 * Keeping track of arithmetic operations
 * and stuff, for use in our Expression parsing
 *
 * TODO: Exceptions?
 */
namespace arithmetic {

    const unsigned char  Exponentiation = 5;
    const unsigned char  Multiplication = 4;
    const unsigned char  Division       = 3;
    const unsigned char  Addition       = 2;
    const unsigned char  Subtraction    = 1;
    const unsigned char  Unknown        = 0;

    /**
     * Returns the corresponding value for the given string operator
     * '^'  ->  Exponentiation
     * '*'  ->  Multiplication
     * '/'  ->  Division
     * '+'  ->  Addition
     * '-'  ->  Subtraction
     * ???  ->  Unknown
     * TODO: Is linear right now...
     *       should turn into std::map
     */
    int opValue(const std::string& op);

    /**
     * Returns true if the string is
     * a known operator
     */
    bool isOperator(const std::string& op);

    /**
     * Returns the precedence - the higher, the earlier evaluated.
     */
    int precedence(const unsigned char op);

    /**
     * Turns a "naturally" written equation
     * into a postfix equation
     * Use with std::move
     */
    std::vector<std::string> shuntingYard(const std::vector<std::string>& equ);

    /**
     * Prints a vector if strings, where each string is a token
     */
    void printEquation(const std::vector<std::string>& equ);

}

#endif // ARITHMETIC_HPP
