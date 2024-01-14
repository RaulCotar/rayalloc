#include "util.h"
#include "config.h"
#include "map.h"
#include "array.h"

thread_local static struct map tlmc[TLMC_SIZE];
thread_local static union ar tlac[TLAC_SIZE];
