#include "allocator.h"

[[gnu::nothrow]]
void free(void *ptr) {
	memfree(ptr, 0);
}

[[gnu::nothrow, gnu::warn_unused_result, gnu::malloc, malloc(free, 1), gnu::alloc_size(1)]]
void *malloc(u64 size) {
	return memalloc(size, 0);
}

[[gnu::nothrow, gnu::warn_unused_result, gnu::malloc, malloc(free, 1), gnu::alloc_size(1, 2)]]
void *calloc(u64 nmemb, u64 size) {
#ifdef __GNUC__
	if ((u128)nmemb * (u128)size > UINT64_MAX)
		return nullptr;
	return memalloc(size * nmemb, 0);
#else
	return nullptr;
#endif
}

[[gnu::nothrow, gnu::warn_unused_result, gnu::alloc_size(2)]]
[[deprecated("Not implemented")]]
void *realloc(void *ptr, u64 size) {
	(void)ptr, (void)size;
	return nullptr;
}
