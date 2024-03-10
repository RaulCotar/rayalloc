#include <stdio.h>
#include <sys/mman.h>
#include "../build/include/allocator.h"
// internals
#include "../src/log.h"

int main() {
	logfile = stderr;
	char const *const msg = "This is a 32-byte long string.\n";
	char *buf = memalloc(32, ALLOC_MAP);
	
	for (u32 i=0; i<32; i++) {
		fprintf(stderr, "%c", msg[i]);
		buf[i] = msg[i];
	}
	
	memfree(buf, 0);
	return 0;
}
