#ifndef MOVES_H
#define MOVES_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class Move {
    public:
        int id;
        std::string name;
        int generation_id;
        int type_id;
        int power;
        int pp;
        int accuracy;
        int priority; 
        int target_id;
        int damage_class_id;
        int effect_id;
        int effect_chance;
        int contest_type_id;
        int contest_effect_id;
        int super_contest_effect_id;
};

std::vector<Move> parse_moves(std::filesystem::path moves_data);

#endif