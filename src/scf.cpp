//----------------------------------------------------------
//
// Copyright (C) GDR Games 2022-2023
//
// This source code is available for distribution and/or
// modification under the terms of either the Apache License
// v2.0 as published by the Apache Software Foundation, or
// the GNU General Public License v2.0 as published by the
// Free Software Foundation.
//
// This source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY. If you are using this code for personal,
// non-commercial/monetary gain, you may use either of the
// licenses permitted, otherwise, you must use the GNU GPL v2.0.
//
// DESCRIPTION: src/scf.cpp
//  parsing config files that inputs directly into the game,
//  effectively settings
//----------------------------------------------------------
#include "n_shared.h"
#include "scf.h"
#include "g_obj.h"
#include "g_items.h"
#include "s_scripted.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_world.h"
#include "g_playr.h"
#include "scf_lexer.hpp"
#include "g_game.h"

static std::vector<scf::keybind_t> binds;
static void SCF_GetBind(Lexer& lex, Token& tok);
static void SCF_ParseIdentifier(Lexer& lex, Token& tok, const std::string_view& str);

namespace scf {
    nomadbool_t music_on = false;
    nomadbool_t sfx_on = false;
    nomadfloat_t music_vol = 0;
    nomadfloat_t sfx_vol = 0;
    nomadushort_t mobspeed = MOB_SPEED_STD;
    nomadushort_t fov[2] = {MAX_VERT_FOV, MAX_HORZ_FOV};
    namespace launch {
        nomadbool_t fastmobs1 = false;
		nomadbool_t fastmobs2 = false;
		nomadbool_t fastmobs3 = false;
		nomadbool_t ext_bff = false;
        nomadbool_t ext_scf = false;
		nomadbool_t deafmobs = false;
		nomadbool_t blindmobs = false;
		nomadbool_t nosmell = false;
		nomadbool_t nomobs = false;
		nomadbool_t godmode = false;
		nomadbool_t infinite_ammo = false;
		nomadbool_t bottomless_clip = false;
		nomadbool_t devmode = false;
	};
    const char* GetSCFBind(nomaduint_t bind)
	{
		switch (bind) {
		case kbMove_n: return "Move Forward";
		case kbStrafe_l: return "Strafe Left";
		case kbMove_s: return "Move Backwards";
		case kbStrafe_r: return "Strafe Right";
		case kbDash_n: return "Dash North";
		case kbDash_w: return "Dash West";
		case kbDash_s: return "Dash South";
		case kbDash_e: return "Dash East";
		case kbMelee: return "Use Melee";
		case kbExitToPause: return "Exit to Pause Menu";
		case kbSlide_n: return "Slide North";
		case kbSlide_w: return "Slide West";
		case kbSlide_s: return "Slide South";
		case kbSlide_e: return "Slide East";
		case kbUseWeapon: return "Use Current Weapon";
		case kbTurn_l: return "Turn Left";
		case kbTurn_r: return "Turn Right";
		case kbSwapWpn_1: return "Swap To Right Arm";
		case kbSwapWpn_2: return "Swap To Left Arm";
		case kbSwapWpn_3: return "Swap To Sidearm";
		case kbSwapWpn_4: return "Swap To Heavy Sidearm";
		case kbSwapWpn_5: return "Swap To Primary";
		case kbSwapWpn_6: return "Swap To Heavy Primary";
		case kbSwapWpn_7: return "Swap To Shotgun";
		case kbSwapWpn_8: return "Swap To Melee 1";
		case kbSwapWpn_9: return "Swap To Melee 2";
		case kbSwapWpn_10: return "Swap To Melee 3";
		case kbShowWpns: return "Show All Weapons";
		case kbQuickShot: return "Quick-Shot w/ Current Weapon";
		case kbOpenConsole: return "Open Text/Command Console";
		};
		assert(false);
        return NULL;
	}

