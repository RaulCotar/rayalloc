#ifndef _RAYALLOC_H_
#define _RAYALLOC_H_

#include "util.h"

#ifdef _RAYALLOC_INTERNAL
	extern thread_local void *_ray_map;
	extern thread_local u64 _ray_map_size; // in bytes
	extern thread_local u64 _ray_arr_cnt; // total nr of arrays

	#define blocks(VAL) ((((u64)VAL) + 15) / 16)
	#define arblocks(AR) blocks(((u64)((AR).cap)) * ((AR).flags>>16))
#endif

ierr raymap_map(u64 size_hint, int add_mmap_flags);

void *rayalloc(u64 cap, u64 elsize, bool raw);

void rayfree(void *ptr);

ierr raymap_resize(u64 new_size);

ierr raymap_unmap(void);

// go through the entire map and coalesce,defragment,etc. - slow AF
void raymap_optimize(void);

#endif /* _RAYALLOC_H_ */
