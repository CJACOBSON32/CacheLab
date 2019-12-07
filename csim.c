#include "cachelab.h"

typedef struct {

	char valid;
	int tag;
	int LRU_counter;

} cache_line;

typedef struct {

	int hit;
	int miss;
	int eviction;

} cache_summary;

int main(int argc, char *argv[])
{
	// TODO: Get the cmd arguments (flags, trace file)
	printSummary(0, 0, 0);
	return 0;
}

cache_summary getCache(int* address) {
	// TODO: Request info from the cache, mark a hit or miss, and updates the cache accordingly
	return 0;
}
