#include "pokemon_species.h"


std::vector<PokemonSpecies> parse_pokemon_species(std::filesystem::path pk_species_data) {
    std::ifstream file(pk_species_data);
    if(!file.is_open()) {
        throw std::runtime_error("Failed to open pokemon_species.csv");
    }

    std::string header;
    std::getline(file, header);

    std::string line;
    std::vector<PokemonSpecies> pk_species_list;

    while(std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string cell;

        PokemonSpecies pk_spec;

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.id = INT_MAX;
            } else {
                pk_spec.id = std::stoi(cell);
            }
        } else {
            pk_spec.id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            pk_spec.name = cell;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.generation_id = INT_MAX;
            } else {
                pk_spec.generation_id= std::stoi(cell);
            }
        } else {
            pk_spec.generation_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.evolves_from_species_id = INT_MAX;
            } else {
                pk_spec.evolves_from_species_id = std::stoi(cell);
            }
        } else {
            pk_spec.evolves_from_species_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.evolution_chain_id = INT_MAX;
            } else {
                pk_spec.evolution_chain_id = std::stoi(cell);
            }
        } else {
            pk_spec.evolution_chain_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.color_id = INT_MAX;
            } else {
                pk_spec.color_id = std::stoi(cell);
            }
        } else {
            pk_spec.color_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.shape_id = INT_MAX;
            } else {
                pk_spec.shape_id = std::stoi(cell);
            }
        } else {
            pk_spec.shape_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.habitat_id = INT_MAX;
            } else {
                pk_spec.habitat_id = std::stoi(cell);
            }
        } else {
            pk_spec.habitat_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.gender_rate = INT_MAX;
            } else {
                pk_spec.gender_rate = std::stoi(cell);
            }
        } else {
            pk_spec.gender_rate = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.capture_rate = INT_MAX;
            } else {
                pk_spec.capture_rate = std::stoi(cell);
            }
        } else {
            pk_spec.capture_rate = INT_MAX;
        }
        
        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.base_happiness = INT_MAX;
            } else {
                pk_spec.base_happiness = std::stoi(cell);
            }
        } else {
            pk_spec.base_happiness = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.is_baby = INT_MAX;
            } else {
                pk_spec.is_baby = std::stoi(cell);
            }
        } else {
            pk_spec.is_baby = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.hatch_counter = INT_MAX;
            } else {
                pk_spec.hatch_counter = std::stoi(cell);
            }
        } else {
            pk_spec.hatch_counter = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.has_gender_differences = INT_MAX;
            } else {
                pk_spec.has_gender_differences = std::stoi(cell);
            }
        } else {
            pk_spec.has_gender_differences = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.growth_rate_id = INT_MAX;
            } else {
                pk_spec.growth_rate_id = std::stoi(cell);
            }
        } else {
            pk_spec.growth_rate_id = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.forms_switchable = INT_MAX;
            } else {
                pk_spec.forms_switchable = std::stoi(cell);
            }
        } else {
            pk_spec.forms_switchable = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.is_legendary = INT_MAX;
            } else {
                pk_spec.is_legendary = std::stoi(cell);
            }
        } else {
            pk_spec.is_legendary = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.is_mythical = INT_MAX;
            } else {
                pk_spec.is_mythical = std::stoi(cell);
            }
        } else {
            pk_spec.is_mythical = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.order = INT_MAX;
            } else {
                pk_spec.order = std::stoi(cell);
            }
        } else {
            pk_spec.order = INT_MAX;
        }

        if (std::getline(lineStream, cell, ',')) {
            if(cell.empty()) {
                pk_spec.conquest_order = INT_MAX;
            } else {
                pk_spec.conquest_order = std::stoi(cell);
            }
        } else {
            pk_spec.conquest_order = INT_MAX;
        }


        pk_species_list.push_back(pk_spec);
    }

    std::cout << header << std::endl;
    for(const PokemonSpecies& m : pk_species_list) {
        std::string info = (m.id == INT_MAX ? "" : std::to_string(m.id)) + ","; 
        info += m.name + ",";
        info += (m.generation_id == INT_MAX ? "" : std::to_string(m.generation_id)) + ",";
        info += (m.evolves_from_species_id == INT_MAX ? "" : std::to_string(m.evolves_from_species_id)) + ",";
        info += (m.evolution_chain_id == INT_MAX ? "" : std::to_string(m.evolution_chain_id)) + ",";
        info += (m.color_id == INT_MAX ? "" : std::to_string(m.color_id)) + ",";
        info += (m.shape_id == INT_MAX ? "" : std::to_string(m.shape_id)) + ",";
        info += (m.habitat_id == INT_MAX ? "" : std::to_string(m.habitat_id)) + ",";
        info += (m.gender_rate == INT_MAX ? "" : std::to_string(m.gender_rate)) + ",";
        info += (m.capture_rate == INT_MAX ? "" : std::to_string(m.capture_rate)) + ",";
        info += (m.base_happiness == INT_MAX ? "" : std::to_string(m.base_happiness)) + ",";
        info += (m.is_baby == INT_MAX ? "" : std::to_string(m.is_baby)) + ",";
        info += (m.hatch_counter== INT_MAX ? "" : std::to_string(m.hatch_counter)) + ",";
        info += (m.has_gender_differences == INT_MAX ? "" : std::to_string(m.has_gender_differences)) + ",";
        info += (m.growth_rate_id == INT_MAX ? "" : std::to_string(m.growth_rate_id)) + ",";
        info += (m.forms_switchable == INT_MAX ? "" : std::to_string(m.forms_switchable)) + ",";
        info += (m.is_legendary == INT_MAX ? "" : std::to_string(m.is_legendary)) + ",";
        info += (m.is_mythical == INT_MAX ? "" : std::to_string(m.is_mythical)) + ",";
        info += (m.order == INT_MAX ? "" : std::to_string(m.order)) + ",";
        info += (m.conquest_order == INT_MAX ? "" : std::to_string(m.conquest_order));
        std::cout << info << std::endl;
    }
    return pk_species_list;
}