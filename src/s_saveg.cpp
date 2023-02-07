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
#include <dirent.h>
#include <limits.h>

#define BUFFER_SIZE 2*1024

static FILE* fp;
static constexpr uint64_t HEADER = 0x5f3759df;
static constexpr auto svdir = "Files/gamedata/SVFILES/";

enum : int8_t
{
	NGD_CHUNK_PLAYR,
	NGD_CHUNK_MOB,
	NGD_CHUNK_NPC,
	NGD_CHUNK_WORLD
};

typedef struct _ngd_header
{
	uint64_t header = HEADER;
	uint16_t nummobs = 0;
	uint16_t numnpcs = 0;
	uint16_t numchunks = 0;
} ngd_header_t;

typedef struct _ngd_chunk
{
	int8_t chunktype;
	char buffer[BUFFER_SIZE];
} ngd_chunk_t;

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

void Game::G_SaveGame(void)
{
//	uint32_t count = countfiles("Files/gamedata/SVFILES/");
	char svname[256];
	snprintf(svname, sizeof(svname), "Files/gamedata/SVFILES/nomadsv.ngd");
	fp = fopen(svname, "wb");

	_ngd_header header;
	header.nummobs = m_Active.size();
	header.numnpcs = b_Active.size();
	fwrite(&header, sizeof(_ngd_header), 1, fp);

	// player data
	fwrite((char *)&playr->name, sizeof(char), 256, fp);
	fwrite(&playr->health, sizeof(playr->health), 1, fp);
	fwrite(&playr->armor, sizeof(playr->armor), 1, fp);
	fwrite(&playr->coin, sizeof(playr->coin), 1, fp);
	fwrite(&playr->lvl, sizeof(playr->lvl), 1, fp);
	fwrite(&playr->pdir, sizeof(playr->pdir), 1, fp);
	fwrite(&playr->inv, sizeof(Item), MAX_PLAYR_ITEMS, fp);
	fwrite(&playr->P_wpns, sizeof(Weapon), MAX_PLAYR_WPNS, fp);
	fwrite(&playr->wpn_slot_current, sizeof(playr->wpn_slot_current), 1, fp);
	fwrite(&playr->pos.y, sizeof(playr->pos.y), 1, fp);
	fwrite(&playr->pos.x, sizeof(playr->pos.x), 1, fp);
	
	// world data
	fwrite(&world->day, sizeof(world->day), 1, fp);
	fwrite(&world->time, sizeof(world->time), 1, fp);
	fwrite(&world->temperature, sizeof(world->temperature), 1, fp);

	// mob data
	for (nomaduint_t i = 0; i < m_Active.size(); ++i) {
		const Mob* mob = m_Active[i];
		fwrite(&mob->health, sizeof(mob->health), 1, fp);
		fwrite(&mob->armor, sizeof(mob->armor), 1, fp);
		fwrite(&mob->mpos.y, sizeof(mob->mpos.y), 1, fp);
		fwrite(&mob->mpos.x, sizeof(mob->mpos.x), 1, fp);
		fwrite(&mob->mstate, sizeof(mob->mstate), 1, fp);
		fwrite(&mob->mticker, sizeof(mob->mticker), 1, fp);
		fwrite(&mob->mdir, sizeof(mob->mdir), 1, fp);
		fwrite(&mob->sector_id, sizeof(mob->sector_id), 1, fp);
		fwrite(&mob->stepcounter, sizeof(mob->stepcounter), 1, fp);
		fwrite(&mob->c_mob, sizeof(mobj_t), 1, fp);
	}
	
	// npc data
	for (nomaduint_t i = 0; i < b_Active.size(); ++i) {
		const NPC* npc = b_Active[i];
		fwrite(&npc->armor, sizeof(npc->armor), 1, fp);
		fwrite(&npc->health, sizeof(npc->health), 1, fp);
		fwrite(&npc->c_npc, sizeof(npc->c_npc), 1, fp);
		fwrite(&npc->ndir, sizeof(npc->ndir), 1, fp);
		fwrite(&npc->nstate, sizeof(npc->nstate), 1, fp);
		fwrite(&npc->nticker, sizeof(npc->nticker), 1, fp);
		fwrite(&npc->pos.y, sizeof(npc->pos.y), 1, fp);
		fwrite(&npc->pos.x, sizeof(npc->pos.x), 1, fp);
	}
	fclose(fp);
}



