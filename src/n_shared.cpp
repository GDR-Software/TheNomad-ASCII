#include "n_shared.h"

#ifdef __cplusplus
nomadbool_t kbhit(nomadushort_t& in)
{
#ifdef _WIN32
	if (_kbhit()) {
		in = (nomadushort_t)_getch();
		return true;
	}
	else {
		return false;
	}
#elif defined(__unix__)
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	if ((FD_ISSET(STDIN_FILENO, &fds))) {
		in = getc(stdin);
		return true;
	}
	else {
		return false;
	}
#endif
}
#else
nomadbool_t kbhit(nomadushort_t* in)
{
	if (_kbhit()) {
		in = (nomadushort_t)_getch();
		return true;
	}
	else {
		return false;
	}
}
#endif

namespace std {
	size_t filelength(const char* filename)
	{
#ifdef __unix__
		struct stat fdata;
		if (stat(filename, &fdata) == -1) {
			return -1;
		}
		else {
			return (size_t)fdata.st_size;
		}
#else // don't really want to be constructing an object whenever we do this
		FILE* fp = std::fopen(filename, "r");
		if (fp == NULL) {
			return -1;
		}
		else {
			std::fseek(fp, 0, SEEK_END);
			size_t fsize = std::ftell(fp);
			std::fseek(fp, 0, SEEK_SET);
			return fsize;
		}
#endif
	}
	size_t filelength(const std::string& filename)
	{
#ifdef __unix__
		struct stat fdata;
		if (stat(filename.c_str(), &fdata) == -1) {
			return -1;
		}
		else {
			return (size_t)fdata.st_size;
		}
#else // don't really want to be constructing an object whenever we do this
		FILE* fp = std::fopen(filename.c_str(), "r");
		if (fp == NULL) {
			return -1;
		}
		else {
			std::fseek(fp, 0, SEEK_END);
			size_t fsize = std::ftell(fp);
			std::fseek(fp, 0, SEEK_SET);
			return fsize;
		}
#endif
	}
};