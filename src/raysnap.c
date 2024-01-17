#include <sys/mman.h>
#include <stdio.h>
#include "config.h"
#include "rayalloc.h"
#include "raysnap.h"
#include "acache.h"

raysnap_t *raysnap_snapshot(void) {
	if (!_ray_isinit)
		return NULL;
	u64 alloc_size = sizeof(raysnap_t) + sizeof(ar_t) * _ray_arr_cnt;
	alloc_size = (alloc_size + PAGE_SIZE - 1) / PAGE_SIZE;

	raysnap_t *snap = mmap(NULL, alloc_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if (snap == MAP_FAILED)
		return NULL;

	*snap = (raysnap_t){alloc_size*PAGE_SIZE, _ray_map, _ray_map_size, ACACHE_SIZE, {}, _ray_arr_cnt, 0, {}};

	for (u32 i=0; i<ACACHE_SIZE; i++)
		snap->acache[i] = acache[i];

	ar_t *ar= _ray_map;
	u64 i=0;
	while ((void*)ar < _ray_map+_ray_map_size && (void*)ar >= _ray_map) {
		snap->ar_count++;
		snap->arrays[i++] = *ar;
		ar += arblocks(*ar) + 1;
	}

	return snap;
}

void raysnap_print(raysnap_t const *s, void *f) {
	u64 offs = 0;
	char const *const sign = "\e[90m#\e[39m ";

	fputs("\e[90m", f);
	for (u32 i=0; i<100-12; i++)
		fputs("=", f);
	fprintf(f, "%6lu bytes\e[39m\n", s->byte_size);

	fprintf(f, "%s_ray_map = %p(incl) - %p(excl)\n%s_ray_map_size = %lu\n%s_ray_arr_cnt = %lu (counted %lu)\n",
		sign, s->map_ptr, s->map_ptr+s->map_size, sign, s->map_size, sign, s->map_arcnt, s->ar_count);
	
	for (u64 i=0; i<s->ar_count; offs+=sizeof(ar_t)+16*arblocks(s->arrays[i]), i++) {
		char color, *flags, cached = ' ';
		if (s->arrays[i].flags & AR_USED) {
			if (s->arrays[i].flags & AR_RAW)
				color = '5', flags = "UR";
			else
				color = '2', flags = "U";
		}
		else if (s->arrays[i].flags & AR_RAW)
			color = '1', flags = " R";
		else
			color = '6', flags = "";
		for (u32 j=0; j<ACACHE_SIZE; j++)
			if (s->map_ptr+offs == s->acache[j])
				cached = '*';

		fprintf(f, "%s\e[3%cm%p\e[33m%c\e[3%cm[\e[1m%-2s\e[22m, elsize=%-3u, cap=%-5u, len=%-4u, ref=%-4u| blk=%lu+1 ]\e[39;49m\n",
			sign, color, s->map_ptr+offs, cached, color, flags, s->arrays[i].flags>>16, s->arrays[i].cap, s->arrays[i].len, s->arrays[i].ref, arblocks(s->arrays[i]));
		
		u32 zcount = 0;
		while (ar_is_zeros(s->arrays[i]))
			zcount++, i++;
		if (zcount)
			fprintf(f, "%s\t\e[36m...%u more all-zeros blocks...\e[39m\n", sign, zcount);
	}

	fprintf(f, "%sacache(%lu):", sign, s->acache_size);
	for (u32 i=0; i<ACACHE_SIZE; i++)
		printf(" %p,", acache[i]);
	printf("\b \n");

	fputs("\e[90m", f);
	for (u32 i=0; i<100; i++)
		fputs("=", f);
	fputs("\e[39m\n", f);
}

void raysnap_csv(raysnap_t const *s, void *f) {
	fprintf(f, "%lu,%p,%lu,%lu,%lu,%lu",
		s->byte_size, s->map_ptr, s->map_size, s->acache_size, s->map_arcnt, s->ar_count);
	for (u64 i=0; i<s->ar_count; i++)
		fprintf(f, ",%u,%u,%u,%u", s->arrays[i].flags, s->arrays[i].cap,
			s->arrays[i].len, s->arrays[i].ref);
}

void raysnap_quickie(void *file) {
	raysnap_t *const snap = raysnap_snapshot();
	raysnap_print(snap, file);
	munmap(snap, snap->byte_size);
}
