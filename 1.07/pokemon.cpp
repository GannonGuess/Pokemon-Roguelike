#include "pokemon.h"


void parse_pokemon(std::filesystem::path pokemon_data) {
    std::ifstream file(pokemon_data);
    if(!file.is_open()) {
        std::cerr << "Failed to open pokemon.csv" << std::endl;
        return;
    }

    std::string header;
    std::getline(file, header);

    std::string line;
    std::vector<Pokemon> pokemon_list;

    while(std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string cell;

        Pokemon pokemon;

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pokemon.id = INT_MAX;
            } else {
                pokemon.id = std::stoi(cell);
            }
        } else {
            pokemon.id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            pokemon.name = cell;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pokemon.species_id = INT_MAX;
            } else {
                pokemon.species_id = std::stoi(cell);
            }
        } else {
            pokemon.species_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pokemon.height = INT_MAX;
            } else {
                pokemon.height = std::stoi(cell);
            }
        } else {
            pokemon.height = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pokemon.weight = INT_MAX;
            } else {
                pokemon.weight = std::stoi(cell);
            }
        } else {
            pokemon.weight = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pokemon.base_exp = INT_MAX;
            } else {
                pokemon.base_exp = std::stoi(cell);
            }
        } else {
            pokemon.base_exp = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pokemon.order = INT_MAX;
            } else {
                pokemon.order = std::stoi(cell);
            }
        } else {
            pokemon.order = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pokemon.is_default = INT_MAX;
            } else {
                pokemon.is_default = std::stoi(cell);
            }
        } else {
            pokemon.is_default = INT_MAX;
        }
        
        pokemon_list.push_back(pokemon);
    }

    std::cout << header << std::endl;
    for(const Pokemon& p : pokemon_list) {
        std::string info = (p.id == INT_MAX ? "" : std::to_string(p.id)) + ","; 
        info += p.name + ",";
        info += (p.species_id == INT_MAX ? "" : std::to_string(p.species_id)) + ",";
        info += (p.height == INT_MAX ? "" : std::to_string(p.height)) + ",";
        info += (p.weight == INT_MAX ? "" : std::to_string(p.weight)) + ",";
        info += (p.base_exp == INT_MAX ? "" : std::to_string(p.base_exp)) + ",";
        info += (p.order == INT_MAX ? "" : std::to_string(p.order)) + ",";
        info += (p.is_default == INT_MAX ? "" : std::to_string(p.is_default));
        std::cout << info << std::endl;
    }
    return;
}