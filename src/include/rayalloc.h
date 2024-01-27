#ifndef _RAYALLOC_H_
#define _RAYALLOC_H_

#include "api/rayalloc.h" // IWYU pragma: export
#include "array.h"
#include "map.h"

// Find suitable free; clear lowest 4 bits before dereferencing!
ar_f *find_free(memmap_t const *map, u64 const size);

ar_f *coalesce(ar_f *from);

#endif /* _RAYALLOC_H_ */
