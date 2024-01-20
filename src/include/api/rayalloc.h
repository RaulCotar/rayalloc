#ifndef _API_RAYALLOC_H_
#define _API_RAYALLOC_H_

#include "util.h"

enum raycfg {
	rayconf_nop, // no-op, does nothing
	rayconf_tl_set_Msize, // Set max thread-local allocation space size. {u64}
	rayconf_sh_set_Msize, // Set max shared allocation space size. {u64}
	rayconf_tl_get_Msize, // Get max thread-local allocation space size. {u64*}
	rayconf_sh_get_Msize, // Get max shared allocation space size. {u64*}
	rayconf_tl_map, // Create initial thread-local mem map with default size
	rayconf_sh_map, // Create initial shared mem map with default size
	rayconf_tl_map_s, // Create initial thread-local mem map of given size. {u64}
	rayconf_sh_map_s, // Create initial shared mem map of given size. {u64}
	rayconf_tl_unmap, // Return all thread-local memory to the OS
	rayconf_sh_unmap, // Return all shared memory to the OS
	rayconf_tl_ptr, // Get the address of the thread-local mem map. {void**}
	rayconf_sh_ptr // Get the address of the shared mem map. {void**}
};

// Get or set allocator options. Includes debug printing in debug builds.
ierr ray_config(enum raycfg const option, ...);

#define RA_AR_RAW 0x3 // raw array => don't keep track of len and ref
#define RA_AR_CIRC 0x4 // circular array => basically ring buffers
#define RA_AR_DIRTY 0x10 // don't zero the memory

// allocate an array
void *rayalloc(u64 capacity, u64 elem_size, i32 flags);

// resize an array
void *rayresize(void *ptr, u64 capacity);

// free an array
void rayfree(void *ptr);

#endif /* _API_RAYALLOC_H_ */
