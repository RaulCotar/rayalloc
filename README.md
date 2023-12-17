`RayAlloc` - WIP
===
Insert explanation of how most allocations are arrays and how that leads to bookkeeping duplication. Introduce rayalloc as a lightweight, 2-in-1 library for memory allocation and dynamic arrays.
Check out the [docs folder](docs) for more in-depth information about the internals.


## Table of Contents
1. [RayAlloc](#rayalloc---wip) . . . . . . . . . . . . . . . . . . . . . . . . . title section
1. [Interface](#interface) . . . . . . . . . . . . . . . . . . . . . . . . . . . API walkthrough
1. [Building](#building) . . . . . . . . . . . . . . . . . . . . . . . . . . . . compilation guide
1. [Testing](#testing) . . . . . . . . . . . . . . . . . . . . . . . . . . . . . about automated tests
1. [TODO](#testing)
1. [Scratchpad](#scratchpad)


## Interface
Well, so far there is none really, but the plan is to have the main interface be the array library part of rayalloc, with an optional header+library that you can include that provides the `malloc` interface (using the array one under the hood). Check out [docs/interface.md](docs/interface.md).


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
	- implement them
	- wite tests and docs as you go


## Scratchpad (outdated)
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

- free algo:
	- we get an array address, and possibly some hints from the application
	- we search all known spans (tlcache then cgsl) and using their address and size deduce whether or not the array belongs to one of them
	- if the array cannot be located, throw a \[silent\] error and return
	- go through the selected span, following the offset-based linked list of arrays, and check again that the array address we got is valid
	- turn the selected array into a free one
	- coalesce the new free array with any free neighbours and check if th whole span is now free (and mark is as such)
	- we are done

	On problem with the above algorithm is that we might be thrashing the cache sinc ewe do not take into account time/space locality between 2 or more allocations, nor a deallocation and an allocation. Solution: rename tlcache to tlsl (tl span list) and have a tlac (tl array cache) that has direct pointers to free arrays, arrays that have a flag set and a backpointer to know that they are part of the cache.
