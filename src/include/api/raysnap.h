#ifndef _API_RAYSNAP_H_
#define _API_RAYSNAP_H_

typedef struct raysnap raysnap_t; // holds the snapshot data

enum raysnap_which {
	RAYSNAP_TL, // snapshot the thread-local map
	RAYSNAP_SH, // snapshot the shared map
};

// go over the map and save a snapshot in a separate mem map
raysnap_t *raysnap_snapshot(enum raysnap_which const which);

// serialize a snapshot in a user-friendly format using ANSI escape codes
void raysnap_print(raysnap_t const *snap, void *file/* FILE* */);

// serialize a snapshot to comma-separated values
void raysnap_csv(raysnap_t const *snap, void *file/* FILE* */);

// dump the snapshot binary to a file
void raysnap_bin(raysnap_t const *snap, void *file/* FILE* */);

// free the memory associated with a snapshot
void raysnap_delete(raysnap_t *snap);

// a combined _snapshot _print _delete for convenience
// basically a replacement for the rayalloc debug print, but more memory hungry
void raysnap_quickie(enum raysnap_which const which, void *file/* FILE* */);

#endif /* _API_RAYSNAP_H_ */
