#ifndef POKEMON_TYPES_H
#define POKEMON_TYPES_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class PokemonType {
    public:
        int pokemon_id;
        int type_id;
        int slot;
};

void parse_pokemon_types(std::filesystem::path pokemon_data);

#endif