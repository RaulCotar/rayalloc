#ifndef _RAYALLOC_H_
#define _RAYALLOC_H_

// first 2 bits are growth rate indicators
#define RA_IDKGRO 0 // no insight available (default)
#define RA_NOGRO 1 // the array will not grow at all
#define RA_LOGRO 2 // the array will not grow much
#define RA_HIGRO 3 // the array will grow a lot

// allocate an array on "the heap"
union array_g *rayalloc(unsigned size, unsigned elsize, unsigned flags);

#endif /* _RAYALLOC_H_ */
