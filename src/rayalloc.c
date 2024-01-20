#include <stdarg.h>
#include <sys/mman.h>
#include "rayalloc.h"
#include "map.h"
#include "util.h"

ar_t *coalesce_fwd(ar_t *from) {
	memmap_t *map;
	if (tl_map.ptr && (void*)from >= tl_map.ptr && (void*)from < tl_map.ptr + tl_map.size)
		map = &tl_map;
	else if (sh_map.ptr && (void*)from >= sh_map.ptr && (void*)from < sh_map.ptr + sh_map.size)
		map = &sh_map;
	else {
		dloge("%p outside known map bounds!", from);
		return from;
	}
	ar_t *next;
	ifDBG(u32 cc = 0;)
	while ((void*)(next = from+arblocks(*from)+1) < map->ptr+map->size && ar_is_free(*next)) {
		u64 const tbsize = ar_bsize(*from) + ar_bsize(*next) + BS;
		u32 const elsize = (tbsize + 1) * BS;
		*from = (ar_t) {elsize << ESO, tbsize / elsize, .fref = NULL};
		cache_remove(map, next);
		ifDBG(cc++;)
	}
	dlogm("coalesced 1+%u arrays", cc);
	return from;
}

ierr ray_config(enum raycfg option, ...) {
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

	case rayconf_tl_ptr:
		*va_arg(va, typeof(tl_map)*) = tl_map;
		ret = IERR_OK;
		break;
	
	case rayconf_sh_ptr:
		*va_arg(va, typeof(sh_map)*) = sh_map;
		ret = IERR_OK;
		break;

	ifDBG(
		case rayconf_tl_print:
			map_dbg_print(&tl_map);
			ret =IERR_OK;
			break;

		case rayconf_sh_print:
			map_dbg_print(&sh_map);
			ret =IERR_OK;
			break;
	)
	}
	va_end(va);
	return ret;
}
