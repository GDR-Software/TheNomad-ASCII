#include "g_game.h"
#include "g_mob.h"
#include "g_playr.h"

static FILE *fp;
static constexpr auto HEADER = 0x5f3759df;
static constexpr auto svfile = "nomadsv.ngd";

static void wrString(const std::string& val);
//static void wrByte(uint64_t val);
//static void wrByte(int64_t val);
static void wrByte(uint32_t val);
static void wrByte(int32_t val);
static void wrByte(uint16_t val);
static void wrByte(gamestate_t val)
{
	byte v = (byte)val ^ (~500);
	fwrite(&v, 1, sizeof(byte), fp);
}
static void wrByte(int16_t val);
static void wrByte(uint8_t val);
//static void wrByte(int8_t val);
static void wrByte(bool val);
static void wrMob(const Mob *mob);
static void wrNPC(const NPC *npc);

static std::string rdString();
static void rdByte(int32_t& in);
static void rdByte(gamestate_t& in)
{
	byte v;
	fread(&v, 1, sizeof(byte), fp);
	v = v ^ (~500);
	in = (gamestate_t)v;
}
static void rdByte(uint32_t& in);
static void rdByte(int16_t& in);
static void rdByte(uint16_t& in);
//static void rdByte(int8_t& in);
static void rdByte(uint8_t& in);
static void rdByte(bool& in);
static void rdMob(Mob * const mob);
static void rdNPC(NPC * const npc);

void Game::G_SaveGame(void)
{
	Z_CheckHeap();
	// std::fstreams gave me shit when I tried this stuff,
	// if you can do this better, then pls do. I accept pulls
	fp = fopen(svfile, "wb");
	if (!fp) {
		return;
	}
	uint8_t vmain = _NOMAD_VERSION;
	uint8_t vupdate = _NOMAD_VERSION_UPDATE;
	uint16_t vpatch = _NOMAD_VERSION_PATCH; // these vars will always stay the same size
	wrByte(vmain);
	wrByte(vupdate);
	wrByte(vpatch);
	wrByte(gamestate);
	wrByte(gamescreen);
	// options
	wrByte(scf::launch::nomobs);
	wrByte(scf::launch::nosmell);
	wrByte(scf::launch::deafmobs);
	wrByte(scf::launch::blindmobs);
	wrByte(scf::launch::devmode);
	wrByte(scf::launch::fastmobs1);
	wrByte(scf::launch::fastmobs2);
	wrByte(scf::launch::fastmobs3);
	wrByte(scf::launch::godmode);
	wrByte(scf::launch::bottomless_clip);
	wrByte(scf::launch::ext_bff);
	wrByte(scf::launch::infinite_ammo);
	wrByte(scf::music_on);
	wrByte(scf::sfx_on);
	wrByte(scf::music_vol);
	wrByte(scf::sfx_vol);
	wrByte(scf::mobspeed);
	// player data
	wrString(playr->name);
	wrByte(playr->health);
	wrByte(playr->armor);
	wrByte(playr->pos.y);
	wrByte(playr->pos.x);
	wrByte(playr->pdir);
	/*for (i = 0; i < MAX_PLAYR_WPNS; i++) {
	        fwrite(&playr->P_wpns[i], 1, sizeof(nomaduint_t), fp);
	} */
	//	fwrite(&playr->sprite, 1, sizeof(sprite_t), fp);
	/*fwrite(&playr->coin, 1, sizeof(nomadint_t), fp);
	fwrite(&playr->lvl, 1, sizeof(nomadushort_t), fp);
	fwrite(&playr->sector_id, 1, sizeof(nomadenum_t), fp);
	uint32_t inv_size = (uint32_t)playr->inv.size();
	fwrite(&inv_size, 1, sizeof(uint32_t), fp);
	for (i = 0; i < playr->inv.size(); i++) {
	        fwrite(&playr->inv[i], 1, sizeof(nomaduint_t), fp);
	} */
	uint32_t nummobs = (uint32_t)m_Active.size();
	wrByte(nummobs);
	for (uint32_t i = 0; i < m_Active.size(); i++) {
		wrMob(m_Active[i]);
	}
	/*uint32_t numnpcs = (uint32_t)b_Active.size();
	wrByte(numnpcs);
	for (uint32_t i = 0; i < b_Active.size(); i++) {
		wrNPC(b_Active[i]);
	} */
	fclose(fp);
}

