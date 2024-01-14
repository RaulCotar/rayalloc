#ifndef _ACACHE_H_
#define _ACACHE_H_

#include "array.h"
#include "config.h"

extern thread_local ar_t *acache[ACACHE_SIZE]; // FIFO of most recently freed arrays

// add new, possibly evicting the oldest; set quick to skip possible coalescing
void acache_append(ar_t *ar, bool quick);

// remove old and append new; set quick to skip possible coalescing
void acache_replace(ar_t *old, ar_t *new, bool quick);

// try to coalesce cache entries and their neighbours - jumps through memory a lot!
void acache_coalesce_all(void);

#endif /* _ACACHE_H_ */
