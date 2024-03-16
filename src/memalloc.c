#include <stdlib.h>
#include "allocator.h"
#include "include/config.h"
#include "mman.h"
#include "util.h"
#include "log.h"

[[gnu::nothrow]]
void remap_index(u64 new_size) {
	new_size = roundToMult(new_size, PAGE_SIZE);
	DLOGV("remapping heap index from %p+%lu to ?+%lu", heap_index, heap_index->map_size, new_size);
	heap_index = mremap(heap_index, heap_index->map_size, new_size, MREMAP_MAYMOVE);
	if (heap_index == MAP_FAILED) {
		DLOGE("mremap failed");
		exit(-1);
	}
	u64 const old_size = heap_index->map_size;
	heap_index->map_size = new_size;
	for (u64 i=heap_index->buk_cnt-1; i<-1UL; i--) {
		INDEX_BUKLO[i] = ((struct bucket*)
			(((void *)heap_index) + old_size - heap_index->buk_cnt * sizeof(struct bucket)))[i];
	}
}

[[gnu::nothrow, gnu::warn_unused_result, gnu::malloc, malloc(memfree, 1)]]
void *alloc_bucket(u32 const flags) {
	struct bucket *const buckets = INDEX_BUKLO;
	for (u64 i=heap_index->buk_cnt-1; i<-1U; i--) {
		if (((u64)(buckets[i].map) & 3) == TP_BUK_FULL) continue;
		for (u64 byte=0; byte<(1<<BUCKET_MAPSIZE)/BUCKET_ELSIZE/8; byte++) {
			int const bit = __builtin_ffs((u8)~(buckets[i].slots[byte])) - 1;
			if (bit < 0) continue;
			buckets[i].slots[byte] |= 1 << bit;
			void *const p = TP_ADRS(buckets[i].map) + BUCKET_ELSIZE * (bit + 8 * byte);
			DLOGV("found slot %lu-%lu-%d (%p)", heap_index->buk_cnt-1-i, byte, bit, p);
			if ((flags & ALLOC_DIRTY) == 0 && false) {
				for (u64 b=0; b<BUCKET_ELSIZE; b++)
					((u8*)p)[b]=0;
			}
			return p;
		}
		DLOGV("marking bucket %lu full", i);
		buckets[i].map = (void*)(((u64)buckets[i].map & (u64)~3) | TP_BUK_FULL);
	}
	DLOGV("no bucket slot available, mapping new bucket...");
	if ((void*)INDEX_BUKLO - (void*)INDEX_RANLO < (i64)sizeof(struct bucket))
		remap_index(heap_index->map_size*4/3);
	heap_index->buk_cnt++;
	INDEX_BUKLO[0].map = mmap(NULL, 1<<BUCKET_MAPSIZE, PROT_RW, MAP_ANONPRIV
		#if 1<<BUCKET_MAPSIZE > PAGE_SIZE
			| MAP_HUGETLB | (BUCKET_MAPSIZE << MAP_HUGE_SHIFT);
		#endif
		, -1, 0);
	if (INDEX_BUKLO[0].map == MAP_FAILED) {
		DLOGE("mmap failed");
		exit(-1);
	}
	INDEX_BUKLO[0].slots[0] |= 1;
	return TP_ADRS(INDEX_BUKLO[0].map);
}

#include <errno.h>
[[gnu::nothrow, gnu::warn_unused_result, gnu::malloc, malloc(memfree, 1)]]
void *alloc_map(u64 const size, u32 const flags) {
	DLOGV("allocating from maps...");
	int map_type = MAP_ANONPRIV;
	u64 map_size = roundToMult(size+8, PAGE_SIZE);
	if (flags & ALLOC_DIRTY) {
		map_type |= MAP_UNINITIALIZED;
	}
	void *const p = mmap(NULL, map_size, PROT_RW, map_type, -1, 0);
	if (p == MAP_FAILED) {
		DLOGE("mmap failed: %d", errno);
		exit(-1);
	}
	*(u64*)p = map_size - 8;
	return p+8;
}

[[gnu::nothrow, gnu::warn_unused_result, gnu::malloc, malloc(memfree, 1)]]
void *alloc_range(u64 const size, u32 const flags) {
	(void) size, (void) flags;
	DLOGE("not implemented");
	exit(-1);
}

[[gnu::nothrow, gnu::warn_unused_result, gnu::malloc, malloc(memfree, 1), gnu::alloc_size(1)]]
void *memalloc(u64 const size, u32 const flags) {
	if (size == 0)
		return heap_index;
	if ((size >= PAGE_SIZE*2 && (flags & 0b011)!=ALLOC_RANGE) || (flags & 0b011)==ALLOC_MAP) {
		return alloc_map(size, flags);
	}
	if (size <= BUCKET_ELSIZE && (flags & 0b011)==0) {
		return alloc_bucket(flags);
	}
	return alloc_range(size, flags);
}
