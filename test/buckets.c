#include <assert.h>
#include <string.h>
#include "../build/include/allocator.h"

int main() {
	#define len 384
	char const msg[] = "Hello World!";
	char *ptrs[len];
	for (u32 i=0; i<len; i++) {
		ptrs[i] = memalloc(sizeof(msg), 0);
	}
	for (u32 i=0; i<len; i++) {
		strncpy(ptrs[i], msg, sizeof(msg));
	}
	for (u32 i=0; i<len; i++) {
		assert(!strcmp(ptrs[i], msg));
		memfree(ptrs[i], FREE_BUCKET);
	}
	return 0;
}