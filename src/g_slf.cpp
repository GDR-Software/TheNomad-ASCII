#include "g_game.h"
#undef byte
#undef assert(x)
#include "nlohmann/json.hpp"
#include "g_slf.h"

using json = nlohmann::json;

slf_t *G_LoadSLF(const char* filepath)
{
    std::ifstream file(filepath, std::ios::in);
    if (file.fail())
        N_Error("could not load .slf file %s!", filepath);
    
    file.close();
    json data = json::parse(file);
    std::string map_file_name = data["mapfile"];
    file.open(map_file_name, std::ios::in);
    if (file.fail())
        N_Error("could not load slf map file %s!", map_file_name.c_str());
    std::vector<std::string> strbuf;
    std::string line;
    while (std::getline(file, line)) { strbuf.push_back(line); };
    file.close();
    slf_t *slf = (slf_t *)Z_Malloc(sizeof(slf_t), TAG_STATIC, &slf);
    slf->lvl_map = (char **)Z_Malloc(sizeof(char *) * strbuf.size(), TAG_STATIC, &slf->lvl_map);
    slf->map_dim = dim_t(strbuf.size(), strbuf[0].size());
    for (nomadshort_t y = 0; y < strbuf.size(); ++y) {
        slf->lvl_map[y] = (char *)Z_Malloc(sizeof(char) * slf->map_dim.width, TAG_STATIC, &slf->lvl_map[y]);
        for (nomadshort_t x = 0; x < strbuf[y].size(); ++x) {
            slf->lvl_map[y][x] = strbuf[y][x];
        }
    }
    std::vector<std::string> mobspawners;
    for (auto& i : data["mobspawners"])
        mobspawners.emplace_back(std::string(i));
    return slf;
}

void G_FreeSLF(slf_t *slf)
{
    for (nomaduint_t i = 0; i < slf->map_dim.height; ++i) {
        Z_Free(slf->lvl_map[i]);
    }
    Z_Free(slf->lvl_map);
    Z_Free(slf);
}