#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "util.h"

// rule: each array occupies a multiple of 16 bytes
#define AR_USED		0x1
#define AR_RAW		0x2 // note that free arrays are never raw
#define AR_UNUSED	0xfffc // currently unused bits
#define AR_ELSIZE	0xffff0000 // >0 always; right shift by 16 instead of mask

typedef struct alignas(16) array {
	u32 flags;
	u32 cap, len; // in elsize
	u32 ref; // this marks the logically first element in a circular array
	char data[];
} ar_t;

#define ar_is_zeros(AR) (!((AR).flags)&&!((AR).cap)&&!((AR).len)&&!((AR).ref))

#endif /* _ARRAY_H_ */
