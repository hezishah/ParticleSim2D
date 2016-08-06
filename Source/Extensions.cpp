#include "Extensions.h"
#include "StringUtil.h"

#include <cstdlib>
#include <cfloat>   // DBL_MAX
#include <cmath>    // std::nextafter
#include <algorithm>

FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }

std::default_random_engine&
std::
global_urng()
{
	static std::default_random_engine u{};
	return u;
}

void
std::
randomize()
{
	static std::random_device rd{};
	global_urng().seed(rd());
}


double
std::
random()
{
    // Note: uniform_real_distribution does [start, stop), but we want to do [start, stop].
    // Pass the next largest value instead.
    static std::uniform_real_distribution<double> next(0, std::nextafter(1, DBL_MAX));
    return next(std::global_urng());
}

double
std::
random(double min, double max)
{
    return min + (random() * (max - min));
}


double
std::
clamp(double value, double min, double max)
{
    return std::min(std::max(min, value), max);
}
