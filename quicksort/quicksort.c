#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

/* Usage: ./quicksort <ARR_SIZE (def: 10)> <FROM_FILE (def: from keyboard)> <NUM_THREADS (def: 8)>
*/

void getArrayKeyboard(int *arr, size_t arrSize);
void getArrayFile(int *a, size_t size, const char* file_name);
void quicksort(int * a, int p, int r);
int partition(int * a, int p, int r);
int isSorted(int *arr, size_t arrSize);
void printArray(int *arr, size_t arrSize);



int main(int argc, char *argv[])
{
    const char* fileName = "test.01.dat";
    size_t arrSize  = (argc > 1) ? atoi(argv[1]) : 10;
    int fromFile = (argc > 2) ? atoi(argv[2]) : 0;
    size_t numThreads = (argc > 3) ? atoi(argv[3]) : 8;
    int *arr = calloc(arrSize,  sizeof(int));

    if (!fromFile)
        getArrayKeyboard(arr, arrSize);
    else
        getArrayFile(arr, arrSize, fileName);

    #ifdef PRINT_ARR
    printf("Unsorted: \n");
    printArray(arr, arrSize);
    #endif

    omp_set_dynamic(0);              /** Explicitly disable dynamic teams **/
    omp_set_num_threads(numThreads); /** Use N threads for all parallel regions **/

    double begin = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
            quicksort(arr, 0, arrSize);
    }
    double end = omp_get_wtime();
    #ifdef PRINT_ARR
    printf("Sorted: \n");
    printArray(arr, arrSize);
    #endif
    printf("Time: %lg (s) \n",end-begin);
    assert(isSorted(arr, arrSize));
    printf("Assertion didn't fail\n");

    free(arr);
    return 0;
}

void getArrayKeyboard(int *arr, size_t arrSize)
{
   for(size_t i = 0; i < arrSize; i++)
       scanf("%d", arr + i);
}

void getArrayFile(int *arr, size_t arrSize, const char* file_name)
{
    FILE* file = fopen(file_name, "r");
    for(size_t i = 0; i < arrSize; i++)
       fscanf(file, "%d", arr + i);
    fclose(file);
}

void printArray(int *arr, size_t arrSize)
{
   for(size_t i = 0; i < arrSize; i++)
       printf("%d ", arr[i]);
    printf("\n");
}

int isSorted(int *arr, size_t arrSize)
{
   for(size_t i = 0; i < arrSize - 1; i++)
      if(arr[i] > arr[i + 1])
        return 0;
   return 1;
}


int partition(int * arr, int p, int r)
{
    int lt[r-p];
    int gt[r-p];
    int i;
    int  j;
    int key = arr[r];
    int  lt_n = 0;
    int  gt_n = 0;

    for(i = p; i < r; i++)
    {
        if(arr[i] < arr[r])
        {
            lt[lt_n++] = arr[i];
        }
        else
        {
            gt[gt_n++] = arr[i];
        }
    }

    for(i = 0; i < lt_n; i++)
    {
        arr[p + i] = lt[i];
    }

    arr[p + lt_n] = key;

    for(j = 0; j < gt_n; j++)
    {
        arr[p + lt_n + j + 1] = gt[j];
    }

    return p + lt_n;
}

void quicksort(int * arr, int p, int r)
{
    int div;

    if(p < r)
    {
        div = partition(arr, p, r);
        #pragma omp task shared(arr)
        quicksort(arr, p, div - 1);
        #pragma omp task shared(arr)
        quicksort(arr, div + 1, r);
    }
}
