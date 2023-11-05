#ifndef STATS_H
#define STATS_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class Stats {
    public:
        int id;
        int damage_class_id;
        std::string name;
        int is_battle_only;
        int game_index;
};

std::vector<Stats> parse_stats(std::filesystem::path pokemon_data);

#endif