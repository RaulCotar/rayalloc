// #include <sys/resource.h>
// #include <sys/mman.h>
// #include <unistd.h>
// #include <string.h>
// #include <stdio.h>
#include "util.h"
struct array_s {
	u32 header; // see array header breakdown doc comment
	u32 cap; // capacity = array size / element size
	u8 bytes[4]; // application data

	#define M_ARR_LOCK 0x1 // does this array require acquiring a lock before r/w?
	#define M_ARR_USED 0x2 // is this span used (false = free)?
	#define M_ARR_ELSZ 0xffc // element size (if managed); 0 means non managed
	// unfortunately, there are 20 unused bits in a non-free array header :(
};

u8 foo() {
	struct array_s a[2] = {
		{0, 0, {1,2,3,4}},
		{13, 13, {1,2,3,4}}
	};
	char c=0;
	u8 v = a[0].bytes[4] + c;
	return v;
}

int main() {
	return foo();
}