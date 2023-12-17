#include "util.h"
#include "config.h"
#include "array.h"
#include "span.h"

#ifndef TLSL_SIZE
	#define TLSL_SIZE 8
#endif
#ifndef TLAC_SIZE
	#define TLAC_SIZE 15
#endif

thread_static struct span tlsl[TLSL_SIZE]; // external headers
thread_static struct span *cgsl; // inline headers
thread_static struct array_f *tlac[TLAC_SIZE];

// alloc MVP
// free MVP
// see README scratchpad