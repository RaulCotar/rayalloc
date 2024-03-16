#include "log.h"
#include "include/config.h"

FILE *logfile = NULL;

#if LOGFILE_CONSTURCTOR == true
[[gnu::constructor(200), gnu::nothrow, gnu::leaf]]
static void _logfile_constructor(void) {
	logfile = stderr;
	DLOGV("logfile set to stderr.");
}
#endif
