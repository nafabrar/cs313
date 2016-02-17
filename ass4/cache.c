/*
 * cache.c
 */
#include "cache.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
 * Initialize a new cache line with a given block size. This function is used as a helper
 * by the cache_set_init function.
 */
static void cache_line_init(cache_line_t *cache_line, size_t block_size)
{
    cache_line->is_valid = 0;
    cache_line->data = (unsigned char *) malloc(block_size * sizeof(unsigned char));
}

/*
 * Initialize a new cache set with the given associativity and block size. This function is
 * used as a helper by the cache_new function.
 */
static void cache_set_init(cache_set_t *cache_set, unsigned int associativity, size_t block_size)
{
    int i;
    cache_set->cache_lines = (cache_line_t **) malloc(associativity * sizeof (cache_line_t *));

    for (i = 0; i < associativity; i++)
    {
	cache_set->cache_lines[i] = (cache_line_t *) malloc(sizeof(cache_line_t));
	cache_line_init(cache_set->cache_lines[i], block_size);
    }
}

/*
 * Helper function that is used to compute the shift and mask given the number of bytes/sets.
 * You do not need to worry about this function.
 */
static void compute_shift_and_mask(unsigned int value, intptr_t *shift, intptr_t *mask, intptr_t init_shift)
{
    *mask = 0;
    *shift = init_shift;

    while (value > 1)
    {
	(*shift)++;
	value >>= 1;
	*mask = (*mask << 1) | 1;
    }
}

/*
 * Create a new cache that contains a total of num_blocks blocks, each of which is block_size
 * bytes long, with the given associativity, and the given set of cache policies for replacement
 * and write operations. Possible values of polices are listed in file cache.h.
 */
cache_t *cache_new(size_t num_blocks, size_t block_size, unsigned int associativity, int policies)
{
    int i;

    /*
     * Error checking.
     */
    assert(block_size % sizeof(long) == 0);
    assert(num_blocks % associativity == 0);

    /*
     * Create the cache and initialize constant fields.
     */
    cache_t *cache = (cache_t *) malloc(sizeof(cache_t));
    cache->access_count = 0;
    cache->miss_count = 0;

    /*
     * Initialize size fields.
     */
    cache->policies = policies;
    cache->block_size = block_size;
    cache->associativity = associativity;
    cache->num_sets = num_blocks / associativity;

    /*
     * Initialize shifts and masks.
     */
    compute_shift_and_mask(block_size, &cache->set_index_shift, &cache->block_offset_mask, 0);
    compute_shift_and_mask(cache->num_sets, &cache->tag_shift, &cache->set_index_mask, cache->set_index_shift);

    /*
     * Initialize cache sets.
     */
    cache->sets = (cache_set_t *) malloc(cache->num_sets * sizeof (cache_set_t));
    for (i = 0; i < cache->num_sets; i++)
    {
	cache_set_init(&cache->sets[i], cache->associativity, cache->block_size);
    }

    /*
     * Done.
     */
    return cache;
}

/*
 * Determine whether or not a cache line is valid and its tag maches the given
 * tag. Returns 1 if both conditions are satisfied, 0 otherwise.
 */
static int cache_line_is_valid_and_both_tags_match(cache_line_t *cache_line, intptr_t tag)
{
    /* TO BE COMPLETED BY THE STUDENT */
    if(cache_line->tag == tag && cache_line->is_valid == 1)
        return 1;
    return 0;
    
}


/*
 * Return  the integer  that is at offset "offset" inside a  cache line. This
 * function assume that the cache line is valid, and that tags match (it does
 * not need to check this again).
 */
static long cache_line_retrieve_data(cache_line_t *cache_line, size_t offset)
{
    return*((long*)(cache_line->data+offset));
    /* TO BE COMPLETED BY THE STUDENT */
}

