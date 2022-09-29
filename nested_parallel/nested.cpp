#include <iostream>
#include <omp.h>

int main() {
  omp_set_max_active_levels(3);
  int thread0 = omp_get_thread_num();
  int size0 = omp_get_num_threads();
  
  #pragma omp parallel 
  {
    int thread1 = omp_get_thread_num();
    int size1 = omp_get_num_threads();
    #pragma omp parallel 
    {
      int thread2 = omp_get_thread_num();
      int size2 = omp_get_num_threads();
      #pragma omp parallel 
      {
        int thread3 = omp_get_thread_num();
        int size3 = omp_get_num_threads();
        #pragma omp critical 
        {
          std::cout
          << " LVL0, I am thread #" << thread0 << " out of: " << size0 << std::endl
          << " LVL1, I am thread #" << thread1 << " out of: " << size1 << std::endl
          << " LVL2, I am thread #" << thread2 << " out of: " << size2 << std::endl
          << " LVL3, I am thread #" << thread3 << " out of: " << size3 << std::endl;
        }
      }
    }
  }
}