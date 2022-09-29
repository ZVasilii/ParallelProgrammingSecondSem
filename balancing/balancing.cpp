#include <omp.h>
#include <iostream>

int main() {
  int count =  65;
  omp_set_num_threads(4);

  #pragma omp parallel for schedule(guided,4)
  for (int i = 0; i < count; ++i)
  {
      #pragma omp critical 
      {
        std::cout<< "Num: " << i << " thread: " << omp_get_thread_num() << "\n";
      }
    }
}