
#include "lodepng.h"
#include "Color.hpp"
#include "Complex.hpp"
#include "EquationTree.hpp"

#include "SomeFunctions.hpp" // For the fixed point stuff

#include <iostream>
#include <cmath>

#include <sstream>
#include <iomanip> // setfill('0') << setw(5)

//random:
#include <cstdlib>
#include <ctime>

/** Example 1, for LodePNG to save PNG images
 * Encode from raw pixels to disk with a single function call
 * The image argument has width * height RGBA pixels or width * height * 4 bytes
 */
unsigned encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height)
{
  //Encode the image
  unsigned error = lodepng::encode(filename, image, width, height);

  //if there's an error, display it
  if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
  return error;
}

int urounding(double n)
{
    return static_cast<int>(n + 0.5);
}

int rounding(double n)
{
    return (n >= 0.0)? static_cast<int>(n + 0.5)
                     : static_cast<int>(n - 0.5);
}

Color color_interpolation(const Color& start, const Color& end, double t)
{
    // Can cause fence post errors for certain values of t
    // ... could have check for if (start.component == end.component) component = end.component;
    unsigned char r = static_cast<unsigned char>( rounding( (1.0 - t)*start.r + t*end.r ) );
    unsigned char g = static_cast<unsigned char>( rounding( (1.0 - t)*start.g + t*end.g ) );
    unsigned char b = static_cast<unsigned char>( rounding( (1.0 - t)*start.b + t*end.b ) );
    unsigned char a = static_cast<unsigned char>( rounding( (1.0 - t)*start.a + t*end.a ) );
    return Color(r, g, b, a);
}

Color basic_color(double t)
{
    static const Color white(255, 255, 255);
    static const Color black(  0,   0,   0);
    return color_interpolation(black, white, t);
}

Color vector_color_mapping(double t)
{
    Color btwn = Color(0, 0, 0);
    //Color btwn2 = Color(255, 255, 255);
    std::vector<Color> colors  {
       Color(255,   0,   0), /* Red */      //btwn2,
       Color(255, 255,   0), /* Yellow */   btwn,
       Color(  0, 255,   0), /* Green */    //btwn2,
       Color(  0, 255, 255), /* Cyan */     btwn,
       Color(  0,   0, 255), /* Blue */     //btwn2,
       Color(255,   0, 255), /* Magneta */  btwn,
       Color(255,   0,   0), /* Red */
    };

    const std::size_t n       = colors.size();
    const std::size_t n_prime = n - 1;
    for (std::size_t i = 1; i < n; i++)
    {
        if (t <= static_cast<double>(i) / n_prime)
        {
            return color_interpolation(
              colors[i-1],
              colors[i],
              (t - static_cast<double>(i-1) / n_prime) * n_prime
            );
        }
    }

    // Code should not reach here if t is normalized [0.0, 1.0]
    return Color(0, 0, 0);
}

Color rainbow_color_mapping(double t)
{
    const double Rainbow_Divider = 6;
    if (t <= 1.0 / 6.0)
    {
        return color_interpolation(
          Color(255,   0, 0), // Red
          Color(255, 255, 0), // Yellow
          t * 6.0
        );
    }
    else if (t <= 2.0 / 6.0)
    {
        return color_interpolation(
          Color(255, 255, 0), // Yellow
          Color(  0, 255, 0), // Green
          (t - 1.0 / 6.0) * 6.0
        );
    }
    else if (t <= 3.0 / 6.0)
    {
        return color_interpolation(
          Color(  0, 255, 0  ), // Green
          Color(  0, 255, 255), // Cyan
          (t - 2.0 / 6.0) * 6.0
        );
    }
    else if (t <= 4.0 / 6.0)
    {
        return color_interpolation(
          Color(  0, 255, 255), // Cyan
          Color(  0,   0, 255), // Blue
          (t - 3.0 / 6.0) * 6.0
        );
    }
    else if (t <= 5.0 / 6.0)
    {
        return color_interpolation(
          Color(  0,   0, 255), // Blue
          Color(255,   0, 255), // Violet
          (t - 4.0 / 6.0) * 6.0
        );
    }
    else if (t <= 6.0 / 6.0)
    {
        return color_interpolation(
          Color(255,   0, 255), // Violet
          Color(255,   0,   0), // red (again)
          (t - 5.0 / 6.0) * 6.0
        );
    }

    // ERROR IF CODE COMES HERE!
    else
    {
        return Color(0, 0, 0);
    }
}

