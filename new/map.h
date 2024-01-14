#ifndef _MAP_H_
#define _MAP_H_

#define _GNU_SOURCE
#include <pthread.h>
#include "util.h"

struct map {
	union {
		void *data; // ptr to page-aligned mmaped memory
		u64 flags; // 12 bits since data is >=4KiB-aligned
	};
	u32 size; // (2^32 -1) * 4KiB pages = ~16TiB/map max
	u32 unused;
	pthread_rwlock_t lock; // ignored if MAP_LOCK is 0
};

#define M_MAP_LOCK 0x1 // does it have a lock (is it shared)
#define M_MAP_MMAP 0x2 // is it actually backed by mmaped memory
#define M_MAP_HUGE 0x4 // is it backed by hugepages
#define M_MAP_USED 0x8 // does it contain a non-free array
// 8 bits left (0xff0)

// create a new `struct map` backed by some mmaped memory
ierr map_create(u32 size, int flags);
#define MAP_SHARED 1
#define MAP_LAZY 2
#define MAP_HUGEPAGES 4
#define MAP_HOTNOW 16

// resize an existing `struct map` using mremap
ierr map_resize(struct map *map, u32 size);

// destruct an existing `struct map` and munmap its memory
ierr map_delete(struct map *map);

#endif /* _MAP_H_ */
