Memory allocator usage
===

There are 2 parts when it comes to usage:
1. The legacy "malloc" part - used by the std and such.
2. The new allocator - used by me.

## Usage
1. For the old malloc interface, we cannot really expect any specific behavioural pattern, so we should have something that is ok-ish in terms of performance and does not fuck up the new allocator part.
2. For my intents and purposes, I have mostly 3 ways in which I use dynamic memory:
    - I often need to allocate space for many objects of the same type (eg: arrays, tree nodes). These objects might then need to be deallocated in an arbitrary order - so a simple arena allocator would lead to a bit too much dead space.
    - Another thing that is very common is string manipulation (think of my string lib), which depends on being able to allocate arbitrary sizes. Again deallocations happen sometimes very quickly after the allocations and sometimes much later. Reallocaitons are pretty common as well.
    - Least common are one-off allocations of just one object, this is not a use case I aim to optimize for.
As you can see, it basically all boils down to _arrays_, so naturally I want my allocation semantics to already have the concept of arrays. This means that there is no longer a need for duplicate bookkeeping for things like array capacities.

## Definitions:
- commission memory = to get more memory from the OS
- decommission memory = to give back memory to the OS
- allocate memory = to mark (figuratively or in some data structure) an area of commissioned memory as used by the application
- deallocate memory = to mark an area of memory as not used by the application, keeping it commissioned for later
- arena = an area of memory that is managed by this allocator, arenas are the top-level constructs for memory allocation
- blocks = sections of an arena that can be used by an application
I will also often refer to the legacy interface of the allocator as "the old/legacy/malloc allocator" and to the normal interface as "the new/array allocator".

## Technical background
On Linux, there are 2 ways to get more memory from the OS:
1. `brk` - move the end of the process's data segment, effectively changing the amount of dynamic memory available to the process
2. `mmap` - create a mappping between an address range in the process's virtual memory and either a file or some anonymous region of memory the OS gives us

`brk` has less overhead, but it can only grow in one direction and from one place (our current program break) so it basically works in a LIFO way. It is also deprecated in POSIX.1-2001.

`mmap` has more overhead but is more flexible and already provides some locking semantics. It is also the main (and often only) syscall used by other allocator such as jemalloc.

## Other considerations
I talked about how arrays are the main memory consumers. This provides a good opportunity to cut back on the double bookkeeping that takes place usually: the application keeps track of length and capacity, and also the memory allocator has some internal data structure that keeps track of similar information for its blocks/arenas/pages/slabs/buckets/whatever. We can thus think of the new allocator as a low-level dynamic array library. 
Something that would also be unseful is explicit allignments, usage hints, thread safety, some callbacks, and of course debugging and statistics (as compile/link-time options perhaps). 
For the legacy interface, we can still use the same underlying data structures and treat `malloc`-ed areas as a byte array. Because of how `free` works, we might still need to store some form of header. Allignment might not actually be an issue here depending on how it ends up being handled in the new allocator. 
One last thing, this library is not meant to be used for allocating hundreds of gigabytes of memory. This means that the implementation may pose some limitaions in terms of the maximum amounf of managed memory. As a rule of thumb, rayalloc should be able to manage at least 64GiB of memory.

## See also
See [details.md](details.md) for some technical details, and the source files (they have comments ya know) for implementation details.
