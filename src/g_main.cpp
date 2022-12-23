#include "g_game.h"

void signal_interrupt(int signum)
{
	werase(stdscr);
	endwin();
	puts("Killing Proccess, Exit Success!");
	exit(EXIT_SUCCESS);
}

void signal_seggy(int signum)
{
	werase(stdscr);
	endwin();
	puts("Caught A Seggy! Goddamn It!\n");
	exit(EXIT_FAILURE);
}

void signal_unnatural_demise(int signum)
{
//	werase(stdscr);
	endwin();
	printf("Signal Abort: %i\n\n", signum);
//	puts("The Game Object Was KIA'd By Strange & Mysterious Forces Beyond Our Knowledege...");
	exit(EXIT_FAILURE);
}

void signal_somethins_corrupt(int signum)
{
	werase(stdscr);
	endwin();
	puts("Somethin's Gotten Corrupt... I Don't Know What, But This Thing is Corrupt, Your Fault, Perhaps?");
	exit(EXIT_FAILURE);
}

void signal_just_continue(int signum)
{
	def_prog_mode();
	system("clear");
	puts("Oh, you really think that'll stop me?\n\n");
	getc(stdin);
	system("clear");
	reset_prog_mode();
	refresh();
}

int main(int argc, char* argv[])
{
#ifdef __unix__
//	struct sigaction* sig;
//	sigaction();
	signal(SIGINT, signal_interrupt);
	signal(SIGSEGV, signal_seggy);
	signal(SIGTERM, signal_interrupt);
	signal(SIGABRT, signal_unnatural_demise);
	signal(SIGILL, signal_somethins_corrupt);
	signal(SIGQUIT, signal_interrupt);
//	signal(SIGSTOP, signal_just_continue);
#endif
	mainLoop(argc, argv);
	return 0;
}