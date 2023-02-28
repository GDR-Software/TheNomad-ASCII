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
//  src/m_inventory.cpp
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

static Playr* playr;
static Game* game;

void InvAssigner(Game* const gptr)
{
    game = gptr;
    playr = game->playr;
}

#define SORT_ABC      0b0000000000 // sort the inventory alphabetically
#define SORT_PRICEY   0b0000000001 // sort based on monetary value
#define SORT_WEIGHT   0b0000000010 // sort based on high-to-low weight

inline void Inv_DisplayMercMissions(const std::vector<Mission>& m_ls);
inline void Inv_DisplayItems();

static inline void Inv_SortItemsABC(void)
{
    std::sort(playr->inv.begin(), playr->inv.end(),
        [](const item_t& a, const item_t& b){ return a.name < b.name; } );
}
static inline void Inv_SortItemPricey(void)
{
    std::sort(playr->inv.begin(), playr->inv.end(),
        [](const item_t& a, const item_t& b){ return a.item_cost < b.item_cost; } );
}
static inline void Inv_SortItemsWeight(void)
{
    std::sort(playr->inv.begin(), playr->inv.end(),
        [](const item_t& a, const item_t& b){ return a.item_weight < b.item_cost; });
}
inline void Inv_SortItems(nomadenum_t sorter)
{
    
    switch (sorter) {
    case SORT_ABC:
        Inv_SortItemsABC();
        break;
    case SORT_PRICEY:
        Inv_SortItemPricey();
        break;
    case SORT_WEIGHT:
        Inv_SortItemsWeight();
        break;
    default:
        N_Error("Unknown/Invalid sorter value for Inv_SortItems! sorter = %hu", sorter);
        break;
    };
}

nomaduint_t P_GetNumItems()
{
    nomaduint_t numitems = 0;
    for (const auto& i : playr->inv) {
        if (i.name)
            ++numitems;
    }
    return numitems;
}


void G_DisplayInventory(void)
{
    ITEM **item_ls;
	char c;	
	MENU *menu;
    nomadint_t n_choices, i;
    nomadshort_t selector = 0;
    nomadbool_t done = false;
    werase(game->screen);
	init_pair(4, COLOR_RED, COLOR_BLACK);
	init_pair(5, COLOR_CYAN, COLOR_BLACK);
    std::vector<const char*> choices;
    for (const auto& i : playr->inv) {
        if (i.name) {
            choices.push_back(i.name);
        }
    }
    choices.emplace_back();
    choices.back() = (const char*)NULL;

	/* Create items */
    n_choices = choices.size();
    item_ls = (ITEM **)Z_Malloc(n_choices * sizeof(ITEM *), TAG_STATIC, &item_ls);
    for(i = 0; i < n_choices; ++i) {
        if (playr->inv[i].name) {
            item_ls[i] = new_item(playr->inv[i].name, NULL);
        }
    }

	/* Crate menu */
	menu = new_menu((ITEM **)item_ls);
     
	/* Set main window and sub window */
    set_menu_win(menu, game->screen);
    set_menu_sub(menu, derwin(game->screen, 10, 38, 3, 1));
	set_menu_format(menu, 10, 1);
			
	/* Set menu mark to the string " * " */
    set_menu_mark(menu, " -> ");

	/* Print a border around the main window and print a title */
    box(game->screen, 0, 0);
    wattron(game->screen, COLOR_PAIR(5));
    mvwprintw(game->screen, 1, 55, "[Inventory]");
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
	while (1) {
        c = getc(stdin);
        switch (c) {
        case KEY_w: {
            --selector;
            if (selector < 0)
                selector = choices.size() - 1;
            menu_driver(menu, REQ_UP_ITEM);
            break; }
        case KEY_s: {
            ++selector;
            if (selector >= choices.size())
                selector = 0;
            menu_driver(menu, REQ_DOWN_ITEM);
            break; }
        case ctrl('x'):
        case KEY_q:
            game->gamestate = GS_MENU;
            goto done;
            break;
        default:
            continue;
            break;
        }
        wrefresh(game->screen);
        std::this_thread::sleep_for(std::chrono::milliseconds(ticrate_mil));
	}
done:
    unpost_menu(menu);
    free_menu(menu);
    for(i = 0; i < n_choices; ++i)
        free_item(item_ls[i]);
}