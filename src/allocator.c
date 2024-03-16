#include <stdlib.h>
#include "mman.h"
#include "allocator.h"
#include "log.h"

struct index *heap_index = NULL;

#if INDEX_CONSTURCTOR == true
[[gnu::constructor(201), gnu::nothrow, gnu::leaf]]
static void _index_constructor(void) {
	heap_index = mmap(NULL, PAGE_SIZE, PROT_RW, MAP_ANONPRIV, -1, 0);
	if (heap_index == MAP_FAILED) {
		DLOGE("mmap failed");
		exit(-1);
	}
	heap_index->map_size = PAGE_SIZE;
	DLOGV("heap_index at %p+%lu.", (void*)heap_index, heap_index->map_size);
}
#endif
