/* This file is used to configure rayalloc using macros.
 * See docs/macro_confg.md for more details.
 * The order of precedence is: compiler flags > this file > source files
*/
#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef PAGE_SIZE
 #define PAGE_SIZE 4096  // should prob use sysconf(_SC_PAGESIZE), but I'm lazy
#endif

#ifndef SPAN_STEP_1
 #define SPAN_STEP_1 8
#endif
#ifndef SPAN_STEP_2
 #define SPAN_STEP_2 32
#endif
#ifndef SPAN_STEP_3
 #define SPAN_STEP_3 64
#endif
#ifndef SPAN_STEP_4
 #define SPAN_STEP_4 265
#endif
#ifndef SPAN_STEP_5
 #define SPAN_STEP_5 1024
#endif

#ifndef TLSL_SIZE
 #define TLSL_SIZE 8
#endif

#ifndef TLAC_SIZE
 #define TLAC_SIZE 15
#endif

#endif /* _CONFIG_H_ */
