#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "util.h"

// type definitions {{{

#define M_AR_USED	0x1
#define M_AR_RAW	0x2
#define M_AR_DETAC	0x4 // detached header, used in ar_det_t
#define M_AR_unused	0x8
#define M_AR_MAPID	0x000ffff0 // 2^16 is a tipical limit for max active mmaps
#define M_AR_ELSIZE	0xfff00000 // >>20; 2^12 allows array size to match map size
// elsize is interpreted as actual_elsize-1, eg: elsize=2 means a size of 3 bytes
// even though elize has byte granularity, arrays always occupy multiples of AR_STEP bytes
// the maximum content size of any array type is 2^12 * (2^32 -1) = ~16TiB = max map size

typedef struct alignas(8) ar_raw {
	u32 flags;
	u32 size; // size in bytes = size*elsize
	// elsize is chosen to allow the max size to hold the requested space
	// but not larger than that since we want to have small increments available
	alignas(8) u8 data[];
} ar_raw_t;

typedef struct alignas(8) ar_dyn {
	u32 flags, cap, len;
	u32 ref; // this marks the logically first element in a circular array
	// ref can also be used to implement a [double-ended] queue
	alignas(8) u8 data[];
	// actual size in bytes = round_up_to_mult8(cap * elsize)

} ar_dyn_t;

typedef struct alignas(8) ar_fre {
	u32 flags, size; // size in bytes = size*elsize
	struct ar_fre *backref; // 3 free LSB
	alignas(8) u8 data[];
} ar_fre_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-variable-sized-type-not-at-end"
typedef union __attribute__ ((__transparent_union__)) ar {
	struct {ar_raw_t raw; char _pad[8];};
	ar_dyn_t dyn;
	ar_fre_t fre;
} ar_t;

typedef struct ar_det {
	union {
		ar_raw_t raw;
		ar_dyn_t dyn;
		ar_fre_t fre;
	};
	void *data;
} ar_det_t;
#pragma GCC diagnostic pop

// type definitions }}}

#define AR_STEP alignof(ar_t)

#endif /* _ARRAY_H_ */
