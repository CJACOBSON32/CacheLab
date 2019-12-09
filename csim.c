/*
 * Names:
 *
 */
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

typedef struct {

	char valid;
	int tag;
	int LRU_counter;

} cache_line;

typedef struct {

	int S; //Number of sets: 2^s
	int E; //Number of lines in a set
	int B; //Number of data blocks in each line: 2^b
	int size;

	cache_line cacheBlock[S][E];

} generalCache;

typedef enum {

	hit,
	miss,
	eviction,
	none

} cache_summary;

generalCache aCache;
int tagSize = 0;

//Method for LRU policy(?)

/*
 * helpFlag - method to provide user the correct arguments when running the program
 */
void helpFlag() {

	printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n");
	printf("Options:\n");
	printf("  -h         Print this help message.\n");
	printf("  -v         Optional verbose flag.\n");
	printf("  -s <num>   Number of set index bits.\n");
	printf("  -E <num>   Number of lines per set.\n");
	printf("  -b <num>   Number of block offset bits.\n");
	printf("  -t <file>  Trace file.\n");
	printf("Examples:\n");
	printf("  linux>   ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
	printf("  linux>   ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");

}


int main(int argc, char *argv[])
{
	//Cache size = S * E * B
	int opt;
	int s;
	int b;
	int numberOfBlocks;
	char *fileName;

	if (argc == 1) {
		helpFlag();
	}

	while((opt = getopt(argc,argv,"hvs:E:b:t:")) != -1) {

		switch(opt) {

		case 'h':
			helpFlag();
			break;

		case 'v':
			break;

		case 's':
			//Cannot be less than or equal to 0
			if (atoi(optarg) <= 0) {

				printf("Error");
				helpFlag();

			}

			s = (atoi(optarg));
			aCache.S = pow(2, s);
			break;

		case 'E':
			//Cannot be less than or equal to 0
			if (atoi(optarg) <= 0) {

				printf("Error");
				helpFlag();

			}

			aCache.E = (atoi(optarg));
			break;

		case 'b':
			//Cannot be less than or equal to 0
			if (atoi(optarg) <= 0) {

				printf("Error");
				helpFlag();
			}

			b = (atoi(optarg));
			aCache.B = pow(2, b);
			break;

		case 't':

			fileName = optarg;
			break;

		default:
			helpFlag();
			break;
		}

		tagSize = 32 - s - b;
	}

	//Helps parse the file: L,M, S
	FILE* traceFile = fopen(fileName, "r");

	//If the trace file is not one of the ones listed, sends out an error.
	if(!traceFile) {

		printf("Error no file or directory found.\n");

	}

	numberOfBlocks = aCache.S * aCache.E;
	aCache.size = numberOfBlocks * aCache.B;

	//Use malloc() to get the size needed for the cache
	aCache.cacheBlock = malloc(aCache.size * sizeof(cache_line));

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
	fclose(traceFile);
	free(aCache.cacheBlock);

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

	// Search the cache for the requested address
	for (cache_line row[aCache.E] : cache) {
		for (cache_line line : row) {
			if(line.tag == address) {
				return {cache_summary.hit, 0}
			}
		}
	}

	// If there is a miss, load into cache and return summary
	store(address, size);

	return {cache_summary.miss, 0};
}

cache_summary* store(int address, int size) {
	// Store into main memory and cache. If there is no room left, replace the one used least recently


	for (cache_line row[] : cache) {
		for (cache_line line : row) {
			if(line == 0) {
				line = {1,0,address,0}
			}
		}
	}

	return {cache_summary.hit, 0}
}

cache_summary* modify(int address, int size) {

	// Record the result of a load and a store
	cache_summary sum1 = load(address, size);
	cache_summary sum2 = store(address, size);

	cache_summary sums[2] = {sum1,sum2};

	return 0;
}

