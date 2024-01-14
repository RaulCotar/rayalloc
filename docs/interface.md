RayAlloc Interface
===

The interface should have 2 parts:
1. the legacy "malloc" interface - for compatibility with the rest of the standard library (optional enabled by macro, has a separate header and built library file)
2. the new allocator interface - combined with the dynamic array part of the library

## Dynamic arrays
create new
create from C array
copy
free
grow
shrink
append elem
append arr
prepend elem
prepend arr
split
generic mix*
foreach
iterator?

### Generic mix
The `arr_mix` operation is similar to string formatting in a way: you give it a format and a couple of arrays and it combines them accordingly. The format is given in the form of a string. For example:
```c
typedef /*my array type*/ ar_t;
#define arr_from ...
#define arr_of ...
#define arr_new ...

ar_t const hello = arr_from("Hello !"); // has a trailing NULL
ar_t const raul = arr_of('R', 'a', 'u', 'l'); // does not have a trailing NULL

ar_t const r1 = arr_mix("1[:?]+2+1[?:-1]", hello, 7, raul, 6);
// r1 = "Hello Raul!" without trailing NULL.
```

operator | description | example  with M={1,2,3,4}, N={a,b,c,d}
---|---|---
`+` | concatenate | `("1 + 2", M, N)` => `"1234abcd"`
`%` | 1:1 interleave | `("1 % 2", M, N)` => `"1a2b3c4d"`
`(x%y)` | x:y interleave | `("1 (2%3) 2", M, N)` => `"12abc34d"`
`*` | repeat | `("1[:2] *3", M)` => `"121212"`

Indexing: `ARRAY[BEGINING:END:STRIDE:SPAN]`:
- `ARRAY` is the index in the variable argument list of the array
- `BEGINING` is the _inclusive_ circular index of the start element (default `0`)
- `END` is the _exclusive_ circular index of the end element (default `-0`)
- `STRIDE` is the increment applied at each iteration (default `1`)
- `SPAN` is the number of elements selected during each iteration (default `1`)

For example, the following indexing expressions are equivalent (non-exhaustive list):
- `1`
- `1[:]`
- `1[::]`
- `1[:::]`
- `1[0:]`
- `1[:-0]`
- `1[0:-0]`
- `1[0:-0:]`
- `1[:0:1]`
- `1[0:-0:1]`
- `1[0:-0:1:]`
- `1[0:-0:1:1]`
- `1[::1:1]`

Here are a few indexing example with results:  
(`A = {0,1,2,3,4,5,6,7,8,9}`)
- `arr_mix("1", A)` -> `0123456789`
- `arr_mix("1[2:9:2]", A)` -> `2468`
- `arr_mix("1[2:9:2:2]", A)` -> `23568`
- `arr_mix("1[:?]", A, -0)` -> ` ` (empty array)
- `arr_mix("1[:-?]", A, 0)` -> `0123456789`

One notable application of this complex indexing system is that one can perform SOA<->AOS conversions:
```c
struct {int n; float f;} raw[3] = {{0, 0.5}, {1, 1.5}, {2, 2.5}};
ar_t aos = arr_from((char*)raw);
struct {int n[3]; float f[3];} soa;
soa.n = arr_mix("1[::?:?]", aos, sizeof(float), sizeof(int)).data;
soa.f = arr_mix("1[?::?:?]", aos, sizeof(float), sizeof(int), sizeof(float)).data;
// soa = {
// 	{0, 1, 2},
//  {0.5, 1.5, 2.5}
// }
```


## Functions
function | description
:--|:--
`union array_g *rayalloc(unsigned size, unsigned elsize, unsigned flags)` | todo
`union array_g *rayalloc_s(unsigned size, unsigned elsize, unsigned flags)` | stack todo
`void rayfree(union array_g *array, unsigned hints)` | todo

function | description
:--|:--
`void *malloc(unsigned long size)` | malloc replacement, see [dedicated section](#malloc_and_free)
`void *realloc(void *ptr, unsigned long size)` | realloc replacement, see [dedicated section](#malloc_and_free)
`void free(void *ptr)` | free replacement, see [dedicated section](#malloc_and_free)


## `malloc` and `free`
`malloc` can work like allocating a non-dynamic array with 16B alignment and element size.