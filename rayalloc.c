#include <sys/mman.h>
#ifndef NDEBUG
	#define DEBUG(...) __VA_ARGS__
	#include <assert.h>
	#include <stdio.h>
	// #include <errno.h>
#else
	#define DEBUG(...)
#endif

#include <execinfo.h>
#include <stdlib.h>
void stacktrace(void) {
	void *tracebuf = malloc(sizeof(void*) * 512);
	int n = backtrace(tracebuf, 512);
	backtrace_symbols_fd(tracebuf, n, 2);
	free(tracebuf);
}

#define PGSZ (4<<10)
#define MIN(A,B) ((A)<=(B)?(A):(B))
#define MAX(A,B) ((A)>=(B)?(A):(B))

typedef unsigned long ulong;
typedef unsigned char uchar;

typedef struct array_dynamic {
	struct chunk *chunk; // parent chunk
	bool free:1; // is this array free (alwyas 0 for this type)
	bool dynamic:1; // do we manage elsize and length
	char _:6;
	ulong size:56;
	ulong elsz:8; // 0 is INVALID here, ie the struct instance is corrupt
	ulong len:56;
	uchar bytes[];
} array_dyn_t;

typedef struct array_free {
	struct chunk *chunk;
	bool free:1; // always 1 for this type
	char _:7;
	ulong size:56; // size as if this was a non-free array (24 byte header)
	ulong sll_next; // the offset to the next (if any) free array in this chunk
} array_fre_t;

typedef struct chunk {
	struct chunk *dll_prev, *dll_next;
	bool free:1;
	char _:7;
	ulong size:56;
	ulong sll_free; // offset to the first (if any) free array
	uchar bytes[];
} chunk_t;

thread_local static chunk_t *chkdll_free, *chkdll_used, *chkdll_full [[maybe_unused]];

chunk_t *chkdll_pop_last(chunk_t *restrict start) {
	while (start)
		start = start->dll_next;
	start->dll_prev->dll_next = NULL;
	start->dll_prev=NULL;
	return start;
}

void chkdll_append(chunk_t **restrict start, chunk_t *const restrict chunk) {
	if (!*start) {
		*start = chunk;
		chunk->dll_prev = NULL;
		chunk->dll_next = NULL;
		return;
	}
	chunk_t *dll = *start;
	while (dll)
		dll = dll->dll_next;
	dll->dll_next = chunk;
	chunk->dll_prev = dll;
	chunk->dll_next = NULL;
}

void chkdll_prepend(chunk_t *restrict start, chunk_t *const restrict chunk) {
	start->dll_prev = chunk;
	chunk->dll_next = start;
	chunk->dll_prev = NULL;
}

chunk_t *mmap_chunk(ulong const size) {
	chunk_t *c = mmap(NULL, sizeof(chunk_t)+size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, 0, 0);
	if (c == MAP_FAILED) return NULL;
	c->free = true;
	c->size = size;
	array_fre_t* f = (void*)c->bytes;
	f->chunk = c;
	f->free = true;
	f->size = c->size-sizeof(array_dyn_t);
	return c;
}

int munmap_chunk(chunk_t *const restrict chunk) {
	return munmap(chunk, sizeof(chunk_t) + chunk->size);
}

array_fre_t *find_free_array(chunk_t const *restrict chunks, ulong const msize) {
	while(chunks) {
		array_fre_t const *ar = (void*)(chunks->bytes+chunks->sll_free);
		while (ar) {
			assert(ar->free);
			if (ar->size >= msize) return (array_fre_t*)ar;
			if (ar->sll_next) ar = ((void*)ar) + ar->sll_next;
			else break;
		}
		chunks = chunks->dll_next;
	}
	return NULL;
}

/* Go through all the used, then free chunks
 * For each of the visited chunks go through the free sll
 * If there you find a suitable free array, turn it into a used one and return
 * If by the end of the chunk lists you've not found an array location, mmap a new chunk
 */
void *alloc_array(ulong const capacity, uchar const elsize) {
	if (!capacity || !elsize) return NULL;
	array_fre_t *farr = find_free_array(chkdll_used, capacity*elsize);
	if (!farr) farr = find_free_array(chkdll_free, capacity*elsize);
	if (!farr) {
		chunk_t *c = mmap_chunk(MAX(4, (capacity+sizeof(array_dyn_t))/PGSZ
										+ ((capacity+sizeof(array_dyn_t))%PGSZ ? 1:0)) * PGSZ);
		if (!c) return NULL;
		chkdll_append(&chkdll_used, c);
		farr = (void*)(c->bytes + c->sll_free);
	}
	array_dyn_t *darr = (void*)farr;
	darr->free = false;
	darr->dynamic = true;
	darr->elsz = elsize;
	darr->len = 0;
	return darr;
}