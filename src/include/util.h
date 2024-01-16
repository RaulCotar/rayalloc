#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

#ifndef NULL
#define NULL nullptr
#endif

#define dprintf(...) printf(DBG_INTRO __VA_ARGS__)

// my errno equivalent
typedef i32 ierr;
#define IERR_OK 0
#define IERR_CANTMAP 2
#define IERR_CANTREMAP 3
#define IERR_CANTUNMAP 4
#define IERR_UNKNOWN -1

#endif /* _UTIL_H_ */
