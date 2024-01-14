// #include <sys/resource.h>
// #include <sys/mman.h>
// #include <unistd.h>
// #include <string.h>
#include <stdio.h>
#include "util.h"

int main() {
	union {
		u64 a;
		u8 b;
	} u;
	u.a=16;
	printf("%u\n", u.b);
	return 0;
}