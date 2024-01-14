#ifndef _RAYALLOC_H_
#define _RAYALLOC_H_

#include "util.h"

extern thread_local void *_ray_map;
extern thread_local u64 _ray_map_size; // in bytes

ierr raymap_map(u64 size_hint, int add_mmap_flags);

void *rayalloc(u64 cap, u64 elsize, bool raw);

void rayfree(void *ptr);

ierr raymap_resize(u64 new_size);

ierr raymap_unmap(void);

void map_dbg_print(void);

#endif /* _RAYALLOC_H_ */
