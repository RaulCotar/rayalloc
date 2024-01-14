/* This file is used to configure rayalloc using macros.
 * See docs/macro_confg.md for more details.
 * The order of precedence is: compiler flags > this file > source files
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef PAGE_SIZE // sytem page size
 #define PAGE_SIZE 4096 // should prob use sysconf(_SC_PAGESIZE), but I'm lazy
#endif

#ifndef HUGEPAGE_SIZE // sytem default hugepage size
 #define HUGEPAGE_SIZE 2097152 // 2 MiB on my PC
#endif

#ifndef TLMC_SIZE // thread local map cache size
 #define TLMC_SIZE 8
#endif

#ifndef TLAC_SIZE // thread local array cache size
 #define TLAC_SIZE 32
#endif

#endif /* _CONFIG_H_ */
