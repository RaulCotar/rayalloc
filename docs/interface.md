The interface should have 2 parts:
1. the legacy "malloc" interface - for compatibility with the rest of the standard library (optional, enbled by macro and in separate header)
2. the new allocator interface - what I will use in my projects


function | description
:--|:--
`void *malloc(u64 size)` | malloc replacement, see [dedicated section](#malloc_and_free)
`void *realloc(void *ptr, u64 size)` | realloc replacement, see [dedicated section](#malloc_and_free)
`void free(void *ptr)` | free replacement, see [dedicated section](#malloc_and_free)


## Typedefs
typedef | underlying type
:---|:---
`u64` | same as for `uint64_t`
`u32` | same as for `uint32_t`
`u16` | same as for `uint16_t`
`u8` | same as for `uint8_t`

## `malloc` and `free`
malloc may work like allocating a non-dynamic array with 16B allignment  
free works in misterious ways?