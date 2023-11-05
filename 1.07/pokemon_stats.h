#ifndef POKEMON_STATS_H
#define POKEMON_STATS_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class PokemonStats {
    public:
        int pokemon_id;
        int stat_id;
        int base_stat;
        int effort;
};

std::vector<PokemonStats> parse_pokemon_stats(std::filesystem::path pokemon_data);

#endif