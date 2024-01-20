#include <sys/mman.h>
#include <errno.h>
#include "map.h"
#include "config.h"

ierr map_map(memmap_t *const restrict map, u64 size, u64 Msize, int mf) {
	if (map->ptr) {
		dlogw("map already exists at %p", map->ptr);
		return IERR_ALREADY;
	}
	size = (size - 1 + PAGE_SIZE) / PAGE_SIZE * PAGE_SIZE;
	if (size > Msize) {
		dloge("size=%lu > Msize=%lu", size, Msize);
		return IERR_BADINPUT;
	}
	map->ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, mf|MAP_ANONYMOUS|((mf&MAP_SHARED || mf&MAP_SHARED_VALIDATE)?0:MAP_PRIVATE), -1, 0);
	if (map->ptr == MAP_FAILED) {
		dloge("mmap failed with errno %d", errno);
		map->ptr = NULL;
		return IERR_CANTMAP;
	}
	*map = (memmap_t) {map->ptr, size, Msize, 1, {map->ptr}};
	u16 const elsize = size / UINT32_MAX + 1;
	*(ar_f*)map->ptr = (ar_f) {elsize<<ESO, (size-1+elsize)/elsize, NULL};
	return IERR_OK;
}

ierr map_unmap(memmap_t *const restrict map) {
	if (!map->ptr) {
		dlogw("map already NULL");
		return IERR_ALREADY;
	}
	if (munmap(map->ptr, map->size)) {
		dloge("munmap failed with errno %d", errno);
		return IERR_CANTUNMAP;
	}
	*map = (memmap_t) {NULL, 0, 0, 0, {}};
	for (i32 i=0; i<ACACHE_SIZE; i++)
		map->cache[i] = NULL;
	return IERR_OK;
}

void cache_append(memmap_t *const restrict map, ar_f const *ar) {
	ifDBG(
		if ((void*)ar >= map->ptr+map->size || (void*)ar < map->ptr)
			dlogw("%p out of bounds!", ar); // note: NULL is always out of bounds
		else if (ar_is_used(*ar))
			dlogw("%p is not free!", ar);
	)

	u32 i=0;
	while (i < ACACHE_SIZE && map->cache[i++]);
	while (--i)
		map->cache[i] = map->cache[i-1];
	map->cache[0] = (ar_f*)ar;
}

void cache_remove(memmap_t *const restrict map, ar_f const *ar) {
	ifDBG(
		if (!ar)
			dlogw("array is NULL!");
	)

	u32 i=0;
	while (i < ACACHE_SIZE)
		if (map->cache[i++] == ar) {
			while (i++ < ACACHE_SIZE)
				map->cache[i-1] = map->cache[i];
			map->cache[ACACHE_SIZE-1] = NULL;
			return;
		}
}

void cache_replace(memmap_t *const restrict map, ar_f const *old, ar_f const *new) {
	ifDBG(
		if ((void*)new >= map->ptr+map->size || (void*)new < map->ptr)
			dlogw("%p (new) out of bounds!", new);
		else if (!new)
			dlogw("new is NULL!");
		else if (ar_is_used(*new))
			dlogw("%p (new) is not free!", new);
		if (!old)
			dlogw("old is NULL!");
	)

	u32 i=0;
	while (i < ACACHE_SIZE)
		if (map->cache[i++] == old) {
			while (--i)
				map->cache[i] = map->cache[i-1];
			map->cache[0] = (ar_f*)new;
			return;
		}
	dlogm("%p (old) was not found, %p (new) was not inserted", old ,new);
}