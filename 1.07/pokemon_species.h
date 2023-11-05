#ifndef POKEMON_SPECIES_H
#define POKEMON_SPECIES_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class PokemonSpecies {
    public:
        int id;
        std::string name;
        int generation_id;
        int evolves_from_species_id;
        int evolution_chain_id;
        int color_id;
        int shape_id;
        int habitat_id; 
        int gender_rate;
        int capture_rate;
        int base_happiness;
        int is_baby;
        int hatch_counter;
        int has_gender_differences;
        int growth_rate_id;
        int forms_switchable;
        int is_legendary;
        int is_mythical;
        int order;
        int conquest_order;
};

std::vector<PokemonSpecies> parse_pokemon_species(std::filesystem::path moves_data);

#endif