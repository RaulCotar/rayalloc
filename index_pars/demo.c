#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

struct selection {
	unsigned ar; // the index in the array argument list (eg: 0 = first array)
	bool var_beg, var_end, var_stride, var_span;
	long beg, end, stride, span;
	// if the flags above are set, the values represent the index of the argument
	// as counted after the last array and from the first variable (first var is 0)
	//
	// eg: mix("2[d::5]", ar, x) though invalid, translates to {1, 1,0,0,0, 3, -1, 5, 1}
};

void mix(char const *fmt, ...) {
	va_list va;
	va_start(va, fmt);

	#define DUMVAL 20 // dummy value for internal buffer sizes, replace pls
	// arrays are referred to with single digits, variables with single alpabetical chars
	//
	// the final thing will process the arrays
	// in that case, the first pass should probably build some internal structure
	// that is then sanity-checked and further used to mix the arrays
	// remember arg order: (fmt_str, arrays..., variables...)

	// TODO: review code so far and try to test by printing the generated `struct selection`
	
	struct selection sel[DUMVAL];
	int sel_len = 0;

	char const *c = fmt;
	while (*c) {
		if (isdigit(*c)) {
			sel[sel_len++] = (struct selection) {*c-'0', 0, 0, 0, 0, 0, -1, 1, 1};
		}
		else if (*c == '[') {
			for (long *atr = &sel[sel_len-1].beg; atr<= &sel[sel_len-1].span; atr++) {
				c++;
				if (*c == ':')
					continue;
				if (*c == ']')
					break;
				else if (*c=='-' && *(c+1)=='0')
					sel[sel_len-1].beg = ~0, c+=2; // note that the user value "-0" is stored as ~0 since we don't know the array length yet
				else if (isdigit(*c))
					sel[sel_len-1].beg = strtol(c, (char**)&c, 10);
				else if (isalpha(*c)) {
					sel[sel_len-1].var_beg = true;
					sel[sel_len-1].beg = *c - 'a';
					c++;
				}
				else { printf("PARSING ERROR AT: %s\n", c); return; }
				if (*c == ':') c++; // eat possible delimiter
			}
			c++;
		}
	}

	va_end(va);
}

int main() {
	// TODO: call mix and print result
	return 0;
}
