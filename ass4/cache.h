/*
 * cache.h
 *
 * Definition of the structure used to represent a cache.
 */
#ifndef CACHE_H
#define CACHE_H

#include <stdlib.h>
#include <inttypes.h>

/*
 * Replacement policies (you only need to implement the LRU policy).
 */
#define CACHE_REPLACEMENTPOLICY_MASK   1

#define CACHE_REPLACEMENTPOLICY_LRU    0
#define CACHE_REPLACEMENTPOLICY_RANDOM 1

/*
 * Other policies (you can use these if you want to add optional debugging
 * messages in your functions).
 */
#define CACHE_TRACE_MASK  8
#define CACHE_TRACEPOLICY 8

/*
 * Structure used to store a single cache line.
 */
typedef struct cache_line_s
{
    /* The valid bit. */
    int is_valid;

    /* The tag: an integer large enough to contain a pointer value. */
    intptr_t tag;

    /* The data: an array of unsigned char's. */
    unsigned char *data;

} cache_line_t;

/*
 * Structure used to store a cache set: a cache set contains a dynamically
 * allocated array of pointers to cache lines.
 */
typedef struct cache_set_s
{
    cache_line_t **cache_lines;
} cache_set_t;

/*
 * Structure used to store a cache.
 */
typedef struct cache_s
{ 
    /* Number of sets in the cache. */
    unsigned int num_sets;

    /* Number of cache lines in each set. */
    unsigned int associativity;

    /* Size of each cache block. */
    size_t block_size;

    /*
     * The following fields, and the >> and & operators, can be used to 
     * extract the cache block offset bits, the set index bits, and the
     * tag from an address after it has been cast to a intptr_t object.
     *
     * For instance, for a 4-way set associative cache with 2048 blocks
     * in total, where  each block  is 32 bytes, the cache_new function
     * would initialize these fields as follows:
     *     block_offset_mask:  31 (decimal), 0x01f (hexadecimal)
     *     set_index_mask   : 511 (decimal), 0x1ff (hexadecimal)
     *     set_index_shift  :   5
     *     tag_shift        :  14
     */
    intptr_t block_offset_mask;
    intptr_t set_index_mask;
    intptr_t set_index_shift;
    intptr_t tag_shift;

    /* Replacement and write policies. */
    unsigned int policies;

    /* Array of sets, each of which is an array of pointers to cache lines. */
    cache_set_t *sets;

    /* Statistics about cache usage. */
    unsigned int access_count, miss_count;
} cache_t;

/*
 * Create a new cache that contains a total of num_blocks blocks, each of which is block_size
 * bytes long, with the given associativity.
 */
cache_t *cache_new(size_t num_blocks, size_t block_size, unsigned int associativity, int policies);

/*
 * Read a single integer from the cache.
 */
long cache_read(cache_t *cache, long *address);

/*
 * Return the number of cache misses since the cache was created.
 */
int cache_miss_count(cache_t *cache);

/*
 * Return the number of cache accesses since the cache was created.
 */
int cache_access_count(cache_t *cache);
#endif