/**
 * Would be a bool, but I wanted an efficient way
 * of returning a pretty-looking gradient, so it instead
 * returns the color based on the number of iterations it took,
 *
 * 0 (black) means it's convergent,
 * anything higher than that means that it was divergent
 */
int pointInsideMandelbrotSet(const Complex& C, const int Iterations = 100)
{
    Complex Z(0.0, 0.0);
    for (int i = 0; i < Iterations; i++)
    {
        Complex Z_squared(Z.x*Z.x - Z.y*Z.y,
                          2.0*Z.x * Z.y);
        Z = Z_squared + C;

        if (Z.magnitude_squared() > 4.0)
            return (int)i*2.5; // 2*[0, 99]
    }
    return 0;
}

Color gradientColor(const Color& begin, const Color& end, int i, const int Iterations)
{
    unsigned char r = static_cast<unsigned char>( begin.r + ( i*(end.r - begin.r) / static_cast<double>(Iterations) + 0.5 ) );
    unsigned char g = static_cast<unsigned char>( begin.g + ( i*(end.g - begin.g) / static_cast<double>(Iterations) + 0.5 ) );
    unsigned char b = static_cast<unsigned char>( begin.b + ( i*(end.b - begin.b) / static_cast<double>(Iterations) + 0.5 ) );
    return Color(r, g, b);
}

Color pointInsideExpressionTreeSet_color(EquationTree<Complex>& expression, const Complex& C, const int Iterations)
{
    // file contains : Z * Z + C or something like that
    expression.set("Z", Complex(0.0, 0.0));
    expression.set("C", C);
    for (int i = 0; i < Iterations; i++)
    {
        Complex Z = expression.set("Z", expression.result());
        if (Z.magnitude_squared() > 4.0)
        {
            if (i < Iterations/2)
                return gradientColor(Color(255, 255, 0), Color(0, 0, 255), i, Iterations/2);
            else
                return gradientColor(Color(0, 0, 255), Color(0, 0,   0), i, Iterations/2);
        }
    }
    return Color(rgb::Min);
}

int pointInsideExpressionTreeSet(EquationTree<Complex>& expression, const Complex& C, const int Iterations)
{
    // file contains : Z * Z + C or something like that
    expression.set("Z", Complex(0.0, 0.0));
    expression.set("C", C);
    for (int i = 0; i < Iterations; i++)
    {
        Complex Z = expression.set("Z", expression.result());
        if (Z.magnitude_squared() > 4.0)
        {
            return static_cast<int>( (i*rgb::Max)/static_cast<double>(Iterations) );
        }
    }
    return rgb::Min;
}
int pointInsideStaticSet(const Complex& C, const int Iterations)
{
    Complex Z(0.0, 0.0);
    for (int i = 0; i < Iterations; i++)
    {
        Z = Z * Z + C;
        if (Z.magnitude_squared() > 4.0)
        {
            return static_cast<int>( (i*rgb::Max)/static_cast<double>(Iterations) );
        }
    }
    return rgb::Min;
}

typedef Color(*ColorMapping)(double);

Color pointInsideMandelBrotSet_colorMapping(const Complex& C, const int Iterations, ColorMapping color_mapping,
                                            ColorMapping convergent_part)
{
    const double t_offset = 0.9;
    Complex Z(0.0, 0.0);
    for (int i = 0; i < Iterations; i++)
    {
        Z = Z * Z + C;

        if (Z.magnitude_squared() > 4.0)
        {
            double t = static_cast<double>(i) / (Iterations - 1);

            t += t_offset;
            if (t > 1.0)
            {
                t = t - 1.0;
            }

            t *= 2;
            if (t > 1.0)
                t = t - 1.0;

            t *= 2;
            if (t > 1.0)
                t = t - 1.0;


            return color_mapping(t);
        }
    }

    return color_mapping(t_offset);
}


const double EulersNumber = std::exp(1.0);
const Complex ImaginaryUnit = Complex(0.0, 1.0);

Complex sin(const Complex& z)
{
    return (pow(EulersNumber, ImaginaryUnit * z) - pow(EulersNumber, -ImaginaryUnit * z))
            / (2 * ImaginaryUnit);
}

