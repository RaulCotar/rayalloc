RayAlloc-WIP
===
Insert explanation of how most allocations are arrays and how that leads to bookkeeping duplication. Introduce rayalloc as a lightweight, non-intrusive 2-in-1 library for memory allocation and dynamic arrays.


## Table of Contents
1. [RayAlloc](#rayalloc-wip) . . . . . . . . . . . . . . . . . . . . . . . . . . . title section
1. [Interface](#interface) . . . . . . . . . . . . . . . . . . . . . . . . . . . API walkthrough
1. [Building](#building) . . . . . . . . . . . . . . . . . . . . . . . . . . . . compilation guide
1. [Testing](#testing) . . . . . . . . . . . . . . . . . . . . . . . . . . . . . about automated tests
1. [TODO](#testing)
1. [Scratchpad](#scratchpad)


## Interface
Well, so far there is none really, but the plan is to have the main interface be the array library part of rayalloc, with an optional header+library that you can include that provides the `malloc` interface (using the array one under the hood).


## Building
Prerequisites:
- a C compiler (I test with `clang 18.0.0git` and `gcc 13.2.1`)
- a `GNU Make`-compatible program (I use `GNU Make 4.4.1`)
- `ar` for the static library (I use `GNU ar 2.41`)
- a `bash`-compatible shell (I use `bash 5.2.21` and `zsh 5.9`)

After that it's as simple as running `make`! You can choose between debug and release builds by defining `NDEBUG` in your `make` command (_feature not yet implemented, debug builds only for now_). Here are some noteworthy targets:
- `rayalloc.o`, `rayalloc.a`, `rayalloc.so` - build the library
- `test` run the tests (default target)

The [Makefile](Makefile) is simple (just has a lot of `echo`s) so don't be afraid to make changes or replace it completely. The build command can be as simple as `gcc rayalloc.c`.


## Testing
Tests are built and run using make and simple bash commands. The first line of each test file is read and a build command is issued according to the compiler and arguments specified in the test file. So to create a test, put a .c file in the test directory and make sure it has that top comment with the build command. Take a look at the existing examples and the Makefile for more insight. Btw, you can redirect test output to separate files instead of stdout by defining TEST_OUT_REDIR in the make command.


## TODO: (see [the scratchpad](#scratchpad))
- figure out the alloc and free algos
	- write them down
	- impleent them
	- wite tests as you go


## Scratchpad
- alloc algo: (for span steps 0-5, steps 6,7 are exceptions)
	- we get a size, some flags, and possibly some hints from the application
	- we search for a span with a suitable step size:
		- search `tlcache`
		- if the CGSL is non-empty, go through it as well
	- if the search was succesful:
		- if there is not enough free capacity, grow the span
			- if growing is not possible search for another span
	- else:
		- create a new span with the step and size we need:
			- if the tlcache is not full, put it there
			- else put it in the CGSL
	- create a new array in the selected span
	- ?put that span/array at the front of its respective list ?optimization
	- return to application, we are done

- free algo: (self-discussion)
	- there is a problem: we need to locate where the hell the array the user gives us is, and if it is even an array at all!
	- we could go through all the spans, look at their sizes and see if the address we have falls within one of them. Even then, what if the user fills the array with header-like data, and then tries to free an address somewhere in the middle of the array? And if that happens, does it leave the span in an inconsistent state though? If the half before the free point looks valid and we have no intra-span data struct that gets affected by this free, then no, the application has managed to split an array :) . 
	- if we want to be extra safe, we should have some intra-span linked list or sth, but that takes space and complexity
	- I think the best is to have each array store its size and kind of form a liked list of offsets inside the span, maybe have the span header store a ptr to the first free array to speed up alloc search. A purposefully-crafted free call can allow the application to split an array, but that's acceptable since array overflows can already mess/merge arrays.
	- to recap, the array solution from the previous version (chunks) can be slightly alterred and used here
	- __RAUL README__: bc we have step sizes, aray sizes can be less bits! this way we can achieve more size-efficient arrays and 4B allignment (last one not a feature tho)
