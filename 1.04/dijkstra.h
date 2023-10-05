#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <math.h>
#include <limits.h>

typedef enum Actors{PLAYER = '@', HIKER = 'H', RIVAL = 'R'}actor_t;
enum Terrain{BOULDER = '%', TALL_GRASS = ':', TREE = '4', WATER = '~', PATH = '#',
             SHORT_GRASS = '.', CENTER = 'C', MART = 'M'};

typedef struct cell_cost {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} cell_cost_t;

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

static int32_t path_cmp(const void *key, const void *with);
int calculate_cost(char current_type, char actor);



#endif