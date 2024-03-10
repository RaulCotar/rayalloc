#include <stdlib.h>
#include <sys/mman.h>
#include "allocator.h"
#include "util.h"
#include "log.h"

[[gnu::nothrow, gnu::warn_unused_result, gnu::malloc, malloc(memfree, 1)]]
void *alloc_bucket(u32 const flags) {
	for (struct bucket *buk=INDEX_BUKBEG; buk >= INDEX_BUKEND; buk--) {
		if ((u64)buk->map & TP_BUK_FULL)
			continue;
		for (u32 bit=0; bit < 1<<(BUCKET_MAPSIZE-3); bit++) {
			if (~buk->bits[bit/8] & 1<<bit) {
				buk->bits[bit/8] |= 1<<bit;
				void *p = TP_ADRS(buk->map) + bit * BUCKET_ELSIZE;
				if (~flags & ALLOC_DIRTY)
					for (u32 byte=0; byte<BUCKET_ELSIZE; byte++)
						((u8*)p)[byte] = 0;
				return p;
			}
		}
		buk->map = (void*)((u64)buk->map | TP_BUK_FULL);
	}
	if ((void*)INDEX_BUKEND <= (void*)INDEX_ARTOP) {
		DLOGE("index map full");
		return NULL; // TODO: extend the index map
	}
	index->buk_cnt++;
#if 1<<BUCKET_MAPSIZE == PAGE_SIZE
	INDEX_BUKEND->map = mmap(NULL, 1<<BUCKET_MAPSIZE, PROT_RW, MAP_ANONPRIV, -1, 0);
#else
	INDEX_BUKEND->map = mmap(NULL, 1<<BUCKET_MAPSIZE, PROT_RW, MAP_ANONPRIV | MAP_HUGETLB | (BUCKET_MAPSIZE << MAP_HUGE_SHIFT), -1, 0);
#endif
	if (INDEX_BUKEND->map == MAP_FAILED) {
		DLOGE("mmap failed");
		exit(-1);
	}
#if 1<<BUCKET_MAPSIZE != PAGE_SIZE
	INDEX_BUKEND->map = (void*)((u64)INDEX_BUKEND->map | TP_BUK_HUGE);
#endif
	INDEX_BUKEND->bits[0] |= 1;
	return TP_ADRS(INDEX_BUKEND->map);
}

[[gnu::nothrow, gnu::warn_unused_result, gnu::malloc, malloc(memfree, 1)]]
void *alloc_map(u64 const size, u32 const flags) {
	int map_type = MAP_ANONPRIV;
	u64 map_size = roundToMult(size+8, PAGE_SIZE);
	if (size >= HUGEPAGE_SIZE) {
		map_type |= MAP_HUGETLB | 21<<MAP_HUGE_SHIFT;
		map_size = roundToMult(size+8, HUGEPAGE_SIZE);
	}
	if (flags & ALLOC_DIRTY) {
		map_type |= MAP_UNINITIALIZED;
	}
	void *const p = mmap(NULL, map_size, PROT_RW, map_type, -1, 0);
	if (p == MAP_FAILED) {
		DLOGE("mmap failed");
		exit(-1);
	}
	*(u64*)p = map_size - 8;
	return p+8;
}

[[gnu::nothrow, gnu::warn_unused_result, gnu::malloc, malloc(memfree, 1), gnu::alloc_size(1)]]
void *memalloc(u64 const s, u32 const flags) {
	u64 const aloc_size = (s+7)/8;
	if ((aloc_size >= PAGE_SIZE*2 && (flags&ALLOC_FORCE_M)!=ALLOC_ARRAY) || (flags&ALLOC_FORCE_M)==ALLOC_MAP) {
		return alloc_map(aloc_size, flags);
	}
	if (aloc_size <= BUCKET_ELSIZE && (flags&ALLOC_FORCE_M)==0) {
		return alloc_bucket(flags);
	}
	DLOGE("array allocator not implemented");
	return NULL;
}
