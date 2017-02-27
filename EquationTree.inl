
///////////////////////////////////////
///  EQUATION TREE INLINING         ///
///  #included in EquationTree.hpp  ///
///////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <stack>

// TODO: Move to better place,
// this doesn't really belong here
template <typename T>
void print(const std::vector<T>& v)
{   using namespace std;
    cout << "  [";
    for (int i = 0; i < v.size()-1; i++)
        cout << v[i] << ", ";
    cout << v[v.size() - 1] << "]" << endl;
}

template <typename T>
inline void EquationTree<T>::delete_r(EquationNode<T>* t)
{
    if (t->left != nullptr) // assuming proper Binary Tree, no need to check right
    {
        delete_r(t->left);
        delete_r(t->right);
    }
    delete t;
}

template <typename T>
inline EquationTree<T>::~EquationTree()
{
    std::cout << "In EquationTree destructor." << std::endl;
    delete_r(mRoot);
    mRoot = nullptr;
}

template <typename T>
inline EquationTree<T>::EquationTree()
: mRoot(nullptr)
{}

template <typename T>
inline EquationTree<T>::EquationTree(const std::vector<std::string>& equ_vector)
: mRoot(nullptr)
{
    createTree(equ_vector);
}
template <typename T>
inline EquationTree<T>::EquationTree(const std::string& filename)
: mRoot(nullptr)
{
    std::ifstream f(filename);
    if (!f.is_open())
    {
        std::cout << "ERROR: Cannot open expression file" << filename << std::endl;
    }

    ///  Basic equation parsing
    /// We're now dealing with variables instead of numbers.
    /// - These variables get replaces by appropriate numbers
    ///   before/during each iteration's tree traversal.

    std::vector<std::string> equ_vector;
    std::string token;
    while (f >> token) // assumes f is correctly-spaced equation
    {
        equ_vector.push_back(token);
    }
    createTree(equ_vector);
}

template <typename T>
inline void EquationTree<T>::createTree(const std::vector<std::string>& equ_vector)
{
    /// Pratt or Shutter Yard:
    ///...let's try Shutter first?

    // Perform shunting yard algorithm to fill postfix_expression
    std::vector<std::string> postfix_expression = std::move(arithmetic::shuntingYard(equ_vector)); // stack instead?

    std::cout << "Original equation: "; arithmetic::printEquation(equ_vector);
    std::cout << "Postfix equation: "; arithmetic::printEquation(postfix_expression);

    /// Now we need to put information in the class's actual tree.

    // Assuming nonzero-length, correctly-made postfix expression:

    /**
     * The templating might be wrong at this point,
     * bear with it.
     * T in EquationNode<T> refers to the key's actual value,
     * once the number is plugged-in.
     *
     * But for right now, we're just putting string keys in for the value.
     */

    /**
     * The following does not work for anything complicated
     * ex: 5 4 3 * +
     */ /*
    mRoot = new EquationNode<T>(postfix_expression[0]); // fix templating and/or Node constructor
    for (int i = 1; i < postfix_expression.size(); i+=2)
    {
        EquationNode<T>* right_node = new EquationNode<T>(postfix_expression[i]);
                                    //      operator,    left node, right node
        mRoot = new EquationNode<T>(postfix_expression[i+1], mRoot, right_node);
    } */

    // other method
    std::stack<EquationNode<T>*> nodes;
    EquationNode<T>* first_node = new EquationNode<T>(postfix_expression[0]);
    nodes.push(first_node);
    std::size_t i = 1;
    nodes.push(new EquationNode<T>(postfix_expression[i]));
    while (i < postfix_expression.size() - 1)
    {

        i++;
        unsigned char op_value = arithmetic::opValue(postfix_expression[i]);
        if (op_value == arithmetic::Unknown)
        {  // Assuming not an error, means it's another number/variable
            nodes.push(new EquationNode<T>(postfix_expression[i]));
            //mVariableMap.
        }
        else
        {
            EquationNode<T>* right = nodes.top();
            nodes.pop();
            EquationNode<T>* left  = nodes.top();
            nodes.pop();
                                       /* a number */
            nodes.push(new EquationNode<T>("OPERATION", op_value, left, right));
        }
    }
    mRoot = nodes.top();
    nodes.pop(); // nodes should be zero now
    if (!nodes.empty())
    {
        std::cout << "ERROR: Nodes stack is not empty! (f: createTree)" << std::endl;
    }
    std::cout << "Tree created." << std::endl;
}

