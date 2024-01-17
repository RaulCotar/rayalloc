#include <stdio.h>
#include "acache.h"
#include "config.h"
#include "rayalloc.h"
#include "util.h"

thread_local ar_t *acache[ACACHE_SIZE]; // keep sorted at all times!

void acache_append(ar_t const *ar, bool quick) {
	ifBC(
		if ((void*)ar >= _ray_map+_ray_map_size || (void*)ar < _ray_map)
			printf(DBG_INTRO "acache_replace: %p out of bounds\n", ar);
	)
	ifDBG(
		if (ar->flags & AR_USED)
			printf(DBG_INTRO "acache_replace: %p is not free\n", ar);
	)

	u32 i = 0;
	while (i < ACACHE_SIZE)
		if (!acache[i++])
			break;
	while (--i)
		acache[i] = acache[i-1];
	acache[0] = (ar_t*)ar;
	if (quick)
		return;

	for (i=0; i<ACACHE_SIZE && acache[i]; i++)
		coalesce_fwd(acache[i]);
}

void acache_remove(ar_t const *ar) {
	ifBC(
		if ((void*)ar >= _ray_map+_ray_map_size || (void*)ar < _ray_map)
			printf(DBG_INTRO "acache_remove: %p out of bounds\n", ar);
	)
	ifDBG(
		if (!ar)
			printf(DBG_INTRO "acache_remove: ar is NULL\n");
	)

	u32 i=0;
	while (i < ACACHE_SIZE)
		if (acache[i++] == ar)
			goto L_aca_rem_found;
	return;
	
	L_aca_rem_found:
	while (i < ACACHE_SIZE)
		acache[i-1] = acache[i], i++;
}

void acache_replace(ar_t const *old, ar_t const *new, bool quick) {
	ifBC(
		if ((void*)new >= _ray_map+_ray_map_size || (void*)new < _ray_map)
			printf(DBG_INTRO "acache_replace: %p out of bounds\n", new);
	)
	ifDBG(
		if (new->flags & AR_USED)
			printf(DBG_INTRO "acache_replace: %p is not free\n", new);
	)

	u32 i = 0;
	while (i < ACACHE_SIZE)
		if (acache[i++] == old)
			goto L_aca_rep_found;
	return;

	L_aca_rep_found:
	while (--i)
		acache[i+1] = acache[i];
	acache[0] = (ar_t*)new;
	if (quick)
		return;

	for (i=0; i<ACACHE_SIZE && acache[i]; i++)
		coalesce_fwd(acache[i]);
}
