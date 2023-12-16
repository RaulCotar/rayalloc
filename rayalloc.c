#include "util.h"
#include "span.h"

#ifndef TLCACHE_SIZE
	#define TLCACHE_SIZE 8
#endif

thread_static struct span tlcache[TLCACHE_SIZE]; // headers in here; index=step
thread_static struct span *globlist; // headers stored in-place

// alloc MVP
// free MVP
// see README scratchpad