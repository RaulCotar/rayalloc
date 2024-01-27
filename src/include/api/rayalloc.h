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
	rayconf_tl_get_ptr, // Get the address of the thread-local mem map. {void**}
	rayconf_sh_get_ptr, // Get the address of the shared mem map. {void**}
	rayconf_tl_set_ff, // Set the thread-local map find free algorithm {enum ray_ff}
	rayconf_sh_set_ff, // Set the shared map find free algorithm {enum ray_ff}
};

// Get or set allocator options, or map/unmap the allcoation space.
ierr ray_config(enum raycfg const option, ...);

enum ray_ff {
	rayFF_CFMF, // first in cache or first in map
	rayFF_CSMF, // smallest in cache or first in map
	rayFF_CSMS, // smallest in cache or smallest in map
};

#define RA_AR_RAW 0x3 // raw array => don't keep track of len and ref
#define RA_AR_CIRC 0x4 // circular array => basically ring buffers
#define RA_AR_DIRTY 0x10 // don't zero the memory
#define RA_AR_SHARED 0x20 // allocate the array in the shared map
#define RA_AR_NOGROW 0x40 // the array is not expected to ever grow

// allocate an array
void *rayalloc(u32 capacity, u16 elem_size, u32 flags);

// resize an array
void *rayresize(void *ptr, u32 capacity);

// free an array
void rayfree(void *ptr);

#endif /* _API_RAYALLOC_H_ */
