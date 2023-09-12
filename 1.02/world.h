#ifndef WORLD_C
#define WORLD_C

#include <stdbool.h>

struct room {
    int width;
    int height;
    int north_gate;
    int south_gate;
    int east_gate;
    int west_gate;
    int man_distance;
    bool hasMart;
    bool hasCenter;
    char tiles[21][80];
};

struct tile {
    int x;
    int y;
};

struct world {
    int x;
    int y;
    int length;
    struct room *world_map[401][401];
};

struct gate {
    int x;
    int y;
};


int room_init(struct room *r);
int room_print(struct room *r);
int make_terrain(struct room *r, char terr_type);
int make_paths(struct room *r);
int make_building(struct room *r, char build_type);

//int world_init(struct world *w);

#endif