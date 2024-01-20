#ifndef _PRINTING_H_
#define _PRINTING_H_

#include "map.h"

void _print_ar(ar_t const *ar, void const * orig_adr, memmap_t const *map, u64 cache_cnt, void *f);
void _print_memmap(memmap_t const *map, u64 ar_cnt, void *f);
void _print_acache(memmap_t const *map, u64 cache_cnt, void *f);

#endif /* _PRINTING_H_ */
