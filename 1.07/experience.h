#ifndef EXPERIENCE_H
#define EXPERIENCE_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class Experience {
    public:
        int growth_rate_id;
        int level;
        int experience;
};

void parse_experience(std::filesystem::path pokemon_data);

#endif