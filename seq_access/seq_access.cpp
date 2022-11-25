#include <omp.h>
#include <iostream>

int main()
{
    int i = 1;
    #pragma omp parallel for ordered shared(i)
    for (auto num = 0; num < omp_get_num_threads(); ++num)
    {
        #pragma omp ordered
        {
            i *= 2;
            std::size_t num_thread = omp_get_thread_num();
            std::cout << "Hello, i'm thread " << num_thread
                << ", I = " << i << std::endl;
        }
    }
    std::cout << "Final value of I is " << i << std::endl;
    return 0;
}
