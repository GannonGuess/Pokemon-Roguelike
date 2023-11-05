#include "experience.h"

void parse_experience(std::filesystem::path experience_data) {
    std::ifstream file(experience_data);
    if(!file.is_open()) {
        std::cerr << "Failed to open exp.csv" << std::endl;
        return;
    }

    std::string header;
    std::getline(file, header);

    std::string line;
    std::vector<Experience> experience_list;

    while(std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string cell;

        Experience exp;

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                exp.growth_rate_id = INT_MAX;
            } else {
                exp.growth_rate_id = std::stoi(cell);
            }
        } else {
            exp.growth_rate_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                exp.level = INT_MAX;
            } else {
                exp.level = std::stoi(cell);
            }
        } else {
            exp.level = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                exp.experience = INT_MAX;
            } else {
                exp.experience = std::stoi(cell);
            }
        } else {
            exp.experience = INT_MAX;
        }
        
        experience_list.push_back(exp);
    }

    std::cout << header << std::endl;
    for(const Experience& e : experience_list) {
        std::string info = (e.growth_rate_id == INT_MAX ? "" : std::to_string(e.growth_rate_id)) + ","; 
        info += (e.level == INT_MAX ? "" : std::to_string(e.level)) + ",";
        info += (e.experience == INT_MAX ? "" : std::to_string(e.experience));
        std::cout << info << std::endl;
    }
    return;
}