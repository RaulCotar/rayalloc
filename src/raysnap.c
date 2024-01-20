#include <sys/mman.h>
#include <stdio.h>

#include "raysnap.h"
#include "array.h"
#include "map.h"

raysnap_t *raysnap_snapshot(enum raysnap_which const which) {
	memmap_t const *const map = which ? &sh_map : &tl_map;
	if (!map)
		return NULL;

	u64 const alloc_size = (sizeof(raysnap_t) + BS * (map->arr_cnt+1/*margin*/) + PAGE_SIZE - 1) / PAGE_SIZE;
	raysnap_t *snap = mmap(NULL, alloc_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if (snap == MAP_FAILED)
		return NULL;
	*snap = (raysnap_t) {alloc_size*PAGE_SIZE, ACACHE_SIZE, 0, *map};

	ar_any *ar= map->ptr;
	while ((void*)ar < map->ptr + map->size && (void*)ar >= map->ptr
	 && (void*)(snap->arrays+snap->ar_cnt) < ((void*)snap)+snap->byte_size) {
		snap->arrays[snap->ar_cnt++] = *ar;
		ar += arblocks(*ar) + 1;
	}

	return snap;
}

void raysnap_print(raysnap_t const *s, void *f) {
	fputs("\e[90m", f);
	for (u32 i=0; i<100-12; i++)
		fputs("=", f);
	fprintf(f, "%6lu bytes\e[39m\n", s->byte_size);

	fprintf(f, "\e[90m#\e[39m map = %p(incl) - %p(excl)\n\e[90m#\e[39m map_size = %lu (max: %lu)\n\e[90m#\e[39m arr_cnt = %lu (counted %lu)\n",
		s->map.ptr, s->map.ptr+s->map.size, s->map.size, s->map.Msize, s->map.arr_cnt, s->ar_cnt);

	for (u64 i=0, offs=0; i<s->ar_cnt; i++, offs += BS + arblocks(s->arrays[i]) * BS) {
		char cached = ' ';
		for (u32 i=0; i<s->cache_cnt; i++)
			if (s->map.cache[i] == s->map.ptr+offs)
				cached = '*';
		if (ar_is_free(s->arrays[i])) {
			fprintf(f, "\e[90m# \e[36m%p\e[1;33m%c\e[22;36m[\e[1mF%c\e[22m, elsize=%-3u, cap=%-5u, nextf=%p | tblk=%lu ]\e[39m\n",
				s->map.ptr+offs, cached, ar_is_lnf(s->arrays[i])?'f':' ', s->arrays[i].flags>>ESO, s->arrays[i].cap, ((ar_f*)s->arrays+i)->nextf, arblocks(s->arrays[i]));
		}
		else if (ar_is_raw(s->arrays[i])) {
			fprintf(f, "\e[90m# \e[35m%p\e[1;33m%c\e[22;35m[\e[1mR%c\e[22m, elsize=%-3u, cap=%-5u | tblk=%lu ]\e[39m\n",
				s->map.ptr+offs, cached, ar_is_lnf(s->arrays[i])?'f':' ', s->arrays[i].flags>>ESO, s->arrays[i].cap, arblocks(s->arrays[i]));
		}
		else {
			char kind = 'T', c = '1';
			if (ar_is_man(s->arrays[i])) {
				if (ar_is_circ(s->arrays[i]))
					kind = 'C', c = '2';
				else
					kind = 'M', c = '2';
			}
			fprintf(f, "\e[90m# \e[3%cm%p\e[1;33m%c\e[22;3%cm[\e[1m%c%c\e[22m, elsize=%-3u, cap=%-5u, len=%-4u, ref=%-4u | tblk=%lu ]\e[39m\n",
				c, s->map.ptr+offs, cached, c, kind, ar_is_lnf(s->arrays[i])?'f':' ', s->arrays[i].flags>>ESO, s->arrays[i].cap, ((ar_m*)s->arrays+i)->len, ((ar_m*)s->arrays+i)->ref, arblocks(s->arrays[i]));

		}
	}

	fprintf(f, "\e[90m#\e[39m acache(%lu):", s->cache_cnt);
	for (u32 i=0; i<s->cache_cnt; i++)
		printf(" \e[%2sm%p\e[39m,", s->map.cache[i]?"39":"90", s->map.cache[i]);
	printf("\b \n");

	fputs("\e[90m", f);
	for (u32 i=0; i<100; i++)
		fputs("=", f);
	fputs("\e[39m\n", f);
}

void raysnap_delete(raysnap_t *s) {
	munmap(s, s->byte_size);
}

void raysnap_quickie(enum raysnap_which const which, void *file) {
	raysnap_t *const snap = raysnap_snapshot(which);
	raysnap_print(snap, file);
	raysnap_delete(snap);
}
