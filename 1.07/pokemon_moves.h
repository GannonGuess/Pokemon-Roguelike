#ifndef POKEMON_MOVES_H
#define POKEMON_MOVES_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class PokemonMove {
    public:
        int pokemon_id;
        int version_group_id;
        int move_id;
        int pokemon_move_method_id;
        int level;
        int order;
};

void parse_pokemon_moves(std::filesystem::path pokemon_data);

#endif