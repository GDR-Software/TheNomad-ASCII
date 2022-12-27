#include "scf.h"
#include "g_game.h"

#ifdef _NOMAD_64
#define ULONG "%lu"
#define LONG "%ld"
#define UINT "%du"
#define INT "%d"
#elif _NOMAD_16
#define ULONG "%du"
#define LONG "%d"
#define UINT "%hu"
#define INT "%h"
#endif

#define VAR_NAME(name) (#name)
#define VAL_NAME(FORMAT, IN, OUT) \
do { \
	sprintf(OUT, FORMAT, IN); \
} while(false)

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
/*
enum
{
	FOREACH_BUTTON(GEN_ENUM)
};

const nomaduint_t button_enum[NUMBUTTONS] = {
	FOREACH_BUTTON(GEN_ENUM)
};

const char* button_str[NUMBUTTONS] = {
	FOREACH_BUTTON(GEN_STRING)
}; */

void strrch(char *__restrict str, char garbage)
{
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}
/*
void Game::G_ReadSCF(void)
{
	std::ifstream read("default.scf", std::ios::in);
	std::string line;
	std::vector<std::string> buf;
	while (std::getline(read, line)) {
		buf.push_back(line);
	};
	for (nomaduint_t i = 0; i < buf.size(); i++) {
		if (buf[i].find("<bind>") != NotFound) {	
			for (nomaduint_t b = 0; b < NUMBUTTONS; b++) {
				if (buf[i].find(button_str[b]) != NotFound) {
					if (buf[i].find("kbMove_n") != NotFound) {
						scf::kb_binds[scf::kbMove_n].button = button_enum[b];
					}
					else if (buf[i].find("kbMove_s") != NotFound) {
						scf::kb_binds[scf::kbMove_s].button = button_enum[b];
					}
					else if (buf[i].find("kbStrafe_l") != NotFound) {
						scf::kb_binds[scf::kbStrafe_l].button = button_enum[b];
					}
					else if (buf[i].find("kbStrafe_r") != NotFound) {
						scf::kb_binds[scf::kbStrafe_r].button = button_enum[b];
					}
					else if (buf[i].find("kbSlide_n") != NotFound) {
						scf::kb_binds[scf::kbSlide_n].button = button_enum[b];
					}
					else if (buf[i].find("kbSlide_w") != NotFound) {
						scf::kb_binds[scf::kbSlide_w].button = button_enum[b];
					}
					else if (buf[i].find("kbSlide_s") != NotFound) {
						scf::kb_binds[scf::kbSlide_s].button = button_enum[b];
					}
					else if (buf[i].find("kbSlide_e") != NotFound) {
						scf::kb_binds[scf::kbSlide_e].button = button_enum[b];
					}
					else if (buf[i].find("kbDash_n") != NotFound) {
						scf::kb_binds[scf::kbDash_n].button = button_enum[b];
					}
					else if (buf[i].find("kbDash_w") != NotFound) {
						scf::kb_binds[scf::kbDash_w].button = button_enum[b];
					}
					else if (buf[i].find("kbDash_s") != NotFound) {
						scf::kb_binds[scf::kbDash_s].button = button_enum[b];
					}
					else if (buf[i].find("kbTurn_l") != NotFound) {
						scf::kb_binds[scf::kbTurn_l].button = button_enum[b];
					}
					else if (buf[i].find("kbTurn_r") != NotFound) {
						scf::kb_binds[scf::kbTurn_r].button = button_enum[b];
					}
					else if (buf[i].find("kbCrouch") != NotFound) {
						scf::kb_binds[scf::kbCrouch].button = button_enum[b];
					}
					else if (buf[i].find("kbLeftArm_use") != NotFound) {
						scf::kb_binds[scf::kbLeftArm_use].button = button_enum[b];
					}
					else if (buf[i].find("kbRightArm_use") != NotFound) {
						scf::kb_binds[scf::kbRightArm_use].button = button_enum[b];
					}
					else if (buf[i].find("kbShowWpns") != NotFound) {
						scf::kb_binds[scf::kbShowWpns].button = button_enum[b];
					}
					else if (buf[i].find("OpenConsole") != NotFound) {
						scf::kb_binds[scf::kbOpenConsole].button = button_enum[b];
					}
					else if (buf[i].find("kbMelee") != NotFound) {
						scf::kb_binds[scf::kbMelee].button = button_enum[b];
					}
					else if (buf[i].find("kbQuickShot") != NotFound) {
						scf::kb_binds[scf::kbQuickShot].button = button_enum[b];
					}
				}
			}
		}
		else if (buf[i].find("<launch>") != NotFound) {
			if (buf[i].find("devmode") != NotFound) {
				scf::launch::devmode = (nomadbool_t)stoi(buf.back());
			}
			else if (buf[i].find("godmode") != NotFound) {
				scf::launch::godmode = (nomadbool_t)stoi(buf.back());
			}
			else if (buf[i].find("fastmobs1") != NotFound) {
				scf::launch::fastmobs1 = (nomadbool_t)stoi(buf.back());
			}
			else if (buf[i].find("fastmobs2") != NotFound) {
				scf::launch::fastmobs2 = (nomadbool_t)stoi(buf.back());
			}
			else if (buf[i].find("fastmobs3") != NotFound) {
				scf::launch::fastmobs3 = (nomadbool_t)stoi(buf.back());
			}
			else if (buf[i].find("nosmell") != NotFound) {
				scf::launch::nosmell = (nomadbool_t)stoi(buf.back());
			}
			else if (buf[i].find("blindmobs") != NotFound) {
				scf::launch::blindmobs = (nomadbool_t)stoi(buf.back());
			}
			else if (buf[i].find("deafmobs") != NotFound) {
				scf::launch::deafmobs = (nomadbool_t)stoi(buf.back());
			}
			else if (buf[i].find("nomobs") != NotFound) {
				scf::launch::nomobs = (nomadbool_t)stoi(buf.back());
			}
			else if (buf[i].find("infinite_ammo") != NotFound) {
				scf::launch::infinite_ammo = (nomadbool_t)stoi(buf.back());
			}
			else if (buf[i].find("bottomless_clip") != NotFound) {
				scf::launch::bottomless_clip = (nomadbool_t)stoi(buf.back());
			}
		}
		else if (buf[i].find("<setting") != NotFound) {
			if (buf[i].find(":bool>") != NotFound) {
				if (buf[i].find("music_on") != NotFound) {
					scf::music_on = (nomadbool_t)stoi(buf.back());
				}
				else if (buf[i].find("sfx_on") != NotFound) {
					scf::sfx_on = (nomadbool_t)stoi(buf.back());
				}
			}
			else if (buf[i].find(":int>") != NotFound) {
				const char* tmp = buf[i].c_str();
				char* str = strchr(tmp, '+');
				strrch(str, '+');
				nomadushort_t n;
				if (strcmp(str, "MOB_SPEED_STD") != NotFound) {
					n = MOB_SPEED_STD;
				}
				else {
					n = (nomadushort_t)stoi(str);
				}
				if (buf[i].find("music_vol") != NotFound) {
					scf::music_vol = n;
				}
				else if (buf[i].find("sfx_vol") != NotFound) {
					scf::sfx_vol = n;
				}
				else if (buf[i].find("vert_fov") != NotFound) {
					scf::fov[0] = n;
				}
				else if (buf[i].find("horz_fov") != NotFound) {
					scf::fov[1] = n;
				}
				else if (buf[i].find("mobspeed") != NotFound) {
					scf::mobspeed = n;
				}
			}
		}
	}
}

#define GEN_STRING(string) #string,
#define GEN_ENUM(enum) enum,

#define FOREACH_BUTTON(STUFF) \
	STUFF(KEY_a) \
	STUFF(KEY_b) \
	STUFF(KEY_c) \
	STUFF(KEY_d) \
	STUFF(KEY_e) \
	STUFF(KEY_f) \
	STUFF(KEY_g) \
	STUFF(KEY_h) \
	STUFF(KEY_i) \
	STUFF(KEY_j) \
	STUFF(KEY_k) \
	STUFF(KEY_l) \
	STUFF(KEY_m) \
	STUFF(KEY_n) \
	STUFF(KEY_o) \
	STUFF(KEY_p) \
	STUFF(KEY_q) \
	STUFF(KEY_r) \
	STUFF(KEY_s) \
	STUFF(KEY_t) \
	STUFF(KEY_u) \
	STUFF(KEY_v) \
	STUFF(KEY_w) \
	STUFF(KEY_x) \
	STUFF(KEY_y) \
	STUFF(KEY_z) \
	STUFF(KEY_A) \
	STUFF(KEY_B) \
	STUFF(KEY_C) \
	STUFF(KEY_D) \
	STUFF(KEY_E) \
	STUFF(KEY_F) \
	STUFF(KEY_G) \
	STUFF(KEY_H) \
	STUFF(KEY_I) \
	STUFF(KEY_J) \
	STUFF(KEY_K) \
	STUFF(KEY_L) \
	STUFF(KEY_M) \
	STUFF(KEY_N) \
	STUFF(KEY_O) \
	STUFF(KEY_P) \
	STUFF(KEY_Q) \
	STUFF(KEY_R) \
	STUFF(KEY_S) \
	STUFF(KEY_T) \
	STUFF(KEY_U) \
	STUFF(KEY_V) \
	STUFF(KEY_W) \
	STUFF(KEY_X) \
	STUFF(KEY_Y) \
	STUFF(KEY_Z) \
	STUFF(KEY_TILDA) \
	STUFF(KEY_ENTER) \
	STUFF(KEY_SPACE) \
	STUFF(KEY_TAB) \
	STUFF(KEY_BSPACE) \
	STUFF(KEY_SCOLON) \
	STUFF(KEY_COLON) \
	STUFF(KEY_DOT) \
	STUFF(KEY_COMMA) \
	STUFF(KEY_LARROW) \
	STUFF(KEY_RARROW) \
	STUFF(KEY_BSLASH) \
	STUFF(KEY_FSLASH) \
	STUFF(KEY_EP) \
	STUFF(KEY_QM) \
	STUFF(KEY_OSB) \
	STUFF(KEY_CSB) \
	STUFF(KEY_OCB) \
	STUFF(KEY_CCB) \
	STUFF(KEY_HASH) \
	STUFF(KEY_DOLLAR) \
	STUFF(KEY_PER) \
	STUFF(KEY_AT) \
	STUFF(KEY_CARET) \
	STUFF(KEY_AND) \
	STUFF(KEY_OR) \
	STUFF(KEY_ONB) \
	STUFF(KEY_CNB) \
	STUFF(KEY_PLUS) \
	STUFF(KEY_EQUAL) \
	STUFF(KEY_UNDER) \
	STUFF(KEY_HYPHEN) \
	STUFF(KEY_STAR)
*/
