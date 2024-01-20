#include <stdio.h>
#include "printing.h"
#include "map.h"

void _print_ar(ar_t const *ar, void const * orig_adr, memmap_t const *map, u64 cache_cnt, void *f) {
	char cached = ' ';
	for (u32 i=0; i<cache_cnt; i++)
			if (map->cache[i] == orig_adr)
				cached = '*';

	if (!ar_is_free(*ar)) {
		char const c = ar_is_raw(*ar) ? '5' : '2';
		char const g[2] = {ar_is_used(*ar)?'U':' ', ar_is_raw(*ar)?'R':' '};
		fprintf(f, "\e[90m# \e[1;3%cm%p\e[22;33m%c\e[3%cm[%s, elsize=%-3u, cap=%-5u, len=%-4u, ref=%-4u | blk=%lu+1 ]\e[39m\n",
				c, orig_adr, cached, c, g, ar->flags>>ESO, ar->cap, ar->len, ar->ref, arblocks(*ar));
	}
	else {
		fprintf(f, "\e[90m# \e[36m%p\e[1;33m%c\e[22;36m[  , elsize=%-3u, cap=%-5u, fref=%p | blk=%lu+1 ]\e[39m\n",
				orig_adr, cached, ar->flags>>ESO, ar->cap, ar->fref, arblocks(*ar));
	}
}

void _print_memmap(memmap_t const *map, u64 ar_cnt, void *f) {
	fprintf(f, "\e[90m#\e[39m map = %p(incl) - %p(excl)\n\e[90m#\e[39m map_size = %lu (max: %lu)\n\e[90m#\e[39m arr_cnt = %lu (counted %lu)\n",
		map->ptr, map->ptr+map->size, map->size, map->Msize, map->arr_cnt, ar_cnt);
}

void _print_acache(memmap_t const *map, u64 cache_cnt, void *f) {
	fprintf(f, "\e[90m#\e[39m acache(%lu):", cache_cnt);
	for (u32 i=0; i<cache_cnt; i++)
		printf(" %p,", map->cache[i]);
	printf("\b \n");
}