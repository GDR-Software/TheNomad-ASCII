//----------------------------------------------------------
//
// Copyright (C) SIGAAMDAD 2022-2023
//
// This source is available for distribution and/or modification
// only under the terms of the SACE Source Code License as
// published by SIGAAMDAD. All rights reserved
//
// The source is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of FITNESS FOR A PARTICLAR PURPOSE. See the SACE
// Source Code License for more details. If you, however do not
// want to use the SACE Source Code License, then you must use
// this source as if it were to be licensed under the GNU General
// Public License (GPL) version 2.0 or later as published by the
// Free Software Foundation.
//
// DESCRIPTION:
//  src/scf.h
//----------------------------------------------------------
#ifndef __SCF__
#define __SCF__

#include "n_shared.h"

#define MAX_VERT_FOV 24
#define MAX_HORZ_FOV 88
#define MOB_SPEED_STD	1

#define KEY_q		'q' // 1
#define KEY_w		'w' // 2
#define KEY_e		'e' // 3
#define KEY_r		'r' // 4
#define KEY_t		't' // 5
#define KEY_y		'y' // 6
#define KEY_u		'u' // 7
#define KEY_i		'i' // 8
#define KEY_o		'o' // 9
#define KEY_p		'p' // 10
#define KEY_a		'a' // 11
#define KEY_s		's' // 12
#define KEY_d		'd' // 13
#define KEY_f		'f' // 14
#define KEY_g		'g' // 15
#define KEY_h		'h' // 16
#define KEY_j		'j' // 17
#define KEY_l		'l' // 18
#define KEY_z		'z' // 19
#define KEY_x		'x' // 20
#define KEY_c		'c' // 21
#define KEY_v		'v' // 22
#define KEY_b		'b' // 23
#define KEY_n		'n' // 24
#define KEY_m		'm' // 25
#define KEY_Q		'Q' // 26
#define KEY_E		'E' // 27
#define KEY_R		'R' // 28
#define KEY_T		'T' // 29
#define KEY_Y		'Y' // 30
#define KEY_U		'U' // 31
#define KEY_I		'I' // 32
#define KEY_O		'O' // 33
#define KEY_P		'P' // 34
#define KEY_S		'S' // 35
#define KEY_D		'D' // 36
#ifdef KEY_F
#undef KEY_F
#endif
#define KEY_F		'F' // 37
#define KEY_G		'G' // 38
#define KEY_H		'H' // 39
#define KEY_J		'J' // 40
#define KEY_K		'K' // 41
#define KEY_L		'L' // 42
#define KEY_Z		'Z' // 43
#define KEY_X		'X' // 44
#define KEY_C		'C' // 45
#define KEY_V		'V' // 46
#define KEY_B		'B' // 47
#define KEY_N		'N' // 48
#define KEY_M		'M' // 49
#define KEY_TILDA	'~' // 50
#define KEY_GRAVE   '`' // 51
#ifdef KEY_ENTER
#undef KEY_ENTER
#endif
#define KEY_ENTER	10 // 52
#define KEY_SPACE	' ' // 53
#define KEY_TAB		'\t' // 54
#define KEY_BSPACE	'\b' // 55
#define KEY_SCOLON	';' // 56
#define KEY_COLON	':' // 57
#define KEY_DOT		'.' // 58
#define KEY_COMMA	',' // 59
#define KEY_LCARROT	'<' // 60
#define KEY_RCARROT	'>' // 61
#define KEY_BSLASH	'\\' // 62
#define KEY_FSLASH	'/' // 63
#define KEY_EP		'!' // 64
#define KEY_QM		'?' // 65
#define KEY_OSB		'[' // 66
#define KEY_CSB		']' // 67
#define KEY_OCB		'{' // 68
#define KEY_CCB		'}' // 69
#define KEY_HASH	'#' // 70
#define KEY_DOLLAR	'$' // 71
#define KEY_PER		'%' // 72
#define KEY_AT      '@' // 73
#define KEY_CARET	'^' // 74
#define KEY_AND		'&' // 75
#define KEY_STAR	'*' // 76
#define KEY_ONB		'(' // 77
#define KEY_CNB		')' // 78
#define KEY_HYPHEN	'-' // 79
#define KEY_UNDER	'_' // 80
#define KEY_EQUAL	'=' // 81
#define KEY_PLUS	'+' // 82
#define KEY_OR      '|' // 83
#define KEY_1       '1' // 84
#define KEY_2       '2' // 85
#define KEY_3       '3' // 86
#define KEY_4       '4' // 87
#define KEY_5       '5' // 88
#define KEY_6       '6' // 89
#define KEY_7       '7' // 90
#define KEY_8       '8' // 91
#define KEY_9       '9' // 92
#define KEY_0       '0' // 93
#define CTRL_q      ctrl('q') // 94
#define CTRL_w      ctrl('w') // 95
#define CTRL_e      ctrl('e') // 96
#define CTRL_r      ctrl('r') // 97
#define CTRL_t      ctrl('t') // 98
#define CTRL_y      ctrl('y') // 99
#define CTRL_u      ctrl('u') // 100
#define CTRL_i      ctrl('i') // 101
#define CTRL_o      ctrl('o') // 102
#define CTRL_p      ctrl('p') // 103
#define CTRL_a      ctrl('a') // 104
#define CTRL_s      ctrl('s') // 105
#define CTRL_d      ctrl('d') // 106
#define CTRL_f      ctrl('f') // 107
#define CTRL_g      ctrl('g') // 108
#define CTRL_h      ctrl('h') // 109
#define CTRL_j      ctrl('j') // 110
#define CTRL_l      ctrl('l') // 111
#define CTRL_z      ctrl('z') // 112
#define CTRL_x      ctrl('x') // 113
#define CTRL_c      ctrl('c') // 114
#define CTRL_v      ctrl('v') // 115
#define CTRL_b      ctrl('b') // 116
#define CTRL_n      ctrl('n') // 117
#define CTRL_m      ctrl('m') // 118
#define CTRL_Q      ctrl('Q') // 119
#define CTRL_E      ctrl('E') // 120
#define CTRL_R      ctrl('R') // 121
#define CTRL_T      ctrl('T') // 122
#define CTRL_Y      ctrl('Y') // 123
#define CTRL_U      ctrl('U') // 124
#define CTRL_I      ctrl('I') // 125
#define CTRL_O      ctrl('O') // 126
#define CTRL_P      ctrl('P') // 127
#define CTRL_S      ctrl('S') // 128
#define CTRL_D      ctrl('D') // 129
#define CTRL_F      ctrl('F') // 130
#define CTRL_G      ctrl('G') // 131
#define CTRL_H      ctrl('H') // 132
#define CTRL_J      ctrl('J') // 133
#define CTRL_K      ctrl('K') // 134
#define CTRL_L      ctrl('L') // 135
#define CTRL_Z      ctrl('Z') // 136
#define CTRL_X      ctrl('X') // 137
#define CTRL_C      ctrl('C') // 138
#define CTRL_V      ctrl('V') // 139
#define CTRL_B      ctrl('B') // 140
#define CTRL_N      ctrl('N') // 141
#define CTRL_M      ctrl('M') // 142
#define KEY_k       'k' // 143
#define KEY_W       'W' // 144
#define KEY_A       'A' // 145
#define NUMBUTTONS 143

