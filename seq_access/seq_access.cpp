#include <omp.h>
#include <iostream>

int main()
{
    int i = 1;
    omp_lock_t lock;
    omp_init_lock(&lock);
    #pragma omp parallel shared(i)
    {
        //omp_set_lock(&lock);
        while (!omp_test_lock(&lock))
        {}

        i *= 2;
        #pragma omp critical
        {
            std::size_t num_thread = omp_get_thread_num();
            std::cout << "Hello, i'm thread " << num_thread 
                << ", I = " << i << std::endl;
        }
        omp_unset_lock(&lock);
    } 
    std::cout << "Final value of I is " << i << std::endl;
    omp_destroy_lock(&lock);
    return 0;
}