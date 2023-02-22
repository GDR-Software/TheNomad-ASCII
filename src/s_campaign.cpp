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
	char c;	
    nomadshort_t selector = 0;
    nomadbool_t done = false;
    werase(game->screen);
	init_pair(4, COLOR_RED, COLOR_BLACK);
	init_pair(5, COLOR_CYAN, COLOR_BLACK);

    const char *choices[] = {
        "Mission 1: ",
        (const char*)NULL
    };

    Menu menu(choices, " -> ");

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
	post_menu(menu.menu);
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
            menu.DownItem();
            break; }
        case KEY_s: {
            ++selector;
            if (selector >= game->playr->c_stage->chapters.size())
                selector = 0;
            menu.DownItem();
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
    unpost_menu(menu.menu);
}