#include "util.h"
#include "span.h"

#ifndef TLCACHE_SIZE
	#define TLCACHE_SIZE 8
#endif

/*  Thread Local Span Cache
 *  Stores a few external span headers for quicker lookups. 
 * Right now I have some conflicting statements: configurable cache size and
 * index=step. I like the first one a bit better, but I have an idea: the steps
 * wrap: index 8 = step 0 and so on. So until sth actually gets implemented,
 * this is what I'm running with in the docs.
*/
thread_static struct span tlcache[TLCACHE_SIZE]; // headers in here; index=step
thread_static struct span *cgsl; // headers stored in-place

// alloc MVP
// free MVP
// see README scratchpad