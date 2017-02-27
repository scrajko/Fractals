
#ifndef EQUATION_TREE_HPP
#define EQUATION_TREE_HPP

#include "Arithmetic.hpp"

#include <string>
#include <vector>
#include <unordered_map>

/**
 * Remember "Modularity"
 * http://www.cplusplus.com/forum/beginner/144179/#msg760321
 */

/**
 * All Node stuff
 * is inline for now...
 *
 * the stored operator is currently by number, not string+
 *
 * Individual node leaves don't have numbers.
 * This is handled by the unordered_map in the EquationTree
 *
 * Should just make public access?
 * Only thing that would be using this Node
 * would be the EquationTree, so you might as well
 */
template <typename T>
class EquationNode {

  public:
    EquationNode(const std::string& var, unsigned char op,
                 EquationNode<T>* leftp, EquationNode<T>* rightp)
    : variable(var), pre_operator(op), left(leftp), right(rightp)
    {}
    EquationNode(const std::string& var)
    : variable(var), left(nullptr), right(nullptr)
    {
        T num;
        // if var is a string that can be a number:
        if ((std::istringstream(variable) >> num))
        {
            std::cout << "Interpreting " << variable << " as a number literal..." << std::endl;
            literal_value = num;
            variable = "none";
        }
    }
    EquationNode(T keyp)
    : literal_value(keyp), left(nullptr), right(nullptr)
    {}
    EquationNode(T keyp, EquationNode<T>* leftp, EquationNode<T>* rightp)
    : literal_value(keyp), left(leftp), right(rightp)
    {}

    /**
     * Operator variable should include functions too?
     * 0 - no operation (leaf)
     * 1 - ...
     * ...
     * 7 - take the log
     * 8 - take the pow... but pow is binary so...
     * pow would be its own node like + or *
     */ /*
    int operation() const { return mOperator; }
    const T& key()  const { return mKey; }
    EquationNode<T>* left()  const { return mLeft;  }
    EquationNode<T>* right() const { return mRight; }
    void setKey(const T& keyp) { mKey = keyp; }
    void setLeft(EquationNode<T>* lp)  { mLeft  = lp; }
    void setRight(EquationNode<T>* rp) { mRight = rp; } */

  public:
    std::string variable; // Z, C, etc.
    // TODO : what about number literals (ex: Z = 2.57 * Z + C)
    //    ... make the variable be "none",
    //     and it directly puts the result in key.
    T literal_value; // only used when variable == "none"
    int post_operator; // log, abs... TODO: change type to arithmetic
    unsigned char pre_operator; // + - * / ^, // TODO: change type to arithmetic
    EquationNode<T>* left;
    EquationNode<T>* right;

  private:
    EquationNode();

};

/**
 * Should be made once,
 * and the values be set every iteration
 *
 * All V leaves should point to the V value
 * All C leaves should point to the C value
 *
 * The USER of this tree should be able to:
 *   load an equation from file,
 *   set its V and C values,
 *   get an equation from that.
 * The user does not need to insert/remove,
 * or access certain nodes
 *  Maybe let them know the current values of C and V.
 *
 */
template <typename T>
class EquationTree {
  public:
    EquationTree();
    ~EquationTree();
    //EquationTree(const char* filename); //redundant?

    // Create a tree from a vector of string tokens
    EquationTree(const std::vector<std::string>& equ_vector);

    // Create a tree from a file containing tokens
    EquationTree(const std::string& filename);

    /**
     * [Re]makes the tree based off of
     * the equation in the new file
     */
    load(const std::string& filename);

    /**
     * Set the variable as the value
     * Uses the unordered_map.
     */
    T set(const std::string& variable, const T& value);

    /**
     * Traverses the tree,
     * evaluating a total value from it
     */
    const T result() const; // make T& if result is data member

    //void setC(const T& Cp);
    //void setZ(const T& Vp);
    //const T& plug_in(const T& Cp, const T& Zp);


  private:
    /**
     * For being able to privately do recursion,
     * starting with result(mRoot);
     */
    const T result(EquationNode<T>*) const;

    /**
     * To avoid repeated code from both the filename constructor,
     * and the std::vector<std::string> equation constructor
     */
    void createTree(const std::vector<std::string>& equ_vector);

    /**
     * Recursively cleans up memory allocation
     * for each node. Called in destructor.
     */
    void delete_r(EquationNode<T>*);
    //std::vector<T> equationVector(const std::string& filename);
    //void insert(const T& key, EquationNode<T>* t);

  private:
    //T C_value; T V_value;
    EquationNode<T>* mRoot;
    //find the variable, get the key (a number)
    std::unordered_map<std::string, T> mVariableMap;
 };

 #include "EquationTree.inl"

 #endif // EQUATION_TREE_HPP
