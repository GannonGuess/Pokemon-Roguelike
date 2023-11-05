#include "pokemon_moves.h"

void parse_pokemon_moves(std::filesystem::path pokemon_moves_data) {
    std::ifstream file(pokemon_moves_data);
    if(!file.is_open()) {
        std::cerr << "Failed to open poke_move.csv" << std::endl;
        return;
    }

    std::string header;
    std::getline(file, header);

    std::string line;
    std::vector<PokemonMove> pokemon_move_list;

    while(std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string cell;

        PokemonMove poke_move;

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                poke_move.pokemon_id = INT_MAX;
            } else {
                poke_move.pokemon_id = std::stoi(cell);
            }
        } else {
            poke_move.pokemon_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                poke_move.version_group_id = INT_MAX;
            } else {
                poke_move.version_group_id = std::stoi(cell);
            }
        } else {
            poke_move.version_group_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                poke_move.move_id = INT_MAX;
            } else {
                poke_move.move_id = std::stoi(cell);
            }
        } else {
            poke_move.move_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                poke_move.pokemon_move_method_id = INT_MAX;
            } else {
                poke_move.pokemon_move_method_id = std::stoi(cell);
            }
        } else {
            poke_move.pokemon_move_method_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                poke_move.level = INT_MAX;
            } else {
                poke_move.level = std::stoi(cell);
            }
        } else {
            poke_move.level = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                poke_move.order = INT_MAX;
            } else {
                poke_move.order = std::stoi(cell);
            }
        } else {
            poke_move.order = INT_MAX;
        }

        
        pokemon_move_list.push_back(poke_move);
    }

    std::cout << header << std::endl;
    for(const PokemonMove& pm : pokemon_move_list) {
        std::string info = (pm.pokemon_id == INT_MAX ? "" : std::to_string(pm.pokemon_id)) + ","; 
        info += (pm.version_group_id == INT_MAX ? "" : std::to_string(pm.version_group_id)) + ",";
        info += (pm.move_id == INT_MAX ? "" : std::to_string(pm.move_id)) + ",";
        info += (pm.pokemon_move_method_id == INT_MAX ? "" : std::to_string(pm.pokemon_move_method_id)) + ",";
        info += (pm.level == INT_MAX ? "" : std::to_string(pm.level)) + ",";
        info += (pm.order == INT_MAX ? "" : std::to_string(pm.order));
        std::cout << info << std::endl;
    }
    return;
}