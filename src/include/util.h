#ifndef _UTIL_H_
#define _UTIL_H_

#include "api/util.h" // IWYU pragma: export
#include "config.h"

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

#endif /* _UTIL_H_ */
