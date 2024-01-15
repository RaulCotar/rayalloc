// Functions to deal with map snapshots.
#ifndef _RAYALLOC_SNAP_H_
#define _RAYALLOC_SNAP_H_

#include "array.h"
#include "config.h"

typedef struct raysnap {
	u64 byte_size; // total size of the snapshot memory map
	void *map_ptr; // = _ray_map
	u64 map_size; // = _ray_map_size
	u64 acache_size; // = ACACHE_SIZE
	ar_t *acache[ACACHE_SIZE]; // = acache
	u64 map_arcnt; // = _ray_arr_cnt
	u64 ar_count; // counted arrays while going over map
	ar_t arrays[];
} raysnap_t;

// go over the map and save a snapshot in a new mmap
raysnap_t *raysnap_snapshot(void);

// serialize a snapshot in a user-friendly format
void raysnap_print(raysnap_t const *snap, void *file/* FILE* */);

// serialize a snapshot to comma-separated values
void raysnap_csv(raysnap_t const *snap, void *file/* FILE* */);

// combines _snapshot with _print and munmap for convenience
void raysnap_quickie(void *file/* FILE* */);

// The orginal debug print. Almost identical printout as raysnap_quickie.
void map_dbg_print(void);

#endif /* _RAYALLOC_SNAP_H_ */
