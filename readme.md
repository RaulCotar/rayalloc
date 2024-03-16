RayAlloc - WIP
===

__RayAlloc__ is a 2-in-1 library that provides a heap allocator and a dynamic array library. Combining the 2 into a single implementation allows for better memory management with less overall bookkeeping.

Current version is `0.0.5-dev`.  
More detailed documentation will become available when the library is more complete.  
Run `make help` (nice help message) then `make ADDCLFAGS=-Wno-pedantic` to get started.

## What's working
- `memalloc`/`memfree` for buckets and maps

## What's not working
- ranges - not implemeted
- array library - not implemented

## Known issues:
- use of `\e` does not comply with ISO C
- use of `?:` does not comply with ISO C
- some flags in [config.h] may lead to empty translation units - invalid ISO C
- too much verbose debug logging left in code
- remnants of HUGEPAGE usage might still be in there
