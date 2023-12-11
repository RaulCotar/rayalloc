RayAlloc <small>- very much WIP</small>
===
Insert explanation of how most allocations are arrays and how that leads to bookkeeping duplication. Introduce rayalloc as a lightweight, non-intrusive 2-in-1 library for memory allocation and dynamic arrays.


## Building
Prerequisites:
- a C compiler (I test with `clang 18.0.0git` and `gcc 13.2.1`)
- a `GNU Make`-compatible program (I use `GNU Make 4.4.1`)
- `ar` for the static library (I use `GNU ar 2.41`)
- a `bash`-compatible shell (I use `bash 5.2.21` and `zsh 5.9`)

After that it's as simple as running `make`! You can choose between debug and release builds by defining `NDEBUG` in your `make` command (_feature not yet implemented, debug builds only for now_). Here are some noteworthy targets:
- `rayalloc.o`, `rayalloc.a`, `rayalloc.so` - build the library
- `test` run the tests (default target)

The [Makefile](Makefile) is simple (just has a lot of `echo`s) so don't be afraid to make changes or replace it completely.


## Testing
Tests are built and run using make and simple bash commands. The first line of each test file is read and a build command is issued according to the compiler and arguments specified in the test file. So to create a test, put a .c file in the test directory and make sure it has that top comment with the build command. Take a look at the existing examples and the Makefile for more insight.