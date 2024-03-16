#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_
#include "include/allocator.h" // IWYU pragma: export
#include "include/config.h"

// tagged pointer (59-bit address + 3-bit tag)
typedef void* tp;
#define TP_ADRS(TP) ((void*)(((u64)(TP)) & (u64)~0x7UL))
#define TP_TAG(TP)  (((u64)(TP)) & 0x7UL)
enum tptyp: u64 {
	// range flags
	TP_MAP_END = 0b000,
	TP_FRE = 0b100,
	TP_DUM = 0b110,
	TP_RAW = 0b001,
	TP_MAN = 0b011,
	// bucket flags
	TP_BUK_FULL = 0b001,
	TP_BUK_EMPTY= 0b011,
};

#define INDEX_END (((void*)heap_index) + heap_index->map_size) // exclusive
#define INDEX_RANLO (heap_index->ranges) // inclusive
#define INDEX_RANHI (heap_index->ranges + heap_index->ran_cnt) // exclusive
#define INDEX_BUKLO ((struct bucket*)(INDEX_END - heap_index->buk_cnt * sizeof(struct bucket))) // inclusive
#define INDEX_BUKHI ((struct bucket*)INDEX_END) // exclusive
struct index {
	u64 map_size;
	u64 ran_cnt, buk_cnt;
	tp ranges[];
} extern *heap_index;

struct bucket {
	tp map;
	u8 slots[(1<<BUCKET_MAPSIZE)/BUCKET_ELSIZE];
};

#endif /* _ALLOCATOR_H_ */