template <typename T>
inline T EquationTree<T>::set(const std::string& variable, const T& value)
{
    mVariableMap[variable] = value;
    return value;
}

template <typename T>
inline const T EquationTree<T>::result() const
{
    return result(mRoot);
}

template <typename T>
inline const T EquationTree<T>::result(EquationNode<T>* t) const
{
    if (t->left == nullptr) // if current node is leaf...
    {
        //TODO "none" support
        auto pvalue = mVariableMap.find(t->variable);
        if (pvalue == mVariableMap.end())
        {
            std::cout << "Error: variable " << t->variable << " not found! (f: result_r)" << std::endl;
        }
        return (*pvalue).second;
    }
    T t1 = result(t->left);
    T t2 = result(t->right);

    // should be put in Arithmetic.cpp?
    // do I ever init pre_operator values??
    switch(t->pre_operator)
    {
      case arithmetic::Addition:
        return t1 + t2;
      case arithmetic::Multiplication:
        return t1 * t2;
      case arithmetic::Subtraction:
        return t1 - t2;
      case arithmetic::Division:
        return t1 / t2;
      case arithmetic::Exponentiation:
        return pow(t1, t2); // namespace problems? Need to declare pow(Complex) inside namespace std?
      default:
        std::cout << "Error: Unknown operator! " << t->pre_operator << " (f: result)" << std::endl;
        return t->literal_value; // probably don't want to be returning this
    }
}


//////////////////////////////////////
//////////////////////////////////////

/**
 * TO DELETE
 */ /*
template <typename T>
inline std::vector<T> EquationTree<T>::equationVector(const std::string& filename)
{
    std::vector<T> equation;
    std::ifstream f(filename);
    if (!f.is_open())
    {
        std::cout << "Error opening equation file" << std::endl;
        return equation;
    }

    T number;
    std::string op;

     //
     // Does not work with a file containing '-3-3'
     // If equation is correctly "tokenized" before
     // vectorizing, this wouldn't be an issue
     //
    while (f >> number)
    {
        using namespace std;
        cout << "number:\t" << number << endl;
        equation.push_back(number);
        f >> op;
        cout << "op:\t" << op << endl;
        if (f.good())
            equation.push_back(opValue(op));
        else
            cout << "End of equation." << endl;
    }
    std::cout << "Equation before return: "; print(equation);
    std::cout << std::endl;
    return equation;
} */


/**
 * Helper function to cut code down,
 * called more than once in EquationTree ctors
 *
 * Second parameter should be an operator value... see Arithmetic.hpp constants
 *
 * Modifies: numbers stack
 * The operator stack's top element should be
 * popped externally!
 */ /*
template <typename T>
void EquationTree<T>::simplifyEquationTop(std::stack<int>& numbers, const int operators_top)
{
    int a = numbers.top(); // change to T
    numbers.pop();
    int b = numbers.top(); // change to T type
    numbers.pop();
    std::cout << "a = " << a << ", b = " <<  b << std::endl;

    switch(operators_top) {
      case arithmetic::Exponentiation:
        //numbers.push(pow(a, b)); //overloads not finished yet?
        break;
      case arithmetic::Multiplication:
        numbers.push(b * a);
        break;
      case arithmetic::Division:
        numbers.push(b / a);
        break;
      case arithmetic::Addition:
        numbers.push(b + a);
        break;
      case arithmetic::Subtraction:
        numbers.push(b - a);
        break;
      default:
        std::cout << "ERROR: Unhandled operator." << std::endl;
        break;
    }
}
*/
