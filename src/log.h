#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h> // IWYU pragma: export

extern FILE *logfile;

#ifndef NDEBUG
	#define ifDBG(...) __VA_ARGS__
	#define DLOGM(FMT, ...) fprintf(logfile, "[%s:%d]: " FMT "\n", __func__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
	#define DLOGW(FMT, ...) fprintf(logfile, "\e[93m[%s:%d]:\e[39m " FMT "\n", __func__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
	#define DLOGE(FMT, ...) fprintf(logfile, "\e[91m[%s:%d]:\e[39m " FMT "\n", __func__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
	#define LOGM(FMT, ...) DLOGM("Info: " FMT __VA_OPT__(,) __VA_ARGS__)
	#define LOGW(FMT, ...) DLOGW("\e[33mWarning:\e[39m " FMT __VA_OPT__(,) __VA_ARGS__)
	#define LOGE(FMT, ...) DLOGE("\e[31mError:\e[39m " FMT __VA_OPT__(,) __VA_ARGS__)
#else
	#define ifDBG(...)
	#define DLODLOGM(...)
	#define DLODLOGW(...)
	#define DLODLOGE(...)
	#define LOGM(FMT, ...) fprintf(logfile, "Info: " FMT "\n" __VA_OPT__(,) __VA_ARGS__)
	#define LOGW(FMT, ...) fprintf(logfile, "\e[33mWarning:\e[39m " FMT "\n" __VA_OPT__(,) __VA_ARGS__)
	#define LOGE(FMT, ...) fprintf(logfile, "\e[31mError:\e[39m " FMT "\n" __VA_OPT__(,) __VA_ARGS__)
#endif

#endif /* _LOG_H_ */
