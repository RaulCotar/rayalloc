#ifndef _RAYALLOC_H_
#define _RAYALLOC_H_

#include "api/rayalloc.h" // IWYU pragma: export
#include "array.h"

// internal array allocation functions

ar_t *coalesce_fwd(ar_t *from);

#endif /* _RAYALLOC_H_ */
