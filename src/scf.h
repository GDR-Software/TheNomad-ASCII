#ifndef __SCF__
#define __SCF__

#include "n_shared.h"

#define MAX_VERT_FOV 24
#define MAX_HORZ_FOV 88
#define MOB_SPEED_STD	1

#define KEY_q		'q'
#define KEY_w		'w'
#define KEY_e		'e'
#define KEY_r		'r'
#define KEY_t		't'
#define KEY_y		'y'
#define KEY_u		'u'
#define KEY_i		'i'
#define KEY_o		'o'
#define KEY_p		'p'
#define KEY_a		'a'
#define KEY_s		's'
#define KEY_d		'd'
#define KEY_f		'f'
#define KEY_g		'g'
#define KEY_h		'h'
#define KEY_j		'j'
#define KEY_l		'l'
#define KEY_z		'z'
#define KEY_x		'x'
#define KEY_c		'c'
#define KEY_v		'v'
#define KEY_b		'b'
#define KEY_n		'n'
#define KEY_m		'm'
#define KEY_Q		'Q'
#define KEY_E		'E'
#define KEY_R		'R'
#define KEY_T		'T'
#define KEY_Y		'Y'
#define KEY_U		'U'
#define KEY_I		'I'
#define KEY_O		'O'
#define KEY_P		'P'
#define KEY_S		'S'
#define KEY_D		'D'
#ifdef KEY_F
#undef KEY_F
#endif
#define KEY_F		'F'
#define KEY_G		'G'
#define KEY_H		'H'
#define KEY_J		'J'
#define KEY_K		'K'
#define KEY_L		'L'
#define KEY_Z		'Z'
#define KEY_X		'X'
#define KEY_C		'C'
#define KEY_V		'V'
#define KEY_B		'B'
#define KEY_N		'N'
#define KEY_M		'M'
#define KEY_TILDA	'~'
#ifdef KEY_ENTER
#undef KEY_ENTER
#endif
#define KEY_ENTER	'\n'
#define KEY_SPACE	' '
#define KEY_TAB		'\t'
#define KEY_BSPACE	'\b'
#define KEY_SCOLON	';'
#define KEY_COLON	':'
#define KEY_DOT		'.'
#define KEY_COMMA	','
#define KEY_LCARROT	'<'
#define KEY_RCARROT	'>'
#define KEY_BSLASH	'\\'
#define KEY_FSLASH	'/'
#define KEY_EP		'!'
#define KEY_QM		'?'
#define KEY_OSB		'['
#define KEY_CSB		']'
#define KEY_OCB		'{'
#define KEY_CCB		'}'
#define KEY_HASH	'#'
#define KEY_DOLLAR	'$'
#define KEY_PER		'%'
#define KEY_AT      '@'
#define KEY_CARET	'^'
#define KEY_AND		'&'
#define KEY_STAR	'*'
#define KEY_ONB		'('
#define KEY_CNB		')'
#define KEY_HYPHEN	'-'
#define KEY_UNDER	'_'
#define KEY_EQUAL	'='
#define KEY_PLUS	'+'
#define KEY_OR      '|'
#define NUMBUTTONS 85
namespace scf {
	extern nomadbool_t music_on;
	extern nomadbool_t sfx_on;
	extern nomadushort_t music_vol;
	extern nomadushort_t sfx_vol;

	extern nomadushort_t fov[2];
	extern nomadushort_t mobspeed;
	extern nomaduint_t ticrate_base;
	extern nomaduint_t ticrate_mil;
	
	enum : nomaduint_t
	{
		kbMove_n,
		kbStrafe_l,
		kbMove_s,
		kbStrafe_r,
		kbSlide_n,
		kbSlide_w,
		kbSlide_s,
		kbSlide_e,
		kbDash_n,
		kbDash_w,
		kbDash_s,
		kbDash_e,
		kbTurn_l,
		kbTurn_r,
		
		kbCrouch,
		kbShoot_n,
		kbShoot_w,
		kbShoot_s,
		kbShoot_e,
		kbLeftArm_use,
		kbRightArm_use,
		kbShowWpns,
		kbOpenConsole,
		kbMelee,
		kbQuickShot,
		
#ifdef DEVMODE
		kbOpenCMD,
#endif
		
		NUMBINDS
	};
	
	typedef struct
	{
		char name[20];
		nomadenum_t button;
		nomaduint_t bind;
	} keybind_t;
	
	extern keybind_t kb_binds[NUMBINDS];

	namespace launch {
		extern nomadbool_t fastmobs1;
		extern nomadbool_t fastmobs2;
		extern nomadbool_t fastmobs3;
		extern nomadbool_t ext_bff;
		extern nomadbool_t deafmobs;
		extern nomadbool_t blindmobs;
		extern nomadbool_t nosmell;
		extern nomadbool_t nomobs;
		extern nomadbool_t godmode;
		extern nomadbool_t infinite_ammo;
		extern nomadbool_t bottomless_clip;
		extern nomadbool_t devmode;
	};
};

#endif
