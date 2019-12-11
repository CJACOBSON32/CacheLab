/*
 * Names: Cameron Jacobson, Winnie Ly
 *
 */
#include "csim.h"
#define INT_SIZE 64

generalCache aCache;
int tagSize = 0;
int s;
int b;
bool extraVerbose = false;

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

	int opt;
	bool verbose = false;
	char *fileName;

	if (argc == 1) {
		helpFlag();
	}

	while((opt = getopt(argc,argv,"ihvs:E:b:t:")) != -1) {

		switch(opt) {

			case 'h':
				printf("Makes it to switch h\n");
				helpFlag();
				break;

			case 'v':
				verbose = true;
				break;

			case 's':
				//Cannot be less than or equal to 0
				if (atoi(optarg) <= 0) {
					printf("Error, s cannot be <= 0");
					helpFlag();
				}

				s = (atoi(optarg));
				aCache.S = (int)pow(2, (double)s);
				break;
			case 'i':
				extraVerbose = true;
				break;

			case 'E':
				//Cannot be less than or equal to 0
				if (atoi(optarg) <= 0) {
					printf("Error, E cannot be <= 0");
					helpFlag();
				}

				aCache.E = (atoi(optarg));
				break;

			case 'b':
				//Cannot be less than or equal to 0
				if (atoi(optarg) <= 0) {

					printf("Error, b cannot be <= 0");
					helpFlag();
				}

				b = (atoi(optarg));
				aCache.B = (int)pow(2, b);
				break;

			case 't':

				fileName = optarg;
				break;

			default:
				helpFlag();
				break;
		}

	}

	aCache.size = aCache.S * aCache.E * aCache.B;

	tagSize = INT_SIZE - s - b;
	if(extraVerbose)
		printf("tagSize = %i\n", tagSize);

	//Use malloc() to get the size needed for the cache
	aCache.cacheBlock = malloc(aCache.S * sizeof(cache_line*));

	//Initialize everything to 0 or null
	for (int i = 0; i < aCache.S; i++) {
		aCache.cacheBlock[i] = malloc(aCache.E * sizeof(cache_line));
		for (int j = 0; j < aCache.E; j++) {
			aCache.cacheBlock[i][j].valid = 0;
			aCache.cacheBlock[i][j].tag = 0;
		}

	}

	// Parse through traces and make memory calls

	FILE* traces = fopen(fileName, "r");
	int line = 0;
	char content[512];

	char type;
	int address;
	int size;

	int hits = 0;
	int misses = 0;
	int evictions = 0;

	if(traces == 0) printf("Couldn't open %s", fileName); // Print an error if the file couldn't be read or is empty
	else {
		
		while(fgets(content, 512, traces)) {
			line ++;
			
			type = content[1];

			// Gets the string of the numerical portion of the line content and parse through them with strtok
			char* numbers = content + 3;
			char* token = strtok(numbers, ",");
			address = atoi(token);
			token = strtok(NULL, ",");
			size = atoi(token);

			cache_summary summary = getCache(type, address, size);

			if(verbose)
				printVerbose(summary, address, size);

			for (int i = 0; i < 2; i++) {
				switch(summary.results[i]) {
					case hit:
						hits ++;
						break;
					case miss:
						misses ++;
						break;
					case eviction:
						evictions ++;
						break;
					default:
						break;
				}
			}
		}
	}



	// TODO: Get the cmd arguments (flags, trace file)
	fclose(traces);
	free(aCache.cacheBlock);

	printSummary(hits, misses, evictions);
	return 0;
}

cache_summary getCache(char type, int address, int size) {

	cache_summary result = {0};

	// Isolate the tag and set from the address
	int tag = address >> (INT_SIZE - tagSize);
	int set = ((address << (tagSize - 1)) >> (tagSize - 1)) >> b;

	if(extraVerbose)
		printf("\naddress:%#05x\ntag: %i\nset: %i\n", address, tag, set);

	if(set < 0)
		printf("Error, set index cannot be negative");

	// Call the appropriate function for each type of memory call
	switch(type) {
		case 'L':
			result = load(tag, set);
			break;
		case 'S':
			result = load(tag, set);
			break;
		case 'M':
			result = modify(tag, set);
			break;
		default:
			printf("%c is not a access type", type);
			return result;
	}

	result.type = type;

	// Increment LRU_counter for every block
	for (int i = 0; i < aCache.S; i++) {
		cache_line* row = aCache.cacheBlock[i];
		for (int j = 0; j < aCache.E; j++) {
			cache_line* line = &row[j];
			if((*line).valid) {
				(*line).LRU_counter ++;
			}
		}
	}

	// Reset the LRU_counter for the accessed block
	(*result.cacheBlock).LRU_counter = 0;

	return result;
}

cache_summary load(int tag, int set) {

	//Initialize summary

	// Search the set for the requested address
	for (int i = 0; i < aCache.E; i++) {

		cache_line* line = &(aCache.cacheBlock[set][i]);

		// Return the line
		if((*line).tag == tag && (*line).valid)
			return (cache_summary) {.cacheBlock = line, .results = {hit, none}};
	}

	// If there is a miss, store into cache and return summary
	cache_summary storeSum = store(tag, set);

	return storeSum;
}

cache_summary store(int tag, int set) {
	cache_summary summary = { .results = {none, none} };

	// cache_line generated from the parameters
	cache_line newLine = {1,tag,0};

	// Initialize the least recent as the first item to have a point of comparison
	cache_line* leastRecent = &(aCache.cacheBlock[0][0]);

	// Find an empty space, if one is found, store the block there and return.
	for (int i = 0; i < aCache.E; i++) {

		cache_line* line = &(aCache.cacheBlock[set][i]);

		// Finds an empty space
		if(!((*line).valid)) {
			*line = newLine;
			summary.cacheBlock = line;
			summary.results[0] = miss;
			return summary;
		}

		// Set a new least recent if one is found
		if((*line).LRU_counter < (*leastRecent).LRU_counter)
			leastRecent = line;
	}

	//Otherwise replace the least recently used and return an eviction
	*leastRecent = newLine;
	summary.cacheBlock = leastRecent;
	summary.results[0] = eviction;

	return summary;
}

cache_summary modify(int tag, int set) {

	// Record the result of a load and a store
	cache_summary sum1 = load(tag, set);
	cache_summary sum2 = load(tag, set);

	// Combine both results into an array
	cache_summary sums = {.cacheBlock = sum2.cacheBlock, .results = {sum1.results[0],sum2.results[0]}};

	return sums;
}

char* substr(char* string, int start, int end) {
	int subSize = end - start;

	char* substring = malloc(sizeof(char) * (subSize + 1));
	
	strncpy( substring, &string[start], subSize + 1);

	return substring;
}

void printVerbose(cache_summary summary, int address, int size) {
	char result[32] = "";

	for(int i = 0; i < 2; i++) {
		switch (summary.results[i])
		{
			case hit:
				strcat(result, "Hit");
				break;
			case miss:
				strcat(result, "Miss");
				break;
			case eviction:
				strcat(result, "Miss Eviction");
				break;
			default:
				break;
		}

		strcat(result, " ");
	}

	printf("%c %i,%i\t%s\n", summary.type, address, size, result);
}
