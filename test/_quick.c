#include <stdio.h>
#include "api/rayalloc.h"
#include "api/raysnap.h"

int main() {
	puts("\e[31mmap--------------------------------------------------\e[39m");
	ray_config(rayconf_tl_map);
	raysnap_quickie(RAYSNAP_TL, stdout);

	puts("\e[31mallocate str---------------------------------------------\e[39m");
	char *str = rayalloc(30, 1, false);
	raysnap_quickie(RAYSNAP_TL, stdout);
	if (!str) puts("str is NULL!");
	else {
		for (int i=0; i<13; i++)
			str[i] = "Hello World!"[i];
		puts(str);
	}

	puts("\e[31mallocate str2-------------------------------------------\e[39m");
	char *str2 = rayalloc(35, 1, false);
	raysnap_quickie(RAYSNAP_TL, stdout);
	if (!str2) puts("str2 is NULL!");
	else {
		for (int i=0; i<12; i++)
			str2[i] = "Salut lume!"[i];
		puts(str2);
	}

	puts("\e[31mfree str------------------------------------------------\e[39m");
	rayfree(str);
	raysnap_quickie(RAYSNAP_TL, stdout);

	puts("\e[31mallocate str3-------------------------------------------\e[39m");
	char *str3 = rayalloc(13, 1, false);
	raysnap_quickie(RAYSNAP_TL, stdout);
	if (!str3) puts("str3 is NULL!");
	else {
		for (int i=0; i<12; i++)
			str3[i] = "Salut lume!"[i];
		puts(str3);
	}

	puts("\e[31mfree str2-----------------------------------------------\e[39m");
	rayfree(str2);
	raysnap_quickie(RAYSNAP_TL, stdout);

	puts("\e[31mallocate str4--------------------------------------------\e[39m");
	char *str4 = rayalloc(262100, 1, false);
	raysnap_quickie(RAYSNAP_TL, stdout);
	if (!str4) puts("str4 is NULL!");
	else {
		str4[0]=1;
		for (int i=0; str4[i?i-1:0]; i++)
			str4[i] = "This is a looooong string."[i];
		puts(str4);
	}

	puts("\e[31munmap-----------------------------------------------\e[39m");
	ray_config(rayconf_tl_unmap);
	putchar('\n');
	return 0;
}