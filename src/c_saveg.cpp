#include "g_game.h"

static FILE* fp;
static constexpr auto HEADER = 0x5f3759df;
static constexpr auto svfile = "nomadsv.ngd";

static void G_ArchivePlayr(const Playr* playr);
static void G_UnArchivePlayr(Playr* const playr);
static void G_ArchiveWorld(const Game* world);
static void G_UnArchiveWorld(Game* const world);
static void G_ArchiveMobs(const std::vector<Mob*>& m_Active);
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active);

void Game::G_SaveGame(void)
{
	Z_CheckHeap();
	// std::fstreams gave me shit when I tried this stuff,
	// if you can do this better, then pls do. I accept pulls
	fp = fopen(svfile, "wb");
	if (!fp) {
		return;
	}
	G_ArchivePlayr(playr);
	G_ArchiveWorld(this);
	G_ArchiveMobs(m_Active);
	fclose(fp);
}

bool Game::G_LoadGame(void)
{
	struct stat fdata;
	if (stat(svfile, &fdata) == -1) {
		return false;
	}
	fp = fopen(svfile, "rb");
	if (!fp) {
		return false;
	}
	G_UnArchivePlayr(playr);
	G_UnArchiveWorld(this);
	G_UnArchiveMobs(m_Active);
	fclose(fp);
	Z_CheckHeap();
	return true;
}

static void G_ArchivePlayr(const Playr* playr)
{
	short size = (short)playr->name.size();
	fwrite(&size, 1, sizeof(short), fp);
	fwrite((char*)&playr->name, playr->name.size(), sizeof(char), fp);
	fwrite(&playr->health, 1, sizeof(playr->health), fp);
	fwrite(&playr->armor, 1, sizeof(playr->armor), fp);
	fwrite(&playr->pos.y, 1, sizeof(playr->pos.y), fp);
	fwrite(&playr->pos.x, 1, sizeof(playr->pos.x), fp);
	fwrite(&playr->coin, 1, sizeof(playr->coin), fp);
	fwrite(&playr->lvl, 1, sizeof(playr->lvl), fp);
}
static void G_UnArchivePlayr(Playr* const playr)
{
	short size;
	fread(&size, 1, sizeof(short), fp);
	fread((char*)&playr->name, size, sizeof(char), fp);
	fread(&playr->health, 1, sizeof(playr->health), fp);
	fread(&playr->armor, 1, sizeof(playr->armor), fp);
	fread(&playr->pos.y, 1, sizeof(playr->pos.y), fp);
	fread(&playr->pos.x, 1, sizeof(playr->pos.x), fp);
	fread(&playr->coin, 1, sizeof(playr->coin), fp);
	fread(&playr->lvl, 1, sizeof(playr->lvl), fp);
}

static void G_ArchiveWorld(const Game* world)
{
	fwrite(&world->c_map, sizeof(world->c_map), sizeof(char), fp);
}
static void G_UnArchiveWorld(Game* const world)
{
	fread(&world->c_map, sizeof(world->c_map), sizeof(char), fp);
}

static void G_ArchiveMobs(const std::vector<Mob*>& m_Active)
{
	short size = (short)m_Active.size();
	fwrite(&size, 1, sizeof(short), fp);
	for (nomaduint_t i = 0; i < m_Active.size(); i++) {
		Mob* mob = m_Active[i];
		fwrite(&mob->c_mob.mtype, 1, sizeof(mob->c_mob.mtype), fp);
		fwrite(&mob->health, 1, sizeof(mob->health), fp);
		fwrite(&mob->armor, 1, sizeof(mob->armor), fp);
		fwrite(&mob->mpos.y, 1, sizeof(mob->mpos.y), fp);
		fwrite(&mob->mpos.x, 1, sizeof(mob->mpos.x), fp);
		fwrite(&mob->mstate.id, 1, sizeof(mob->mstate.id), fp);
		fwrite(&mob->mticker, 1, sizeof(mob->mticker), fp);
	}
}
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active)
{
	nomaduint_t i;
	for (i = 0; i < m_Active.size(); i++) {
		Z_Free(m_Active[i]);
	}
	m_Active.clear();
	short size;
	fread(&size, 1, sizeof(short), fp);
	m_Active.reserve(size);
	for (i = 0; i < size; i++) {
		m_Active.emplace_back();
		m_Active.back() = (Mob*)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active.back());
		Mob* mob = m_Active.back();
		fread(&mob->c_mob.mtype, 1, sizeof(mob->c_mob.mtype), fp);
		fread(&mob->health, 1, sizeof(mob->health), fp);
		fread(&mob->armor, 1, sizeof(mob->armor), fp);
		fread(&mob->mpos.y, 1, sizeof(mob->mpos.y), fp);
		fread(&mob->mpos.x, 1, sizeof(mob->mpos.x), fp);
		fread(&mob->mstate.id, 1, sizeof(mob->mstate.id), fp);
		fread(&mob->mticker, 1, sizeof(mob->mticker), fp);
	}
}