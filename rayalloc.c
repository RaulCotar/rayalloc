#include "util.h"
#include "config.h"
#include "array.h"
#include "span.h"
#include "rayalloc.h"

#ifndef TLSL_SIZE
	#define TLSL_SIZE 8
#endif
#ifndef TLAC_SIZE
	#define TLAC_SIZE 15
#endif

thread_static struct span tlsl[TLSL_SIZE]; // external headers
thread_static struct span *cgsl; // inline headers
thread_static struct array_f *tlac[TLAC_SIZE];

union array_g *rayalloc(unsigned size, unsigned elzs, unsigned f) {
	// search tlac for suitable array
	// see which step this falls in
	// search for a suitable span
	// search for a suitable array
	// create span/array
	// update tlac?
	for (u32 a=0; a<TLAC_SIZE; a++) {
		if (tlac[a])
	}
}

// alloc MVP
// free MVP
// see README scratchpad