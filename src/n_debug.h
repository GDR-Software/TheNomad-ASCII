#ifdef _NOMAD_DEBUG

#ifndef _N_DEBUG_
#define _N_DEBUG_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

//#include <zip.h>
//#include <zlib.h>

#define PATH "Files/debug"
#define LOG_PATH "Files/debug/debuglog.txt"

#define LOG(...) \
{ \
	FILE* dbgfile = fopen(LOG_PATH, "a"); \
	fprintf(dbgfile, "%s: ", __func__); \
	fprintf(dbgfile, __VA_ARGS__); \
	fprintf(dbgfile, "\n"); \
	fclose(dbgfile);                              \
}

#endif

#else
#endif