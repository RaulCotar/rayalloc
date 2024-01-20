#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "util.h"

// rule: each array occupies a multiple of 16 bytes
#define AR_USED		0x1
#define AR_RAW		0x2 // note that free arrays are never raw
#define AR_CIRC		0x4 // circular arrays have different semantics
#define AR_UNUSED	0xfff8 // currently unused bits
#define AR_ELSIZE	0xffff<<ESO // >0 always

typedef struct array {
	alignas(16) u32 flags;
	u32 cap;
	// note: cap for free arrays is always a multiple of BS
	union {
		struct {
			u32 len;
			u32 ref; // this marks the logically first element in a circular array
		};
		struct array *fref; // if non-NULL: another free array
		// `fref`s can be used when acache is not sufficient
	};
} ar_t;

#define ESW 16 // elsize width = UINT16_WIDTH
#define ESO 16 // elsize leftshift offset
#define BS sizeof(ar_t) // block size

#define blocks(VAL) ((((u64)VAL) + BS-1) / BS)
#define arblocks(AR) blocks(((u64)((AR).cap)) * ((AR).flags>>ESO))

#define ar_is_zeros(AR) (!((AR).flags || (AR).cap || (AR).len || (AR).ref))
#define ar_is_used(AR) ((AR).flags & AR_USED)
#define ar_is_raw(AR) ((AR).flags & AR_RAW)
#define ar_is_free(AR) (!((AR).flags & (AR_USED||AR_RAW)))
#define ar_is_circ(AR) ((AR).flags & AR_CIRC)

#define ar_elsize(AR) ((u64)((AR).flags >> ESO))
#define ar_bsize(AR) (((u64)((AR).flags >> ESO)) * (AR).cap)

#endif /* _ARRAY_H_ */
