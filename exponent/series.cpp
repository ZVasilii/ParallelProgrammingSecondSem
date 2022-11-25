#include <omp.h>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>

static inline long double fact(long double x)
{
	if (x < 1)
		return 1;
	else
		return x * fact(x - 1);
}

static inline long double inversed_fact(long double x)
{
	return 1.0 / fact(x);
}

static inline long double inversed_n_squared(long double x)
{
	if (x > 0)
		return 1.0 / (x * x);
	else return 0;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Wrong number of arguments (expected 1)" << std::endl;
        std::exit(0);
    }
    long N = std::atol(argv[1]);
    if (N < 1)
    {
        std::cout << "N should be positive, at least 1" << std::endl;
        std::exit(0);
    }

    long double exp = 0.0;
    long double pi = 0.0;

    #pragma omp parallel for reduction(+:exp) schedule (guided)
        for (std::size_t i = 0; i < static_cast<std::size_t>(N + 1); ++i)
            exp += inversed_fact(i);

    std::cout << "Exponent is " << exp << std::endl;


    #pragma omp parallel for reduction(+:pi) schedule (guided)
        for (std::size_t i = 0; i < static_cast<std::size_t>(N + 1); ++i)
            pi += inversed_n_squared(i);

    std::cout << "Pi is " << sqrt(pi * 6) << std::endl;

    return 0;

}