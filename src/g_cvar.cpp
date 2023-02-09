#include "g_cvar.h"
#include "scf.h"

cvar_t cvars[] = {
	{"g_fov_y",        std::to_string(MAX_VERT_FOV)},
	{"g_fov_x",        std::to_string(MAX_HORZ_FOV)},
	{"g_pspeed_base",  "1"},
	{"g_pspeed_dash",  "5"},
	{"g_pspeed_slide", "4"},
	{"g_maxp_items",   std::to_string(MAX_PLAYR_ITEMS)},
	{"c_maxmobs",      std::to_string(MAX_MOBS_ACTIVE)},
	{"c_maxbots",      std::to_string(MAX_NPC_ACTIVE)},
	{"c_defsvfile",    "nomadsv.ngd"},
	{"c_defbff",       "nomadmain.bffl"},
	{"c_defscf",       "default.scf"},
};
