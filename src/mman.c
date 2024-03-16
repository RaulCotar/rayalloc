#ifndef __USE_GNU
#include "mman.h"
#include <stdarg.h>
#include "log.h"

[[gnu::nothrow]]
void *mremap(void *const old_address, u64 const old_size, u64 const new_size, int const flags, ...) {
	if ((flags & MREMAP_MAYMOVE) == 0) {
		DLOGW("attention: my mremap implementation is questionable, especially without MAP_MAYMOVE");
		return mmap(old_address, new_size, PROT_RW, MAP_ANONPRIV, -1, 0);
	}
	void *new_addr = NULL;
	if (flags & MREMAP_FIXED) {
		va_list ap;
		va_start(ap);
		new_addr = va_arg(ap, void*);
		va_end(ap);
	}
	void *new = mmap(new_addr, new_size, PROT_RW, MAP_ANONPRIV, -1, 0);
	if (new == MAP_FAILED) {
		DLOGE("mmap failed");
		return new;
	}
	DLOGV("remapping %p+%lu to %p+%lu", old_address, old_size, new, new_size);
	for (u64 i=0; i<(old_size<new_size? old_size:new_size)/8; i++)
		((u64*)new)[i] = ((u64*)old_address)[i];
	if ((flags & MREMAP_DONTUNMAP) == 0)
		if (munmap(old_address, old_size)) {
			LOGE("Could not unmap the old index - potential memory leak!");
			munmap(new, new_size);
			return MAP_FAILED;
		}
	return new;
}

#endif /* ifndef __USE_GNU */
