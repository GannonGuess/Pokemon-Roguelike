#ifndef TYPE_NAMES_H
#define TYPE_NAMES_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class TypeName {
    public:
        int type_id;
        int local_language_id;
        std::string name;
};

void parse_type_names(std::filesystem::path pokemon_data);

#endif