    scfbind kbStrings[NUMBINDS] = { SCF_KB_STRINGS };
    scfbutton buttonStrings[NUMBUTTONS] = {
        	SCF_BUTTON(KEY_q),
	        SCF_BUTTON(KEY_w),
	        SCF_BUTTON(KEY_e),
	        SCF_BUTTON(KEY_r),
	        SCF_BUTTON(KEY_t),
	        SCF_BUTTON(KEY_y),
	        SCF_BUTTON(KEY_u),
	        SCF_BUTTON(KEY_i),
	        SCF_BUTTON(KEY_o),
	        SCF_BUTTON(KEY_p),
	        SCF_BUTTON(KEY_a),
	        SCF_BUTTON(KEY_s),
	        SCF_BUTTON(KEY_d),
	        SCF_BUTTON(KEY_f),
	        SCF_BUTTON(KEY_g),
	        SCF_BUTTON(KEY_h),
	        SCF_BUTTON(KEY_j),
	        SCF_BUTTON(KEY_l),
	        SCF_BUTTON(KEY_z),
	        SCF_BUTTON(KEY_x),
	        SCF_BUTTON(KEY_c),
	        SCF_BUTTON(KEY_v),
	        SCF_BUTTON(KEY_b),
	        SCF_BUTTON(KEY_n),
	        SCF_BUTTON(KEY_m),
	        SCF_BUTTON(KEY_Q),
	        SCF_BUTTON(KEY_E),
	        SCF_BUTTON(KEY_R),
	        SCF_BUTTON(KEY_T),
	        SCF_BUTTON(KEY_Y),
	        SCF_BUTTON(KEY_U),
	        SCF_BUTTON(KEY_I),
	        SCF_BUTTON(KEY_O),
	        SCF_BUTTON(KEY_P),
	        SCF_BUTTON(KEY_S),
	        SCF_BUTTON(KEY_D),
	        SCF_BUTTON(KEY_F),
	        SCF_BUTTON(KEY_G),
	        SCF_BUTTON(KEY_H),
	        SCF_BUTTON(KEY_J),
	        SCF_BUTTON(KEY_K),
	        SCF_BUTTON(KEY_L),
	        SCF_BUTTON(KEY_Z),
	        SCF_BUTTON(KEY_X),
	        SCF_BUTTON(KEY_C),
	        SCF_BUTTON(KEY_V),
	        SCF_BUTTON(KEY_B),
	        SCF_BUTTON(KEY_N),
	        SCF_BUTTON(KEY_M),
	        SCF_BUTTON(KEY_TILDA),
	        SCF_BUTTON(KEY_GRAVE),
	        SCF_BUTTON(KEY_ENTER),
	        SCF_BUTTON(KEY_SPACE),
	        SCF_BUTTON(KEY_TAB),
	        SCF_BUTTON(KEY_BSPACE),
	        SCF_BUTTON(KEY_SCOLON),
	        SCF_BUTTON(KEY_COLON),
	        SCF_BUTTON(KEY_DOT),
	        SCF_BUTTON(KEY_COMMA),
	        SCF_BUTTON(KEY_LCARROT),
	        SCF_BUTTON(KEY_RCARROT),
	        SCF_BUTTON(KEY_BSLASH),
	        SCF_BUTTON(KEY_FSLASH),
	        SCF_BUTTON(KEY_EP),
	        SCF_BUTTON(KEY_QM),
	        SCF_BUTTON(KEY_OSB),
	        SCF_BUTTON(KEY_CSB),
	        SCF_BUTTON(KEY_OCB),
	        SCF_BUTTON(KEY_CCB),
	        SCF_BUTTON(KEY_HASH),
	        SCF_BUTTON(KEY_DOLLAR),
	        SCF_BUTTON(KEY_PER),
	        SCF_BUTTON(KEY_AT),
	        SCF_BUTTON(KEY_CARET),
	        SCF_BUTTON(KEY_AND),
	        SCF_BUTTON(KEY_STAR),
	        SCF_BUTTON(KEY_ONB),
	        SCF_BUTTON(KEY_CNB),
	        SCF_BUTTON(KEY_HYPHEN),
	        SCF_BUTTON(KEY_UNDER),
	        SCF_BUTTON(KEY_EQUAL),
	        SCF_BUTTON(KEY_PLUS),
	        SCF_BUTTON(KEY_OR),
            SCF_BUTTON(KEY_1),
            SCF_BUTTON(KEY_2),
            SCF_BUTTON(KEY_3),
            SCF_BUTTON(KEY_4),
            SCF_BUTTON(KEY_5),
            SCF_BUTTON(KEY_6),
            SCF_BUTTON(KEY_7),
            SCF_BUTTON(KEY_8),
            SCF_BUTTON(KEY_9),
            SCF_BUTTON(KEY_0),
            SCF_BUTTON(CTRL_q),
            SCF_BUTTON(CTRL_w),
            SCF_BUTTON(CTRL_e),
            SCF_BUTTON(CTRL_r),
            SCF_BUTTON(CTRL_t),
            SCF_BUTTON(CTRL_y),
            SCF_BUTTON(CTRL_u),
            SCF_BUTTON(CTRL_i),
            SCF_BUTTON(CTRL_o),
            SCF_BUTTON(CTRL_p),
            SCF_BUTTON(CTRL_a),
            SCF_BUTTON(CTRL_s),
            SCF_BUTTON(CTRL_d),
            SCF_BUTTON(CTRL_f),
            SCF_BUTTON(CTRL_g),
            SCF_BUTTON(CTRL_h),
            SCF_BUTTON(CTRL_j),
            SCF_BUTTON(CTRL_l),
            SCF_BUTTON(CTRL_z),
            SCF_BUTTON(CTRL_x),
            SCF_BUTTON(CTRL_c),
            SCF_BUTTON(CTRL_v),
            SCF_BUTTON(CTRL_b),
            SCF_BUTTON(CTRL_n),
            SCF_BUTTON(CTRL_m),
            SCF_BUTTON(CTRL_Q),
            SCF_BUTTON(CTRL_E),
            SCF_BUTTON(CTRL_R),
            SCF_BUTTON(CTRL_T),
            SCF_BUTTON(CTRL_Y),
            SCF_BUTTON(CTRL_U),
            SCF_BUTTON(CTRL_I),
            SCF_BUTTON(CTRL_O),
            SCF_BUTTON(CTRL_P),
            SCF_BUTTON(CTRL_S),
            SCF_BUTTON(CTRL_D),
            SCF_BUTTON(CTRL_F),
            SCF_BUTTON(CTRL_G),
            SCF_BUTTON(CTRL_H),
            SCF_BUTTON(CTRL_J),
            SCF_BUTTON(CTRL_K),
            SCF_BUTTON(CTRL_L),
            SCF_BUTTON(CTRL_Z),
            SCF_BUTTON(CTRL_X),
            SCF_BUTTON(CTRL_C),
            SCF_BUTTON(CTRL_V),
            SCF_BUTTON(CTRL_B),
            SCF_BUTTON(CTRL_N),
            SCF_BUTTON(CTRL_M),
            SCF_BUTTON(KEY_k),
    };

