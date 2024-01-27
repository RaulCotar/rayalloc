#include <stdarg.h>
#include <sys/mman.h>
#include "rayalloc.h"
#include "api/rayalloc.h"
#include "array.h"
#include "config.h"
#include "map.h"
#include "util.h"

// LSB is used to indicate that the array is in acache
ar_f *find_free(memmap_t const *map, u64 const size) {
	u64 bests = -1;
	ar_f *besta = NULL;
	for (i32 i=0; i<ACACHE_SIZE && map->cache[i]; i++) {
		u64 const s = ar_Bsize(*map->cache[i]);
		if (s >= size) {
			if (map->ff == rayFF_CFMF)
				return map->cache[i];
			else if (s < bests)
				besta = map->cache[i], bests = s;
		}
	}
	if (besta)
		return besta;
	for (ar_f *p=map->f1; (void*)p<map->ptr+map->size; p=p->nextf) {
		u64 const s = ar_Bsize(*p);
		if (s >= size) {
			if (map->ff==rayFF_CFMF || map->ff==rayFF_CSMF)
				return p;
			else if (s < bests)
				besta = p, bests = s;
		}
	}
	if (besta)
		return besta;
	return NULL;
}

// discombobulate
void *rayalloc(u32 capacity, u16 elem_size, u32 flags) {
	memmap_t *const map = flags&RA_AR_SHARED ? &sh_map : &tl_map;
	ar_f *const f = find_free(map, (u64)capacity * elem_size);
	if (!f) {
		dloge("remapping not implemented yet");
		return NULL;
	}
	u64 const fb = arblocks(*f);
	u64 ab = blocks(capacity*elem_size)+1;
	if (fb > ab+2) { // we need to split
		u16 const es = ar_calc_good_es((fb-ab-1));
		ar_f *nf = (ar_f*)f - ab + fb;
		*nf = (ar_f) {es<<ESO, (fb-ab-1)*BS/es, .nextf=f->nextf};
		*(nf+fb-ab-1) = (ar_f) {nf->flags|AR_K_FOOT, nf->cap, .prevf=ar_get_foot(f)->prevf};
		cache_replace(map, f, nf);
		if (map->f1 == f)
			map->f1 = nf;
		if (f->nextf)
			ar_get_foot(f->nextf)->prevf = nf;
		if (ar_get_foot(f)->prevf)
			ar_get_foot(f)->prevf->nextf = nf;
	}
	else {
		ab = fb;
		cache_remove(map, f);
		if (map->f1 == f)
			map->f1 = f->nextf;
		if (f->nextf)
			ar_get_foot(f->nextf)->prevf = ar_get_foot(f)->prevf;
		if (ar_get_foot(f)->prevf)
			ar_get_foot(f)->prevf->nextf = f->nextf;
	}
	*((ar_m*)f) = (ar_m) {
		((flags|AR_K_MAN)&AR_M_KIND)
			| (f->flags&AR_M_LNFREE)
			| (flags&AR_M_CIRC)
			| elem_size<<ESO,
		 (ab-1)*BS/elem_size, 0, 0
	};
	if (!(flags & RA_AR_DIRTY))
		for (u32 i=1; i<=ab; i++)
			*(f+i) = (ar_f){};
	return f+1;
}

void rayfree(void *ptr) {
	if (!ptr)
		return;
	ar_f *ar = ((ar_f*)ptr)-1;
	memmap_t *const map = ar->flags&RA_AR_SHARED ? &sh_map : &tl_map;
	ifBC(if ((void*)ar < map->ptr || (void*)ptr >= map->ptr+map->size) {
			dloge("address %p outside map boundaries!", ptr);
			return;
	})

	ar->flags = ar->flags & ~AR_M_KIND;
	coalesce(ar);

	if (map->f1 > ar)
		map->f1 = ar;
	if (!cache_replace(map, ar, ar))
		cache_append(map, ar);
}

