#include "rayalloc.h"
#include "array.h"
#include "config.h"
#include "util.h"
#include <stdio.h>
#define __USE_GNU // for mremap
#include <sys/mman.h>

thread_local void *_ray_map;
thread_local u64 _ray_map_size; // in bytes
thread_local ar_t *acache[ACACHE_SIZE]; // FIFO of most recent freed arrays

#define blocks(VAL) ((((u64)VAL) + 15) / 16)
#define arblocks(AR) blocks(((u64)((AR).cap)) * ((AR).flags>>16))

static void put_in_aacache(ar_t const *const ptr) {
	#if defined(DEBUG_ACAHE) || !defined(NDEBUG)
		if ((void*)ptr >= _ray_map+_ray_map_size || (void*)ptr < _ray_map)
			dprintf("put_in_acache: %p out of bounds\n", ptr);
		if (ptr->flags & AR_USED)
			dprintf("put_in_acache: %p is not free\n", ptr);
	#endif

	// push to FIFO and bring any NULLs to the front
	u32 n=0;
	for (u32 i=0; i<ACACHE_SIZE; i++) {
		if (acache[i]==NULL || (i==ACACHE_SIZE-1 && !n)) {
			n++;
			for (u32 j=i; j>=n; j--)
				acache[j] = acache[j-1];
		}
	}
	for (u32 i=0; i<n; i++)
		acache[i] = NULL;
	acache[n-1] = (ar_t*)ptr;

	#if defined(DEBUG_ACAHE) || !defined(NDEBUG)
		printf("acache:");
		for (u32 i=0; i<ACACHE_SIZE; i++) printf(" %p,", acache[i]);
		printf("\b \n");
	#endif
}

static ar_t *coalesce_fwd(ar_t *from) {
	printf("called to coalesce from %p\n", from);
	ar_t *next;
	while ((void*)(next = from+arblocks(*from)+1) < _ray_map+_ray_map_size
			&& !((next)->flags & AR_USED)) {
		#if defined(DEBUG_RAYALLOC) || !defined(NDEBUG)
			printf("coalesce_fwd: coalescing %p with %p\n", from, next);
		#endif
		*from = (ar_t){16<<16, ((from->flags>>16) * from->cap + (next->flags>>16) * next->cap) / 16 + 1, 0, 0};
		for (u32 i=0; i<ACACHE_SIZE; i++)
			if (acache[i] == (ar_t*)next)
				acache[i] = NULL;
	}
	return from;
}

void *rayalloc(u64 cap, u64 elsize, bool raw) {
	if (!elsize)
		return NULL;
	#if defined(DEBUG_RAYALLOC) || !defined(NDEBUG)
		printf("rayalloc: searching aacache for %lu blocks\n", blocks(cap*elsize));
	#endif
	for (u32 i=0; i<ACACHE_SIZE; i++) {
		if (!acache[i])
			continue;
		ar_t *ar = coalesce_fwd(acache[i]);
		u64 fbl = arblocks(*ar), abl = blocks(cap*elsize);
		if (abl > fbl)
			continue;
		// the free space is good, let's allocated it
		acache[i] = NULL;
		if (fbl-abl <= 1) { // slightly extend the new aray to fill free space
			#if defined(DEBUG_RAYALLOC) || !defined(NDEBUG)
				printf("rayalloc: extending array to fit free space\n");
			#endif
			cap += (fbl-abl)*16/elsize; // bug: may overflow cap
		}
		else { // if there is plenty of free space, we simply divide it
			#if defined(DEBUG_RAYALLOC) || !defined(NDEBUG)
				printf("rayalloc: cutting from %p\n", ar);
			#endif
			ar[1+abl] = (ar_t){16<<16, fbl-abl-1, 0, 0}; // bug: may overflow cap
			put_in_aacache(ar+abl+1);
		}
		*ar = (ar_t){AR_USED | (raw? AR_RAW:0) | elsize<<16, cap, 0, 0};
		return ar->data;
	}
	#if defined(DEBUG_RAYALLOC) || !defined(NDEBUG)
		dprintf("rayalloc: no suitable entry in aacache\n");
	#endif
	return NULL;
}

