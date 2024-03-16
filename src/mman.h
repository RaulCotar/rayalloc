#ifndef _MMAN_H_
#define _MMAN_H_
// #define __USE_GNU // mremap
#include <sys/mman.h>
#include "include/typedefs.h"

#define PROT_RW (PROT_READ|PROT_WRITE)
#define MAP_ANONPRIV (MAP_ANONYMOUS|MAP_PRIVATE)
#ifndef MAP_UNINITIALIZED
	#define MAP_UNINITIALIZED 0
#endif

#ifndef __USE_GNU

#define MREMAP_MAYMOVE 1
#define MREMAP_FIXED 2
#define MREMAP_DONTUNMAP 4

[[gnu::nothrow]]
void *mremap(void *const old_address, u64 const old_size, u64 const new_size, int const flags, ... /* void *new_address */);

#endif /* ifndef __USE_GNU */

#endif /* _MMAN_H_ */
