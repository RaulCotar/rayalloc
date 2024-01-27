#ifndef _MAP_H_
#define _MAP_H_

#include "api/rayalloc.h"
#include "array.h"

typedef struct memmap {
	void *ptr;
	u64 size;
	u64 Msize;
	u64 arr_cnt;
	ar_f *f1;
	enum ray_ff ff;
	ar_f *cache[ACACHE_SIZE];
} memmap_t;

extern thread_local memmap_t tl_map;
extern memmap_t sh_map;

// create a mem_map
ierr map_map(memmap_t *const restrict map, u64 size, u64 Msize, int mmap_flags);

// resize a mem_map
ierr map_remap(memmap_t *const restrict map, u64 size);

// unmap a mem_map
ierr map_unmap(memmap_t *const restrict map);

// add new, possibly evicting the oldest
void cache_append(memmap_t *const restrict map, ar_f const *ar);

// remove an entry from the acache (if present)
bool cache_remove(memmap_t *const restrict map, ar_f const *ar);

// combined remove and append
bool cache_replace(memmap_t *const restrict map, ar_f const *old, ar_f const *new);

#endif /* _MAP_H_ */