Color pointInsideCustomSet_colorMapping(/*const*/ Complex& C, const int Iterations, ColorMapping color_mapping)
{
    const double t_offset = 0.0;
    Complex Z(0.0, 0.0);

    const auto orig_C = C;

    for (int i = 0; i < Iterations; i++)
    {
        Z = Z * Z + C;

        //Z = Z * Z + C;
        //C = (rand()%8 == 0) ? Z : orig_C;
        /// Looks like a patterned eagle thing:
        //Z = Z * Z + C;
        //C = Z * pow(Z, C);

        // only exact middle seems to be convergent, could be cool pattern:
        //Z = Z * Z + C;
        //C = Z + C;

        // this looks kinda beautiful:
        //Z = Z * Z + C;
        //C = Z * Z / C;

        /// basic circular ripples, maybe could simulate water ripples with animation:
        //Z = Z*Z + Complex(std::sin(abs(Z)), std::cos(abs(C)));

        /// promising:
        //Z = Z*Z + Complex(abs(Z), abs(C)) + C;

        /// Weird "crown" mandelbrot left:
        // Z = pow(Z + C, C) + C;

        /// Cool eye mandelbrot:
        //Z = Z * Z  + 1.0 / (C*C);

        if (Z.magnitude_squared() > 4.0)
        {
            double t = static_cast<double>(i) / (Iterations - 1);

            t += t_offset;
            if (t > 1.0)
                t = t - 1.0;

            return color_mapping(t);
        }
    }
    return color_mapping(t_offset);
}


/**
 * To represent the boundaries of coordinates for an image
 */
class DimensionBox {
  public:
    /**
     * Left should always be < Right.
     * Bottom should always be < Top.
     */
    DimensionBox(double x_min, double x_max,
                 double y_min, double y_max)
    : left(x_min),   right(x_max),
      bottom(y_min), top(y_max)
    {}
    void setSquareRatio(const unsigned width, const unsigned height, bool change_x /*= true*/);
  public:
    double left;   // Minimum X
    double right;  // Maximum X
    double bottom; // Minimum Y
    double top;    // Maximum Y
  private:
    DimensionBox() {}
};
/**
 * Given a width and a height, changes the dimensions of the
 * box to keep it a square ratio to the width:height
 */
void DimensionBox::setSquareRatio(const unsigned width, const unsigned height,
                                  bool change_x = true)
{
    double original_range;
    double range;
    if (change_x)
    {
        original_range = right - left;
        range = width*(top - bottom)/height;
    } else
    {
        //range of corrected x-axis
        original_range = top - bottom;
        range = width*(right - left)/height;
    }
    double avg_diff = std::abs(original_range - range);
    //increases or decreases border coordinates accordingly
    if (range > original_range)
    {
        if (change_x)
        {
            left  -= avg_diff/2.0;
            right += avg_diff/2.0;
        } else
        {
            bottom -= avg_diff/2.0;
            top    += avg_diff/2.0;
        }
    } else
    {
        if (change_x)
        {
            left  += avg_diff/2.0;
            right -= avg_diff/2.0;
        } else
        {
            bottom += avg_diff/2.0;
            top    -= avg_diff/2.0;
        }
    }
}

DimensionBox operator /(const DimensionBox& lhs, double rhs)
{
    return DimensionBox(lhs.left / rhs, lhs.right / rhs,
                        lhs.bottom / rhs, lhs.top / rhs);
}

DimensionBox squareRatio(const unsigned width, const unsigned height,
                         const DimensionBox& box, bool change_x = true)
{
    double original_range;
    double range;
    if (change_x)
    {
        original_range = box.right - box.left;
        range = width*(box.top - box.bottom)/height;
    } else
    {
        //range of corrected x-axis
        original_range = box.top - box.bottom;
        range = width*(box.right - box.left)/height;
    }
    double avg_diff = std::abs(original_range - range);
    //increases or decreases border coordinates accordingly
    if (range > original_range)
    {
        if (change_x)
        {
            return DimensionBox(box.left - avg_diff/2.0, box.right + avg_diff/2.0, box.bottom, box.top);
        } else
        {
            return DimensionBox(box.left, box.right, box.bottom - avg_diff/2.0, box.top + avg_diff/2.0);
        }
    } else
    {
        if (change_x)
        {
            return DimensionBox(box.left + avg_diff/2.0, box.right - avg_diff/2.0, box.bottom, box.top);
        } else
        {
            return DimensionBox(box.left, box.right, box.bottom + avg_diff/2.0, box.top - avg_diff/2.0);
        }
    }
}

