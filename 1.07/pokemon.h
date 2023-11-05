#ifndef POKEMON_H
#define POKEMON_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class Pokemon {
    public:
        int id;
        std::string name;
        int species_id;
        int height;
        int weight;
        int base_exp;
        int order;
        int is_default; 
};

std::vector<Pokemon> parse_pokemon(std::filesystem::path pokemon_data);

#endif