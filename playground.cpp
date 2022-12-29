#include "src/g_game.h"
#include "src/g_rng.h"

int main()
{
	srand(time(NULL));
	printf("sizeof mobinfo: %ld\n", sizeof(mobinfo)/sizeof(mobinfo[0]));
	printf("NUMMOBS: %i\n", NUMMOBS);
	for (nomaduint_t i = 0; i < MAX_MOBS_ACTIVE; i++) {
		nomaduint_t a = rand() % NUMMOBS;
		if (a == 9) {
			printf("DIGRESSION!\n");	
		}
		else {
			printf("num: %i\n", P_Random() % NUMMOBS);
		}
	}
	return 0;
}