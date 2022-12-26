#include "n_shared.h"

static void __attribute__((constructor)) set_nonblock(void)
{
	struct termios ttystate;
	
	// get the terminal state
	tcgetattr(STDIN_FILENO, &ttystate);
	
	// turn off canonical mode
	ttystate.c_lflag &= ~ICANON;
	
	// minimum of number input read.
	ttystate.c_cc[VMIN] = 1;
	
	// set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

static void set_block(void)
{
	struct termios ttystate;
	
	// get the terminal state
	tcgetattr(STDIN_FILENO, &ttystate);
	
	//turn on canonical mode
	ttystate.c_lflag |= ICANON;

	// set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void __attribute__((destructor)) kill_process(void)
{
	kill_game();
	kill_zone();
	set_block();
}

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
	return FD_ISSET(STDIN_FILENO, &fds);
#endif
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