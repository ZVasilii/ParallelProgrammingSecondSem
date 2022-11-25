#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static inline long double fact(long double x);
static inline long double inversed_fact(long double x);

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Wrong number of arguments (expected 1)\n");
        exit(0);
    }
    long N = atol(argv[1]);
    if (N < 1)
    {
        printf("N should be positive, at least 1\n");
        exit(0);
    }

    long double exp = 0.0;

    #pragma omp parallel for reduction(+:exp) schedule (guided)
        for (long i = 0; i < (N + 1); ++i)
            exp += inversed_fact(i);

    printf("Exponent is %.8Lg\n", exp);
    return 0;

}

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