/**
 * In TI-83/84, ZoomSquare adjusts the x-axis,
 * and keeps the y-axis constant.
 *
 * This does the same thing, but always allows
 * the x-axis to be kept constant, while
 * changing the y-axis endpoints.
 *
 * If change_x is true, the x boundaries are changed,
 * and the y boundaries are kept unchanged.
 */
void setSquareRatio(const unsigned width, const unsigned height,
                    DimensionBox& box, bool change_x = true)
{
    double original_range;
    double range;
    if (change_x)
    {
        original_range = box.right - box.left;
        range = width*(box.top - box.bottom)/height;
    } else
    {
        //range of corrected x-axis
        original_range = box.top - box.bottom;
        range = width*(box.right - box.left)/height;
    }
    double avg_diff = std::abs(original_range - range);
    //increases or decreases border coordinates accordingly
    if (range > original_range)
    {
        if (change_x)
        {
            box.left  -= avg_diff/2.0;
            box.right += avg_diff/2.0;
        } else
        {
            box.bottom -= avg_diff/2.0;
            box.top    += avg_diff/2.0;
        }
    } else
    {
        if (change_x)
        {
            box.left  += avg_diff/2.0;
            box.right -= avg_diff/2.0;
        } else
        {
            box.bottom += avg_diff/2.0;
            box.top    -= avg_diff/2.0;
        }
    }
}

void setSquareRatio( const unsigned width, const unsigned height,
                     double & min_x, double & max_x,
                     double & min_y, double & max_y,
                     bool change_x = true)
{
    double original_range = -1.0;
    double range = 1.0;
    if (change_x)
    {
        //range of corrected x-axis
        original_range = max_x - min_x;
        range = width*(max_y - min_y)/height;
    } else
    {
        //range of corrected y-axis
        original_range = max_y - min_y;
        range = height*(max_x - min_x)/width;
    }


    double avg_diff = std::abs(original_range - range);

    //
    //increases or decreases border coordinates accordingly
    //
    if (range > original_range)
    {
        if (change_x)
        {
            min_x -= avg_diff/2.0;
            max_x += avg_diff/2.0;
        } else
        {
            min_y -= avg_diff/2.0;
            max_y += avg_diff/2.0;
        }
    } else
    {
        if (change_x)
        {
            min_x += avg_diff/2.0;
            max_x -= avg_diff/2.0;
        } else
        {
            min_y += avg_diff/2.0;
            max_y -= avg_diff/2.0;
        }
    }
}

/**
 * This function is only for one dimension,
 * call it twice for a 2D coordinate system.
 *
 * Note: if aspect ratio of widths and heights
 * (length for each dimension, defined in main) used are not in a
 * "square" aspect ratio, the image will be stretched/smooshed in some way
 *
 * pixel = current coordinate of pixel
 * length = length, in pixels, of the row or column being dealt with
 * Min = Min point of coordinate axis (ex: -1.0 for Mandelbrot's x-axis)
 * Max = Max point of coordinate axis (ex:  1.5 for Mandelbrot's x-axis)
 */
double pixelToCoordinate(unsigned pixel, unsigned length, const double Min, const double Max)
{
    //start: Min
    //step:  range/length
    //end:   Max
    double range = Max - Min;
    return Min + pixel*(range/length);
}


Coordinate coordinateToPixel(const Coordinate& position, const DimensionBox& bounds,
                             unsigned Width, unsigned Height)
{
    double range_x = bounds.right - bounds.left;
    double range_y = bounds.top   - bounds.bottom;
    /*
    given [-1.5, 1.0] coordinates
          [0, 1000]   pixels

    range = 2.5
    [  |            |            |             |               |            | ]
    [-1.5         -1.0         -0.5           0.0             0.5          1.0]
       0          200          400            600             800          1000
    -1.5 -> pixel 0
     1.0 -> pixel 1000

     */

    int x = ((position.x - bounds.left)   / range_x) * (Width  - 1);
    int y = ((position.y - bounds.bottom) / range_y) * (Height - 1);
    return Coordinate(x, y);
}

