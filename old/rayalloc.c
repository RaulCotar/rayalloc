#include "rayalloc.h"
#include "util.h"
#include "config.h"
#include "acache.h"
#include <stdio.h>
#define __USE_GNU // for mremap
#include <sys/mman.h>

thread_local void *_ray_map;
thread_local u64 _ray_map_size; // in bytes
thread_local u64 _ray_arr_cnt; // total nr of arrays (including free)

void *rayalloc(u64 cap, u64 elsize, bool raw) {
	if (!elsize)
		return NULL;
	#if defined(DEBUG_RAYALLOC) || !defined(NDEBUG)
		printf("rayalloc: searching aacache for %lu blocks\n", blocks(cap*elsize));
	#endif
	for (u32 i=0; i<ACACHE_SIZE && acache[i]; i++) {
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
			acache_append(ar+abl+1, true);
			_ray_arr_cnt++;
		}
		*ar = (ar_t){AR_USED | (raw? AR_RAW:0) | elsize<<16, cap, 0, 0};
		return ar->data;
	}
	#if defined(DEBUG_RAYALLOC) || !defined(NDEBUG)
		printf(DBG_INTRO "rayalloc: no suitable entry in aacache\n");
	#endif
	raymap_resize((_ray_map_size+sizeof(ar_t)+cap*elsize+PAGE_SIZE-1)/PAGE_SIZE);
	return rayalloc(cap, elsize, raw);
}

void *rayresize(void *ptr, u32 new_cap) {
	ar_t *ar = ((ar_t*)ptr)-1;
	if (ar->cap == new_cap)
		return ptr;
	#if defined(DEBUG_RAYRESIZE) || !defined(NDEBUG)
		if ((void*)ar >= _ray_map+_ray_map_size || (void*)ar < _ray_map)
			printf(DBG_INTRO "rayresize: ptr out of bounds");
		if (!(ar->flags & AR_USED))
			printf(DBG_INTRO "rayresize: ptr is free\n");
	#endif
	puts("TODO");
	return ptr;
}

void rayfree(void *ptr) {
	ar_t *ar = ((ar_t*)ptr)-1;
	#if defined(DEBUG_RAYFREE) || !defined(NDEBUG)
		if ((void*)ar >= _ray_map+_ray_map_size || (void*)ar < _ray_map)
			printf(DBG_INTRO "rayfree: ptr out of bounds");
		if (!(ar->flags & AR_USED))
			printf(DBG_INTRO "rayfree: ptr is already free\n");
	#endif

	*ar = (ar_t){16<<16, arblocks(*ar), 0, 0};
	coalesce_fwd(ar);
	acache_append((ar_t*)ar, true);
}

ierr raymap_map(u64 size_hint, int add_mmap_flags) {
	if (_ray_isinit) {
		#if defined(DEBUG_MAP_MAP) || !defined(NDEBUG)
			printf(DBG_INTRO "raymap_map: rayalloc is already initialized and mapped at %p\n", _ray_map);
		#endif
		return IERR_ALREADY;
	}
	_ray_map = mmap(NULL, size_hint?:PAGE_SIZE*64, PROT_READ|PROT_WRITE, add_mmap_flags|MAP_ANONYMOUS|((add_mmap_flags&MAP_SHARED || add_mmap_flags&MAP_SHARED_VALIDATE)?0:MAP_PRIVATE), -1, 0);
	if (_ray_map == MAP_FAILED) {
		#if defined(DEBUG_MAP_MAP) || !defined(NDEBUG)
			perror(DBG_INTRO "raymap_map");
		#endif
		_ray_map = NULL;
		return IERR_CANTMAP;
	}
	_ray_map_size = size_hint?:PAGE_SIZE*64;
	*(ar_t*)_ray_map = (ar_t){16<<16, _ray_map_size/16-1, 0, 0}; // bug: may overflow cap
	acache_append(_ray_map, true);
	_ray_arr_cnt = 1;
	_ray_isinit = true;
	return IERR_OK;
}

ierr raymap_resize(u64 new_size) {
	if (!new_size) {
		if (_ray_isinit)
			return raymap_unmap();
		else {
			#if defined(DEBUG_MAP_MAP) || !defined(NDEBUG)
				printf(DBG_INTRO "raymap_resize(0): rayalloc is was not initilized");
			#endif
			return IERR_OK;
		}
	}
	if (!_ray_isinit) {
		#if defined(DEBUG_MAP_MAP) || !defined(NDEBUG)
			printf(DBG_INTRO "raymap_resize: rayalloc is was not initilized, calling raymap_map now");
		#endif
		return raymap_map(new_size, 0);
	}
	void *new_map = mremap(_ray_map, _ray_map_size, new_size, 0);
	if (new_map == MAP_FAILED) {
		#if defined(DEBUG_MAP_RESIE) || !defined(NDEBUG)
			perror(DBG_INTRO "raymap_resize");
		#endif
		return IERR_CANTREMAP;
	}
	_ray_map = new_map;
	ar_t *newf = _ray_map + _ray_map_size;
	*newf = (ar_t){16<<16, (new_size-_ray_map_size)/16-1, 0, 0};
	_ray_map_size = new_size;
	acache_append(newf, false);
	return IERR_OK;
}

ierr raymap_unmap(void) {
	if (!_ray_isinit) {
		#if defined(DEBUG_MAP_UNMAP) || !defined(NDEBUG)
			perror(DBG_INTRO "raymap_unmap: rayalloc was uninitialized");
		#endif
		return IERR_ALREADY;
	}
	if (munmap(_ray_map, _ray_map_size)) {
		#if defined(DEBUG_MAP_UNMAP) || !defined(NDEBUG)
			perror(DBG_INTRO "raymap_unmap");
		#endif
		return IERR_CANTUNMAP;
	}
	_ray_map_size = 0;
	_ray_map = NULL;
	_ray_arr_cnt = 0;
	_ray_isinit = false;
	return IERR_OK;
}

void map_dbg_print(void) {
	if (!_ray_map) {
		puts("_ray_map = NULL");
		return;
	}
	printf("_ray_map = %p(incl) - %p(excl)\n_ray_map_size = %lu\n_ray_arr_cnt = %lu\n",
		_ray_map, _ray_map+_ray_map_size, _ray_map_size, _ray_arr_cnt);
	ar_t *ar= _ray_map;
	while ((void*)ar < _ray_map+_ray_map_size && (void*)ar >= _ray_map) {
		char color, *flags, cached=' ';
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
		for (u32 j=0; j<ACACHE_SIZE; j++)
			if (ar == acache[j])
				cached = '*';
		printf("\e[3%cm%p\e[33m%c\e[3%cm[\e[1m%-2s\e[22m, elsize=%-3u, cap=%-5u, len=%-4u, ref=%-4u| blk=%lu+1 ]\e[39;49m\n",
			color, ar, cached, color, flags, ar->flags>>16, ar->cap, ar->len, ar->ref, arblocks(*ar));
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
