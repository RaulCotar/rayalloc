#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "util.h"

#define AR_M_KIND	0x3 // free=0, footer=1, managed=2, raw=3
#define AR_M_CIRC	0x4 // is this a circular array (managed only)
#define AR_M_LNFREE	0x8 // is the lower neighbour free
#define AR_M_ELSIZE 0xffff<<ESO // 0xffff0000

typedef struct array_free {
	alignas(16) u32 flags;
	u32 cap;
	struct array_free *nextf;
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

#define ar_is_zeros(AR)	(!((AR).flags || (AR).cap || (AR).nextf))
#define ar_is_free(AR) 	(((AR).flags & AR_M_KIND) == 0)
#define ar_is_foot(AR)	(((AR).flags & AR_M_KIND) == 1)
#define ar_is_man(AR)	(((AR).flags & AR_M_KIND) == 2)
#define ar_is_raw(AR)	(((AR).flags & AR_M_KIND) == 3)
#define ar_is_used(AR)	(((AR).flags & AR_M_KIND) > 1)
#define ar_is_circ(AR)	((AR).flags & AR_M_CIRC)
#define ar_is_lnf(AR)	((AR).flags & AR_M_LNFREE)

#define ar_elsize(AR) ((u64)((AR).flags >> ESO))
#define ar_Bsize(AR) (((u64)((AR).flags >> ESO)) * (AR).cap)

#endif /* _ARRAY_H_ */
