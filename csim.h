#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

/**
 * @brief Contains a single block of cache
 * 
 */
typedef struct {

	bool valid;
	int tag;
	int LRU_counter;

} cache_line;

/**
 * @brief Contains an entire cache
 * 
 * @param S Number of sets: 2^s
 * @param E Number of lines in a set
 * @param B Number of data blocks in each line: 2^b
 * @param size Total size of the cache (in bytes)
 */
typedef struct {

	int S; //Number of sets: 2^s
	int E; //Number of lines in a set
	int B; //Number of data blocks in each line: 2^b
	int size;

	cache_line* cacheBlock;

} generalCache;

enum cache_result {

	hit,
	miss,
	eviction,
	none

};

/**
 * @brief Result
 * 
 * @param results List of two result enums based (second one is used if modify is called)
 * @param cacheBlock Pointer to the cache block loaded and/or stored 
 */
typedef struct {

	cache_result results[2];
	cache_line* cacheBlock;

} cache_summary;

/**
 * @brief Load without changing memory, if there is a cache hit, return summary.
 * 
 * @param tag The tag to search for
 * @param size 
 * @return cache_summary* 
 */
cache_summary load(int tag, int size);

/**
 * @brief Store into main memory and cache. If there is no room left, replace the one used least recently
 * 
 * @param tag The tag to give the cache_line being stored
 * @param size The size of the stored variable (not used, may remove)
 * @return cache_summary array of length 2
 */
cache_summary store(int tag, int size);

/**
 * @brief loads and stores the tag and returns the results contiguously
 * 
 * @param tag The tag to search for
 * @param size 
 * @return cache_summary* 
 */
cache_summary modify(int tag, int size);

/**
 * @brief Request info from the cache, mark a hit or miss, and update the cache accordingly
 * 
 * @param type A char signifying how the cache is to be accessed (L - load, S - store, M - modify)
 * @param address The address in a 32 bit word in the form {tag, s, b}. The tag will be extracted.
 * @param size The size of the memory block (not used, may be removed)
 * @return cache_summary* 
 */
cache_summary getCache(char type, int address, int size);

