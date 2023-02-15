#include "n_shared.h"
#include "n_cheats.h"

static const char* cheat_table[] = {
	CHEAT_1,
	CHEAT_2,
	CHEAT_3,
	CHEAT_4,
};

nomadbool_t G_CheckCheat(const char* str)
{
	char decrypt[80];
	char *ptr = decrypt;
	for (nomaduint_t i = 0; i < ARRAY_SIZE(cheat_table); ++i) {
		for (nomaduint_t l = 0; l < strlen(cheat_table[i]); ++l) {
			*ptr = cheat_table[i][l] ^ MAGIC_XOR;
			++ptr;
		}
	}
	if (strcmp(str, decrypt)) {
		return true;
	} else {
		return false;
	}
}
