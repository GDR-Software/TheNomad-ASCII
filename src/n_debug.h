#ifndef _N_DEBUG_
#define _N_DEBUG_

#ifndef LOGGER_OUTFILE
#define LOGGER_OUTFILE dbg_file
#endif

#ifdef _NOMAD_DEBUG
#include <assert.h>
#define LOG_DEBUG(...)                                  \
{                                                       \
	fprintf(LOGGER_OUTFILE, "[DEBUG](%s): ", __func__); \
	fprintf(LOGGER_OUTFILE, __VA_ARGS__);               \
	fprintf(LOGGER_OUTFILE, "\n");                      \
}
#else
#define assert(x)
#define LOG_DEBUG(...)
#endif
#define DBG_LOG(...) LOG_DEBUG(__VA_ARGS__)

#define TAG_TO_STR(x) \
({char str[100]; \
	switch (x) { \
	case TAG_FREE: \
		strcpy(str, "TAG_FREE"); \
		break; \
	case TAG_STATIC: \
		strcpy(str, "TAG_STATIC"); \
		break; \
	case TAG_MISSION: \
		strcpy(str, "TAG_MISSION"); \
		break; \
	case TAG_PURGELEVEL: \
		strcpy(str, "TAG_PURGELEVEL"); \
		break; \
	case TAG_SCOPE: \
		strcpy(str, "TAG_SCOPE"); \
		break; \
	}; \
	str;})
#define LOG_FREETAGS(lowtag, hightag, nblocks, bfreed)   \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"Zone Daemon Log:\n"                                 \
	"\tlog type    => FREETAGS\n"                        \
	"\tlowtag      => %s\n"                              \
	"\thightag     => %s\n"                              \
	"\tbytes freed => %i\n"                              \
	"\tblocks freed=> %i\n",                             \
	TAG_TO_STR(lowtag),                                  \
	TAG_TO_STR(hightag), nblocks, bfreed);               \
}

#define LOG_ALLOC(ptr, tag, size)                        \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"Zone Daemon Log:\n"                                 \
	"\tlog type        => ALLOCATION\n"                  \
	"\tbytes allocated => %i\n"                          \
	"\tblock tag       => %i\n"                          \
	"\tuser pointer    => %p\n",                         \
	size, tag, ptr);                                     \
}
#define LOG_DEALLOC(ptr, tag, size)                      \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"Zone Daemon Log:\n"                                 \
	"\tlog type     => DEALLOCATION\n"                   \
	"\tbytes freed  => %i\n"                             \
	"\tblock tag    => %i\n"                             \
	"\tuser pointer => %p\n",                            \
	size, tag, ptr);                                     \
}

#define LOG_INFO(...)                                    \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"%s[INFO]%s(%s): ", C_GREEN, C_RESET, __func__);     \
	fprintf(LOGGER_OUTFILE, __VA_ARGS__);                \
	fprintf(LOGGER_OUTFILE, "\n");                       \
}
#define LOG_WARN(...)                                    \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"%sWARNING:%s%s\n"                                   \
	"\tfunction: %s\n"                                   \
	"\twhat: ",                                          \
		C_RED, C_RESET, C_YELLOW, __func__);             \
	fprintf(LOGGER_OUTFILE, __VA_ARGS__);                \
	fprintf(LOGGER_OUTFILE, "%s\n", C_RESET);            \
}
#define LOG_ERROR(...)                                   \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"%sERROR:%s%s\n"                                     \
	"\tfunction: %s\n"                                   \
	"\twhat: ",                                          \
		C_RED, C_RESET, C_YELLOW, __func__);             \
	fprintf(LOGGER_OUTFILE, __VA_ARGS__);                \
	fprintf(LOGGER_OUTFILE, "%s\n", C_RESET);            \
	exit(EXIT_FAILURE);                                \
}
#define LOG_TRACE_VAR(var) LOG_DEBUG(#var " = %s (%s:%d)", std::to_string(var).c_str(), __FILE__, __LINE__)
#define LOG_PROFILE(x) Profiler profile(__func__)
#define NOMAD_ASSERT(x, ...) if (!(x)) N_Error(__VA_ARGS__)

// pointer checks, they be givin' me them seggies
#define NULL_CHECK 0
#if defined(POINTER_SAFETY) || defined(POINTER_CHECKS)
#define PTR_CHECK(type, ptr)                                             \
{                                                                        \
	LOG_INFO("running pointer check %s on \"%s\"",                       \
		#type,                                                           \
		#ptr);                                                           \
	if (type == NULL_CHECK) {                                            \
		if (!ptr) {                                                      \
			N_Error("pointer \"%s\" is equal to NULL!", #ptr);           \
		}                                                                \
	}                                                                    \
}
#else
#ifdef _NOMAD_DEBUG
#define PTR_CHECK(type, ptr) assert(ptr)
#else
#define PTR_CHECK(type, ptr) if (!ptr) { LOG_ERROR("pointer \"%s\" == NULL!", #ptr); }
#endif
#endif

#define LOG_DUMP(x) \
{ \
	fclose(dbg_file); \
	FILE* out = fopen("Files/debug/debuglog.txt", "r"); \
	char buffer[1024]; \
	char *it; \
	while (fgets(buffer, sizeof(buffer), out)) { \
		if (strstr(buffer, "WARNING")) { \
			fprintf(stderr, "\x1b[33m"); \
			it = buffer; \
			while (*it != ':') { \
				fputc(*it, stderr); \
				++it; \
			} \
			fprintf(stderr, "\x1b[0m\x1b[31m"); \
			while (*it != '\0') { \
				fputc(*it, stderr); \
				++it; \
			} \
			putc('\n', stderr); \
		} \
		else if (strstr(buffer, "ERROR")) { \
			fprintf(stderr, "\x1b[31m"); \
			it = buffer; \
			while (*it != ':') { \
				fputc(*it, stderr); \
				++it; \
			} \
			fprintf(stderr, "\x1b[0m"); \
			while (*it != '\0') { \
				fputc(*it, stderr); \
				++it; \
			} \
			putc('\n', stderr); \
		} \
		else if (strstr(buffer, "DEBUG")) { \
			fprintf(stdout, "\x1b[32m"); \
			it = buffer; \
			while (*it != ':') { \
				fputc(*it, stdout); \
				++it; \
			} \
			fprintf(stdout, "\x1b[0m"); \
			while (*it != '\0') { \
				fputc(*it, stdout); \
				++it; \
			} \
			putc('\n', stdout); \
		} \
		else if (strstr(buffer, "INFO")) { \
			fprintf(stdout, "%s\n", buffer); \
		} \
	} \
	fclose(out); \
}


// the following below is for internal use, don't use yourself
class Profiler
{
private:
	const char* function;
	clock_t timer;
public:
	Profiler(const char* func)
		: function(func)
	{
		timer = clock();
		fprintf(LOGGER_OUTFILE, "%s%s[PROFILER START]%s: %s\n", C_BG_MAGENTA, C_WHITE, C_RESET, func);
	}
	~Profiler() {
		clock_t end = clock();
		float time = (end - timer)/(float)CLOCKS_PER_SEC;
		fprintf(LOGGER_OUTFILE,
			"%s%s[PROFILER END]%s:\n"
			"\tfunction: %s\n"
			"\ttime: %f\n", C_BG_MAGENTA, C_WHITE, C_RESET, function, time);
	}
};

#endif
