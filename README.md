`RayAlloc` - WIP
===
Insert explanation of how most allocations are arrays and how that leads to bookkeeping duplication. Introduce rayalloc as a lightweight, 2-in-1 library for memory allocation and dynamic arrays writen in C2X.
Check out the [docs folder](docs) for more in-depth information about the internals. See the [todo list](./todo.md) if you are interested in the near-future plans.


## Table of Contents
1. [RayAlloc](#rayalloc---wip) . . . . . . . . . . . . . . . . . . . . . . . . . . . title section
1. [Interface](#interface) . . . . . . . . . . . . . . . . . . . . . . . . . . . API walkthrough
1. [Configuration](#configuration) . . . . . . . . . . . . . . . . . . . . . . . macro config
1. [Building](#building) . . . . . . . . . . . . . . . . . . . . . . . . . . . . compilation guide
1. [Testing](#testing) . . . . . . . . . . . . . . . . . . . . . . . . . . . . . about automated tests


## Interface
The interface has (will have rather) 2 parts: the main array interface and the "malloc" one. The later is separate (different header and lib) and is just a very thing wrapper (like 20 SLOC thin) around the array interface for the sake of having a `malloc` replacement. Check the headers for the complete interface. Here you can see a non-exhautive list of functions and objects:
function/object | description
:--|:--
`void *rayalloc(u64 cap, u64 elsize, bool raw)` | allocate a new array
`void rayfree(void *ptr)` | deallocate an array
`void *rayresize(void *ptr, u32 new_cap)` | not yet implemented
`map_dbg_print(void)` | print debug information about the allocator


## Cofiguration
The file [`config.h`](./src/config.h) contains a series of macros that can be used to configure rayalloc. Compiler arguments take precedence over the defaults in [`config.h`](./src/config.h), so you can use `-D` flags instead of editing the file. Here is a list (non-comprehensive) of them:
macro | description
:--|:--
`ACACHE_SIZE` | number of entries in the thread local array cache


## Building
Prerequisites:
- a __c2x-compatible__ C compiler (I use `clang 18.0.0git` and `gcc 13.2.1`)
- a `GNU Make`-compatible program (I use `GNU Make 4.4.1`)
- `ar` for the static library (I use `GNU ar 2.41`)
- a `bash`-compatible shell (I use `bash 5.2.21` and `zsh 5.9`)

Relevant `make` targets:
- `quicktest` - (default) Compile and run [test/_quick.c](test/_quick.c). Used to quickly and interactively test code changes.
- `build/rayalloc.a` - Compile and achive the static library.
- `runtest` - Phony target that compiles and runs all the tests (except [_quick.c](test/_quick.c)).

Relevant output files:
- [build/rayalloc.a](build/) - the static library
- [src/include/rayalloc.h](src/include/rayalloc.h) - the user-facing header
- [test/*.log](test/) - test results


## Testing
Any `[^_]*.c` file in [test](./test) is treated as a test program. When its target is updated by Make, it is compiled and run, and its output is stored in a `*.log` file. Only the exit status of each test is printed during the `make runtest` command, so in case of errors you should check the logs.  
Remember that [_quick.c](test/_quick.c) (note the leading underscore) is _not_ run together with the other tests during `runtest`.