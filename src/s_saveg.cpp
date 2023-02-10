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
//  src/s_saveg.cpp
//----------------------------------------------------------
#include "g_game.h"

#undef byte
#include "nlohmann/json.hpp"

static constexpr auto svdir = "Files/gamedata/SVFILES/";

enum : int8_t
{
	NGD_CHUNK_PLAYR,
	NGD_CHUNK_MOB,
	NGD_CHUNK_NPC,
	NGD_CHUNK_WORLD
};

#ifdef UNIX_NOMAD
typedef int32_t num_t;
#elif defined(WIN32_NOMAD)
typedef INT32 num_t;
#endif

static constexpr num_t HEADER = 0x5f3759df;

typedef struct _ngd_header
{
	num_t header = HEADER;
	num_t nummobs = 0;
	num_t numnpcs = 0;
	num_t numchunks = 0;
	char svname[256];
} ngd_header_t;

typedef struct _ngd_chunk
{
	num_t chunktype;
	char buffer[BUFFER_SIZE];
} ngd_chunk_t;

// nomadascii game data (.ngd) save-file
typedef struct _ngd_file
{
	ngd_header_t header;
	ngd_chunk_t* chunks;
} ngd_file_t;

#ifdef UNIX_NOMAD
static uint32_t countfiles(const char *path) {
    DIR *dir_ptr = NULL;
    struct dirent *direntp;
    char *npath;
    if (!path) return 0;
    if( (dir_ptr = opendir(path)) == NULL ) return 0;

    int count=0;
    while( (direntp = readdir(dir_ptr)))
    {
        if (strcmp(direntp->d_name,".")==0 ||
            strcmp(direntp->d_name,"..")==0) continue;
        switch (direntp->d_type) {
        case DT_REG: {
            if (!strstr(direntp->d_name, ".dat"))
				++count;
            break; }
        case DT_DIR:            
            npath = (char *)malloc(strlen(path) + strlen(direntp->d_name)+2);
            sprintf(npath,"%s/%s",path, direntp->d_name);
            count += countfiles(npath);
            free(npath);
            break;
        };
    }
    closedir(dir_ptr);
    return count;
}
#endif

#define MAGIC_XOR 300

using json = nlohmann::json;

static void G_ArchivePlayr(const Playr* playr, json& data)
{
	auto health = playr->health.load();
	auto armor = playr->armor.load();
	data["playr"] = {
		{"name", playr->name},
		{"pdir", playr->pdir},
		{"health", health},
		{"armor", armor},
		{"pos.y", playr->pos.y},
		{"pos.x", playr->pos.x},
		{"wpn_slot_current", playr->wpn_slot_current},
		{"pstate", playr->pstate.id},
		{"pticker", playr->pticker},
		{"coin", playr->coin},
		{"lvl", playr->lvl},
		{"sprite", playr->sprite},
		{"sector_id", playr->sector_id},
	};
}

static void G_ArchiveMobs(const std::vector<Mob*>& m_Active, json& data)
{
	for (nomaduint_t i = 0; i < m_Active.size(); ++i) {
		std::string node_name = "mob_"+std::to_string(i);
		Mob* const mob = m_Active[i];
		auto health = mob->health.load();
		auto armor = mob->armor.load();
		data[node_name] = {
			{"health", health},
			{"armor", armor},
			{"mpos.y", mob->mpos.y},
			{"mpos.x", mob->mpos.x},
			{"mdir", mob->mdir},
		};
	}
}
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active, json& data, nomaduint_t nummobs)
{
	for (nomaduint_t i = 0; i < nummobs; ++i) {
		std::string node_name = "mob_"+std::to_string(i);
		Mob* const mob = m_Active[i];
		mob->health = data[node_name]["health"];
		mob->armor = data[node_name]["armor"];
		mob->mpos.y = data[node_name]["mpos.y"];
		mob->mpos.x = data[node_name]["mpos.x"];
		mob->mdir = data[node_name]["mdir"];
	}
}

static void G_ArchiveBots(const std::vector<NPC*>& b_Active, json& data)
{
	for (nomaduint_t i = 0; i < b_Active.size(); ++i) {
		std::string node_name = "bot_"+std::to_string(i);
		NPC* const npc = b_Active[i];
		auto health = npc->health.load();
		auto armor = npc->armor.load();
		data[node_name] = {
			{"health", health},
			{"armor", armor},
		};
	}
}

