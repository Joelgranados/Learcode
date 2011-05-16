
#ifndef REPORT_TIMINGS
#ifdef DO_TIMINGS

#define REPORT_TIMINGS(context,clock_start,clock_end) { \
    std::cerr << __FILE__<<':' << __LINE__<<"::" << "Timing " << (clock_end - clock_start) << " ms || " << context << std::endl; } 
#define SET_CLOCK_VAR(x) x

#else

#define REPORT_TIMINGS(context,clock_start,clock_end) 
#define SET_CLOCK_VAR(x) x

#endif
#endif // #ifndef REPORT_TIMINGS

#ifndef SHOWDEBUG
#ifdef DO_DEBUG
#define SHOWDEBUG(x)  {                                                             \
    std::cerr << __FILE__ << ':' << __LINE__ << "::" << x << std::endl;             \
}

#define SHOWDEBUG_PARTA(x)  {                                                       \
    std::cerr << __FILE__ << ':' << __LINE__ << "::" << x << std::ends;             \
}

#define SHOWDEBUG_PARTB(x)  { std::cerr << x << std::endl; }

#define SHOWDEBUG_PARTC(x)  { std::cerr << x << std::ends; }

#else

#define SHOWDEBUG(x)  
#define SHOWDEBUG_PARTA(x)  
#define SHOWDEBUG_PARTB(x)  
#define SHOWDEBUG_PARTC(x)

#endif
#endif // #ifndef SHOWDEBUG

///// File header starts now...
#ifndef _LEAR_UTILS_H_
#define _LEAR_UTILS_H_

#include <cctype>
#include <string>
//#include <algorithm>

namespace lear {

//  int pow(const int x, const int y) {
//      int res=1;
//      for (int i= 0; i< y; ++i) 
//          res *=x;
//      return res;
//  }

template<class T> inline T square(const T& o) {
    return o*o;
}

/**
 * Perform case independent comparison of two strings
 */
int NoCaseComparison(const std::string & s1, const std::string& s2);

/**
 * Convert a string to upper case string
 */
std::string toUpper(const std::string& str);

/**
 * Warp long multi-line string to warped multi-line string. Currently,
 * only appropiate terminating 'space' is converted to newline.
 *
 * @warning Current warping method doesnot support tabs
 * Also, the function is not efficient. So do not use it for very long
 * strings
 */
std::string& warptext(std::string& orig, const unsigned width=78);

/**
 * Returns time in ms since program started execution.
 */
double getRunningTime();

}

#endif // _LEAR_UTILS_H_
