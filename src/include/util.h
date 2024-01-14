#ifndef _UTIL_H_
#define _UTIL_H_

typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long i64;
typedef int i32;
typedef short i16;
typedef char i8;

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
