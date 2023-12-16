/*  What is a span?
 *
 *  A span is region of contiguous (virtual) memory that corresponds to one mmap
 * mapping. Spans hold the arrays that are allcoated by the application.
 *
 *  Each thread gets an array of 8(configurable) span headers that point to a
 * remote area in memory where the span's contents are. On top of that, the
 * Common Global Span List is a singly-linked list that holds more spans and is
 * shared between all threads. Note that this does not mean that the arrays they
 * contain are shared, it merely means that all threads know those spans exist.
 * Refer to the `struct span` definition for implementation details on the
 * "inline" and "external" span header styles (CGSL uses inline usually).
 *
 *  On the topic of threads sharing memory, they do it by sharing individual
 * arrays. Each time a thread wants to make changes to a shared span (not its
 * contents, but the hearder or memory map itself) it has to lock it. Something
 * similar happend for arrays: if a thread wans to access a shared array, it
 * first needs to acquire its respective lock. That being said, by default,
 * arrays are not shared by default, case in which no locks are invlved.
 *
 *  Spans can grow and shrink (using `mremap`) when more space is needed. They
 * do so in steps, fized sizes that are also used to select appropriate spans
 * for arrays based on their element size, capacity and growth hints.
*/
#ifndef _SPAN_H_
#define _SPAN_H_

#include "util.h"

struct span {
	u64 header;
	union {
		u8 *data; // data if the header is not inlined
		struct span *next; // assuming CGSL: next span in CGSL, data is in bytes[]
	};
	u8 bytes[]; // data if the header is inlined

	// span header bitmasks
	#define M_SPAN_LOCK 0x1 // does this span require acquiring a lock before r/w?
	#define M_SPAN_USED	0x2 // is this span used (false = completely free)?
	#define M_SPAN_CGSL 0x4 // is this span part of the common global span dll?
	#define M_SPAN_HUGE 0x8 // is this span backed by hugepages?
	#define M_SPAN_STEP 0x70 // step id, see table
	#define M_SPAN_BIT8 0x80
	#define M_SPAN_SIZE	0xffffff00 // total size of the span in bytes
};

/* span step table
_____________________
| id | align | size |
|----|-------|------|
|  0 |     4 |    4 | for "small short-lived strings"
|  1 |     8 |    8 | \
|  2 |     8 |   32 |  \
|  3 |     8 |   64 |   >- these are configurable via macros
|  4 |     8 |  256 |  /
|  5 |    1K |   1K | /
|  6 |  page | page | this is the way to get full pages from the allocator
|  7 |  page | user | single array per span, primary contender for hugepages
---------------------
*/
#define SPAN_STEP_0 1
#ifndef SPAN_STEP_1
	#define SPAN_STEP_1 8
#endif
#ifndef SPAN_STEP_2
	#define SPAN_STEP_2 32
#endif
#ifndef SPAN_STEP_3
	#define SPAN_STEP_3 64
#endif
#ifndef SPAN_STEP_4
	#define SPAN_STEP_4 265
#endif
#ifndef SPAN_STEP_5
	#define SPAN_STEP_5 1024
#endif
#ifndef PAGE_SIZE
	#define PAGE_SIZE 4096 // should prob use sysconf(_SC_PAGESIZE), but I'm lazy
#endif
#define SPAN_STEP_6 PAGE_SIZE
#define SPAN_STEP_7 0

#endif /* _SPAN_H_ */
