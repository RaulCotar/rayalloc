#ifndef _RAYSNAP_H_
#define _RAYSNAP_H_

#include "api/raysnap.h" // IWYU pragma: export
#include "map.h"

struct raysnap {
	u64 byte_size; // total size of the snapshot memory map
	u64 cache_cnt; // = ACACHE_SIZE
	u64 ar_cnt; // counted arrays while going over map (should eq map.arr_cnt)
	memmap_t map; // the map meta data, cache has cache_cnt entries
	ar_t arrays[/*ar_count*/]; // all array headers
};

#endif /* _RAYSNAP_H_ */
