
#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdlib>

namespace rgb {
    const unsigned char Min   = 0;
    const unsigned char Max   = 255;
    const unsigned char Alpha = 255;
}

/*
unsigned char noise()
{
    //return rand()%(static_cast<int>(rgb::Max) + 1);
    return rand()%256;
}*/

/**
 * Class for representing an RGB color.
 * This class does not incorporate the alpha value.
 */
class Color {
  public:
    /// Default ctor makes all values 0
    explicit Color();
    /// 1-arg ctor makes grayscale image of the given value
    explicit Color(unsigned char value);
    /// 3-arg ctor assigns the corresponding RGB coordinates
    explicit Color(unsigned char r, unsigned char g, unsigned char b);
    explicit Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

  public:
    Color getInverse() const;
    //other cool color properties?
    //std::string hex_str() const;

    friend bool operator==(const Color& lhs, const Color& rhs);
    friend bool operator!=(const Color& lhs, const Color& rhs);

};



#endif // COLOR_HPP
