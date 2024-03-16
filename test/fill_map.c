#include <assert.h>
#include "../build/include/allocator.h"
#include "../src/log.h"

int main() {
	#define sizeS ((1<<13) - 8)
	#define sizeL ((1<<23) - 8)
	
	char *mapS = memalloc(sizeS, ALLOC_MAP);
	LOGV("mapS at %p", mapS);
	for (u64 i=0; i<sizeS; i++) {
		mapS[i] = 'S';
	}

	char *mapL = memalloc(sizeL, ALLOC_MAP);
	LOGV("mapL at %p", mapL);
	for (u64 i=0; i<sizeL; i++) {
		mapL[i] = 'L';
	}

	for (u64 i=0; i<sizeS; i++) {
		assert(mapS[i] == 'S');
	}
	for (u64 i=0; i<sizeL; i++) {
		assert(mapL[i] == 'L');
	}

	memfree(mapS, FREE_MAP);
	memfree(mapL, FREE_MAP);
	return 0;
}