bool Game::G_LoadGame(const char* svfile)
{
	fp = fopen(svfile, "rb");
	if (!fp) N_Error("could not load save file!");
	for (auto* i : m_Active) {
		Z_Free(i);
		i = nullptr;
	}
	for (auto* i : b_Active) {
		Z_Free(i);
		i = nullptr;
	}
	_ngd_header header;
	fread(&header, sizeof(_ngd_header), 1, fp);

	fread((char *)&playr->name, sizeof(char), 256, fp);
	fread(&playr->health, sizeof(playr->health), 1, fp);
	fread(&playr->armor, sizeof(playr->armor), 1, fp);
	fread(&playr->coin, sizeof(playr->coin), 1, fp);
	fread(&playr->lvl, sizeof(playr->lvl), 1, fp);
	fread(&playr->pdir, sizeof(playr->pdir), 1, fp);
	fread(&playr->inv, sizeof(Item), MAX_PLAYR_ITEMS, fp);
	fread(&playr->P_wpns, sizeof(Weapon), MAX_PLAYR_WPNS, fp);
	fread(&playr->wpn_slot_current, sizeof(playr->wpn_slot_current), 1, fp);
	fread(&playr->pos.y, sizeof(playr->pos.y), 1, fp);
	fread(&playr->pos.x, sizeof(playr->pos.x), 1, fp);

	fread(&world->day, sizeof(world->day), 1, fp);
	fread(&world->time, sizeof(world->time), 1, fp);
	fread(&world->temperature, sizeof(world->temperature), 1, fp);
	for (nomaduint_t i = 0; i < header.nummobs; ++i) {
		Mob* mob = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &mob);
		fread(&mob->health, sizeof(mob->health), 1, fp);
		fread(&mob->armor, sizeof(mob->armor), 1, fp);
		fread(&mob->mpos.y, sizeof(mob->mpos.y), 1, fp);
		fread(&mob->mpos.x, sizeof(mob->mpos.x), 1, fp);
		fread(&mob->mstate, sizeof(mob->mstate), 1, fp);
		fread(&mob->mticker, sizeof(mob->mticker), 1, fp);
		fread(&mob->mdir, sizeof(mob->mdir), 1, fp);
		fread(&mob->sector_id, sizeof(mob->sector_id), 1, fp);
		fread(&mob->stepcounter, sizeof(mob->stepcounter), 1, fp);
	}
	for (nomaduint_t i = 0; i < b_Active.size(); ++i) {
		NPC* npc = (NPC *)Z_Malloc(sizeof(NPC), TAG_STATIC, &npc);
		fread(&npc->armor, sizeof(npc->armor), 1, fp);
		fread(&npc->health, sizeof(npc->health), 1, fp);
		fread(&npc->ndir, sizeof(npc->ndir), 1, fp);
		fread(&npc->nstate, sizeof(npc->nstate), 1, fp);
		fread(&npc->nticker, sizeof(npc->nticker), 1, fp);
		fread(&npc->pos.y, sizeof(npc->pos.y), 1, fp);
		fread(&npc->pos.x, sizeof(npc->pos.x), 1, fp);
	}

	fclose(fp);
	Z_CheckHeap();
	return true;
}

struct ngd_file_t
{
	_ngd_header header;
	uint32_t filenum;
	uint64_t size;
};

void G_DisplaySlots(Game* const game)
{
	std::vector<ngd_file_t> svfiles;
	ITEM** files = (ITEM **)Z_Malloc(sizeof(Item), TAG_STATIC, &files);
	MENU* svmenu;
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

*/