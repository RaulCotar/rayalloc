#include <stdlib.h>
#include <sys/mman.h>
#include "allocator.h"
#include "log.h"

[[gnu::nothrow]]
static bool try_free_bucket(void *ptr, u32 const flags) {
	(void) flags;
	if ((u64)ptr % BUCKET_ELSIZE)
		return false;
	for (struct bucket *buk=INDEX_BUKBEG; buk >= INDEX_BUKEND; buk--) {
		void *const map = TP_ADRS(buk->map);
		if (ptr < map || ptr >= map + (1<<BUCKET_MAPSIZE))
			continue;
		u64 const i = (ptr - map) / BUCKET_ELSIZE;
		buk->bits[i/8] &= ~(1 << i%8);
		buk->map = (void*)((u64)buk->map & !(TP_BUK_FULL));
		return true;
	}
	return false;
}

[[gnu::nothrow]]
static bool try_free_array(void *ptr, u32 flags) {
	(void)ptr, (void)flags;
	DLOGW("array deallocator not implemented");
	return false;
}

[[gnu::nothrow]]
void memfree(void *ptr, u32 const flags) {
	if (!ptr)
		return;
	if ((flags & 3) == FREE_HINT_BUCKET) {
		if (try_free_bucket(ptr, flags) || try_free_array(ptr, flags))
			return;
	}
	else if ((flags & 3) != FREE_MAP) {
		if (try_free_array(ptr, flags) || try_free_bucket(ptr, flags))
			return;
	}
	if (munmap(ptr-8, *(u64*)(ptr-8))) {
		DLOGE("munmap failed");
		exit(-1);
	}
}