bool singleFractal(const std::string& filename,
                   unsigned width, unsigned height,
                   double special_number)
{
    std::vector<unsigned char> image;
	image.resize(width*height*4);

    ///////////////////////
    ///  PIXEL SETTING  ///
    ///////////////////////

    double MinX = -1.5;
	double MaxX =  1.0;
	double MinY = -1.0;
	double MaxY =  1.0;

	setSquareRatio(width, height, MinX, MaxX, MinY, MaxY, true);
	std::cout << "x: [" << MinX << ", " << MaxX << "]\n";
	std::cout << "y: [" << MinY << ", " << MaxY << "]\n";


	for (unsigned y = 0; y < height; y++)
    {
        int color_outer = -1;
	    for (unsigned x = 0; x < width; x++)
        {
            Complex C(pixelToCoordinate(x, width,  MinX, MaxX),
                       pixelToCoordinate(y, height, MinY, MaxY));

            const int color = pointInsideMandelbrotSet(C, 100);
            const size_t tslo = 4 * width * y + 4 * x;

            image[tslo]     = color/8;
            image[tslo + 1] = color/2;
            image[tslo + 2] = color/2;
            image[tslo + 3] = rgb::Max;
        }
        if (y%(height/(width/100))==0)
            std::cout << y+1 << "/" << height << "  ("<<color_outer<<")\n";
    }
    std::cout << "Writing image...\n";
    /// WRITING THE DATA ///
	return encodeOneStep(filename.c_str(), image, width, height);
}


/**
 * Gives a DimensionBox of the wanted zoom
 * First parameter is the point being zoomed in on.
 * Second parameter is range x (real) values box will have.
 * Third parameter is range y (imaginary) values box will have.
 */
DimensionBox pointZoom(const Complex& point, const double x_range, const double y_range)
{
    return DimensionBox(point.x - x_range/2.0, point.x + x_range/2.0,
                        point.y - y_range/2.0, point.y + y_range/2.0);
}

using Coordinate = Complex;


 #define FRACTAL_MAIN 6
/*************************************************************\
 --------------------------------------------------------------
  Table of #define Contents
 --------------------------------------------------------------
 (search for the line)
 1. Main program

 2. Complex test
 3. EquationTree int test
 4. EquationTree complex test
 5. Expression input to console test

 6. Little Mandelbrot test

\*************************************************************/


/**
 * Sets up dimensions and pixels of image.
 * Writes the pixels to a .png image file
 */
#if FRACTAL_MAIN == 1
// 1. Main program
int main(int argc, char* argv[])
{
    std::string filename = "error";
    double special_number = -1.0;

    if (argc == 1) // (Only arg is executable name)
    {
        std::cout << "Enter filename, not including .png\n>";
        std::cin >> filename;
    }
    else
    {
        filename = argv[1];
        std::cout << "filename will be " << filename << ".png\n";
        if (argc == 3)
        {
            std::stringstream ss(argv[2]);

            if ( !( ss >> special_number ) )
            {
                std::cout << "Error: text is not double.\n";
                return 2;
            }
            std::cout << "special number = " << special_number << std::endl;
        } else {
            std::cout << "Error in number of arguments.\n";
            return 3;
        }
    }

	if (std::cin.fail())
    {
        std::cout << "Invalid filename.\n";
        return 1;
    }

    unsigned width  = 1920/2;
    unsigned height = 1080/2;

    if (special_number == -1.0)
    {
        std::string full_filename = filename + ".png";
        singleFractal(full_filename, width, height, special_number);
    }
    else
    {
        int i = 1;
        while (special_number < 3.0 + 0.001)
        {
            std::stringstream ss;
            ss << filename << std::setfill('0') << std::setw(4) << i << ".png"; i++;

            std::cout << "Processing " << ss.str() << "..." << std::endl;

            if (singleFractal(ss.str(), width, height, special_number))
                return 0xF;
            special_number += 0.01;
        }
    }
}


#elif FRACTAL_MAIN == 2
/**
 * 2. Complex test
 */
int main()
{
    // log() test
    Complex c1 = log(Complex(0.0, -3.0));
    // TODO << ostream& overload
    std::cout << "(" << c1.x << ", "<<c1.y<<"i)" << std::endl;
    Complex c2 = log(Complex(0.0,  3.0));
    std::cout << "(" << c2.x << ", "<<c2.y<<"i)" << std::endl;
    Complex c3 = log(Complex(3.0,  0.0));
    std::cout << "(" << c3.x << ", "<<c3.y<<"i)" << std::endl;
    Complex c4 = log(Complex(-3.0,  0.0));
    std::cout << "(" << c4.x << ", "<<c4.y<<"i)" << std::endl;
}

#elif FRACTAL_MAIN == 3
/**
 * 3. EquationTree int test
 */
