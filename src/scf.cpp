#include "scf.h"
#include "g_game.h"

namespace scf {
	nomadbool_t music_on = false;
	nomadbool_t sfx_on = false;
	nomadushort_t music_vol = 0;
	nomadushort_t sfx_vol = 0;
	nomadushort_t mobspeed = MOB_SPEED_STD;
	nomadushort_t fov[2] = {MAX_VERT_FOV, MAX_HORZ_FOV};
	nomaduint_t ticrate_mil  = 28;
	nomaduint_t ticrate_base = 35;

	namespace launch {
		nomadbool_t fastmobs1 = false;
		nomadbool_t fastmobs2 = false;
		nomadbool_t fastmobs3 = false;
		nomadbool_t ext_bff = false;
		nomadbool_t deafmobs = false;
		nomadbool_t blindmobs = false;
		nomadbool_t nosmell = false;
		nomadbool_t nomobs = false;
		nomadbool_t godmode = false;
		nomadbool_t infinite_ammo = false;
		nomadbool_t bottomless_clip = false;
		nomadbool_t devmode = false;
	};
};
/* might use this... (in the future)
void strrch(char *__restrict str, char garbage)
{
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}
*/