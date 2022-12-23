#include "g_game.h"

biome_t Game::B_GetBiome(void)
{
	
}

void Game::I_InitBiomes(void)
{
	puts("I_InitBiomes(): Fetching Biome Data For The Maps...");
	std::ifstream bfile("Files/gamedata/MAP/biomes.txt", std::ios::in);
	std::string line;
	std::vector<std::string> buffer;
	while (std::getline(bfile, line)) {
		buffer.push_back(line);
	};
	for (nomaduint_t i = 0; i < buffer.size(); i++) {
		if (buffer[i] == "numbiomes:") {
			num_biomes = stoi(buffer[i+1]);
			biomes.reserve(num_biomes);
		}
		else if (buffer[i] == "<biome> 1") {
			biomes.emplace_back();
			biome_t& biome = biomes.back();
			for (;; i++) {
				if (buffer[i] == "<biome> 0") {
					break;
				}
				else if (buffer[i] == "name:") {
					strncpy(biome.name, buffer[i+1].c_str(), strlen(buffer[i+1].c_str()));
				}
				else if (buffer[i] == "sector:") {
					biome.sector = stoi(buffer[i+1]);
				}
			/*	else if (buffer[i] == "coords: 1") {
					for (;; i++) {
						if (buffer[i] == "coords: 0") {
							break;
						}
						else {
							if (buffer[i] == "north-west:") {
								biome.bcoords[0].y = stoi(buffer[i+1]);
								biome.bcoords[0].x = stoi(buffer[i+2]);
								printf("%i, %i\n", biome.bcoords[0].y, biome.bcoords[0].x);
							}
							else if (buffer[i] == "north-east") {
								biome.bcoords[1].y = stoi(buffer[i+1]);
								biome.bcoords[1].x = stoi(buffer[i+2]);
							}
							else if (buffer[i] == "south-west") {
								biome.bcoords[2].y = stoi(buffer[i+1]);
								biome.bcoords[2].x = stoi(buffer[i+2]);
							}
							else if (buffer[i] == "south-east") {
								biome.bcoords[3].y = stoi(buffer[i+1]);
								biome.bcoords[3].x = stoi(buffer[i+2]);
							}
						}
					} 
				} */
				else if (buffer[i] == "temperature:") {
					biome.temperature = stoi(buffer[i+1]);
				}
				else if (buffer[i] == "mob spawns: [") {
					for (;; i++) {
						if (buffer[i] == "mob spawns: ]") {
							break;
						}
						else {
							biome.mobspawns.push_back(buffer[i].c_str());
						}
					}
				}
				else if (buffer[i] == "item spawns: [") {
					for (;; i++) {
						if (buffer[i] == "item spawns: ]") {
							break;
						}
						else {
							biome.itemspawns.push_back(buffer[i].c_str());
						}
					}
				}
				else if (buffer[i] == "npc spawns: [") {
					for (;; i++) {
						if (buffer[i] == "npc spawns: ]") {
							break;
						}
						else {
							biome.npcspawns.push_back(buffer[i].c_str());
						}
					}
				}
			}
		}
	}
	
	bfile.close();
}