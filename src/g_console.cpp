#include "g_cvar.h"
#include "g_game.h"
#include "scf.h"

constexpr const char* commands[] = {
	"listcmds",
	"listcvars",
	"quit", // duh
	"crash", // also duh
	"exit", // extra duh
	"iamacheater", // enables cheat codes
	"iamnotacheater", // disables cheat codes
	"listcheats", // lists all cheat codes
	"wimpmode", // turns on all cheat codes, the alternative was "iamapussy", but that's a wee bit, well...
	"bravemode", // will only ever do anything if wimpmode is == "1", turns off all cheat codes
	"killmobs", // kills all mobs, doesn't disable them, sorta a memory wipe
	"killbots", // does the same as killmobs but for npcs/bots
	"bind",
	"set",
};

cvar_t cvars[] = {
	{"g_fov_y",        std::to_string(MAX_VERT_FOV),     false, NULL},
	{"g_fov_x",        std::to_string(MAX_HORZ_FOV),     false, NULL},
	{"g_pspeed_base",  "1",                              false, (void*)&scf::cvar::g_pspeed_base},
	{"g_pspeed_dash",  "5",                              false, (void*)&scf::cvar::g_pspeed_dash},
	{"g_pspeed_slide", "4",                              false, (void*)&scf::cvar::g_pspeed_slide},
	{"g_maxp_items",   std::to_string(MAX_PLAYR_ITEMS),  true,  NULL},
	{"g_maxp_wpns",    std::to_string(MAX_PLAYR_WPNS),   true,  NULL},
	{"c_maxmobs",      std::to_string(MAX_MOBS_ACTIVE),  true,  (void*)&scf::cvar::c_maxmobs},
	{"c_maxbots",      std::to_string(MAX_NPC_ACTIVE),   true,  (void*)&scf::cvar::c_maxbots},
	{"c_defsvfile",    "nomadsv.ngd",                    false, (void*)&scf::cvar::c_defsvfile},
	{"c_defbff",       "nomadmain.bffl",                 false, (void*)&scf::cvar::c_defbff},
	{"c_defscf",       "default.scf",                    false, (void*)&scf::cvar::c_defscf},
	{"b_kbMove_n",     "w",                              false, (void*)&scf::kb_binds[kbMove_n].button},
	{"b_kbStrafe_l",   "a",                              false, (void*)&scf::kb_binds[kbStrafe_l].button},
	{"b_kbMove_s",     "s",                              false, (void*)&scf::kb_binds[kbMove_s].button},
	{"b_kbStrafe_r",   "d",                              false, (void*)&scf::kb_binds[kbStrafe_r].button},
	{"b_Dash_n",       "@",                              false, (void*)&scf::kb_binds[kbDash_n].button},
	{"b_Dash_w",       "!",                              false, (void*)&scf::kb_binds[kbDash_w].button},
	{"b_Dash_s",       "#",                              false, (void*)&scf::kb_binds[kbDash_s].button},
	{"b_Dash_e",       "$",                              false, (void*)&scf::kb_binds[kbDash_e].button},
	{"b_UseWeapon",    "ctrl-v",                         false, (void*)&scf::kb_binds[kbUseWpn].button},
};
