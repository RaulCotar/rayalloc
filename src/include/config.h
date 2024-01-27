/* This file is used to configure rayalloc using macros.
 * The order of precedence is: compiler flags > this file > source files
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef ACACHE_SIZE
#define ACACHE_SIZE 16
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096 // should prob use sysconf(_SC_PAGESIZE)
#endif

#ifndef HUGEPAGE_SIZE
#define HUGEPAGE_SIZE 2097152 // 2 MiB default on x64 Linux
#endif

#ifndef DEFAULT_MAP_SIZE
#define DEFAULT_MAP_SIZE ((u64)64 * PAGE_SIZE)
#endif

#ifndef DEFAULT_MAP_MSIZE
#define DEFAULT_MAP_MSIZE ((u64)2<<32) // for testing, otherwise -1
#endif

#ifndef DEFAULT_FF_ALGO
#define DEFAULT_FF_ALGO rayFF_CSMF
#endif

#ifndef DLOG_FMTM
#define DLOG_FMTM "\e[94;1m %s>\e[39;22m "
#endif
#ifndef DLOG_ARGSM
#define DLOG_ARGSM ,__func__
#endif

#ifndef DLOG_FMTW
#define DLOG_FMTW "\e[93;1m %s:%d>\e[39;22m "
#endif
#ifndef DLOG_ARGSW
#define DLOG_ARGSW ,__func__, __LINE__
#endif

#ifndef DLOG_FMTE
#define DLOG_FMTE "\e[91;1m %s:%d>\e[39;22m "
#endif
#ifndef DLOG_ARGSE
#define DLOG_ARGSE ,__func__, __LINE__
#endif

#endif /* _CONFIG_H_ */
