RayAlloc - WIP
===
*The project just had a large rewrite, some informations on this page may be outdated.*

Insert explanation of how most allocations are arrays and how that leads to bookkeeping duplication. Introduce rayalloc as a lightweight, 2-in-1 library for memory allocation and dynamic arrays writen in C2X.
Check out the [docs folder](docs) (empty still) for more in-depth information about the internals. See the [todo list](./todo.md) if you are interested in the near-future plans.


## Table of Contents
1. [RayAlloc](#rayalloc---wip) . . . . . . . . . . . . . . . . . . . . . . . . . . title section
1. [Interface](#interface) . . . . . . . . . . . . . . . . . . . . . . . . . . . API walkthrough
1. [Working Principle](#working-principle) . . . . . . . . . . . . . . . . . . . allocation scheme
1. [Configuration](#configuration) . . . . . . . . . . . . . . . . . . . . . . . macro config
1. [Profiling](#profiling) . . . . . . . . . . . . . . . . . . . . . . . . . . . . raysnap utils
1. [Building](#building) . . . . . . . . . . . . . . . . . . . . . . . . . . . . compilation guide
1. [Testing](#testing) . . . . . . . . . . . . . . . . . . . . . . . . . . . . . about automated tests


## Interface
The interface has 3 parts: the main array interface, the "malloc" one and the debug and profiling stuff. The "malloc" interface is just a very thin wrapper (like 20 SLOC thin) around the array interface for the sake of having a `malloc` replacement. Check the headers for the complete interface. Check out the [interface reference](docs/api_reference.md) for more details.


## Working Principle
Rayalloc creates 2 virtual memory maps using `mmap(2)`: `sh_map` (shared) and `tl_map` (thread-local). Inside these maps there are free and used arrays, each with its own 16 byte (usually) header. All arrays have at least 16 bytes of payload (24 for raw), and their headers are always 16-byte aligned.

Arrays can be of 3 types:
- **free:** unused space. Free arrays also store a footer at the end for 2-way map traversal of free arrays and other goodies.
- **managed:** dynamic array that can be used with the rest of the array manipulation functions. A lenght and reference point are also stored for these. Managed arrays can be *linear* or *circular*.
- **raw:** a simple range of memory that the user can use for anything. Raw array headers are only 8 bytes, so the payload is always alligned to an odd multiple of 8 bytes.

By storing the capacity and element size for each array, they form an implicit linked list inside the map. All arrays store the `AR_M_LNFREE` flag to keep track of the free status of the low neighbour. Free arrays also have direct pointers between them (stored in the headers and footers) that allow the "free linked list" to be traversed both ways.

The every map also has a cache containing `ACACHE_SIZE` entries: pointers to free arrays, in order of most recently used. The acache is good in that it does not require jumping through the entire map (and possibly faulting many pages), but it can quickly get overwhelmed when lots of deallocation happen in quick succession. Coalescing of free arrays is also implemented to merge together neighbouring free arrays.

There are 2 allocation algorithms planned for now:
1. **first:** choose the *first* free array that has enough space; first search the cache, then the entire map if needed.
2. **smallest:** choose the *smallest* free array that has enough space; first search the cache, then the entire map if needed.

The shared map is shared by all threads. Rayalloc only handles the synchronization of its own actions regaring the allocator data structures (**not implemented yet**). The user should handle rpossible race conditions when accessing shared arrays.


## Configuration
The file [`config.h`](./src/config.h) contains a series of macros that can be used to configure rayalloc. Compiler arguments take precedence over the defaults in [`config.h`](./src/config.h), so you can use `-D` flags instead of editing the file. Here is a non-comprehensive list of them:
macro | default value | description
:--|:--|:--
`ACACHE_SIZE` | `16` | number of entries in the thread local array cache
`DEFAULT_MAP_SIZE` | `64*PAGE_SIZE` | defaul maximum map size in bytes
`DEFAULT_MAP_MSIZE` | `(u64)-1` | defaul maximum map size in bytes
`PAGE_SIZE` | `4096` | system page size
`HUGEPAGE_SIZE` | `2097152` | system hugepage size
`BOUNDS_CHECKS` | undefined | enable supplementary bounds checking
`NDEBUG` | undefined | disable debuging stuff (debug includes bounds checking)


## Profiling
Profiling is a strong word for what [raysnap.h](src/include/api/raysnap.h) offers. The plan is to have good support for monitoring the state of the allocator as well as ways to compute usage patterns and statistics, and make configuration changes at runtime.
At the moment, ony the ability to take snapshots and serialize them (terminal, csv, binary) is implemented, but support for more is definitely in the works.

Snapshots are what they sound like: copies of all the allocator data structures at specific moments. They can be used for debugging or for inspecting the allocation space. They are very easy to make and visualize (see [raysnap_quickie](src/include/api/raysnap.h)).


## Building
Prerequisites:
- a __c2x-compatible__ C compiler (I use `clang 18.0.0git` and `gcc 13.2.1`)
- a `GNU Make`-compatible program (I use `GNU Make 4.4.1`)
- `ar` for archiving the static library (I use `GNU ar 2.41`)
- a `bash`-compatible shell (I use `bash 5.2.21` and `zsh 5.9`)

Relevant `make` targets:
- `quicktest` - (default) Compile and run [test/_quick.c](test/_quick.c). Used to quickly and interactively test code changes.
- `build/librayalloc.a` - Compile and achive the static library.
- `runtest` - Phony target that compiles and runs all the tests (except [_quick.c](test/_quick.c)).

Relevant output files:
- [build/librayalloc.a]() - the static library
- [src/include/rayalloc.h](src/include/rayalloc.h) - the user-facing header
- [test/*.log]() - test results


## Testing
Any `[^_]*.c` file in [test](./test) is treated as a test program. When its target is updated by Make, it is compiled and run, and its output is stored in a `*.log` file. Only the exit status of each test is printed during the `make runtest` command, so in case of errors you should check the logs.  
Remember that [_quick.c](test/_quick.c) (note the leading underscore) is _not_ run together with the other tests during `runtest`.
