#include "cache.h"
#include <stdio.h>

#define CACHE_SIZE_IN_BLOCKS 32

#define MAXROWS 32
#define MAXCOLS 256

static cache_t *cache;
long __attribute__ ((aligned (64))) test_array[MAXROWS * MAXCOLS];

int block_size = 64;

/*
 * Write data to the array, bypassing the "cache".
 */
void fillArray(long *a, int numrows, int numcols)
{
    int i, j;

    for (i = 0; i < numrows; i++)
        for (j = 0; j < numcols; j ++)
            a[i * numcols + j] = (i + 1) * (j + 1);
}

/*
 * Sum the array elements, one way.
 */
long sumA(long *a, int numrows, int numcols)
{
    int i, j;
    long sum = 0;

    for (i = 0; i < numrows; i++)
        for (j = 0; j < numcols; j ++)
	    sum += cache_read(cache, &a[i * numcols + j]);

    return sum;
}

/*
 * Sum the array elements, another way.
 */
long sumB(long *a, int numrows, int numcols)
{
    int i, j;
    long sum = 0;

    for (j = 0; j < numcols; j ++)
        for (i = 0; i < numrows; i++)
	    sum += cache_read(cache, &a[i * numcols + j]);

    return sum;
}

/*
 * Sum the array elements, a third way.
 */
long sumC(long *a, int numrows, int numcols)
{
    int i, j;
    long sum = 0;

    for (j = 0; j < numcols; j += 2)
        for (i = 0; i < numrows; i += 2)
            sum += cache_read(cache, &a[i * numcols + j]) + cache_read(cache, &a[(i+1) * numcols + j]) +
	   	   cache_read(cache, &a[i * numcols + (j+1)])  + cache_read(cache, &a[(i+1) * numcols + (j+1)]);

    return sum;
}

void print_stats(long x)
{
    int mc = cache_miss_count(cache);
    int ac = cache_access_count(cache);

    printf("Sum = %ld\n", x);
    if (ac == 0)
    {
	printf("The cache wasn't used.\n");
    }
    else
    {
	printf("Miss rate = %8.4f\n", (double) mc/ac);
    }
}

int main()
{
    fillArray(test_array, 4, 128);

    cache = cache_new(CACHE_SIZE_IN_BLOCKS, block_size, 1, CACHE_REPLACEMENTPOLICY_LRU);
    print_stats(sumA(test_array, 4, 128));

    cache = cache_new(CACHE_SIZE_IN_BLOCKS, block_size, 1, CACHE_REPLACEMENTPOLICY_LRU);
    print_stats(sumB(test_array, 4, 128));

    cache = cache_new(CACHE_SIZE_IN_BLOCKS, block_size, 1, CACHE_REPLACEMENTPOLICY_LRU);
    print_stats(sumC(test_array, 4, 128));
}
