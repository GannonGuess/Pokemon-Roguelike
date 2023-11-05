#include "pokemon_stats.h"

void parse_pokemon_stats(std::filesystem::path pk_stats_data) {
    std::ifstream file(pk_stats_data);
    if(!file.is_open()) {
        std::cerr << "Failed to open pk_stat.csv" << std::endl;
        return;
    }

    std::string header;
    std::getline(file, header);

    std::string line;
    std::vector<PokemonStats> pk_stats_list;

    while(std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string cell;

        PokemonStats pk_stat;

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_stat.pokemon_id = INT_MAX;
            } else {
                pk_stat.pokemon_id = std::stoi(cell);
            }
        } else {
            pk_stat.pokemon_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_stat.stat_id = INT_MAX;
            } else {
                pk_stat.stat_id = std::stoi(cell);
            }
        } else {
            pk_stat.stat_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_stat.base_stat = INT_MAX;
            } else {
                pk_stat.base_stat = std::stoi(cell);
            }
        } else {
            pk_stat.base_stat = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_stat.effort = INT_MAX;
            } else {
                pk_stat.effort = std::stoi(cell);
            }
        } else {
            pk_stat.effort = INT_MAX;
        }
        
        pk_stats_list.push_back(pk_stat);
    }

    std::cout << header << std::endl;
    for(const PokemonStats& ps : pk_stats_list) {
        std::string info = (ps.pokemon_id == INT_MAX ? "" : std::to_string(ps.pokemon_id)) + ","; 
        info += (ps.stat_id == INT_MAX ? "" : std::to_string(ps.stat_id)) + ",";
        info += (ps.base_stat == INT_MAX ? "" : std::to_string(ps.base_stat)) + ",";
        info += (ps.effort == INT_MAX ? "" : std::to_string(ps.effort));
        std::cout << info << std::endl;
    }
    return;
}