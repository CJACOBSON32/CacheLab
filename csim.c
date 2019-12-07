#include "cachelab.h"

typedef struct {

	char valid;
	int tag;
	int LRU_counter;

} cache_line;

typedef enum {

	hit,
	miss,
	eviction,
	none

} cache_summary;

int S;
int E;

cache_line cache[][];

int main(int argc, char *argv[])
{
	// TODO: Get the cmd arguments (flags, trace file)
	printSummary(0, 0, 0);
	return 0;
}

cache_summary* getCache(char type, int address, int size) {
	// Request info from the cache, mark a hit or miss, and update the cache accordingly
	cache_summary result[2] = 0;

	switch(type) {
		case 'L':
			result = load(address, size);
			break;
		case 'S':
			result = store(address, size);
			break;
		case 'M':
			result = modify(address, size);
			break;
	}

	return result;
}

cache_summary* load(int address, int size) {
	// Load without changing memory, if there is a cache hit, return summary.


	// If there is a miss, load into cache and return summary


	return 0;
}

cache_summary* store(int address, int size) {
	// Store into main memory and cache

	return 0;
}

cache_summary* modify(int address, int size) {

	// Record the result of a load and a store
	cache_summary sum1 = load(address, size);
	cache_summary sum2 = store(address, size);

	cache_summary sums[2] = {sum1,sum2};

	return 0;
}

