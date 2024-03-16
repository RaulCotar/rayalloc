#ifndef NDEBUG
#include <stdio.h>
#include "allocator.h"
#include "log.h"

void _dbg_print_index(void) {
	fprintf(logfile, "Heap index: %p+%lu\n", (void*)heap_index, heap_index->map_size);
	fprintf(logfile, "ranges: %lu\n", heap_index->ran_cnt);
	for (u64 i=0; i<heap_index->ran_cnt; i++) {
		u64 const len = (i+1 < heap_index->ran_cnt)? TP_ADRS(heap_index->ranges[i+1])-TP_ADRS(heap_index->ranges[i]) : 0;
		fprintf(logfile, "%3lb %p %lu\n", TP_TAG(heap_index->ranges[i]), TP_ADRS(heap_index->ranges[i]), len);
	}
	fprintf(logfile, "buckets: %lu\n", heap_index->buk_cnt);
	for (struct bucket *buk=INDEX_BUKLO; buk < INDEX_BUKHI; buk++) {
		fprintf(logfile, "%p: {", TP_ADRS(buk->map));
		for (u32 bit=0; bit < 1<<(BUCKET_MAPSIZE-3); bit++)
			if (buk->slots[bit/8] & 1<<bit%8)
				fprintf(logfile, " %u", bit);
		fputs(" }\n", logfile);
	}
}

#endif /* ifndef NDEBUG */
