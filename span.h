#ifndef _SPAN_H_
#define _SPAN_H_

#include "util.h"
#include "config.h"

struct span {
	u64 header;
	union {
		u8 *data; // data if the header is not inlined (TLSL)
		struct span *next; // assuming CGSL: next span in CGSL, data is in bytes[]
	};
	u8 bytes[]; // data if the header is inlined

	// span header bitmasks
	#define M_SPAN_LOCK 0x1 // does this span require acquiring a lock before r/w?
	#define M_SPAN_USED	0x2 // is this span used (false = completely free)?
	#define M_SPAN_INLINE 0x4 // does this span have an inline header?
	#define M_SPAN_HUGE 0x8 // is this span backed by hugepages?
	#define M_SPAN_STEP 0x70 // step id, see table
	#define M_SPAN_MMAP 0x80 // is this span actually backed by (mmaped) memory?
	#define M_SPAN_SIZE	0xffffffffffffff00 // total size of the span in bytes
};	
/* An all-zeros span struct has the following meaning:
 * not locked
 * not used (free)
 * external header
 * normal pages
 * step0
 * not backed
 * zero-sized
*/

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
	#define PAGE_SIZE 4096
#endif
#define SPAN_STEP_6 PAGE_SIZE
#define SPAN_STEP_7 0

#endif /* _SPAN_H_ */
