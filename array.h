#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "util.h"

// non-managed array
struct array_s {
	u32 header; // see array header breakdown doc comment
	u32 cap; // capacity = array size / element size
	u8 bytes[]; // application data

	#define M_ARR_LOCK 0x1 // does this array require acquiring a lock before r/w?
	#define M_ARR_USED 0x2 // is this span used (false = free)?
	#define M_ARR_ELSZ 0xffc // element size (if managed); 0 means non managed
	// unfortunately, there are 20 unused bits in a non-free array header :(
};

// managed array
struct array_m {
	u32 header; // same as for array_s; ELSZ!=0
	u32 cap, len; // we also keep track of the length used by the application
	u8 bytes[]; // application data
	// unfortunately the size is not a multiple of 8 bytes :(
};

// free array
struct array_f {
	u32 header; // not the same as the normal array header!! use M_ARF_*
	u32 size; // lower 32 bits of the array size (size in bytes)
	u32 next; // lower 32 bits of the offset to the next free array in the span
	u32 backref1, backref2; // array_f **backref, but not aligned; iff BREF

	#define M_ARF_LOCK 0x1 // does this array require acquiring a lock before r/w?
	#define M_ARF_USED 0x2 // always 0
	#define M_ARF_BREF 0x4 // is there a backref? (iff array is free)
	// bit 0x8 is unused
	#define M_ARF_USIZ 0x3fff0 // upper 14 bits of the array size
	#define M_ARF_UNXT 0xfffc0000 // upper 14 bits of the "next" offset

	/*  The reason we do much bit packing is because the size of this struct
	 * dictates the minimum array size. Without this packing, that size would be
	 * 32 instead of 24 bytes. This matters mostly when the application has many
	 * small strings. We do pay a few CPU cycles, but I think it's worth it.
	 *
	 *  We need the extra upper bits in "size" and next" because:
	 * 1: we have the size in bytes, not elsize, so we need to compensate
	 * 2: the "next" offset might need to jump over a few large arrays
	 */
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-variable-sized-type-not-at-end"
// generic array (sucky but nice for the API)
union __attribute__ ((__transparent_union__)) array_g {
	struct {struct array_s s; char _[4+8];};
	struct {struct array_m m; char __[8];};
	struct array_f f;
};
#pragma GCC diagnostic pop

#endif /* _ARRAY_H_ */
