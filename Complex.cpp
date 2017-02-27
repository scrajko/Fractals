

#include "Complex.hpp"

#include <cmath>

//////////////////////////
///  MEMBER FUNCTIONS  ///
//////////////////////////

//Not sure why I need this, see header
Complex::Complex() : x(0.0), y(0.0)
{}

Complex::Complex(double xp = 0.0, double yp = 0.0)
: x(xp),
  y(yp)
{}

Complex::Complex(int xp)
: x(static_cast<double>(xp)),
  y(0.0)
{}

double Complex::magnitude() const
{
    return std::sqrt(x*x + y*y);
}
double Complex::magnitude_squared() const
{
    return x*x + y*y;
}

///////////////////////////////////
///  MEMBER OPERATOR OVERLOADS  ///
///////////////////////////////////

//assignment

///////////////////////////////////
///  FRIEND OPERATOR OVERLOADS  ///
///////////////////////////////////
/// Note: Inlining must be done in same place as header
/// http://stackoverflow.com/questions/4769479/c-inlining-class-methods-causes-undefined-reference

Complex operator +(const Complex& left, const Complex& right)
{
    return Complex(left.x + right.x, left.y + right.y);
}
Complex operator *(const Complex& left, const Complex& right)
{
    //(x1 + y1 )*(x2 + y2) unfactored:
    return Complex(left.x * right.x - left.y * right.y,
                   left.x * right.y + left.y * right.x);
}
Complex operator -(const Complex& v)
{
    return Complex(-v.x, -v.y);
}


Complex operator -(const Complex& left, const Complex& right)
{
    return Complex(left.x - right.x, left.y - right.y);
}

Complex operator *(const Complex& left, double right)
{
    return Complex(right*left.x, right*left.y);
}
Complex operator *(double left, const Complex& right)
{
    return Complex(left*right.x, left*right.y);
}


Complex operator /(const Complex& left, double right)
{
    return Complex(left.x/right, left.y/right);
}

Complex operator /(const Complex& left, const Complex& right)
{
    // TODO: TEST
    return Complex(left.x * right.x + left.y * right.y, left.y * right.x - left.x * right.y)
            / (right.x * right.x + right.y * right.y);
}
Complex operator /(double left, const Complex& right)
{
    // TODO: TEST
    return (left / (right.x * right.x + right.y * right.y)) * Complex(right.x, -right.y);
}

bool operator==(const Complex& left, const Complex& right)
{
    return left.x == right.x && left.y == right.y;
}

std::ostream& operator<<(std::ostream& os, const Complex& src)
{
    std::cout << '(' << src.x << ", " << src.y << ')';
    return os;
}
std::istream& operator>>(std::istream& is, Complex& src)
{
    is >> src.x >> src.y;
    return is;
}

double arg(const Complex& z)
{
    return std::atan(z.y / z.x);
}

///////////////////////////////////
///  NON-MEMBER MATH OVERLOADS  ///
///////////////////////////////////
Complex pow(const Complex& base, const Complex& power)
{
    auto& a = base.x;
    auto& b = base.y;
    auto& c = power.x;
    auto& d = power.y;

    if (base.x == 0.0 && base.y == 0.0)
        return base;

    return std::pow(a*a + b*b, c / 2.0) * std::exp(-d * arg(base)) *
     Complex( std::cos( c * arg(base) + (1.0 / 2.0) * d * std::log(a*a + b*b)),
              std::sin( c * arg(base) + (1.0 / 2.0) * d * std::log(a*a + b*b))
     );
}
Complex pow(const Complex& base, double power)
{
    auto& a = base.x;
    auto& b = base.y;
    auto& c = power;
    //    d = 0;

    /*
    return std::pow(a*a + b*b, c / 2.0) *
     Complex( std::cos(c * arg(base)), std::sin(c * arg(base)));
    */
    return pow(base, Complex(power, 0.0));
}
Complex pow(double base, const Complex& power)
{
    // Lazy way, TODO: Manually rewrite without the 0!
    return pow(Complex(base, 0.0), power);
}
/**
 * Natural logarithm of complex number,
 * represented as a Vector2<double>
 */
Complex log(const Complex& c)
{
    return Complex(std::log(c.magnitude()),
                   std::atan2(c.y, c.x));
}

double real(const Complex& c)
{
    return c.x;
}
double imag(const Complex& c)
{
    return c.y;
}
double abs(const Complex& c)
{
    return c.magnitude();
}

///
/// OLD, UNUSED
///

/*
void Complex::updateMagnitude()
{
    magnitude = std::sqrt(x*x + y*y);
}

void Complex::setX(double xp)
{
    x = xp;
    updateMagnitude();
}
void Complex::setY(double yp)
{
    y = yp;
    updateMagnitude();
}
Complex::Complex(double xp = 0.0, double yp = 0.0)
: x(xp),
  y(yp),
  magnitude(std::sqrt(xp*xp + yp*yp))
{}*/