bool Game::G_LoadGame(void)
{
	uint32_t i;
	struct stat fdata;
	if (stat(svfile, &fdata) == -1) {
		return false;
	}
	fp = fopen(svfile, "rb");
	if (!fp) {
		return false;
	}
	for (i = 0; i < m_Active.size(); i++) {
		Z_Free(m_Active[i]);
	}
	m_Active.clear();
	uint8_t vmain, vupdate;
	uint16_t vpatch;
	rdByte(vmain);
	rdByte(vupdate);
	rdByte(vpatch);
	rdByte(gamestate);
	rdByte(gamescreen);
	// options
	rdByte(scf::launch::nomobs);
	rdByte(scf::launch::nosmell);
	rdByte(scf::launch::deafmobs);
	rdByte(scf::launch::blindmobs);
	rdByte(scf::launch::devmode);
	rdByte(scf::launch::fastmobs1);
	rdByte(scf::launch::fastmobs2);
	rdByte(scf::launch::fastmobs3);
	rdByte(scf::launch::godmode);
	rdByte(scf::launch::bottomless_clip);
	rdByte(scf::launch::ext_bff);
	rdByte(scf::launch::infinite_ammo);
	rdByte(scf::music_on);
	rdByte(scf::sfx_on);
	rdByte(scf::music_vol);
	rdByte(scf::sfx_vol);
	rdByte(scf::mobspeed);
	// player data
	playr->name = rdString();
	rdByte(playr->health);
	rdByte(playr->armor);
	rdByte(playr->pos.y);
	rdByte(playr->pos.x);
	rdByte(playr->pdir);
	/*for (i = 0; i < MAX_PLAYR_WPNS; i++) {
	        fwrite(&playr->P_wpns[i], 1, sizeof(nomaduint_t), fp);
	} */
	//	fwrite(&playr->sprite, 1, sizeof(sprite_t), fp);
	/*fwrite(&playr->coin, 1, sizeof(nomadint_t), fp);
	fwrite(&playr->lvl, 1, sizeof(nomadushort_t), fp);
	fwrite(&playr->sector_id, 1, sizeof(nomadenum_t), fp);
	uint32_t inv_size = (uint32_t)playr->inv.size();
	fwrite(&inv_size, 1, sizeof(uint32_t), fp);
	for (i = 0; i < playr->inv.size(); i++) {
	        fwrite(&playr->inv[i], 1, sizeof(nomaduint_t), fp);
	} */
	uint32_t nummobs;
	rdByte(nummobs);
	m_Active.reserve(nummobs);
	for (i = 0; i < nummobs; i++) {
		m_Active.emplace_back();
		m_Active.back() = (Mob*)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active.back());
		rdMob(m_Active.back());
	} /*
	uint32_t numnpcs;
	rdByte(numnpcs);
	b_Active.reserve(numnpcs);
	for (i = 0; i < numnpcs; i++) {
		b_Active.emplace_back();
		b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
		rdNPC(b_Active.back());
	}*/
	fclose(fp);
	Z_CheckHeap();
	return true;
}

static void wrString(const std::string &val)
{
	const char *name = val.c_str();
	int len = strlen(name);
	fwrite(&len, 1, sizeof(int), fp);
	for (size_t i = 0; i < strlen(name); i++) {
		char xor_byte = name[i] ^ (~500);
		fwrite(&xor_byte, 1, sizeof(char), fp);
	}
}

static void wrByte(uint32_t val)
{
	val = val ^ (~500);
	fwrite(&val, 1, sizeof(uint32_t), fp);
}

static void wrByte(int32_t val)
{
	val = val ^ (~500);
	fwrite(&val, 1, sizeof(int32_t), fp);
}

static void wrByte(uint16_t val)
{
	val = val ^ (~500);
	fwrite(&val, 1, sizeof(uint16_t), fp);
}

static void wrByte(int16_t val)
{
	val = val ^ (~500);
	fwrite(&val, 1, sizeof(int16_t), fp);
}

static void wrByte(uint8_t val)
{
	val = val ^ (~500);
	fwrite(&val, 1, sizeof(uint8_t), fp);
}

static void wrByte(int8_t val)
{
	val = val ^ (~500);
	fwrite(&val, 1, sizeof(int8_t), fp);
}

static void wrByte(bool val)
{
	val = val ^ (~500);
	fwrite(&val, 1, sizeof(bool), fp);
}

