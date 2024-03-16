#ifndef _INCLUDE_TYPEDEFS_H_
#define _INCLUDE_TYPEDEFS_H_
#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t	u8;
typedef int64_t	i64;
typedef int32_t	i32;
typedef int16_t	i16;
typedef int8_t	i8;

#ifdef __GNUC__
typedef unsigned __int128 u128;
typedef signed __int128 i128;
#endif

#define uN(N) (unsigned _BitInt((N)))
#define iN(N) (signed _BitInt((N)))

#endif /* _INCLUDE_TYPEDEFS_H_ */
