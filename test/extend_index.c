#include <assert.h>
#include <string.h>
#include "../build/include/allocator.h"

int main() {
	#define len (1<<13)
	char const msg[] = "Hello World!";
	char **ptrs = memalloc(len * sizeof(char*), ALLOC_MAP);
	for (u32 i=0; i<len; i++) {
		ptrs[i] = memalloc(sizeof(msg), 0);
		strncpy(ptrs[i], msg, sizeof(msg));
	}
	for (u32 i=0; i<len; i++) {
		assert(!strcmp(ptrs[i], msg));
		memfree(ptrs[i], FREE_BUCKET);
	}
	memfree(ptrs, FREE_MAP);
	return 0;
}