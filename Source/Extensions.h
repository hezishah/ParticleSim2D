#pragma once

#include <random>

namespace std
{
	std::default_random_engine&
	global_urng();
	
	void
	randomize();

    double
    random();

    double
    random(double min, double max);

    double
    clamp(double value, double min, double max);
}