int main(int argc, char* argv[])
{
    #define MEMORY_LEAK_TEST 0
    using namespace std;

    #if MEMORY_LEAK_TEST
    for (int i = 0; i < 1000000; i++)
    {
    #endif

        EquationTree<int> tree("scripts/equation_test1.txt");
        tree.set("A", 2);
        tree.set("B", 3);
        tree.set("C", 4);
        tree.set("D", 5);
        tree.set("E", 6);
        cout << "Result = " << tree.result() << endl;

    #if MEMORY_LEAK_TEST
    }
    #endif

}

#elif FRACTAL_MAIN == 4
/**
 * 4. EquationTree complex test
 */
int main()
{
    using namespace std;
    EquationTree<Complex> tree("scripts/equation_test1.txt");
    // Int or double literal passed in non-explicitly!
    //explicit would be: Complex(5), Complex(11)
    tree.set("C", Complex(0.2, 0.3));
    tree.set("Z", Complex(0.4, 0.5));
    cout << "Result = " << tree.result() << endl;
}


#elif FRACTAL_MAIN == 5
/**
 * 5. Expression input to console test
 *
 * Instead of loading variables from a file,
 * the user types in the expression as they go.
 * Similar to Python command line.
 */
#include <sstream>
int main()
{
    using namespace std;

    while (true)
    {
        std::string equation_str;
        std::vector<std::string> equation;
        std::string token;

        cout << ">>> ";
        getline(cin, equation_str);
        std::stringstream oss(equation_str);
        while (oss >> token)
        {
            equation.push_back(token);
        }

        /** Equation tree should have a vector of tokens constructor? **/
        EquationTree<double> tree(equation);
        tree.set("N", 0);
        tree.set("A", 1.1);
        tree.set("B", 2);
        tree.set("C", 3);
        tree.set("D", 4);
        tree.set("E", 5);
        tree.set("F", 6);

        cout << tree.result() << endl;
    }

}

#elif FRACTAL_MAIN == 6
/**
 * 6. Little Mandelbrot test
 */
int main()
{
    const char* filename1 = "mandelbrot_script_test_tree.png";
    const char* filename2 = "mandelbrot_script_test_static.png";
    unsigned width  = 1920;
    unsigned height = 1080;

    std::vector<unsigned char> image1;
	image1.resize(width*height*4);
    std::vector<unsigned char> image2;
	image2.resize(width*height*4);

	EquationTree<Complex> expression("scripts/mandelbrot.txt");

    ///////////////////////
    ///  PIXEL SETTING  ///
    ///////////////////////
    const Complex julia_point1(-0.743643887037151-0.000001, 0.131825904205330+0.0000008);
    DimensionBox bounds = squareRatio(width, height, pointZoom(julia_point1, 0.000015/2, 0.00001/2), true);

    std::cout << "x: [" << bounds.left   << ", " << bounds.right << "]\n";
	std::cout << "y: [" << bounds.bottom << ", " << bounds.top   << "]\n";

	std::cout << "Enter anything to continue...";
	char c; std::cin >> c;

	for (unsigned y = 0; y < height; y++)
    {
	    for (unsigned x = 0; x < width; x++)
        {
            //Complex C(pixelToCoordinate(x, width,  MinX, MaxX),
            //          pixelToCoordinate(y, height, MinY, MaxY));
            Complex C(pixelToCoordinate(x, width,  bounds.left,  bounds.right),
                      pixelToCoordinate(y, height, bounds.bottom, bounds.top));
            const Color color1 = pointInsideExpressionTreeSet_color(expression, C, 1050);
            const int   color2 = pointInsideStaticSet(C, 4000);

            const size_t pixel_index = 4 * width * (height-1 - y) + 4 * x; // y reversed
            image1[pixel_index]     = color1.r;
            image1[pixel_index + 1] = color1.g;
            image1[pixel_index + 2] = color1.b;
            image1[pixel_index + 3] = rgb::Max;

            image2[pixel_index]     = 0;
            image2[pixel_index + 1] = color2;
            image2[pixel_index + 2] = 0;
            image2[pixel_index + 3] = rgb::Max;
        }
        std::cout << "row " << y << ", done\n";
    }
    std::cout << "Writing images...\n";
	encodeOneStep(filename1, image1, width, height);
    encodeOneStep(filename2, image2, width, height);
}


#endif // FRACTAL_MAIN