/*
 * Move the cache lines inside a cache set so the cache line with the given index is
 * tagged as the most recently used one. When we use a LRU replacement policy (the
 * only one where this function should be called), the most recently used cache line
 * will be the 0'th one in the set, the second most recently used cache line will be
 * next, etc. Cache lines whose valid bit is 0 will occur after all cache lines whose
 * valid bit is 1.
 */
static cache_line_t *cache_line_make_mru(cache_set_t *cache_set, size_t line_index)
{
    int i;
    cache_line_t *line = cache_set->cache_lines[line_index];

    for (i = line_index - 1; i >= 0; i--)
    {
	cache_set->cache_lines[i + 1] = cache_set->cache_lines[i];
    }

    cache_set->cache_lines[0] = line;
    return line;
}

/*
 * Retrieve a matching cache line from a set, if one exists. This function is
 * responsible for verifying that the cache line it returns is valid and that
 * the tags match. This function returns NULL if no matching line was found.
 */
static cache_line_t *cache_set_find_matching_line(cache_t *cache, cache_set_t *cache_set, intptr_t tag)
{
    /* TO BE COMPLETED BY THE STUDENT */

    /*
     * Don't forget to call cache_line_make_mru(cache_set, i) if you find a that cache line i
     * is a match.
     */
    int i;
    for(i=0;i < cache->associativity;i++){
        if((cache_line_is_valid_and_both_tags_match(cache_set->cache_lines[i], tag)) == 1)
            return cache_line_make_mru(cache_set,i);
    }
    return NULL;

}

/*
 * Function to find a cache line to use for new data. It should return a
 * pointer to a  cache line in the set that we will use to store a block
 * retrieved from main memory.
 */
static cache_line_t *find_available_cache_line(cache_t *cache, cache_set_t *cache_set)
{
    /* TO BE COMPLETED BY THE STUDENT */

    /*
     * Don't forget to call cache_line_make_mru(cache_set, i) once you have decided to
     * use cache line i.
     */
    int i;
    for(i=0;i < cache->associativity;i++){
        if(cache->sets->cache_lines[i]->is_valid)
            return cache_line_make_mru(cache_set, i);
    }
    return NULL;
}

/*
 * Add a block to a given cache set.
 */
static cache_line_t *cache_set_add(cache_t *cache, cache_set_t *cache_set, intptr_t address, intptr_t tag)
{
    /*
     * First locate the cache line to use.
     */
    cache_line_t *line = find_available_cache_line(cache, cache_set);

    /*
     * Now set it up.
     */
    line->tag = tag;
    line->is_valid = 1;
    memcpy(line->data, (void *) (address & ~cache->block_offset_mask), cache->block_size);

    /*
     * And return it.
     */
    return line;
}

/*
 * Read a single integer from the cache. The parameter "address" contains the
 * memory address of the long to be read.
 */
long cache_read(cache_t *cache, long *address)
{
    /* TO BE COMPLETED BY THE STUDENT */
    intptr_t tagindex= (intptr_t)address >> cache-> tag_shift;
    intptr_t blockoffset = (intptr_t)address & cache->block_offset_mask;
    intptr_t address1 =(intptr_t) address;
    long setindex = (address1 >> cache->set_index_shift) & cache->set_index_mask;
    cache_set_t cache_set_index = cache->sets[setindex];
    
    cache_line_t* linematch = cache_set_find_matching_line(cache, &cache_set_index, tagindex);
    
    if (linematch) {
        cache->access_count++;
        
        return cache_line_retrieve_data(linematch, blockoffset);
    }
    
    else if(!linematch) {
        cache->access_count++;
        cache->miss_count++;
        linematch = cache_set_add(cache, &cache_set_index, address1, tagindex);
        return cache_line_retrieve_data(linematch, blockoffset);
    }return 0;

}

/*
 * Return the number of cache misses since the cache was created.
 */
int cache_miss_count(cache_t *cache)
{
    return cache->miss_count;
}

/*
 * Return the number of cache accesses since the cache was created.
 */
int cache_access_count(cache_t *cache)
{
    return cache->access_count;
}
