//clang -O0 --std=c99 -rdynamic -fno-omit-frame-pointer -fsanitize=undefined -fsanitize=address rayalloc.o
#include <string.h>
#include <stdio.h>
#include "../rayalloc.h"
int main() {
	char *buf = alloc_array(128, 1);
	memcpy(buf, "Hello World!", 13);
	puts(buf);
	// memory leaks here (missing free)
	return 0;
}
