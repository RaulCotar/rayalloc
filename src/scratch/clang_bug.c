#include <stdio.h>
#include <string.h>
#include <ctype.h>

bool alt; // false

int foo_1(int x) {
    return 10*x;
}

int foo_2(int x) {
    return x/10;
}

int (*resolve_foo(void))(int) {
    if (alt)
        return (void*)foo_1;
    return (void*)foo_2;
}

int foo (int x) __attribute__ ((ifunc ("resolve_foo")));

[[gnu::constructor(101)]]
void setalt(void) {
	alt = true;
}

int main() {
    printf("foo(25) = %d\n", foo(25));
    alt = false;
    printf("foo(25) = %d\n", foo(25));
    return 0;
}
