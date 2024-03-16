#ifndef _INCLUDE_ALLOCATOR_H_
#define _INCLUDE_ALLOCATOR_H_
#include "typedefs.h" // IWYU pragma: export

#define ALLOC_RANGE 1 // force the allocation of a range
#define ALLOC_MAP   3 // force the allocation of a map
#define ALLOC_DIRTY 4 // hint to not zero the allocated bytes

[[gnu::nothrow]]
void memfree(void *ptr, u32 const flags);

#define FREE_BUCKET 2 // hint to search buckets before ranges
#define FREE_MAP 3 // directly free a map, not a hint
#define FREE_HOT 4 // hint to put the resulting free memory in the allocator cache

[[gnu::nothrow, gnu::warn_unused_result, gnu::malloc, malloc(memfree, 1), gnu::alloc_size(1)]]
void *memalloc(u64 const size, u32 const flags);

#ifndef NDEBUG
void _dbg_print_index(void);
#endif

#endif /* _INCLUDE_ALLOCATOR_H_ */
