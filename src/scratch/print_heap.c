#ifndef NDEBUG
#include "allocator.h"

void print_heap(FILE *file) {
	const struct index *const h = index;
	fprintf(file,
		"\e[1;30;47m### LOCAL HEAP ###\e[22;39;49m\n"
		"%p + %lu with %lu arrays\n",
		h, h->size, h->arlen
	);
	enum:u8 {OUT,IN} group = OUT;
	void const *prev_adr = NULL;
	for (u64 i=0; i<h->arlen+CACHE_SIZE; i++) {
		void const *const p = TP_ADRS(h->ar[i]);
		u64 const len = (h->arlen+CACHE_SIZE - i > 1) ? h->ar[i+1]-h->ar[i] : 0;
		char const * const bg = i % 2 ? "0" : "233";
		char warn[3] = "";
		char const *tag, *grp;
		char detail[32] = "            ";
		switch ((u64)h->ar[i] & TP_TYP) {
			case TP_FRE:
				tag = "\e[32mfre";
				snprintf(detail, sizeof(detail), "%-lub", len);
				goto L_tp_nonedge;
			case TP_RAW:
				tag = "\e[36mraw";
				snprintf(detail, sizeof(detail), "%-lub", len);
				goto L_tp_nonedge;
			case TP_MAN:
				tag = "\e[34mman";
				snprintf(detail, sizeof(detail), "%-lub (TBD)", len);
			L_tp_nonedge:
				if (i>0 && p < TP_ADRS(h->ar[i-1]))
					warn[0] = 'O';
				switch (group) {
					case IN:  grp = "│"; break;
					default:  grp = "?"; break;
				}
				break;
			case TP_EDG:
				if (((u64)h->ar[i] & TP_TAGM) == TP_BEG) {
					tag = "\e[90mbeg";
					group = IN;
					grp = "┌";
					prev_adr = p;
					if (h->arlen+CACHE_SIZE-i>1 && p!=TP_ADRS(h->ar[i+1]))
						warn[0] = 'D';
				}
				else {
					tag = "\e[90mend";
					group = OUT;
					grp = "└";
					snprintf(detail, sizeof(detail), "=> %-lub", p-prev_adr);
				}
		}
		fprintf(file,
			"\e[90m%s\e[48:5:%sm%s  %-12s %-16p \e[1;31m%-3s\e[22;39;49m\n",
			grp, bg, tag, detail, p, warn
		);
	}
}

#endif /* ifndef NDEBUG */
