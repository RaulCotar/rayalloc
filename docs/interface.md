RayAlloc Interface
===

The interface should have 2 parts:
1. the legacy "malloc" interface - for compatibility with the rest of the standard library (optional enabled by macro, has a separate header and built library file)
2. the new allocator interface - combined with the dynamic array part of the library

## Functions
function | description
:--|:--
`union array_g *rayalloc(unsigned size, unsigned elsize:10, unsigned flags)` | todo
`union array_g *rayalloc_s(unsigned size, unsigned elsize/*<1024*/, unsigned flags)` | todo
`void rayfree(union array_g *array, unsigned hints)` | todo

function | description
:--|:--
`void *malloc(unsigned long size)` | malloc replacement, see [dedicated section](#malloc_and_free)
`void *realloc(void *ptr, unsigned long size)` | realloc replacement, see [dedicated section](#malloc_and_free)
`void free(void *ptr)` | free replacement, see [dedicated section](#malloc_and_free)


## `malloc` and `free`
`malloc` can work like allocating a non-dynamic array with 16B alignment and element size.