#ifndef WORLD_C
#define WORLD_C

#include <stdbool.h>

struct room {
    int map_x;
    int map_y;
    int north_gate;
    int south_gate;
    int east_gate;
    int west_gate;
    int man_distance;
    bool hasMart;
    bool hasCenter;
    char tiles[21][80];
};

struct tile { // Currently unused
    int x;
    int y;
};

struct world {
    struct room *world_map[401][401];
};

int room_init(struct room *r, int map_x, int map_y);
int room_print(struct room *r);
int make_terrain(struct room *r, char terr_type);
int make_trees(struct room *r);
int make_boulders(struct room *r);
int make_paths(struct room *r);
int make_building(struct room *r, char build_type);
int terraform(struct room *r);
struct world world_init(int start_x, int start_y);
int expand(struct world *w, int x, int y);
int room_output(struct world *w, int x, int y);

#endif