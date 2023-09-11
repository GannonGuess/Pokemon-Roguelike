#ifndef ROOM_C
#define ROOM_C

struct room {
    int width;
    int height;
    char tiles[21][80];
};


int room_init(struct room *r);
int room_print(struct room *r);
int make_terrain(struct room *r, char terr_type);
int make_paths(struct room *r);
int make_building(struct room *r, char build_type);

#endif