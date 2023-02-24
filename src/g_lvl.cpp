#include "g_game.h"
//#undef byte
//#undef assert(x)
//#include "nlohmann/json.hpp"
#include "g_bff.h"

//using json = nlohmann::json;

#define BLF_ALLOC_SIZE (sizeof(bff_lvl_t*)*bfflvls.size())

void G_AllocLevelList(const std::vector<bff_lvl_t*>& bfflvls)
{
	if (bfflvls.size() < 1)
		return;
	
	bff_lvl_t **lvl_list = (bff_lvl_t **)R_Malloc(BLF_ALLOC_SIZE, TAG_STATIC, &lvl_list);
	for (nomadulong_t i = 0; i < bfflvls.size(); ++i) {
		lvl_list[i] = (bff_lvl_t *)R_Malloc(sizeof(bff_lvl_t), TAG_STATIC, &lvl_list[i]);
		bff_lvl_t* lvl = bfflvls[i];
		bff_lvl_t* blf = lvl_list[i];
		blf->map_height = lvl->map_height;
		blf->map_width = lvl->map_width;
		blf->lvl_map = (char **)R_Malloc(sizeof(char *) * (lvl->map_height+2), TAG_STATIC, &blf->lvl_map);
		for (nomadshort_t y = 0; y < lvl->map_height + 2; ++y) {
			blf->lvl_map[y] = (char *)R_Malloc(sizeof(char) * lvl->map_width, TAG_STATIC, &blf->lvl_map[y]);
			memcpy(&blf->lvl_map[y][0], &lvl->lvl_map[y][0], lvl->map_width);
		}
		blf->lvl_name = lvl->lvl_name;
		blf->map_height = lvl->map_height;
		blf->map_width = lvl->map_width;
	}
}

bff_lvl_t* G_LoadLevel(const std::string& name, bff_lvl_t* old)
{
    bff_lvl_t* blf = (bff_lvl_t *)Z_Malloc(sizeof(bff_lvl_t), TAG_STATIC, &blf);
	blf->lvl_map = (char **)Z_Malloc(sizeof(char *) * (old->map_height+2), TAG_STATIC, &blf->lvl_map);
	for (nomadshort_t y = 0; y < old->map_height+2; ++y) {
		blf->lvl_map[y] = (char *)Z_Malloc(sizeof(char) * old->map_width+2, TAG_STATIC, &blf->lvl_map[y]);
		memcpy(&blf->lvl_map[y][0], &old->lvl_map[y][0], old->map_width+2);
		R_Free(old->lvl_map[y]);
	}
	R_Free(old->lvl_map);
	blf->map_height = old->map_height;
	blf->map_width = old->map_width;
	blf->lvl_name = old->lvl_name;
	R_Free(old);
	return blf;
}

void G_FreeLevel(bff_lvl_t* lvl)
{
	// put it back into the reserved heap
	bff_lvl_t* reserve = (bff_lvl_t *)R_Malloc(sizeof(bff_lvl_t), TAG_STATIC, &reserve);
	//fixme - doesn't do the dynamic lvl_map allocations
   	memcpy(&(*reserve), &(*lvl), sizeof(bff_lvl_t));
	for (nomaduint_t i = 0; i < lvl->map_height; ++i) {
        Z_Free(lvl->lvl_map[i]);
    }
    Z_Free(lvl->lvl_map);
    Z_Free(lvl);
}