void *rayresize(void *ptr, u32 new_cap) {
	ar_t *ar = ((ar_t*)ptr)-1;
	if (ar->cap == new_cap)
		return ptr;
	#if defined(DEBUG_RAYRESIZE) || !defined(NDEBUG)
		if ((void*)ar >= _ray_map+_ray_map_size || (void*)ar < _ray_map)
			dprintf("rayresize: ptr out of bounds");
		if (!(ar->flags & AR_USED))
			dprintf("rayresize: ptr is free\n");
	#endif
	puts("TODO");
	return ptr;
}

void rayfree(void *ptr) {
	ar_t *ar = ((ar_t*)ptr)-1;
	#if defined(DEBUG_RAYFREE) || !defined(NDEBUG)
		if ((void*)ar >= _ray_map+_ray_map_size || (void*)ar < _ray_map)
			dprintf("rayfree: ptr out of bounds");
		if (!(ar->flags & AR_USED))
			dprintf("rayfree: ptr is already free\n");
	#endif

	*ar = (ar_t){16<<16, arblocks(*ar), 0, 0};
	coalesce_fwd(ar);
	put_in_aacache((ar_t*)ar);
}

ierr map_map(u64 size_hint, int add_mmap_flags) {
	if (_ray_map)
		return IERR_OK;
	_ray_map = mmap(NULL, size_hint?:PAGE_SIZE*64, PROT_READ|PROT_WRITE, add_mmap_flags|MAP_ANONYMOUS|((add_mmap_flags&MAP_SHARED || add_mmap_flags&MAP_SHARED_VALIDATE)?0:MAP_PRIVATE), -1, 0);
	if (_ray_map == MAP_FAILED) {
		#if defined(DEBUG_MAP_MAP) || !defined(NDEBUG)
			perror(DBG_INTRO "map_map");
		#endif
		_ray_map = NULL;
		return IERR_CANTMAP;
	}
	_ray_map_size = size_hint?:PAGE_SIZE*64;
	*(ar_t*)_ray_map = (ar_t){16<<16, _ray_map_size/16-1, 0, 0}; // bug: may overflow cap
	put_in_aacache(_ray_map);
	return IERR_OK;
}

ierr map_resize(u64 new_size) {
	if (!new_size)
		return map_unmap();
	void *new_map = mremap(_ray_map, _ray_map_size, new_size, 0);
	if (new_map == MAP_FAILED) {
		#if defined(DEBUG_MAP_RESIE) || !defined(NDEBUG)
			perror(DBG_INTRO "map_resize");
		#endif
		return IERR_CANTREMAP;
	}
	_ray_map = new_map;
	_ray_map_size = new_size;
	return IERR_OK;
}

ierr map_unmap(void) {
	if (munmap(_ray_map, _ray_map_size)) {
		#if defined(DEBUG_MAP_UNMAP) || !defined(NDEBUG)
			perror(DBG_INTRO "map_unmap");
		#endif
		return IERR_CANTUNMAP;
	}
	_ray_map_size = 0;
	_ray_map = NULL;
	return IERR_OK;
}

void map_dbg_print(void) {
	if (!_ray_map) {
		puts("_ray_map = NULL");
		return;
	}
	printf("_ray_map = %p(incl) - %p(excl)\n_ray_map_size = %lu\n", _ray_map, _ray_map+_ray_map_size, _ray_map_size);
	ar_t *ar= _ray_map;
	while ((void*)ar < _ray_map+_ray_map_size && (void*)ar >= _ray_map) {
		char color, *flags;
		if (ar->flags & AR_USED) {
			if (ar->flags & AR_RAW)
				color = '5', flags = "UR";
			else
				color = '2', flags = "U";
		}
		else if (ar->flags & AR_RAW)
			color = '1', flags = " R";
		else
			color = '6', flags = "";
		printf("\e[3%cm%p [\e[1m%-2s\e[22m, elsize=%-3u, cap=%-5u, len=%-4u, ref=%-4u| blk=%lu+1 ]\e[39;49m\n",
			color, ar, flags, ar->flags>>16, ar->cap, ar->len, ar->ref, arblocks(*ar));
		if (ar_is_zeros(*ar)) {
			u32 count=0;
			while ((void*)(++ar) < _ray_map+_ray_map_size && (void*)ar >= _ray_map && ar_is_zeros(*ar))
				count++;
			printf("\t\e[36m...%u more all-zeros blocks...\e[39m\n", count);
		}
		else
			ar += arblocks(*ar)+1;
	}
}
