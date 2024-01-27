#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "util.h"

#define AR_M_KIND	0x3 // see AR_K_*
#define AR_M_CIRC	0x4 // is this a circular array (managed only)
#define AR_M_LNFREE	0x8 // is the lower neighbour free
#define AR_M_ELSIZE (0xffff<<ESO) // 0xffff0000
#define AR_K_FREE	0x0
#define AR_K_FOOT	0x1
#define AR_K_MAN	0x2
#define AR_K_RAW	0x3

typedef struct array_free {
	alignas(16) u32 flags;
	u32 cap;
	union {
		struct array_free *nextf; // in headers
		struct array_free *prevf; // in footers
	};
} ar_f;

typedef struct array_managed {
	alignas(16) u32 flags;
	u32 cap, len, ref;
} ar_m;

typedef struct array_raw {
	alignas(16) u32 flags;
	u32 cap; // the 8 bytes of padding are actually user data
} ar_r;

typedef union [[gnu::transparent_union]] array {
	ar_f free;
	ar_m man;
	ar_r raw;
	struct { // the common parts of all raay types
		alignas(16) u32 flags;
		u32 cap;
	};
} ar_any;

#define ESO 16 // elsize leftshift offset
#define BS  16 // block size

// ocupied blocks
#define blocks(VAL) ((((u64)VAL) + BS-1) / BS)
// total ocupied blocks incuding header
#define arblocks(AR) blocks(((u64)((AR).cap)) * ((AR).flags>>ESO) + BS)

#define ar_is_zeros(AR)	(!((AR).flags || (AR).cap || ((AR)).nextf))
#define ar_is_free(AR) 	(((AR).flags & AR_M_KIND) == AR_K_FREE)
#define ar_is_foot(AR)	(((AR).flags & AR_M_KIND) == AR_K_FOOT)
#define ar_is_man(AR)	(((AR).flags & AR_M_KIND) == AR_K_MAN)
#define ar_is_raw(AR)	(((AR).flags & AR_M_KIND) == AR_K_RAW)
#define ar_is_used(AR)	(((AR).flags & AR_M_KIND) > AR_K_FOOT)
#define ar_is_circ(AR)	((AR).flags & AR_M_CIRC)
#define ar_has_lnf(AR)	((AR).flags & AR_M_LNFREE)

#define ar_elsize(AR) ((u64)((AR).flags >> ESO))
#define ar_Bsize(AR) (((u64)((AR).flags >> ESO)) * (AR).cap)
#define ar_get_foot(AR) (((ar_f*)(AR))-(u64)1+arblocks(*((ar_f*)(AR)))) // takes ar ptr
#define ar_calc_good_es(BYTES) ((u16)(((u64)(BYTES))/UINT32_MAX+1))

#endif /* _ARRAY_H_ */
