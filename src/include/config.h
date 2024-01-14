/* This file is used to configure rayalloc using macros.
 * The order of precedence is: compiler flags > this file > source files
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef PAGE_SIZE
 #define PAGE_SIZE 4096 // should prob use sysconf(_SC_PAGESIZE), but I'm lazy
#endif

#ifndef HUGEPAGE_SIZE // sytem default hugepage size
 #define HUGEPAGE_SIZE 2097152 // 2 MiB on my PC
#endif

#ifndef ACACHE_SIZE
 #define ACACHE_SIZE 8
#endif

#ifndef CANARY_VAL
 #define CANARY_VAL ((u64)0xdeadbeef)
#endif

#ifndef DBG_INTRO
 #define DBG_INTRO "\e[33m \e[39m "
#endif

#endif /* _CONFIG_H_ */
