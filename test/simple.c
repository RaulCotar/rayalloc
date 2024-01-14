#include <string.h>
#include "rayalloc.h"

int main() {
	raymap_map(0, 0);
	char *str = rayalloc(13, 1, false);
	strcpy(str, "Hello World!");
	rayfree(str);
	raymap_unmap();
	return 0;
}