constexpr uint8_t ticrate_mil = 28;
constexpr uint8_t ticrate_base = 35;
constexpr uint16_t ticrate_minute = ticrate_base*30; // an in-game "minute"
constexpr uint16_t ticrate_hour = ticrate_minute*48;

typedef void(*paction_t)();

namespace scf {
	extern nomadbool_t music_on;
	extern nomadbool_t sfx_on;
	extern nomadushort_t music_vol;
	extern nomadushort_t sfx_vol;

	extern nomadushort_t fov[2];
	extern nomadushort_t mobspeed;
	
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
		kbSwapWpn_1,
		kbSwapWpn_2,
		kbSwapWpn_3,
		kbSwapWpn_4,
		kbSwapWpn_5,
		kbSwapWpn_6,
		kbSwapWpn_7,
		kbSwapWpn_8,
		kbSwapWpn_9,
		kbSwapWpn_10,
		kbUseWeapon,
		kbLeftArm_use,
		kbRightArm_use,
		kbShowWpns,
		kbOpenConsole,
		kbMelee,
		kbQuickShot,
		kbExitToPause,
		kbInteract,
		
#ifdef DEVMODE
		kbOpenCMD,
#endif
		
		NUMBINDS
	};
	
	typedef struct
	{
		const char* name;
		nomadenum_t button;
		nomaduint_t bind;
		paction_t actionp;
	} keybind_t;
	
	extern keybind_t kb_binds[NUMBINDS];

	namespace launch {
		extern nomadbool_t fastmobs1;
		extern nomadbool_t fastmobs2;
		extern nomadbool_t fastmobs3;
		extern nomadbool_t ext_bff;
		extern nomadbool_t ext_scf;
		extern nomadbool_t deafmobs;
		extern nomadbool_t blindmobs;
		extern nomadbool_t nosmell;
		extern nomadbool_t nomobs;
		extern nomadbool_t godmode;
		extern nomadbool_t infinite_ammo;
		extern nomadbool_t bottomless_clip;
		extern nomadbool_t devmode;
		static constexpr uint16_t numlaunchparams = 11;
	};
	struct scfbind
    {
        const std::string_view str;
        paction_t actionp;
    };
	struct scfbutton
    {
        const std::string_view str;
        const nomaduint_t button;
    };
	extern scfbind kbStrings[NUMBINDS];
	extern scfbutton buttonStrings[NUMBUTTONS];

	void G_LoadSCF(const char* filepath);

	const char* GetSCFBind(nomaduint_t bind);
	/*
	const char* GetSCFButton(nomaduint_t button)
	{
		switch (button) {
		case KEY_a: return "a";
		case KEY_b: return "b";
		case KEY_c: return "c";
		case KEY_d: return "d";
		case KEY_e: return "e";
		case KEY_f: return "f";
		case KEY_g: return "g";
		case KEY_h: return "h";
		case KEY_i: return "i";
		case KEY_j: return "j";
		case KEY_k: return "k";
		case KEY_l: return "l";
		case KEY_m: return "m";
		case KEY_n: return "n";
		case KEY_o: return "o";
		case KEY_p: return "p";
		case KEY_q: return "q";
		case KEY_r: return "r";
		case KEY_s: return "s";
		case KEY_t: return "t";
		case KEY_u: return "u";
		case KEY_v: return "v";
		case KEY_w: return "w";
		case KEY_x: return "x";
		case KEY_y: return "y";
		case KEY_z: return "z";
		case KEY_F: return "F";
		case KEY_G: return "G";
		case KEY_H: return "H";
		case KEY_J: return "J";
		case KEY_K: return "K";
		case KEY_L: return "L";
		case KEY_Z: return "Z";
		case KEY_X: return "X";
		case KEY_C: return "C";
		case KEY_V: return "V";
		case KEY_B: return "B";
		case KEY_N: return "N";
		case KEY_M: return "M";
		case KEY_Q: return "Q";
		case KEY_E: return "E";
		case KEY_R: return "R";
		case KEY_T: return "T";
		case KEY_Y: return "Y";
		case KEY_U: return "U";
		case KEY_I: return "I";
		case KEY_O: return "O";
		case KEY_P: return "P";
		case KEY_S: return "S";
		case KEY_D: return "D";
		case ctrl_q: return "ctrl-q";
		case ctrl_w: return "ctrl-w";
		case ctrl_e: return "ctrl-e";
		case ctrl_r: return "ctrl-r";
		case ctrl_t: return "ctrl-t";
		case ctrl_y: return "ctrl-y";
		case ctrl_u: return "ctrl-u";
		case ctrl_i: return "ctrl-i";
		case ctrl_o: return "ctrl-o";
		case ctrl_p: return "ctrl-p";
		case ctrl_a: return "ctrl-a";
		case ctrl_s: return "ctrl-s";
		case ctrl_d: return "ctrl-d";
		case ctrl_f: return "ctrl-f";
		case ctrl_g: return "ctrl-g";
		case ctrl_h: return "ctrl-h";
		case ctrl_j: return "ctrl-j";
		case ctrl_l: return "ctrl-l";
		case ctrl_z: return "ctrl-z";
		case ctrl_x: return "ctrl-x";
		case ctrl_c: return "ctrl-c";
		case ctrl_v: return "ctrl-v";
		case ctrl_b: return "ctrl-b";
		case ctrl_n: return "ctrl-n";
		case ctrl_m: return "ctrl-m";
		case ctrl_Q: return "ctrl-Q";
		case ctrl_E: return "ctrl-E";
		case ctrl_R: return "ctrl-R";
		case ctrl_T: return "ctrl-T";
		case ctrl_Y: return "ctrl-Y";
		case ctrl_U: return "ctrl-U";
		case ctrl_I: return "ctrl-I";
		case ctrl_O: return "ctrl-O";
		case ctrl_P: return "ctrl-P";
		case ctrl_S: return "ctrl-S";
		case ctrl_D: return "ctrl-D";
		case ctrl_F: return "ctrl-F";
		case ctrl_G: return "ctrl-G";
		case ctrl_H: return "ctrl-H";
		case ctrl_J: return "ctrl-J";
		case ctrl_K: return "ctrl-K";
		case ctrl_L: return "ctrl-L";
		case ctrl_Z: return "ctrl-Z";
		case ctrl_X: return "ctrl-X";
		case ctrl_C: return "ctrl-C";
		case ctrl_V: return "ctrl-V";
		case ctrl_B: return "ctrl-B";
		case ctrl_N: return "ctrl-N";
		case ctrl_M: return "ctrl-M";
		case KEY_1: return "1";
		case KEY_2: return "2";
		case KEY_3: return "3";
		case KEY_4: return "4";
		case KEY_5: return "5";
		case KEY_6: return "6";
		case KEY_7: return "7";
		case KEY_8: return "8";
		case KEY_9: return "9";
		case KEY_0: return "0";
		case KEY_ENTER: return "Enter/Return";
		case KEY_SPACE: return "Space";
		case KEY_EP: return "!";
		case KEY_QM: return "?";
		case KEY_AT: return "@";
		case KEY_HASH: return "#";
		case KEY_DOLLAR: return "$";
		case KEY_PER: return "%";
		case KEY_CARET: return "^";
		case KEY_AND: return "&";
		case KEY_STAR: return "*";
		case KEY_ONB: return "(";
		case KEY_CNB: return ")";
		case KEY_BSPACE: return "Backspace";
		case KEY_TILDA: return "~";
		case KEY_GRAVE: return "`";
		case KEY_UNDER: return "_";
		case KEY_HYPHEN: return "-";
		case KEY_PLUS: return "+";
		case KEY_EQUAL: return "=";
		case KEY_COLON: return ":";
		case KEY_SCOLON: return ";";
		case KEY_LCARROT: return "<";
		case KEY_RCARROT: return ">";
		case KEY_OCB: return "{";
		case KEY_CCB: return "}";
		case KEY_OSB: return "[";
		case KEY_CSB: return "]";
		case KEY_OR: return "|";
		case KEY_BSLASH: return "\\";
		case KEY_FSLASH: return "/";
		};
		assert(false);
	}
	*/	
};

