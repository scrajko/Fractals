
#ifndef FIXED_POINT_FUNCTIONS_HPP
#define FIXED_POINT_FUNCTIONS_HPP

using Coordinate = Complex;

double derivative(double(*f)(double), double x)
{
    double h = 0.000001;
    return ( f(x + h) - f(x) ) / h;
}

Coordinate f1(const Coordinate& p)
{
    return Coordinate((p.x*p.x - 3.0) / 2.0 - p.x*p.x*p.x, p.y - p.y*p.y);//( p.y*p.y - 3.0) / 2.0);
}

// two off-shooting tentacles
Coordinate f2(const Coordinate& p)
{
    return Coordinate((p.x*p.x*p.x - 3.0*p.x) / 2.0 - 1.0 / (p.x*p.y), 1.0 / p.y - p.x*p.y*p.y);
}

// The "golden-magma" one:
Coordinate f3(const Coordinate& p)
{
    return Coordinate((p.x*p.x*p.x*std::cos(p.x*p.y) - 3.0*p.x*p.x),
                      std::sin(p.y) / p.y );
}
Coordinate f4(const Coordinate& p)
{
    return Coordinate(std::cos(p.x*p.y + p.x), std::sin(p.x*p.y - p.x));
}
Coordinate f5(const Coordinate& p)
{
    return Coordinate(std::cos(p.x), std::sin(p.y));
}
Coordinate f6(const Coordinate& p)
{
    double x = 2*p.x;
    double y = p.y;

    return Coordinate(x, y);
}
double fx7(double x)
{
    return ( 1 / (x*x*sin(x))) / 2 + (x*x);
}
Coordinate f7(const Coordinate& p)
{
    return p + Coordinate(derivative(fx7, p.y), p.x - p.y + p.y*p.x*(rand()%10));
}

Coordinate f8(const Coordinate& p)
{
    return Coordinate( (p.x)*std::cos(p.x + p.y), p.y - std::cos(p.y));
}

#endif // FIXED_POINT_FUNCTIONS_HPP

