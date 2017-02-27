
#ifndef COMPLEX_HPP
#define COMPLEX_HPP

#include <iostream> // ostream&

/**
 * Complex class.
 *  Used to represent complex numbers in the xy plane,
 * where the x-axis represents the real part,
 * and the y-axix represents the imaginary part
 * of a complex number.
 *
 *
 * TODO: Operator Overloading for
 *       addition, subtraction, and multiplication
 *
 * Decision: NOT using "real" and "imag":
 *   "x" is the real part
 *   "y" is the imaginary part
 * Reason: Correlates to xy complex plane,
 *         used to plot fractals
 *  These are publicly accessible!
 */
class Complex {
  public:

    /**
     * Not sure why I need to have this,
     * if my 2-arg ctor by default sets both x and y
     * to 0 if not given in the arguments
     */
    Complex();

    /**
     * Constructor
     * Note: 0-arg ctor sets values to 0.0,
     *       1-arg ctor sets imaginary value to 0.0
     * How does passing an int literal work?
     */
    Complex(double xp, double yp);

    /**
     * Testing, to be able to pass
     * in int literals as the real value
     */
    Complex(int xp);

    /**
     * Relatively expensive - calls sqrt()
     * Use magnitude_squared() if you only
     * need the value for comparison purposes
     */
    double magnitude() const;

    /**
     * Avoids calling sqrt() by returning the square,
     * for performance reasons
     */
    double magnitude_squared() const;

  public:
    double x;
    double y;


    ////////////////////////////////////////////////////////
    ///  Non-class friends (not that it really matters)  ///
    ////////////////////////////////////////////////////////

    // Negative operator, inverts each component
    friend Complex operator -(const Complex& c);

    //Complex addition:
    friend Complex operator +(const Complex& left, const Complex& right);

    //Complex subtraction:
    friend Complex operator -(const Complex& left, const Complex& right);

    //Complex multiplication:
    friend Complex operator *(const Complex& left, const Complex& right);
    friend Complex operator *(const Complex& left, double right);
    friend Complex operator *(double left, const Complex& right);

    //Complex division:
    friend Complex operator /(const Complex& left, const Complex& right);
    //Divides each component by the "scalar" right
    friend Complex operator /(const Complex& left, double right);

    friend bool operator ==(const Complex& left, const Complex& right);

    friend std::ostream& operator<<(std::ostream& os, const Complex& src);
    friend std::istream& operator>>(std::istream& is, Complex& src);
};

//////////////////////////////////////
///  NON-CLASS FUNCTION OVERLOADS  ///
//////////////////////////////////////

/* rhs to the power of lhs */
Complex pow(const Complex&, const Complex&);
Complex pow(const Complex&, double power);
Complex pow(double base, const Complex&);

/* Natural Log */
Complex log(const Complex&);

double real(const Complex&);
double imag(const Complex&);
double abs(const Complex&);

#endif // COMPLEX_HPP
