Macro Configuration Options
===

You can configure certain aspecs of `rayalloc` by defining some macros when compiling the library, or when including a header in your application.

macro | default | when | description
---|---|---|---
`TLCACHE_SIZE` | `8` | comp | the number of spans in the thread span cache
`SPAN_STEP_1` | `8` | comp | size (in bytes) of step 1
`SPAN_STEP_2` | `32` | comp | size (in bytes) of step 2
`SPAN_STEP_3` | `64` | comp | size (in bytes) of step 3
`SPAN_STEP_4` | `256` | comp | size (in bytes) of step 4
`SPAN_STEP_5` | `1024` | comp | size (in bytes) of step 5
`PAGE_SIZE` | `4096` | comp | system page size