    void G_LoadSCF(const char* filepath)
    {
        puts("G_LoadSCF(): Loading SACE Configuration File...");
       	if (!strstr(filepath, ".scf")) {
    		N_Error("SACE Configuration File (scf) isn't correct format! (must be suffixed with .scf)");
        }
        LOG_INFO("Attempting to stat() .scf file");
        struct stat fdata;
        if (stat(filepath, &fdata) == -1) {
            N_Error("Failed to get data from .scf file!");
        }
	    LOG_INFO("Successful stat() of .scf file");
        FILE* fp = fopen(filepath, "r");
        if (!fp) {
            N_Error("Failed to open .scf file!");
        }
        assert(fp);
        LOG_INFO("Successful fopen() of .scf file");
        const char* buffer = (char *)mmap(NULL, fdata.st_size, PROT_READ, MAP_PRIVATE, fileno(fp), 0);
	    if (!buffer) {
            N_Error("Failed to read data from .scf file!");
        }
        assert(buffer);
        LOG_INFO("Successful read of .scf file");
        Lexer lex(buffer);
        munmap(&buffer, fdata.st_size);
        fclose(fp);
        for (auto tok = lex.next(); !tok.is(Token::Kind::End); tok = lex.next()) {
            switch (tok.kind()) {
            case Token::Kind::Unexpected:
                N_Error("Found unexpected token within .scf file!");
                break;
            case Token::Kind::Identifier:
                SCF_ParseIdentifier(lex, tok, tok.lexeme());
                break;
            };
        }
        nomaduint_t index = 0;
        for (auto& i : kb_binds) {
            i = binds[index];
            ++index;
        }
    }
};

static void SCF_GetBind(Lexer& lex, Token& tok)
{
    binds.emplace_back();
    tok = lex.next();
    const std::string_view bindstr = tok.lexeme();
    tok = lex.next();
    const std::string_view buttonstr = tok.lexeme();

    scf::keybind_t bind;
    bind.bind = scf::kbMove_n;
    for (; bind.bind < scf::NUMBINDS; ++bind.bind) {
        if (scf::kbStrings[bind.bind].str == bindstr) {
            break;
        }
    }
    bind.actionp = scf::kbStrings[bind.bind].actionp;
    for (const auto& i : scf::buttonStrings) {
        if (i.str == buttonstr) {
            bind.button = i.button;
        }
    }
    binds.back() = bind;
}

static void SCF_ParseIdentifier(Lexer& lex, Token& tok, const std::string_view& str)
{
    // found a keyboard bind
    if (str == "bind") {
        SCF_GetBind(lex, tok);
    }
}

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
