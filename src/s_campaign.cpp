#include "g_game.h"
#include "s_scripted.h"

static Game* game;

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

void G_CampaignSelect()
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

    const char* choices[] = {
        "Mission 1: From Eagle's Peak",
        "Mission 2: To Guns 'n' Grenades",
        "Mission 3: Invasion of Galakas",
        "Mission 4: A Duel With Death",
        (const char *)NULL
    };
	/* Create items */
    n_choices = ARRAY_SIZE(choices);
    item_ls = (ITEM **)Z_Malloc(n_choices * sizeof(ITEM *), TAG_STATIC, &item_ls);
    for(i = 0; i < n_choices; ++i)
        item_ls[i] = new_item(choices[i], NULL);

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
    mvwprintw(game->screen, 1, 55, "[Mission Select]");
    wattroff(game->screen, COLOR_PAIR(5));
	mvwaddch(game->screen, 2, 46, '+');
	mvwhline(game->screen, 2, 47, '-', 33);
	mvwaddch(game->screen, 2, 80, '+');
    mvwaddch(game->screen, 1, 46, '|');
    mvwaddch(game->screen, 1, 80, '|');
	mvwaddstr(game->screen, 2, 85, "[Mission Statistics]");
    mvwprintw(game->screen, 3, 85, "Highest Difficulty: %s",
        chapter_highest_diff(game->playr->c_stage->chapters[selector]));
    mvwprintw(game->screen, 4, 85, "Completed: %s",
        booltostr(game->playr->c_stage->chapters[selector].done));
    wrefresh(game->screen);
	/* Post the menu */
	post_menu(menu);
	wrefresh(game->screen);
	while (1) {
        mvwaddstr(game->screen, 2, 85, "[Mission Statistics]");
        mvwprintw(game->screen, 3, 85, "Highest Difficulty: %s",
            chapter_highest_diff(game->playr->c_stage->chapters[selector]));
        mvwprintw(game->screen, 4, 85, "Completed: %s",
            booltostr(game->playr->c_stage->chapters[selector].done));
        c = getc(stdin);
        switch (c) {
        case KEY_w: {
            --selector;
            if (selector < 0)
                selector = game->playr->c_stage->chapters.size() - 1;
            menu_driver(menu, REQ_UP_ITEM);
            break; }
        case KEY_s: {
            ++selector;
            if (selector >= game->playr->c_stage->chapters.size())
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