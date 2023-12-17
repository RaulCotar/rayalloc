The Documention
===

This file contains descriptions and explanations of how `rayalloc` works, in more detail than the project readme. For implementation minutia check out the comments in the source files.  
This file also contains an index of all the other files in the docs folder.


## Table of Contents
1. [The Documentation](#the-documention) . . . . . . . . . . . . . . . . . . . . title section
1. [Directory Index](#directory-index) . . . . . . . . . . . . . . . . . . . . . useful files
1. [Data Structures](#data-structures) . . . . . . . . . . . . . . . . . . . . . important data structures
	1. [`span`](#span) . . . . . . . . . . . . . . . . . . . . . . . . . . . . . Spans
	1. [`array_s`](#array_s) . . . . . . . . . . . . . . . . . . . . . . . . . . Unmanaged arrays
	1. [`array_m`](#array_m) . . . . . . . . . . . . . . . . . . . . . . . . . . Managed arrays
	1. [`array_f`](#array_f) . . . . . . . . . . . . . . . . . . . . . . . . . . Free arrays
	1. [`tlsl`](#tlsl) . . . . . . . . . . . . . . . . . . . . . . . . . . . . . Thread-Local Span List
	1. [`cgsl`](#cgsl) . . . . . . . . . . . . . . . . . . . . . . . . . . . . . Common Global Span List
	1. [`tlac`](#tlac) . . . . . . . . . . . . . . . . . . . . . . . . . . . . . Thread-Local Array Cache


## Directory Index
- [interface.md](interface.md) - API reference
- [macro_config.md](macro_config.md) - `rayalloc` configuration using macros
- [../old/*](../old) - old code, discussions and ideas; may not be relevant anymore


## Data Structures
### `span`
Defined in [span.h](../span.h), a `span` is a contiguous region of (virtual) memory obtained from `mmap` or `mremap` that holds arrays. The header that stores bookkeeping information is either inline (directly preceeds the contents) or external (refers to the contents via a pointer). Spans reside in 2 places: the [`tlsl`](#tlsl) (external headers) or the [`cgsl`](#cgsl) (inline headers).  
Spans can be shared with other threads by setting a header flag and using locks.  
Spans have "step sizes", fixed increments used to allocate array sizes. For more details on those check the code and the `SPAN_STEP_*` config macros.

### `tlsl`
Defined in [rayalloc.c](../rayalloc.c), the "thread-local span list" is siply a C array of external span headers. Each thread gets a fxes number of them (see `TLSL_SIZE`). They act as the thread's primary source of spans, and are by default not shared. The array starts with all elements being non-backed spans (meaning that no memory is actually mapped) and they are mapped and expanded as needed. The first 8 spans have their step id equal to their index, the following spans have their step id set et runtime.

### `cgsl`
Defined in [rayalloc.c](../rayalloc.c), the "common global span list" is a singly-linked list of spans (with inline headers) that is shared by all threads. It starts empty and expands as more shared spans are created, either because the user requested some shared memory, or because the `tlsl` of the requesting thread is full.

### `tlac`
Defined in [rayalloc.c](../rayalloc.c), the "thread-local array cache" is a C array of pointers to `array_f`s. Each thread gets a fxes number of them (see `TLAC_SIZE`). They are used for quickly finding free arrays by directly referencing them rather than having to go through linked lists of spans and arrays. The `tlac` is sorted in order of access time, with the most recently accessed arrays first; this way, we have a higher cache hit ratio (hopefully).