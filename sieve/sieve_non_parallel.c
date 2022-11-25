#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Wrong number of arguments (expected 1)\n");
        exit(0);
    }
    long long N = atoll(argv[1]);
    if (N < 2)
    {
        printf("N should be positive, at least 2\n");
        exit(0);
    }

	bool* a = (bool*) calloc (N/2, sizeof(bool));
    assert(a && "Calloc error");
	long long result = 0;

    //Initialising
	for (long long i=0; i<N/2; i++)
		a[i] = true;

    //Sieve of Eratosthenes
	for (long long i=3; i<=sqrt(N) + 1; i+=2)
		if (a[i/2])
			for (long long j=i*i; j<=N; j+=i*2)
				a[j/2]=false;

    //Count prime numbersе
	for (long long i=0; i<N/2; i++)
		if (a[i]) result++;

	printf("Number of priime numbers from 1 to %lld = %lld\n", N, result);
	free(a);
	return 0;
}