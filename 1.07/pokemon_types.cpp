#include "pokemon_types.h"

std::vector<PokemonType> parse_pokemon_types(std::filesystem::path pk_types_data) {
    std::ifstream file(pk_types_data);
    if(!file.is_open()) {
        throw std::runtime_error("Failed to open pokemon_types.csv");
    }

    std::string header;
    std::getline(file, header);

    std::string line;
    std::vector<PokemonType> pk_types_list;

    while(std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string cell;

        PokemonType pk_type;

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_type.pokemon_id = INT_MAX;
            } else {
                pk_type.pokemon_id = std::stoi(cell);
            }
        } else {
            pk_type.pokemon_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_type.type_id = INT_MAX;
            } else {
                pk_type.type_id = std::stoi(cell);
            }
        } else {
            pk_type.type_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_type.slot = INT_MAX;
            } else {
                pk_type.slot = std::stoi(cell);
            }
        } else {
            pk_type.slot = INT_MAX;
        }
        
        pk_types_list.push_back(pk_type);
    }

    std::cout << header << std::endl;
    for(const PokemonType& pt : pk_types_list) {
        std::string info = (pt.pokemon_id == INT_MAX ? "" : std::to_string(pt.pokemon_id)) + ","; 
        info += (pt.type_id == INT_MAX ? "" : std::to_string(pt.type_id)) + ",";
        info += (pt.slot == INT_MAX ? "" : std::to_string(pt.slot));
        std::cout << info << std::endl;
    }
    return pk_types_list;
}