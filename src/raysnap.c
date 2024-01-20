#include <sys/mman.h>
#include <stdio.h>

#include "raysnap.h"
#include "map.h"
#include "printing.h"

raysnap_t *raysnap_snapshot(enum raysnap_which const which) {
	memmap_t const *const map = which ? &sh_map : &tl_map;
	if (!map)
		return NULL;

	u64 const alloc_size = (sizeof(raysnap_t) + BS * (map->arr_cnt+1/*margin*/) + PAGE_SIZE - 1) / PAGE_SIZE;
	raysnap_t *snap = mmap(NULL, alloc_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if (snap == MAP_FAILED)
		return NULL;

	*snap = (raysnap_t) {alloc_size*PAGE_SIZE, ACACHE_SIZE, 0, *map};

	ar_t *ar= map->ptr;
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

	_print_memmap(&s->map, s->ar_cnt, f);
	for (u64 i=0, offs=0; i<s->ar_cnt; i++) {
		_print_ar(s->arrays+i, s->map.ptr+offs, &s->map, s->cache_cnt, f);
		offs += BS + arblocks(s->arrays[i]) * BS;
	}
	_print_acache(&s->map, s->cache_cnt, f);

	fputs("\e[90m", f);
	for (u32 i=0; i<100; i++)
		fputs("=", f);
	fputs("\e[39m\n", f);
}

void raysnap_csv(raysnap_t const *s, void *f) {
	(void)s;
	(void)f;
	dloge("not implemented");
}

void raysnap_quickie(int const which, void *file) {
	raysnap_t *const snap = raysnap_snapshot(which);
	raysnap_print(snap, file);
	raysnap_delete(snap);
}

void raysnap_delete(raysnap_t *s) {
	munmap(s, s->byte_size);
}
