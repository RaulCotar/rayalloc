#ifndef _INCLUDE_ALLOCATOR_H_
#define _INCLUDE_ALLOCATOR_H_
#include "typedefs.h"

#define ALLOC_DIRTY 1 // hint to not zero the allocated bytes
#define ALLOC_ARRAY 2 // force the allocation of an array
#define ALLOC_MAP   6 // force the allocation of a map

[[gnu::nothrow]]
void memfree(void *ptr, u32 const flags);

#define FREE_HINT_BUCKET 2 // hint to search buckets before arrays
#define FREE_MAP 3 // directly free a map, not just a hint

[[gnu::nothrow, gnu::warn_unused_result, gnu::malloc, malloc(memfree, 1), gnu::alloc_size(1)]]
void *memalloc(u64 const size, u32 const flags);

#endif /* _INCLUDE_ALLOCATOR_H_ */
