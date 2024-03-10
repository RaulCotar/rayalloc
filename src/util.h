#ifndef _UTIL_H_
#define _UTIL_H_

// fighting the compiler

#ifndef NULL
	#define NULL ((void*)0)
#endif

#ifndef unreachable
	#define unreachable() __builtin_unreachable()
#endif

// bitops and arithmetic macros

#define roundToMult(uint_A, uint_B) (((((uint_A)?:1)-1)/(uint_B)+1)*(uint_B))


// mmap stuff

#define PROT_RW (PROT_READ|PROT_WRITE)
#define MAP_ANONPRIV (MAP_ANONYMOUS|MAP_PRIVATE)
#ifndef MAP_UNINITIALIZED
	#define MAP_UNINITIALIZED 0
#endif


#endif /* _UTIL_H_ */
