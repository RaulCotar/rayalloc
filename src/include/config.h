/* This file is used to configure rayalloc using macros.
 * The order of precedence is: compiler flags > this file > source files
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096UL // usually 4KB
#endif
#ifndef HUGEPAGE_SIZE
#define HUGEPAGE_SIZE 2097152UL // usually 2MB
#endif

#ifndef INDEX_CONSTURCTOR
#define INDEX_CONSTURCTOR true
#endif

#ifndef BUCKET_MAPSIZE // ls /sys/kernel/mm/hugepages for available sizes
// #define BUCKET_MAPSIZE 21 // 2MB (HUGETLB)
// #define BUCKET_MAPSIZE 30 // 1GB (HUGETLB)
#define BUCKET_MAPSIZE 12 // 4KB (not HUGETLB)
#endif

#ifndef BUCKET_ELSIZE
#define BUCKET_ELSIZE 32UL
#endif

#endif /* _CONFIG_H_ */
