#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include <ncurses.h>
#include <vector>
#include "pair.h"

#define MAP_X           20
#define MAP_Y           20

#define EMPTY_SYMBOL        '.'
#define HIT_SYMBOL          'x'
#define MISS_SYMBOL         'o'
#define ERROR_SYMBOL        '&'


typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_empty,
  ter_hit,
  ter_miss,
  ter_debug
} terrain_type_t;

class Ship {
    public:
        int length;
        pair_t head;
        pair_t tail;
};

class map {
    public:
        int height;
        int width;
        std::vector<Ship> ships;
};


class map {
public:
    terrain_type_t map[MAP_Y][MAP_X];
};


#endif