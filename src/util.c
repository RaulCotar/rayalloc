#if NEED_BUILTINS == true
#include "util.h"
int __builtin_ffs(int const x) {
	for (int i=0; i<(int)(8*sizeof(int)); i++)
		if (x & 1<<i)
			return i + 1;
	return 0;
}
#endif /* if NEED_BUILTINS == true */
