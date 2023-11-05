#include "type_names.h"
#include <cstdio>

std::vector<TypeName> parse_type_names(std::filesystem::path type_names_data) {
    std::ifstream file(type_names_data);
    if(!file.is_open()) {
        throw std::runtime_error("Failed to open type_name.csv");
    }

    std::string header;
    std::getline(file, header);

    std::string line;
    std::vector<TypeName> type_names_list;

    while(std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string cell;

        TypeName type_name;

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                type_name.type_id = INT_MAX;
            } else {
                type_name.type_id = std::stoi(cell);
            }
        } else {
            type_name.type_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                type_name.local_language_id = INT_MAX;
            } else {
                type_name.local_language_id = std::stoi(cell);
            }
        } else {
            type_name.local_language_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            type_name.name = cell;
        }
        
        type_names_list.push_back(type_name);
    }

    std::cout << header << std::endl;
    for(const TypeName& tn : type_names_list) {
        std::string info = (tn.type_id == INT_MAX ? "" : std::to_string(tn.type_id)) + ","; 
        info += (tn.local_language_id == INT_MAX ? "" : std::to_string(tn.local_language_id)) + ",";
        info += tn.name;
        std::cout << info << std::endl;
    }
    return type_names_list;
}