void Game::G_SaveGame(void)
{
	constexpr const char* svname = "nomadascii_sv";
	const char* svfile = "nomadsv.ngd";
	std::ofstream file(svfile, std::ios::out | std::ios::trunc);
	json data;
	data["header"] = {
		{"version", NOMAD_VERSION},
		{"version.major", NOMAD_VERSION_MAJOR},
		{"version.update", NOMAD_VERSION_MINOR},
		{"version.patch", NOMAD_VERSION_PATCH},
		{"svname", svname},
		{"nummobs", m_Active.size()},
		{"numbots", b_Active.size()},
		{"slot", 0},
	};
	G_ArchivePlayr(playr, data);
	G_ArchiveMobs(m_Active, data);
	file << data;
	file.close();
}


bool Game::G_LoadGame(const char* svfile)
{
	std::ifstream file(svfile, std::ios::in);
	json data = json::parse(file);
	file.close();
	nomaduint_t nummobs = data["header"]["nummobs"];
	playr->pos.y = data["playr"]["pos.y"];
	playr->pos.x = data["playr"]["pos.x"];
	G_UnArchiveMobs(m_Active, data, nummobs);
	return true;
}

/*

static uint32_t countfiles(const char *path);
static void UpdateDat();
static void G_GetSlots(Game* const game, MENU* svmenu, ITEM** files, std::vector<std::string>& svnames)
{
	std::vector<std::string> svfiles;
	DIR* savedir;
	struct dirent *dir;
	savedir = opendir(svdir);
	if (savedir) {
		while ((dir = readdir(savedir)) != NULL) {
			if (strstr(dir->d_name, ".dat") == NULL) {
				svfiles.push_back(dir->d_name);
			}
		}
		closedir(savedir);
	} else {
		N_Error("Failed to open the Save File directory %s!", svdir);
	}
	for (const auto& i : svfiles)
		svnames.push_back(i);
	werase(game->screen);
	files = (ITEM **)Z_Malloc(svnames.size(), TAG_STATIC, &files);
	std::vector<const char*> savenames;
	for (const auto& i : svnames) {
		std::string file = svdir;
		file += i;
		fp = fopen(file.c_str(), "r");
		struct _ngd_header header;
		fread(&header, sizeof(struct _ngd_header), 1, fp);
		savenames.push_back(header.svname);
	}
	for (nomadint_t i = 0; i < savenames.size(); ++i)
		files[i] = new_item(savenames[i], NULL);
	
	files[svnames.size()] = (ITEM *)NULL;
	svmenu = new_menu((ITEM **)files);
	set_menu_win(svmenu, game->screen);
	set_menu_sub(svmenu, derwin(game->screen, (getmaxy(game->screen) - 2), getmaxx(game->screen), 2, 0));
	set_menu_mark(svmenu, " -> ");
	box(game->screen, 0, 0);
	mvwaddstr(game->screen, 0, 55, "[SAVE SLOTS]");
	wrefresh(game->screen);

	post_menu(svmenu);
	wrefresh(game->screen);
}

void Game::G_SaveState(void)
{
	MENU* svmenu;
	ITEM** files;
	std::vector<std::string> svnames;
	G_GetSlots(this, svmenu, files, svnames);
	nomadint_t selector;
	nomadbool_t selecting = false;
	char c;
	selector = 0;
	while ((c = kbhit()) != KEY_q) {
		switch (c) {
		case KEY_w: {
			--selector;
			if (selector < 0) {
				selector = svnames.size();
			}
			menu_driver(svmenu, REQ_UP_ITEM);
			break; }
		case KEY_s: {
			if (!selecting) {
				++selector;
				if (selector >= svnames.size()) {
					selector = 0;
				}
				menu_driver(svmenu, REQ_DOWN_ITEM);
			}
			break; }
		case 10:
			if (!selecting)
				selecting = true;
			else
				selecting = false;
			break;
		case KEY_1: {
			echo();
			char svname[257];
			svname[256] = '\0';
			mvwprintw(screen, 1, 50, "Enter Name For Save File: ");
			mvwscanw(screen,  1, 78, "%s", svname);
			noecho();
			G_SaveGame(svname);
			break; }
		case KEY_2: {
			mvwprintw(screen, 1, 5, "<Enter> overwrite save slot");
			char a = wgetch(screen);
			switch (a) {
			case 10:
				G_SaveGame(svnames[selector].c_str());
				break;
			default: break;
			};
			break; }
		default: break;
		};
		wrefresh(screen);
	}
	unpost_menu(svmenu);
	for (nomaduint_t i = 0; i < svnames.size(); ++i)
		free_item(files[i]);
	Z_Free(files);
	free_menu(svmenu);
	UpdateDat();
}

void Game::G_SaveRecentSlot(void)
{

}

static uint32_t countfiles(const char *path) {
    DIR *dir_ptr = NULL;
    struct dirent *direntp;
    char *npath;
    if (!path) return 0;
    if( (dir_ptr = opendir(path)) == NULL ) return 0;

    int count=0;
    while( (direntp = readdir(dir_ptr)))
    {
        if (strcmp(direntp->d_name,".")==0 ||
            strcmp(direntp->d_name,"..")==0) continue;
        switch (direntp->d_type) {
        case DT_REG: {
            if (!strstr(direntp->d_name, ".dat"))
				++count;
            break; }
        case DT_DIR:            
            npath = (char *)malloc(strlen(path) + strlen(direntp->d_name)+2);
            sprintf(npath,"%s/%s",path, direntp->d_name);
            count += countfiles(npath);
            free(npath);
            break;
        };
    }
    closedir(dir_ptr);
    return count;
}

static void UpdateDat()
{
	struct _ngd_dat_file file;
	file.numsv = countfiles(svdir);
	FILE* dat = fopen("Files/gamedata/SVFILES/ngd_data.dat", "w");
	fwrite(&file, sizeof(struct _ngd_dat_file), 1, dat);
	fclose(dat);
}

static void G_ArchivePlayr(const Playr* playr);
static void G_UnArchivePlayr(Playr* const playr);
static void G_ArchiveWorld(const World* world);
static void G_UnArchiveWorld(World* const world);
static void G_ArchiveMobs(const std::vector<Mob*>& m_Active);
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active);
static void G_ArchiveNPCs(const std::vector<NPC*>& b_Active);
static void G_UnArchiveNPCs(std::vector<NPC*>& b_Active);

void Game::G_SaveGame(const char* svfile)
{
	ngd_header_t header;
	strncpy(header.svname, svfile, strlen(svfile));
	uint32_t filecount = countfiles(svdir);
	std::string svname = svdir;
	svname += "nomadsv_";
	svname += std::to_string(filecount);
	svname += ".ngd";
	fp = fopen(svname.c_str(), "w");
	fwrite(&header.header, sizeof(uint64_t), 1, fp);
	fwrite(&header.svname, sizeof(char), sizeof(header.svname), fp);
	fclose(fp);
	UpdateDat();
}

bool Game::G_LoadGame(const char* svfile)
{
	
	return true;
}

static void G_ArchivePlayr(const Playr* playr, std::ofstream& file)
{
	
}
static void G_UnArchivePlayr(Playr* const playr)
{
}

static void G_ArchiveWorld(const World* world)
{
}
static void G_UnArchiveWorld(World* const world)
{
}

static void G_ArchiveMobs(const std::vector<Mob*>& m_Active, std::ofstream& file)
{
	
}
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active)
{
}

static void G_ArchiveNPCs(const std::vector<NPC*>& b_Active)
{
}
static void G_UnArchiveNPCs(std::vector<NPC*>& b_Active)
{
}

static void G_LoadNGD(std::vector<std::string>& tokens, const char* svname)
{
	std::ifstream file(svname, std::ios::in);
	std::string line;
	nomaduint_t i;
	while (std::getline(file, line)) {
		std::string buf;
		for (i = 0; i < line.size(); ++i) {
			switch (line[i]) {
			case '\n':
			case ' ':
			case '\0':
				tokens.push_back(buf);
				buf.clear();
				break;
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case '-':
			case ';':
			case '=':
			case '+':
			case '\\':
			case '/':
			case ':':
				buf.push_back(line[i]);
				tokens.push_back(buf);
				buf.clear();
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '_':
				buf.push_back(line[i]);
				break;
			};
		}
	}
	file.close();
}

*/