// coalesce low with its high neighbour
void _coalesce2(ar_f *low) {
	ar_f *const high = low-1+arblocks(*low);
	ar_f *const prevf = ar_get_foot(low)->prevf;

	u64 const newBsize = ar_Bsize(*low) + ar_Bsize(*high) + BS;
	u32 const elsize = ar_calc_good_es(newBsize);
	
	low->flags = (elsize << ESO) | (low->flags & ~AR_M_ELSIZE);
	low->cap = (newBsize - 1 + elsize) / elsize;
	low->nextf = high->nextf;
	*ar_get_foot(low) = (ar_f) {low->flags|AR_K_FOOT, low->cap, .prevf=prevf};
}

ar_f *coalesce(ar_f *from) {
	memmap_t *map;
	if (tl_map.ptr && (void*)from >= tl_map.ptr && (void*)from < tl_map.ptr + tl_map.size)
		map = &tl_map;
	else if (sh_map.ptr && (void*)from >= sh_map.ptr && (void*)from < sh_map.ptr + sh_map.size)
		map = &sh_map;
	else {
		dloge("%p outside known map bounds!", from);
		return from;
	}
	// fwd
	ar_f *next;
	while ((void*)(next = from+arblocks(*from)+1) < map->ptr+map->size && ar_is_free(*next)) {
		_coalesce2(from);
		cache_remove(map, next);
	}
	// bwd
	while (from->flags & AR_M_LNFREE) {
		ar_f *const low = from - arblocks(*(from-1));
		_coalesce2(low);
		cache_remove(map, from);
	}
	return from;
}

ierr ray_config(enum raycfg const option, ...) {
	ierr ret = IERR_BADINPUT;
	va_list va;
	va_start(va, option);
	switch (option) {
	case rayconf_nop:
		ret = IERR_OK;
		break;

	case rayconf_tl_map:
		ret = map_map(&tl_map, DEFAULT_MAP_SIZE, DEFAULT_MAP_MSIZE, MAP_PRIVATE);
		break;

	case rayconf_sh_map:
		ret = map_map(&sh_map, DEFAULT_MAP_SIZE, DEFAULT_MAP_MSIZE, MAP_SHARED_VALIDATE);
		break;

	case rayconf_tl_map_s:
		ret = map_map(&tl_map, va_arg(va, u64), DEFAULT_MAP_MSIZE, MAP_PRIVATE);
		break;
	
	case rayconf_sh_map_s:
		ret = map_map(&sh_map, va_arg(va, u64), DEFAULT_MAP_MSIZE, MAP_SHARED_VALIDATE);
		break;

	case rayconf_tl_unmap:
		ret = map_unmap(&tl_map);
		break;

	case rayconf_sh_unmap:
		ret = map_unmap(&sh_map);
		break;

	case rayconf_tl_set_Msize:
		tl_map.Msize = va_arg(va, typeof(tl_map.Msize));
		ret = IERR_OK;
		break;

	case rayconf_sh_set_Msize:
		sh_map.Msize = va_arg(va, typeof(sh_map.Msize));
		ret = IERR_OK;

	case rayconf_tl_get_Msize:
		*va_arg(va, typeof(tl_map.Msize)*) = tl_map.Msize;
		ret = IERR_OK;
		break;

	case rayconf_sh_get_Msize:
		*va_arg(va, typeof(sh_map.Msize)*) = sh_map.Msize;
		ret = IERR_OK; 
		break;

	case rayconf_tl_get_ptr:
		*va_arg(va, typeof(tl_map)*) = tl_map;
		ret = IERR_OK;
		break;
	
	case rayconf_sh_get_ptr:
		*va_arg(va, typeof(sh_map)*) = sh_map;
		ret = IERR_OK;
		break;
	
	case rayconf_tl_set_ff:
		tl_map.ff = va_arg(va, typeof(sh_map.ff));
		ret = IERR_OK;
		break;
	
	case rayconf_sh_set_ff:
		sh_map.ff = va_arg(va, typeof(sh_map.ff));
		ret = IERR_OK;
		break;

	// don't use default -> make sure all valid opts are handled
	}
	va_end(va);
	return ret;
}
