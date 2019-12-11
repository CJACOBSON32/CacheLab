#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

// stdbool not working for some reason
typedef int bool;
#define true 1;
#define false 0;

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
	none

};

/**
 * @brief Result
 * 
 * @param results List of two result enums based (second one is used if modify is called)
 * @param cacheBlock Pointer to the cache block loaded and/or stored
 * @param type Char representing whether the access was L (load), S (store), or M (modify)
 */
typedef struct {

	enum cache_result results[2];
	bool eviction;
	cache_line** cacheBlock;
	char type;

} cache_summary;

/**
 * @brief Load without changing memory, if there is a cache hit, return summary.
 * 
 * @param tag The tag to search for
 * @param size 
 * @return cache_summary* 
 */
cache_summary load(int tag, int set);

/**
 * @brief Store into main memory and cache. If there is no room left, replace the one used least recently
 * 
 * @param tag The tag to give the cache_line being stored
 * @param size The size of the stored variable (not used, may remove)
 * @return cache_summary array of length 2
 */
cache_summary store(int tag, int set);

/**
 * @brief loads and stores the tag and returns the results contiguously
 * 
 * @param tag The tag to search for
 * @param size 
 * @return cache_summary* 
 */
cache_summary modify(int tag, int set);

/**
 * @brief Request info from the cache, mark a hit or miss, and update the cache accordingly
 * 
 * @param type A char signifying how the cache is to be accessed (L - load, S - store, M - modify)
 * @param address The address in a 32 bit word in the form {tag, s, b}. The tag will be extracted.
 * @param size The size of the memory block (not used, may be removed)
 * @return cache_summary* 
 */
cache_summary getCache(char type, int address, int size);

/**
 * @brief Gets the substring of a given string (inclusive range)
 * 
 * @param string The string to get the substring of
 * @param start The starting index of the substring
 * @param end The final index of the substring
 * @return char* (substring)
 */
char* substr(char* string, int start, int end);

/**
 * @brief Prints a cache_summary to the terminal with printf
 * 
 * @param summary 
 */
void printVerbose(cache_summary summary, int address, int size);
