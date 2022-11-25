#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

void print_matrix(const int* m, const size_t height, const size_t width);
void fill_rand(int* m, const size_t max_elem, const size_t height, const size_t width);

int main(int argc, char** argv)
{
/*
    Usage: ./matrix <NUM_THREADS (def: 8)>
    -DPRINT compile option for printing matrices
*/

    //A_matr {N * M}
    //B_matr {M * L}
    //C_matr {N * L}
    // C_matr = A_matr * B_matr
    size_t numThreads = (argc > 1) ? atoi(argv[1]) : 8;
    omp_set_num_threads(numThreads);
    const size_t N_SZ =1000;
    const size_t M_SZ =2000;
    const size_t L_SZ =3000;
    const size_t MAX_ELEM = 20;
    int * A_matr = (int *) calloc(N_SZ * M_SZ, sizeof(int));
    int * B_matr = (int *) calloc(M_SZ * L_SZ, sizeof(int));
    int * C_matr = (int *) calloc(N_SZ * L_SZ, sizeof(int));
    assert(A_matr);
    assert(B_matr);
    assert(C_matr);
    srand(time(NULL));
    fill_rand(A_matr, MAX_ELEM, N_SZ, M_SZ);
    fill_rand(B_matr, MAX_ELEM, M_SZ, L_SZ);

    double begin = omp_get_wtime();
    #pragma omp parallel for shared(A_matr, B_matr, C_matr) schedule (guided)
    for(size_t i = 0; i < N_SZ; ++i)
        for (size_t j = 0; j < L_SZ; ++j)
            for (size_t k = 0; k < M_SZ; ++k)
                C_matr[i * L_SZ + j] += A_matr[i * M_SZ + k] * B_matr[k * L_SZ + j];
    double end = omp_get_wtime();

    #ifdef PRINT
    printf("A_matrix:\n");
    print_matrix(A_matr, N_SZ, M_SZ);
    printf("B_matrix:\n");
    print_matrix(B_matr, M_SZ, L_SZ);
    printf("C_matrix:\n");
    print_matrix(C_matr, N_SZ, L_SZ);
    #endif
    printf("Time: %lg (s) \n",end-begin);

    free(A_matr);
    free(B_matr);
    free(C_matr);

}

void print_matrix(const int* m, const size_t height, const size_t width)
{
    assert(m);
    for (size_t i = 0; i < height; ++i)
    {
        for (size_t j = 0; j < width; ++j)
            printf("%d ", m[i * width + j]);
        printf("\n");
    }
    printf("\n");
}

void fill_rand(int* m, const size_t max_elem, const size_t height, const size_t width)
{
    assert(m);
    for (size_t i = 0; i < height; ++i)
        for (size_t j = 0; j < width; ++j)
           m[i * width + j] = rand() % max_elem;
}
