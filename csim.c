#include "cachelab.h"

typedef struct {

	int sets;
	int lines;
	int blocks;

} generalCache;

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

//Method for LRU policy(?)

int main(int argc, char *argv[])
{
	//Cache size = S * E * B
	int opt;
	char *fileName;
	while((opt = getopt(argc,argv,"s:E:b:t:vh")) != -1) {

		switch(opt) {

		case 's':
			//Cannot be less than or equal to 0
			break;
		case 'E':
			//Cannot be less than or equal to 0
			break;
		case 'b':
			//Cannot be less than or equal to 0
			break;
		case 't':
			break;
		case 'v':
			break;
		case 'h':
			break;
		default:
			return 0;
		}
	}
	//Parse the file: L,M, S
	File *traceFile = fopen(fileName, "r");

	//If valid bit = 0, miss
	//Otherwise, if tag bit of address = tag bit of cache, hit
	//If neither miss or hit, it is evicted

	// TODO: Get the cmd arguments (flags, trace file)
	printSummary(0, 0, 0);
	return 0;
}

cache_summary getCache(int* address) {
	// TODO: Request info from the cache, mark a hit or miss, and updates the cache accordingly
	return 0;
}
