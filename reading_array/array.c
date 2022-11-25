#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
    const size_t ARR_SIZE = 100000;
    int * Aarr = (int*) calloc(ARR_SIZE, sizeof(int));
    long double * Barr = (long double*) calloc(ARR_SIZE, sizeof(long double));
    assert(Aarr);
    assert(Barr);
    for (size_t i = 0; i < ARR_SIZE; ++i)
        Aarr[i] = i;
    printf("A array initialized\n");

    #pragma omp parallel for shared(Barr, Aarr) schedule (static)
        for (size_t i = 1; i < ARR_SIZE - 1; ++i)
        {
                Barr[i] = Aarr[i - 1] * Aarr[i] * Aarr[i + 1] / 3.0;
        }
     printf("B array filled\n");

    //Checking correctness
    for (size_t i = 1; i < ARR_SIZE - 1; ++i)
    {
        assert(Barr[i] == Aarr[i - 1] * Aarr[i] * Aarr[i + 1] / 3.0);
    }

    printf("Assertion didn't fail\n");
    free(Aarr);
    free(Barr);
    return 0;
}
