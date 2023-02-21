#ifndef _G_SLF_
#define _G_SLF_

#pragma once

typedef struct sace_level_file
{
    std::string lvl_name;
    nomadenum_t mindif, maxdif;
    std::vector<mobj_t> mobspawners;
    dim_t map_dim;
    char** lvl_map;
} slf_t;

#endif