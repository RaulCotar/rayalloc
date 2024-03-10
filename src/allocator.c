#include <stdlib.h>
#include <sys/mman.h>
#include "allocator.h"
#include "log.h"
#include "util.h"

struct index *index = NULL;

#if INDEX_CONSTURCTOR == true
[[gnu::constructor]]
#endif
[[gnu::nothrow]]
void _index_constructor(void) {
	index = mmap(NULL, PAGE_SIZE, PROT_RW, MAP_ANONPRIV, -1, 0);
	if (index == MAP_FAILED) {
		DLOGE("mmap failed");
		exit(-1);
	}
	index->map_size = PAGE_SIZE;
}
