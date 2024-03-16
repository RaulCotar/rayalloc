// This file is used to configure rayalloc using macros.

#ifndef _INCLUDE_CONFIG_H_
#define _INCLUDE_CONFIG_H_

#ifndef PAGE_SIZE // TODO: idea: make these vars and have a constructor
#define PAGE_SIZE 4096UL // usually 4KB
#endif

#ifndef NEED_BUILTINS // enable if compiler is complaining about missing __builtin*
#define NEED_BUILTINS false
#endif

#ifndef INDEX_CONSTURCTOR
#define INDEX_CONSTURCTOR true
#endif

#ifndef LOGFILE_CONSTURCTOR
#define LOGFILE_CONSTURCTOR true
#endif

#ifndef VERBOSE
#define VERBOSE false
#endif

#ifndef BUCKET_MAPSIZE // ls /sys/kernel/mm/hugepages for available sizes
// #define BUCKET_MAPSIZE 21 // 2MB (HUGETLB)
// #define BUCKET_MAPSIZE 30 // 1GB (HUGETLB)
#define BUCKET_MAPSIZE 12 // 4KB (normal TLB)
#endif

#ifndef BUCKET_ELSIZE
#define BUCKET_ELSIZE 32UL
#endif

#endif /* _INCLUDE_CONFIG_H_ */