#define SCF_STR(x, action) { #x, action }
#define SCF_BUTTON(x) { #x, x }

#define SCF_KB_STRINGS \
	SCF_STR(kbMove_n, P_MoveN), \
	SCF_STR(kbStrafe_l, P_MoveW), \
	SCF_STR(kbMove_s, P_MoveS), \
	SCF_STR(kbStrafe_r, P_MoveE), \
	SCF_STR(kbSlide_n, P_SlideN), \
	SCF_STR(kbSlide_w, P_SlideW), \
	SCF_STR(kbSlide_s, P_SlideS), \
	SCF_STR(kbSlide_e, P_SlideE), \
	SCF_STR(kbDash_n, P_DashN), \
	SCF_STR(kbDash_w, P_DashW), \
	SCF_STR(kbDash_s, P_DashS), \
	SCF_STR(kbDash_e, P_DashE), \
	SCF_STR(kbTurn_l, P_ChangeDirL), \
	SCF_STR(kbTurn_r, P_ChangeDirR), \
	SCF_STR(kbSwapWpn_1, P_ChangeWeapon1), \
	SCF_STR(kbSwapWpn_2, P_ChangeWeapon2), \
	SCF_STR(kbSwapWpn_3, P_ChangeWeapon3), \
	SCF_STR(kbSwapWpn_4, P_ChangeWeapon4), \
	SCF_STR(kbSwapWpn_5, P_ChangeWeapon5), \
	SCF_STR(kbSwapWpn_6, P_ChangeWeapon6), \
	SCF_STR(kbSwapWpn_7, P_ChangeWeapon7), \
	SCF_STR(kbSwapWpn_8, P_ChangeWeapon8), \
	SCF_STR(kbSwapWpn_9, P_ChangeWeapon9), \
	SCF_STR(kbSwapWpn_10, P_ChangeWeapon10), \
	SCF_STR(kbUseWeapon, P_UseWeapon), \
	SCF_STR(kbLeftArm_use, P_UseLeftArm), \
	SCF_STR(kbRightArm_use, P_UseRightArm), \
	SCF_STR(kbShowWpns, P_ShowWeapons), \
	SCF_STR(kbOpenConsole, CommandConsole), \
	SCF_STR(kbMelee, P_UseMelee), \
	SCF_STR(kbQuickShot, P_QuickShot), \
	SCF_STR(kbExitToPause, P_PauseMenu), \

#endif
