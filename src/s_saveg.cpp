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

#define BUFFER_SIZE 4*1024

static FILE* fp;
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

#define WRITE(chunk, buffer)                           \
{                                                      \
	memset(&chunk.buffer, '\0', BUFFER_SIZE);          \
	memcpy(&chunk.buffer, buffer, sizeof(buffer));     \
	for (num_t i = 0; i < BUFFER_SIZE; ++i) {          \
		chunk.buffer[i] = chunk.buffer[i] ^ MAGIC_XOR; \
	}                                                  \
	fwrite(&chunk, sizeof(ngd_chunk_t), 1, fp);        \
}
#define READ(chunk, buffer)                            \
{                                                      \
	fread(&chunk.buffer, sizeof(char), BUFFER_SIZE);   \
	for (num_t i = 0; i < BUFFER_SIZE; ++i) {          \
		chunk.buffer[i] = chunk.buffer[i] ^ MAGIC_XOR; \
	}                                                  \
	memcpy(buffer, &chunk.buffer, BUFFER_SIZE);        \
}

void Game::G_SaveGame(void)
{
	char svname[256];
	snprintf(svname, sizeof(svname), "Files/gamedata/SVFILES/nomadsv.ngd");
	fp = fopen(svname, "wb");
	
	ngd_file_t svfile;
	ngd_header_t& header = svfile.header;
	header.nummobs = ARRAY_SIZE(m_Active);
	header.numnpcs = ARRAY_SIZE(b_Active);
	header.numchunks = header.nummobs + header.numnpcs + 2;
	memset(&header.svname, '\0', sizeof(header.svname));
	strncpy(header.svname, "nomadascii_svfile", sizeof(header.svname) - 1);
	
	// only time we'll really ever make an allocation outside the main zone
	ngd_chunk_t* chunks = (ngd_chunk_t *)malloc(sizeof(ngd_chunk_t) * header.numchunks);
	nomadushort_t chunks_written = 0;
	// anything beyond the second chunk'll be the mob/npc/wpn/item chunks
	ngd_chunk_t& pchunk = chunks[chunks_written];
	chunks_written++;
	ngd_chunk_t& wchunk = chunks[chunks_written];
	chunks_written++;
	
	const Playr& p = *playr;
	const World& w = *world;
	
	fwrite(&header, sizeof(ngd_header_t), 1, fp);
	WRITE(pchunk, &p);
	WRITE(wchunk, &w);
	
	// mob data
	for (nomaduint_t i = 0; i < ARRAY_SIZE(m_Active); ++i) {
		ngd_chunk_t& mchunk = chunks[chunks_written];
		mchunk.chunktype = NGD_CHUNK_MOB;
		const Mob& mob = *m_Active[i];
		WRITE(mchunk, &mob);
		chunks_written++;
	}
	
	// npc data
	for (nomaduint_t i = 0; i < ARRAY_SIZE(b_Active); ++i) {
		ngd_chunk_t& nchunk = chunks[chunks_written];
		nchunk.chunktype = NGD_CHUNK_NPC;
		const NPC& npc = *b_Active[i];
		WRITE(nchunk, &npc);
		chunks_written++;
	}
	free(chunks);
	fclose(fp);
}



bool Game::G_LoadGame(const char* svfile)
{
	fp = fopen(svfile, "rb");
	if (!fp) N_Error("could not load save file!");
	ngd_file_t svfile;
	fread(&svfile.header, sizeof(ngd_header_t), 1, fp);
	if (!(svfile.header & HEADER))
		N_Error(".ngd save file header is the wrong number, corrupt save file?");
	if (svfile.header.nummobs > MAX_MOBS_ACTIVE)
		N_Error("save file nummobs is greater than maximum allowed mobs, from a different version?");
	if (svfile.header.numnpcs > MAX_NPC_ACTIVE)
		N_Error("save file numnpcs is greater than maxmium allowed npcs, from a different version?");
	
	// free not-needed/unnecessary non-player entity memory, and allocate if needed
	if (header.nummobs < MAX_MOBS_ACTIVE) {
		nomaduint_t moffset = ARRAY_SIZE(m_Active) - header.nummobs;
		while (moffset != MAX_MOBS_ACTIVE) {
			if (m_Active[moffset])
				Z_Free(m_Active[moffset]);
			m_Active[moffset] = nullptr;
			++moffset;
		}
	}
	else if (header.nummobs > G_GetNumMobs(this)) {
		nomaduint_t c_mob = GetNumMob(this);
		nomaduint_t count, i;
		count = 0;
		i = 0;
		while (count < header.nummobs) {
			if (!m_Active[i]) {
				m_Active[i] = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active[i]);
				++count;
			}
			++i;
		}
	}
	if (header.numnpcs < MAX_NPC_ACTIVE) {
		nomaduint_t noffset = ARRAY_SIZE(b_Active) - header.numnpcs;
		while (noffset != MAX_NPC_ACTIVE) {
			if (b_Active[noffset])
				Z_Free(b_Active[noffset]);
			b_Active[noffset] = nullptr;
			++noffset;
		}
	}
	else if (header.numnpcs > G_GetNumBots(this)) {
		nomaduint_t c_mob = G_GetNumBots(this);
		nomaduint_t count, i;
		count = 0;
		i = 0;
		while (count < header.numnpcs) {
			if (!b_Active[i]) {
				b_Active[i] = (NPC *)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active[i]);
				++count;
			}
			++i;
		}
	}
	ngd_chunk_t* chunks = (ngd_chunk_t *)malloc(sizeof(ngd_chunk_t) * svfile.header.numchunks);
	nomaduint_t mcount, ncount;
	mcount = 0;
	ncount = 0;
	for (nomaduint_t i = 0; i < svfile.header.numchunks; ++i) {
		fread(&chunks[i], sizeof(ngd_chunk_t), 1, fp);
		switch (chunks[i].chunktype) {
		case NGD_CHUNK_PLAYR:
			*playr = *(Playr *)chunks[i].buffer;
			break;
		case NGD_CHUNK_MOB:
			*m_Active[mcount] = *(Mob *)chunks[i].buffer;
			++mcount;
			break;
		case NGD_CHUNK_NPC:
			*b_Active[ncount] = *(NPC *)chunks[i].buffer;
			++ncount;
			break;
		case NGD_CHUNK_WORLD:
			*world = *(World *)chunks[i].buffer;
			break;
		};
	}
	free(chunks);
	fclose(fp);
	Z_CheckHeap();
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

*/
