#ifndef _RAYALLOC_H_
#define _RAYALLOC_H_

#include "util.h"

#ifdef _RAYALLOC_INTERNAL
	#include "array.h"
	#define blocks(VAL) ((((u64)VAL) + 15) / 16)
	#define arblocks(AR) blocks(((u64)((AR).cap)) * ((AR).flags>>16))

	extern thread_local void *_ray_map;
	extern thread_local u64 _ray_map_size; // in bytes
	extern thread_local u64 _ray_arr_cnt; // total nr of arrays
	extern thread_local bool _ray_isinit;

	ar_t *coalesce_fwd(ar_t *from);
#endif

ierr raymap_map(u64 size_hint, int add_mmap_flags);

void *rayalloc(u64 cap, u64 elsize, bool raw);

void rayfree(void *ptr);

ierr raymap_resize(u64 new_size);

ierr raymap_unmap(void);

// The orginal debug print. Almost identical printout as raysnap_quickie.
void map_dbg_print(void);

// go through the entire map and coalesce,defragment,etc. - slow AF
[[gnu::cold]] void raymap_optimize(void);

#endif /* _RAYALLOC_H_ */
