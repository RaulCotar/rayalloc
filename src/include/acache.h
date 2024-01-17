#ifndef _ACACHE_H_
#define _ACACHE_H_

#include "array.h"
#include "config.h"

extern thread_local ar_t *acache[ACACHE_SIZE]; // FIFO of most recently freed arrays

// add new, possibly evicting the oldest; set quick to skip possible coalescing
void acache_append(ar_t const *ar, bool quick);

// remove an entry from the acache (if present); never coalesces
void acache_remove(ar_t const *ar);

// replce old entry with new entry, moving it to the top
// ; do nothing if old not found
// ; set quick to skip possible coalescing
void acache_replace(ar_t const *old, ar_t const *new, bool quick);

// try to coalesce cache entries and their neighbours - jumps through memory a lot!
[[gnu::cold]] void acache_coalesce_all(void);

#endif /* _ACACHE_H_ */
