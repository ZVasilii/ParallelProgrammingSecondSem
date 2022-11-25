#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

//Count prime numbers using eratosphenes sieve
//Blockwise version of the algorithm for parallel version
const int sliceSize = 512*1024;  // Number of chunk for every parallel workload
                                 //Evaluated expirementally
long long parallel(long long N, long long sliceSize);
long long parallelSingleBlock(const long long from, const long long to);
long long not_parallel(long N);

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
      printf("Wrong long long of arguments (expected 1)\n");
      exit(0);
  }
  long long N = atoll(argv[1]);
  if (N < 2)
  {
      printf("N should be positive, at least 2\n");
      exit(0);
  }
  printf("Primes between 2 and %lld\n\n", N);

  printf("OpenMP uses up to %d threads running on %d processors\n\n",
         omp_get_max_threads(), omp_get_num_procs());

  printf("Not parallel Sieve\n");
  double startTime = clock();
  long long found = not_parallel(N);
  double duration  = clock() - startTime;
  printf("%lld primes found in %.2lgs\n\n", found, duration / CLOCKS_PER_SEC);

  printf("Parallel Sieve\n");
  startTime = clock();
  found = parallel(N, sliceSize);
  duration  = clock() - startTime;
  printf("%lld primes found in %.2lgs\n\n", found, duration / CLOCKS_PER_SEC);

  return 0;
}

// process slice-by-slice, odd long longs only
long long parallel(long long N, long long sliceSize)
{
  long long found = 0;
  // each slices covers ["from" ... "to"], incl. "from" and "to"
  #pragma omp parallel for reduction(+:found) schedule(guided)
    for (long long from = 2; from <= N; from += sliceSize)
    {
      long long to = from + sliceSize;
      if (to > N)
        to = N;

      found += parallelSingleBlock(from, to);
    }
  return found;
}

// process only odd numbers of a specified block
long long parallelSingleBlock(const long long from, const long long to)
{
  const long long memorySize = (to - from + 1) / 2;

  // initialize
  bool* isPrime = (bool*) calloc(memorySize, sizeof(bool));
  for (long long i = 0; i < memorySize; i++)
    isPrime[i] = true;

  for (long long i = 3; i*i <= to; i += 2)
  {

    // skip multiples of three: 9, 15, 21, 27, ...
    //(to speed up calculations according to current sliceSize)
    if (i >= 3*3 && i % 3 == 0)
      continue;
    // skip multiples of five
    if (i >= 5*5 && i % 5 == 0)
      continue;
    // skip multiples of seven
    if (i >= 7*7 && i % 7 == 0)
      continue;
    // skip multiples of eleven
    if (i >= 11*11 && i % 11 == 0)
      continue;
    // skip multiples of thirteen
    if (i >= 13*13 && i % 13 == 0)
      continue;

    // skip numbers before current slice
    long long minJ = ((from+i-1)/i)*i;
    if (minJ < i*i)
      minJ = i*i;

    // start value must be odd
    if ((minJ & 1) == 0)
      minJ += i;

    // find all odd non-primes
    for (long long j = minJ; j <= to; j += 2*i)
    {
      long long index = j - from;
      isPrime[index/2] = false;
    }
  }

  // count primes in this block
  long long found = 0;
  for (long long i = 0; i < memorySize; i++)
    if (isPrime[i])
      found ++;
  // 2 is not odd => include on demand
  if (from <= 2)
    found++;

  free(isPrime);
  return found;
}

  // simple serial sieve of Eratosthenes
long long not_parallel(long N)
{
  // initialize
  bool* isPrime = (bool*) calloc (N, sizeof(bool));
  for (long long i = 0; i <= N; i++)
    isPrime[i] = true;

  // find all non-primes
  for (long long i = 2; i*i <= N; i++)
    if (isPrime[i])
      for (long long j = i*i; j <= N; j += i)
        isPrime[j] = false;

  // sieve is complete, count primes
  int found = 0;
  for (long long i = 2; i <= N; i++)
    if (isPrime[i])
      found++;

  free(isPrime);
  return found;
}
