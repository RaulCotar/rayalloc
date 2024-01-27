API reference
===

This is a reference for the "public API" - the functions and objects that are exposed through the [api/](src/include/api) headers and meant to be used by the users of this library. Some apects presented on this page may require some of knowledge of how Rayalloc works - which is recommended to know anyway.

## Table of contents
1. [rayalloc](#void-rayallocu32-capacity-u16-elem_size-u32-flagsinclude-rayalloch) - allocated an array
1. [rayfree](#void-rayfreevoid-ptrinclude-rayalloch) - deallocate an array
1. [rayresize](#void-rayresizevoid-ptr-u32-capacityinclude-rayalloch) - resize an array
1. [ray_config](#ierr-ray_configenum-raycfg-const-option-include-rayalloch) - configure the allocator

---
### `void *rayalloc(u32 capacity, u16 elem_size, u32 flags)`<br><small><small>\#include [rayalloc.h](src/include/api/rayalloc.h)</small></small>
Allocate an array on the heap.
argument | description
:--|:--
`u32 capacity` | The max number of elements the array can hold.
`u16 elem_size` | The size of an element in bytes, cannot be `0`.
`u32 flags` | A bitwise or of: <br>`RA_AR_RAW` - mark the array as fully manged by the programmer <br>`RA_AR_CIRC` - marl the array as circular <br>`RA_AR_DIRTY` - do not zero the array; use with care <br>`RA_AR_SHARED` - store the array in the shared map <br>`RA_AR_NOGROW` - hint that the array will never grow
**return value** | A ponter to the first element of a newly allocated array, or `NULL` on failure.

---
### `void rayfree(void *ptr)`<br><small><small>\#include [rayalloc.h](src/include/api/rayalloc.h)</small></small>
Free an array, or do nothing if  `ptr` is `NULL`.  
If given a pointer not previously obtained from `rayalloc`, the internal structure of the allocator may get currupted.
argument | description
:--|:--
`void *ptr` | A pointer previously obtained from `rayalloc`, or `NULL`.
**return value** | N/A

---
### `void *rayresize(void *ptr, u32 capacity)`<br><small><small>\#include [rayalloc.h](src/include/api/rayalloc.h)</small></small>
Resize an array, possibly relocating it.  
If called with an invalid pointer, the internal structure of the allocator may get currupted.
argument | description
:--|:--
`void *ptr` | A pointer previously obtained from `rayalloc`.
`u32 capacity` | The new capacity of the array.
**return value** | The adress of the new array.

---
### `ierr ray_config(enum raycfg const option, ...)`<br><small><small>\#include [rayalloc.h](src/include/api/rayalloc.h)</small></small>
This function can do many things related to the management of the allocator.
option | variadic arguments | description
:--|:--|:--
`rayconf_nop` | | No-op, does nothing
`rayconf_tl_set_Msize` | `u64` | Set max thread-local allocation space size.
`rayconf_sh_set_Msize` | `u64` | Set max shared allocation space size.
`rayconf_tl_get_Msize` | `u64*` | Get max thread-local allocation space size.
`rayconf_sh_get_Msize` | `u64*` | Get max shared allocation space size.
`rayconf_tl_map` | | Create initial thread-local memory map with default size.
`rayconf_sh_map` | | Create initial shared memory map with default size.
`rayconf_tl_map_s` | `u64` | Create initial thread-local memory map of given size.
`rayconf_sh_map_s` | `u64` | Create initial shared memory map of given size.
`rayconf_tl_unmap` | | Return all thread-local memory to the OS.
`rayconf_sh_unmap` | | Return all shared memory to the OS.
`rayconf_tl_get_ptr` | `void**` | Get the address of the thread-local memory map.
`rayconf_sh_get_ptr` | `void**` | Get the address of the shared memory map.
`rayconf_tl_set_ff` | `enum ray_ff` | Set the thread-local map find free algorithm.
`rayconf_sh_set_ff` | `enum ray_ff` | Set the shared map find free algorithm.
**return value** | | An errno-like value described by the [IERR_](src/include/api/util.h) macros.