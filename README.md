`RayAlloc` - WIP
===
Insert explanation of how most allocations are arrays and how that leads to bookkeeping duplication. Introduce rayalloc as a lightweight, 2-in-1 library for memory allocation and dynamic arrays writen in C2X.
Check out the [docs folder](docs) for more in-depth information about the internals. See the [todo list](./todo.md) if you are interested in the near-future plans.


## Table of Contents
1. [RayAlloc](#rayalloc---wip) . . . . . . . . . . . . . . . . . . . . . . . . . . title section
1. [Interface](#interface) . . . . . . . . . . . . . . . . . . . . . . . . . . . API walkthrough
1. [Configuration](#configuration) . . . . . . . . . . . . . . . . . . . . . . . macro config
1. [Building](#building) . . . . . . . . . . . . . . . . . . . . . . . . . . . . compilation guide
1. [Testing](#testing) . . . . . . . . . . . . . . . . . . . . . . . . . . . . . about automated tests


## Interface
The interface has 3 parts: the main array interface, the "malloc" one and the debug and profiling stuff. The "malloc" interface is just a very thin wrapper (like 20 SLOC thin) around the array interface for the sake of having a `malloc` replacement. Check the headers for the complete interface. Here you can see a non-exhautive list of functions and objects:
function/object | header | description
:--|:--|:--
`void *rayalloc(u64 cap, u64 elsize, bool raw)` | [rayalloc.h](src/include/rayalloc.h) | allocate a new array
`void rayfree(void *ptr)` | [rayalloc.h](src/include/rayalloc.h) | deallocate an array
`void rayresize(void *ptr)` | [rayalloc.h](src/include/rayalloc.h) | resize a managed array (not implemented yet)
`void map_dbg_print(void)` | [rayalloc.h](src/include/rayalloc.h) | debug print the state of the allocator, almost identical output to `raysnap_quickie`
`ierr raymap_map(u64 size_hint, int add_mmap_flags)` | [raylloc.h](src/include/rayalloc.h) | initilize backing memory map
`ierr raymap_resize(u64 new_size)` | [raylloc.h](src/include/rayalloc.h) | resize backing memory map
`ierr raymap_unmap(void)` | [raylloc.h](src/include/rayalloc.h) | free backing memory map
`raysnap_t *raysnap_snapshot(void)` | [raysnap.h](src/include/raysnap.h) | take a snapshot of the allocator state
`void raysnap_print(raysnap_t const *ss, void *file)` | [raysnap.h](src/include/raysnap.h) | print a raysnap
`void raysnap_csv(raysnap_t const *ss, void *file)` | [raysnap.h](src/include/raysnap.h) | serialize a raysnap to csv
`void raysnap_quickie(void *file)` | [raysnap.h](src/include/raysnap.h) | snapshot+print combo, alsmot identical output to `map_print_dbg`

## Cofiguration
The file [`config.h`](./src/config.h) contains a series of macros that can be used to configure rayalloc. Compiler arguments take precedence over the defaults in [`config.h`](./src/config.h), so you can use `-D` flags instead of editing the file. Here is a non-comprehensive list of them:
macro | default value | description
:--|:--|:--
`ACACHE_SIZE` | `8` | number of entries in the thread local array cache
`BOUNDS_CHECKS` | undefined | enable supplementary bounds checking
`NDEBUG` | undefined | disable debuging stuff


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