static void wrNPC(const NPC* npc)
{
	auto i = npc->health ^ (~500);
	fwrite(&i, 1, sizeof(npc->health), fp);
	i = npc->armor ^ (~500);
	fwrite(&i, 1, sizeof(npc->armor), fp);
	i = npc->pos.y ^ (~500);
	fwrite(&i, 1, sizeof(npc->pos.y), fp);
	i = npc->pos.x ^ (~500);
	fwrite(&i, 1, sizeof(npc->pos.x), fp);
	write(fileno(fp), &npc->c_npc, sizeof(npc->c_npc));
}

static void wrMob(const Mob* mob)
{
	auto i = mob->health ^ (~500);
	fwrite(&i, 1, sizeof(mob->health), fp);
	i = mob->armor ^ (~500);
	fwrite(&i, 1, sizeof(mob->armor), fp);
	i = mob->c_mob.mtype ^ (~500);
	fwrite(&i, 1, sizeof(mob->c_mob.mtype), fp);
	i = mob->mpos.y ^ (~500);
	fwrite(&i, 1, sizeof(mob->mpos.y), fp);
	i = mob->mpos.x ^ (~500);
	fwrite(&i, 1, sizeof(mob->mpos.x), fp);
	i = mob->mticker ^ (~500);
	fwrite(&i, 1, sizeof(mob->mticker), fp);
	i = mob->mstate.id ^ (~500);
	fwrite(&i, 1, sizeof(mob->mstate.id), fp);
	i = mob->mdir ^ (~500);
	fwrite(&i, 1, sizeof(mob->mdir), fp);
}

static void rdNPC(NPC * const npc)
{
	fread(&npc->health, 1, sizeof(npc->health), fp);
	fread(&npc->armor, 1, sizeof(npc->armor), fp);
	fread(&npc->pos.y, 1, sizeof(npc->pos.y), fp);
	fread(&npc->pos.x, 1, sizeof(npc->pos.x), fp);
	read(fileno(fp), &npc->c_npc, sizeof(npc->c_npc));
	npc->health = npc->health ^ (~500);
	npc->armor = npc->armor ^ (~500);
	npc->pos.y = npc->pos.y ^ (~500);
	npc->pos.x = npc->pos.x ^ (~500);
}

static void rdMob(Mob * const mob)
{
	fread(&mob->health, 1, sizeof(mob->health), fp);
	fread(&mob->armor, 1, sizeof(mob->armor), fp);
	fread(&mob->c_mob.mtype, 1, sizeof(mob->c_mob.mtype), fp);
	fread(&mob->mpos.y, 1, sizeof(mob->mpos.y), fp);
	fread(&mob->mpos.x, 1, sizeof(mob->mpos.x), fp);
	fread(&mob->mticker, 1, sizeof(mob->mticker), fp);
	fread(&mob->mstate.id, 1, sizeof(mob->mstate.id), fp);
	fread(&mob->mdir, 1, sizeof(mob->mdir), fp);
	mob->health = mob->health ^ (~500);
	mob->armor = mob->armor ^ (~500);
	mob->c_mob.mtype = mob->c_mob.mtype ^ (~500);
	mob->mpos.y = mob->mpos.y ^ (~500);
	mob->mpos.x = mob->mpos.x ^ (~500);
	mob->mticker = mob->mticker ^ (~500);
	int mstate = (int)mob->mstate.id ^ (~500);
	mob->mstate.id = (state_t)mstate;
	mob->mdir = mob->mdir ^ (~500);
}

static std::string rdString()
{
	int len = fread(&len, 1, sizeof(int), fp);
	char name[len];
	fread(&name, len, sizeof(char), fp);
	return (name);
}

static void rdByte(uint32_t& in)
{
	fread(&in, 1, sizeof(uint32_t), fp);
	in = in ^ (~500);
}
static void rdByte(int32_t& in)
{
	fread(&in, 1, sizeof(int32_t), fp);
	in = in ^ (~500);
}
static void rdByte(uint16_t& in)
{
	fread(&in, 1, sizeof(uint16_t), fp);
	in = in ^ (~500);
}
static void rdByte(int16_t& in)
{
	fread(&in, 1, sizeof(int16_t), fp);
	in = in ^ (~500);
}
static void rdByte(uint8_t& in)
{
	fread(&in, 1, sizeof(uint8_t), fp);
	in = in ^ (~500);
}
static void rdByte(int8_t& in)
{
	fread(&in, 1, sizeof(int8_t), fp);
	in = in ^ (~500);
}
static void rdByte(bool& in)
{
	fread(&in, 1, sizeof(bool), fp);
	in = in ^ (~500);
}