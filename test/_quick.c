#include <stdio.h>
#include "rayalloc.h"

int main() {
	puts("\e[31mmap map--------------------------------------------------\e[39m");
	raymap_map(0, 0);
	map_dbg_print();

	puts("\e[31mallocate str---------------------------------------------\e[39m");
	char *str = rayalloc(33, 1, false);
	map_dbg_print();
	if (!str) puts("str is NULL!");
	else {
		for (int i=0; i<13; i++)
			str[i] = "Hello World!"[i];
		puts(str);
	}

	puts("\e[31mallocate str2-------------------------------------------\e[39m");
	char *str2 = rayalloc(35, 1, false);
	map_dbg_print();
	if (!str2) puts("str2 is NULL!");
	else {
		for (int i=0; i<12; i++)
			str2[i] = "Salut lume!"[i];
		puts(str2);
	}

	puts("\e[31mfree str------------------------------------------------\e[39m");
	rayfree(str);
	map_dbg_print();

	puts("\e[31mfree str2-----------------------------------------------\e[39m");
	rayfree(str2);
	map_dbg_print();

	puts("\e[31mallocate str3--------------------------------------------\e[39m");
	char *str3 = rayalloc(50, 1, false);
	map_dbg_print();
	if (!str3) puts("str3 is NULL!");
	else {
		str3[0]=1;
		for (int i=0; str3[i?i-1:0]; i++)
			str3[i] = "This is a longer string."[i];
		puts(str3);
	}

	puts("\e[31mmap munmap-----------------------------------------------\e[39m");
	raymap_unmap();
	putchar('\n');
	return 0;
}