#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

typedef struct {

	bool valid;
	int tag;
	int LRU_counter;

} cache_line;

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

typedef struct {

	cache_result results[2];
	cache_line* cacheBlock;

} cache_summary;

generalCache aCache;
int tagSize = 0;

//Method for LRU policy(?)
//Method for help flag(?)

int main(int argc, char *argv[])
{
	//Cache size = S * E * B
	int opt;
	int s;
	int b;
	int numberOfBlocks;
	char *fileName;
	
	while((opt = getopt(argc,argv,"hvs:E:b:t:")) != -1) {

		switch(opt) {

		case 'h':
			break;

		case 'v':
			break;

		case 's':
			//Cannot be less than or equal to 0
			if (atoi(optarg) <= 0) {

				printf("Error");

			}

			s = atoi(optarg);
			aCache.S = pow(2, s);
			break;

		case 'E':
			//Cannot be less than or equal to 0
			if (atoi(optarg) <= 0) {

				printf("Error");

			}

			aCache.E = atoi(optarg);
			break;

		case 'b':
			//Cannot be less than or equal to 0
			if (atoi(optarg) <= 0) {

				printf("Error");
			}

			b = atoi(optarg);
			aCache.B = pow(2, b);
			break;

		case 't':

			fileName = optarg;
			break;

		default:

			break;
		}

		tagSize = 32 - s - b;
	}
	//Helps parse the file: L,M, S
	File *traceFile = fopen(fileName, "r");

	numberOfBlocks = aCache.S * aCache.E;
	aCache.size = numberOfBlocks * aCache.B;

	//Use malloc() to get the size needed for the cache
	aCache.cacheBlock = malloc(aCache.size, aCache.E * sizeof(cache_line));

	int i;
	//Initialize everything to 0
	for (i = 0; i < numberOfBlocks; i++) {

		aCache.cacheBlock[i].valid = 0;
		aCache.cacheBlock[i].tag = 0;

	}
	//If valid bit = 0, miss
	//Otherwise, if tag bit of address = tag bit of cache, hit
	//If neither miss or hit, it is evicted

	// TODO: Get the cmd arguments (flags, trace file)
	printSummary(0, 0, 0);
	return 0;
}

/**
 * @brief Request info from the cache, mark a hit or miss, and update the cache accordingly
 * 
 * @param type A char signifying how the cache is to be accessed (L - load, S - store, M - modify)
 * @param address The address in a 32 bit word in the form {tag, s, b}. The tag will be extracted.
 * @param size The size of the memory block (not used, may be removed)
 * @return cache_summary* 
 */
cache_summary* getCache(char type, int address, int size) {
	
	cache_summary result[2] = NULL;

	// Isolate the tag from the address
	int tag = address >> (32 - tagSize);

	// Call the appropriate function for each type of memory call
	switch(type) {
		case 'L':
			result = load(tag, size);
			break;
		case 'S':
			result = store(tag, size);
			break;
		case 'M':
			result = modify(tag, size);
			break;
		default:
			printf("%c is not a access type", type);
			return NULL;
	}

	// Increment LRU_counter for every block
	for (int i = 0; i < aCache.S, i++) {
		cache_line row[aCache.E] = aCache.cacheBlock[i];
		for (int j = 0; j < aCache.E; j++) {
			cache_line line = row[j];
			if(line.valid) {
				line.LRU_counter ++;
			}
		}
	}

	return result;
}

/**
 * @brief Load without changing memory, if there is a cache hit, return summary.
 * 
 * @param tag The tag to search for
 * @param size 
 * @return cache_summary* 
 */
cache_summary load(int tag, int size) {

	// Search the cache for the requested address
	for (int i = 0; i < aCache.S, i++) {
		cache_line row[aCache.E] = aCache.cacheBlock[i];
		for (int j = 0; j < aCache.E; j++) {
			
			cache_line* line = &row[j];
			
			if((*line).tag == tag) {
				return (cache_summary) {.cacheBlock = line, .result = {hit, NULL}}
			}
		}
	}

	// If there is a miss, store into cache and return summary
	store(address, size);

	return (cache_summary) {.result = {miss, NULL}};
}

/**
 * @brief Store into main memory and cache. If there is no room left, replace the one used least recently
 * 
 * @param tag The tag to give the cache_line being stored
 * @param size The size of the stored variable (not used, may remove)
 * @return cache_summary array of length 2
 */
cache_summary store(int tag, int size) {
	cache_summary summary = { .result = {hit, NULL} };

	// cache_line generated from the parameters
	cache_line newLine = {1,tag,0};

	// Initialize the least recent as the first item to have a point of comparison
	cache_line* leastRecent = &(aCache.cacheBlock[0][0]);

	// Find an empty space, if one is found, store the block there and return.
	for (int i = 0; i < aCache.S, i++) {
		cache_line row[aCache.E] = aCache.cacheBlock[i];
		for (int j = 0; j < aCache.E; j++) {

			cache_line* line = &row[j];

			// Finds an empty space
			if(!line.valid) {
				*line = newLine;
				summary.cacheBlock = line;
				return ;
			}
			
			// Set a new least recent if one is found
			if(line.LRU_counter < leastRecent.LRU_counter)
				leastRecent = line;
		}
	}

	//Otherwise replace the least recently used
	*leastRecent = newLine;
	summary.cacheBlock = leastRecent;

	return 
}

/**
 * @brief loads and stores the tag and returns the results contiguously
 * 
 * @param tag The tag to search for
 * @param size 
 * @return cache_summary* 
 */
cache_summary modify(int tag, int size) {

	// Record the result of a load and a store
	cache_summary sum1 = load(tag, size);
	cache_summary sum2 = store(tag, size);

	// Combine both results into an array
	cache_summary sums = {.cacheBlock = sum1.cacheBlock, .results = {sum1.results[0],sum2.results[0]}};

	return sums;
}