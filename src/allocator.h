#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_
#include "include/allocator.h" // IWYU pragma: export
#include "include/config.h"

// tagged pointer (59-bit address + 3-bit tag)
typedef void* tp;
#define TP_ADRS(TP) ((void*)(((u64)(TP)) & 0xfffffffffffffff8UL))
#define TP_TAG(TP)  ((void*)(((u64)(TP)) & 0x7UL))
enum tptyp: u64 {
	// marks end of map
	TP_MAP_END = 0b000,
	// free, next free at least +N away
	TP_FREE1 = 0b001,
	TP_FREE2 = 0b010,
	TP_FREE3 = 0b011,
	TP_FREE4 = 0b100,
	TP_FREE5 = 0b101,
	// in-use, mask 0x6
	TP_RAW = 0b110,
	TP_MAN = 0b111,
	// bucket flags
	TP_BUK_FULL = 0b001, // bucket completely full
	TP_BUK_HUGE = 0b010, // uses a hugepage
};

#define INDEX_END (((void*)index) + index->map_size)
#define INDEX_ARTOP (index->arrays + index->ar_cnt)
#define INDEX_BUKEND ((struct bucket*)(INDEX_END - index->buk_cnt * sizeof(struct bucket)))
#define INDEX_BUKBEG ((struct bucket*)(INDEX_END - sizeof(struct bucket)))
struct index {
	u64 map_size;
	u64 ar_cnt, buk_cnt;
	tp arrays[];
} extern *index;

struct bucket {
	tp map;
	alignas(8) u8 bits[1 << (BUCKET_MAPSIZE-3)];
};

#define ALLOC_FORCE_M 6 // mask for _ARRAY and _MAP flags

#endif /* _ALLOCATOR_H_ */
