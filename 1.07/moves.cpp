#include "moves.h"


std::vector<Move> parse_moves(std::filesystem::path moves_data) {
    std::ifstream file(moves_data);
    if(!file.is_open()) {
        throw std::runtime_error("Failed to open moves.csv");
    }

    std::string header;
    std::getline(file, header);

    std::string line;
    std::vector<Move> moves_list;

    while(std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string cell;

        Move move;

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.id = INT_MAX;
            } else {
                move.id = std::stoi(cell);
            }
        } else {
            move.id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            move.name = cell;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.generation_id = INT_MAX;
            } else {
                move.generation_id= std::stoi(cell);
            }
        } else {
            move.generation_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.type_id = INT_MAX;
            } else {
                move.type_id = std::stoi(cell);
            }
        } else {
            move.type_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.power = INT_MAX;
            } else {
                move.power = std::stoi(cell);
            }
        } else {
            move.power = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.pp = INT_MAX;
            } else {
                move.pp = std::stoi(cell);
            }
        } else {
            move.pp = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.accuracy = INT_MAX;
            } else {
                move.accuracy = std::stoi(cell);
            }
        } else {
            move.accuracy = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.priority = INT_MAX;
            } else {
                move.priority = std::stoi(cell);
            }
        } else {
            move.priority = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.target_id = INT_MAX;
            } else {
                move.target_id = std::stoi(cell);
            }
        } else {
            move.target_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.damage_class_id = INT_MAX;
            } else {
                move.damage_class_id = std::stoi(cell);
            }
        } else {
            move.damage_class_id = INT_MAX;
        }
        
        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.effect_id = INT_MAX;
            } else {
                move.effect_id = std::stoi(cell);
            }
        } else {
            move.effect_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.effect_chance = INT_MAX;
            } else {
                move.effect_chance = std::stoi(cell);
            }
        } else {
            move.effect_chance = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.contest_type_id = INT_MAX;
            } else {
                move.contest_type_id = std::stoi(cell);
            }
        } else {
            move.contest_type_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.contest_effect_id = INT_MAX;
            } else {
                move.contest_effect_id = std::stoi(cell);
            }
        } else {
            move.contest_effect_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                move.super_contest_effect_id = INT_MAX;
            } else {
                move.super_contest_effect_id = std::stoi(cell);
            }
        } else {
            move.super_contest_effect_id = INT_MAX;
        }
        
        moves_list.push_back(move);
    }

    std::cout << header << std::endl;
    for(const Move& m : moves_list) {
        std::string info = (m.id == INT_MAX ? "" : std::to_string(m.id)) + ","; 
        info += m.name + ",";
        info += (m.generation_id == INT_MAX ? "" : std::to_string(m.generation_id)) + ",";
        info += (m.type_id == INT_MAX ? "" : std::to_string(m.type_id)) + ",";
        info += (m.power == INT_MAX ? "" : std::to_string(m.power)) + ",";
        info += (m.pp == INT_MAX ? "" : std::to_string(m.pp)) + ",";
        info += (m.accuracy == INT_MAX ? "" : std::to_string(m.accuracy)) + ",";
        info += (m.priority == INT_MAX ? "" : std::to_string(m.priority)) + ",";
        info += (m.target_id == INT_MAX ? "" : std::to_string(m.target_id)) + ",";
        info += (m.damage_class_id == INT_MAX ? "" : std::to_string(m.damage_class_id)) + ",";
        info += (m.effect_id == INT_MAX ? "" : std::to_string(m.effect_id)) + ",";
        info += (m.effect_chance == INT_MAX ? "" : std::to_string(m.effect_chance)) + ",";
        info += (m.contest_type_id== INT_MAX ? "" : std::to_string(m.contest_type_id)) + ",";
        info += (m.contest_effect_id == INT_MAX ? "" : std::to_string(m.contest_effect_id)) + ",";
        info += (m.super_contest_effect_id == INT_MAX ? "" : std::to_string(m.super_contest_effect_id));
        std::cout << info << std::endl;
    }
    return moves_list;
}