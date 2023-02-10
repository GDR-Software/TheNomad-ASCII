#ifndef _N_DEBUG_
#define _N_DEBUG_

#ifndef LOGGER_OUTFILE
#define LOGGER_OUTFILE stdout
#endif

#ifdef _NOMAD_DEBUG
#define LOG_DEBUG(fmt,...)                              \
{                                                       \
	fprintf(LOGGER_OUTFILE, "[DEBUG](%s): ", __func__); \
	fprintf(LOGGER_OUTFILE, fmt, ##__VA_ARGS__);        \
	fprintf(LOGGER_OUTFILE, "\n");                      \
}
#else
#define LOG_DEBUG(fmt,...)
#endif
#define DBG_LOG(fmt,...) LOG_DEBUG(fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt,...)                              \
{                                                      \
	fprintf(LOGGER_OUTFILE, "[INFO](%s): ", __func__); \
	fprintf(LOGGER_OUTFILE, fmt, ##__VA_ARGS__);       \
	fprintf(LOGGER_OUTFILE, "\n");                     \
}
#define LOG_WARN(fmt,...)                                \
{                                                        \
	fprintf(LOGGER_OUTFILE, "WARNING (%s): ", __func__); \
	fprintf(LOGGER_OUTFILE, fmt, ##__VA_ARGS__);         \
	fprintf(LOGGER_OUTFILE, "\n");                       \
}
#define LOG_ERROR(fmt,...)                             \
{                                                      \
	fprintf(LOGGER_OUTFILE, "ERROR (%s): ", __func__); \
	fprintf(LOGGER_OUTFILE, fmt, ##__VA_ARGS__);       \
	fprintf(LOGGER_OUTFILE, "\n");                     \
	exit(EXIT_FAILURE);                                \
}
#define LOG_TRACE_VAR(var) LOG_DEBUG(#var " = %s (%s:%d)", std::to_string(var).c_str(), __FILE__, __LINE__)
#define LOG_PROFILE(x) Profiler profile(__func__)
#define NOMAD_ASSERT(x, ...) if (!(x)) N_Error(__VA_ARGS__)
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
				++it; \ \
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
#define LOG_PROFILE_BEGIN(func) fprintf(LOGGER_OUTFILE, "[PROFILER](start) -> %s\n", func)
#define LOG_PROFILE_END(func,time) fprintf(LOGGER_OUTFILE, "[PROFILER](end) -> %s, time: %f\n", func, time)

class Profiler
{
private:
	const char* function;
	clock_t timer;
public:
	Profiler(const char* func) : function(func) { timer = clock(); LOG_PROFILE_BEGIN(); }
	~Profiler() {
		clock_t end = clock();
		float time = (end - timer)/(float)CLOCKS_PER_SEC;
		LOG_PROFILE_END(function,time);
	}
};

#endif
