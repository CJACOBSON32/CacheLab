/*
 * Names:
 *
 */
#include "csim.h"

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

	// Parse through traces and make memory calls

	FILE* traces = fopen(fileName, "r");
	int line = 0;
	char content[512];

	char type;
	int address;
	int size;

	if(traces == 0) printf("Couldn't open %s", fileName); // Print an error if the file couldn't be read or is empty
	else {
		//If valid bit = 0, miss
		//Otherwise, if tag bit of address = tag bit of cache, hit
		//If neither miss or hit, it is evicted
		
		while(fgets(content, 512, traces)) {
			line ++;
			
			type = content[0];
			address = atoi(substr(content, 3, 4));
			size = atoi(substr(content, 6, 6));

			cache_summary summary = getCache(type, address, size);
		}
	}

	

	// TODO: Get the cmd arguments (flags, trace file)
	fclose(traceFile);
	free(aCache.cacheBlock);

	printSummary(0, 0, 0);
	return 0;
}

cache_summary getCache(char type, int address, int size) {
	
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
	for (int i = 0; i < aCache.S; i++) {
		cache_line row[aCache.E] = aCache.cacheBlock[i];
		for (int j = 0; j < aCache.E; j++) {
			cache_line* line = &row[j];
			if(line.valid) {
				*line.LRU_counter ++;
			}
		}
	}

	return result;
}

cache_summary load(int tag, int size) {

	// Search the cache for the requested address
	for (int i = 0; i < aCache.S; i++) {
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

cache_summary modify(int tag, int size) {

	// Record the result of a load and a store
	cache_summary sum1 = load(tag, size);
	cache_summary sum2 = store(tag, size);

	// Combine both results into an array
	cache_summary sums = {.cacheBlock = sum1.cacheBlock, .results = {sum1.results[0],sum2.results[0]}};

	return sums;
}

char* substr(char* string, int start, int end) {
	int subSize = end - start + 2;
	char substring[subSize];
	
	memcpy( substring, &string[start], subSize - 1);

	substring[subSize - 1] = "\0";

	return substring;
}