#include "stats.h"

std::vector<Stats> parse_stats(std::filesystem::path stats_data) {
    std::ifstream file(stats_data);
    if(!file.is_open()) {
        throw std::runtime_error("Failed to open stats.csv");
    }

    std::string header;
    std::getline(file, header);

    std::string line;
    std::vector<Stats> stats_list;

    while(std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string cell;

        Stats stat;

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                stat.id = INT_MAX;
            } else {
                stat.id = std::stoi(cell);
            }
        } else {
            stat.id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                stat.damage_class_id = INT_MAX;
            } else {
                stat.damage_class_id = std::stoi(cell);
            }
        } else {
            stat.damage_class_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            stat.name = cell;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                stat.is_battle_only = INT_MAX;
            } else {
                stat.is_battle_only = std::stoi(cell);
            }
        } else {
            stat.is_battle_only = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                stat.game_index = INT_MAX;
            } else {
                stat.game_index = std::stoi(cell);
            }
        } else {
            stat.game_index = INT_MAX;
        }
        
        stats_list.push_back(stat);
    }

    std::cout << header << std::endl;
    for(const Stats& s : stats_list) {
        std::string info = (s.id == INT_MAX ? "" : std::to_string(s.id)) + ","; 
        info += (s.damage_class_id == INT_MAX ? "" : std::to_string(s.damage_class_id)) + ",";
        info += s.name + ",";
        info += (s.is_battle_only == INT_MAX ? "" : std::to_string(s.is_battle_only)) + ",";
        info += (s.game_index == INT_MAX ? "" : std::to_string(s.game_index));
        std::cout << info << std::endl;
    }
    return stats_list;
}