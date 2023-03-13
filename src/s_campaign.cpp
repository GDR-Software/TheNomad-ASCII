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
// DESCRIPTION: src/s_campaign.cpp
//  really just the initializers for running levels
//----------------------------------------------------------
#include "n_shared.h"
#include "scf.h"
#include "g_zone.h"
#include "g_items.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_scripted.h"
#include "s_world.h"
#include "g_playr.h"
#include "g_game.h"

static Game* game;
static coord_t origin = coord_t(260, 260);
constexpr float mob_mult_base = 1.5f;
constexpr float playr_mult_base = 1.7f;
constexpr float bot_mult_base = 1.4f;
constexpr float world_mult_base = 3.5f;

void CampaignAssigner(Game* const gptr)
{
    game = gptr;
}

nomadfixed_t G_CalcMobMult()
{
    switch (game->difficulty) {
    case DIF_NOOB:
        return float_to_fixed(mob_mult_base) >> 1;
        break;
    case DIF_RECRUIT:
        return float_to_fixed(mob_mult_base);
        break;
    case DIF_MERC:
        return float_to_fixed((mob_mult_base * 1.6f));
        break;
    case DIF_NOMAD:
        return float_to_fixed((mob_mult_base * 2.9f));
        break;
    case DIF_BLACKDEATH:
        return float_to_fixed((mob_mult_base * 5.6f));
        break;
    case DIF_MINORINCONVENIENCE:
        return float_to_fixed((mob_mult_base * 5.666f));
        break;
    default:
        LOG_WARN("unknown/invalid game->difficulty value %hu, returning DIF_NOOB mults", game->difficulty);
        return float_to_fixed(mob_mult_base) >> 1;
        break;
    };
    return (nomadfixed_t)0;
}

inline auto chapter_highest_diff(chapter_t& chapter) -> const char*
{
    if (!chapter.done || chapter.highest_completed == NUMDIFS)
        return "Uncompleted";
    else
        return difftostr(chapter.highest_completed);

    return (const char *)NULL;
};

static nomadbool_t inlvl = false;

static void G_StartupCampaign(nomadshort_t difficulty)
{
    if (inlvl)
        return;
    
    if (difficulty == DIF_NOOB) {
        for (auto& i : mobinfo) {
            i.health >>= 1;
            i.armor >>= 1;
        }
    }
    else if (difficulty == DIF_NOMAD) {
        for (auto& i : mobinfo) {
            i.health *= 1.5;
            i.armor *= 1.5;
        }
    }
    else if (difficulty == DIF_BLACKDEATH) {
        for (auto& i : mobinfo) {
            i.health *= 2;
            i.armor *= 2;
        }
    }
    else if (difficulty == DIF_MINORINCONVENIENCE) {
        for (auto& i : mobinfo) {
            i.health <<= 2;
            i.armor <<= 2;
        }
    }
    game->difficulty = difficulty;
    std::shared_ptr<Level> lvl = game->bff->levels[0];
    game->playr->pmode = P_MODE_MISSION;
    char mapbuffer[9][120][120];
    memset(mapbuffer, '#', sizeof(mapbuffer));
    nomadshort_t y{}, x{};
    memcpy(mapbuffer[8], lvl->lvl_map, sizeof(lvl->lvl_map));
    for (std::vector<Mob*>::iterator it = game->m_Active.begin(); it != game->m_Active.end(); ++it) {
        M_KillMob(it);
    }
    lvl->G_LoadSpawners(game->bff, mapbuffer);
    LOG_INFO("Initializing spawners");
    FILE* fp = fopen("Files/gamedata/RUNTIME/mapfile.txt", "w");
	for (y = 0; y < 80; ++y) {
		for (x = 0; x < MAP_MAX_X+160; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", mapbuffer[0][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", mapbuffer[7][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", mapbuffer[6][y][x]);
		}
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", mapbuffer[1][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", mapbuffer[8][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", mapbuffer[5][y][x]);
		}
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", mapbuffer[2][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", mapbuffer[3][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", mapbuffer[4][y][x]);
		}
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
    for (y = 0; y < 80; ++y) {
		for (x = 0; x < MAP_MAX_X+160; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
    NOMAD_ASSERT(fp, "failed to open Files/gamedata/RUNTIME/mapfile.txt!");
    fclose(fp);
    inlvl = true;
}


void G_CampaignSelect()
{
    MENU *menu;
	nomadshort_t n_choices, i;
	char c;	
    nomadshort_t selector = 0;
    nomadbool_t done = false;
    werase(game->screen);
	init_pair(4, COLOR_RED, COLOR_BLACK);
	init_pair(5, COLOR_CYAN, COLOR_BLACK);
    std::array<const char*, NUMDIFS> difs = {
        difftostr(DIF_NOOB),
        difftostr(DIF_RECRUIT),
        difftostr(DIF_MERC),
        difftostr(DIF_NOMAD),
        difftostr(DIF_BLACKDEATH),
        difftostr(DIF_MINORINCONVENIENCE)
    };

    n_choices = difs.size();
    std::vector<ITEM*> item_ls;
    item_ls.reserve(n_choices);
    for(i = 0; i < n_choices; ++i) {
        item_ls.emplace_back();
        item_ls.back() = new_item(difs[i], (const char *)NULL);
    }
    item_ls.push_back((ITEM *)NULL);
	menu = new_menu((ITEM **)item_ls.data());
    if (!menu)
		N_Error("failed to allocate ncurses menu memory!");
    
    set_menu_win(menu, game->screen);
    set_menu_sub(menu, derwin(game->screen, 10, 45, 3, 1));
	set_menu_format(menu, 10, 1);

    set_menu_mark(menu, " -> ");

	/* Print a border around the main window and print a title */
    box(game->screen, 0, 0);
    wattron(game->screen, COLOR_PAIR(5));
    mvwprintw(game->screen, 1, 55, "SELECT DIFFICULTY");
    wattroff(game->screen, COLOR_PAIR(5));
	mvwaddch(game->screen, 2, 46, '+');
	mvwhline(game->screen, 2, 47, '-', 33);
	mvwaddch(game->screen, 2, 80, '+');
    mvwaddch(game->screen, 1, 46, '|');
    mvwaddch(game->screen, 1, 80, '|');
    wrefresh(game->screen);
	/* Post the menu */
	post_menu(menu);
	wrefresh(game->screen);
    goto difselect;

difselect:
	while (1) {
        c = getc(stdin);
        switch (c) {
        case KEY_w: {
            --selector;
            if (selector < 0)
                selector = difs.size() - 1;
            menu_driver(menu, REQ_UP_ITEM);
            break; }
        case KEY_s: {
            ++selector;
            if (selector >= difs.size())
                selector = 0;
            menu_driver(menu, REQ_DOWN_ITEM);
            break; }
        case ctrl('x'):
        case KEY_q:
            goto done;
            break;
        case '\n':
        case KEY_SPACE:
            G_StartupCampaign(selector);
            break;
        default:
            continue;
            break;
        }
        wrefresh(game->screen);
        std::this_thread::sleep_for(std::chrono::milliseconds(ticrate_mil));
	}
done:
    game->gamestate = GS_MENU;
    unpost_menu(menu);
    for (nomaduint_t i = 0; i < n_choices; ++i) {
		free_item(item_ls[i]);
	}
	free_menu(menu);
}