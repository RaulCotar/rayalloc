#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>
#include "config.h"

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

#if defined(BOUNDS_CHECKS) || !defined(NDEBUG)
	#define ifBC(...) __VA_ARGS__
#else
	#define ifBC(...)
#endif

#ifndef NDEBUG
	#include <stdio.h> // IWYU pragma: export
	#define ifDBG(...) __VA_ARGS__
	#define dlogm(FMT, ...) fprintf(stderr, DLOG_FMTM FMT "\n" DLOG_ARGSM __VA_OPT__(,) __VA_ARGS__)
	#define dlogw(FMT, ...) fprintf(stderr, DLOG_FMTW FMT "\n" DLOG_ARGSW __VA_OPT__(,) __VA_ARGS__)
	#define dloge(FMT, ...) fprintf(stderr, DLOG_FMTE FMT "\n" DLOG_ARGSE __VA_OPT__(,) __VA_ARGS__)
#else
	#define ifDBG(...)
	#define dlodlogm(...)
	#define dlodlogw(...)
	#define dlodloge(...)
#endif

// Similar to errno
typedef i32 ierr;
#define IERR_OK 0
#define IERR_CANTMAP 2
#define IERR_CANTREMAP 3
#define IERR_CANTUNMAP 4
#define IERR_ALREADY 5
#define IERR_BADINPUT 6
#define IERR_UNKNOWN -1

#endif /* _UTIL_H_ */
