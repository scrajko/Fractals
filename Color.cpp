
#include "Color.hpp"

Color::Color()
: r(0), g(0), b(0), a(255)
{}
Color::Color(unsigned char value)
: r(value), g(value), b(value), a(255)
{}
Color::Color(unsigned char r_, unsigned char g_, unsigned char b_)
: r(r_), g(g_), b(b_), a(255)
{}
Color::Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
: r(red), g(green), b(blue), a(alpha)
{}


// FUNCTIONS BELOW ARE NOT UPDATED FOR ALPHA SPECIFICATION


Color Color::getInverse() const
{
    return Color(rgb::Max - r,
                 rgb::Max - g,
                 rgb::Max - b);
}
Color inverse(const Color& c)
{
    return Color(rgb::Max - c.r,
                 rgb::Max - c.g,
                 rgb::Max - c.b);
}

bool operator==(const Color& lhs, const Color& rhs)
{
    return lhs.r == rhs.r &&
           lhs.g == rhs.g &&
           lhs.b == rhs.b;
}
bool operator!=(const Color& lhs, const Color& rhs)
{
    return !(lhs == rhs);
}

Color noise()
{
    return Color(rand()%256, rand()%256, rand()%256);
}
