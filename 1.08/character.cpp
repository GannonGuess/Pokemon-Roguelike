#include <climits>

#include "character.h"
#include "poke327.h"
#include "io.h"
#include "db_parse.h"
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

/* Just to make the following table fit in 80 columns */
#define PM DIJKSTRA_PATH_MAX
/* Swimmers are not allowed to move onto paths in general, and this *
 * is governed by the swimmer movement code.  However, paths over   *
 * or adjacent to water are bridges.  They can't have inifinite     *
 * movement cost, or it throws a wrench into the turn queue.        */
int32_t move_cost[num_character_types][num_terrain_types] = {
//  boulder,tree,path,mart,center,grass,clearing,mountain,forest,water,gate,bailey
  { PM, PM, 10, 10, 10, 20, 10, PM, PM, PM, 10, 10 },
  { PM, PM, 10, 50, 50, 15, 10, 15, 15, PM, PM, 50 },
  { PM, PM, 10, 50, 50, 20, 10, PM, PM, PM, PM, 50 },
  { PM, PM,  7, PM, PM, PM, PM, PM, PM,  7, PM, PM },
  { PM, PM, 10, 50, 50, 20, 10, PM, PM, PM, PM, 50 },
};
#undef PM

const char *char_type_name[num_character_types] = {
  "PC",
  "Hiker",
  "Rival",
  "Swimmer",
  "Trainer"
};

#define is_adjacent(pos, ter)                                \
  ((world.cur_map->map[pos[dim_y] - 1][pos[dim_x] - 1] == ter) || \
   (world.cur_map->map[pos[dim_y] - 1][pos[dim_x]    ] == ter) || \
   (world.cur_map->map[pos[dim_y] - 1][pos[dim_x] + 1] == ter) || \
   (world.cur_map->map[pos[dim_y]    ][pos[dim_x] - 1] == ter) || \
   (world.cur_map->map[pos[dim_y]    ][pos[dim_x] + 1] == ter) || \
   (world.cur_map->map[pos[dim_y] + 1][pos[dim_x] - 1] == ter) || \
   (world.cur_map->map[pos[dim_y] + 1][pos[dim_x]    ] == ter) || \
   (world.cur_map->map[pos[dim_y] + 1][pos[dim_x] + 1] == ter))

bool is_pc(character *c)
{
  return c == &world.pc;
}

uint32_t can_see(map *m, character *voyeur, character *exhibitionist)
{
  /* Application of Bresenham's Line Drawing Algorithm.  If we can draw a   *
   * line from v to e without intersecting any foreign terrain, then v can  *
   * see * e.  Unfortunately, Bresenham isn't symmetric, so line-of-sight   *
   * based on this approach is not reciprocal (Helmholtz Reciprocity).      *
   * This is a very real problem in roguelike games, and one we're going to *
   * ignore for now.  Algorithms that are symmetrical are far more          *
   * expensive.                                                             */

  /* Adapted from rlg327.  For the purposes of poke327, can swimmers see    *
   * the PC adjacent to water or on a bridge?  v is always a swimmer, and e *
   * is always the player character.                                        */

  pair_t first, second;
  pair_t del, f;
  int16_t a, b, c, i;

  first[dim_x] = voyeur->pos[dim_x];
  first[dim_y] = voyeur->pos[dim_y];
  second[dim_x] = exhibitionist->pos[dim_x];
  second[dim_y] = exhibitionist->pos[dim_y];

  if (second[dim_x] > first[dim_x]) {
    del[dim_x] = second[dim_x] - first[dim_x];
    f[dim_x] = 1;
  } else {
    del[dim_x] = first[dim_x] - second[dim_x];
    f[dim_x] = -1;
  }

  if (second[dim_y] > first[dim_y]) {
    del[dim_y] = second[dim_y] - first[dim_y];
    f[dim_y] = 1;
  } else {
    del[dim_y] = first[dim_y] - second[dim_y];
    f[dim_y] = -1;
  }

  if (del[dim_x] > del[dim_y]) {
    a = del[dim_y] + del[dim_y];
    c = a - del[dim_x];
    b = c - del[dim_x];
    for (i = 0; i <= del[dim_x]; i++) {
      if (((mappair(first) != ter_water) && (mappair(first) != ter_path)) &&
          i && (i != del[dim_x])) {
        return 0;
      }
      first[dim_x] += f[dim_x];
      if (c < 0) {
        c += a;
      } else {
        c += b;
        first[dim_y] += f[dim_y];
      }
    }
    return 1;
  } else {
    a = del[dim_x] + del[dim_x];
    c = a - del[dim_y];
    b = c - del[dim_y];
    for (i = 0; i <= del[dim_y]; i++) {
      if (((mappair(first) != ter_water) && (mappair(first) != ter_path)) &&
          i && (i != del[dim_y])) {
        return 0;
      }
      first[dim_y] += f[dim_y];
      if (c < 0) {
        c += a;
      } else {
        c += b;
        first[dim_x] += f[dim_x];
      }
    }
    return 1;
  }

  return 1;
}

static void move_hiker_func(character *c, pair_t dest)
{
  int min;
  int base;
  int i;
  
  base = rand() & 0x7;

  dest[dim_x] = c->pos[dim_x];
  dest[dim_y] = c->pos[dim_y];
  min = DIJKSTRA_PATH_MAX;
  
  for (i = base; i < 8 + base; i++) {
    if ((world.hiker_dist[c->pos[dim_y] + all_dirs[i & 0x7][dim_y]]
                         [c->pos[dim_x] + all_dirs[i & 0x7][dim_x]] <=
         min) &&
        !world.cur_map->cmap[c->pos[dim_y] + all_dirs[i & 0x7][dim_y]]
                            [c->pos[dim_x] + all_dirs[i & 0x7][dim_x]] &&
        c->pos[dim_x] + all_dirs[i & 0x7][dim_x] != 0 &&
        c->pos[dim_x] + all_dirs[i & 0x7][dim_x] != MAP_X - 1 &&
        c->pos[dim_y] + all_dirs[i & 0x7][dim_y] != 0 &&
        c->pos[dim_y] + all_dirs[i & 0x7][dim_y] != MAP_Y - 1) {
      dest[dim_x] = c->pos[dim_x] + all_dirs[i & 0x7][dim_x];
      dest[dim_y] = c->pos[dim_y] + all_dirs[i & 0x7][dim_y];
      min = world.hiker_dist[dest[dim_y]][dest[dim_x]];
    }
    if (world.hiker_dist[c->pos[dim_y] + all_dirs[i & 0x7][dim_y]]
                        [c->pos[dim_x] + all_dirs[i & 0x7][dim_x]] == 0) {
      io_battle(c, &world.pc);
      break;
    }
  }
}

static void move_rival_func(character *c, pair_t dest)
{
  int min;
  int base;
  int i;
  
  base = rand() & 0x7;

  dest[dim_x] = c->pos[dim_x];
  dest[dim_y] = c->pos[dim_y];
  min = DIJKSTRA_PATH_MAX;
  
  for (i = base; i < 8 + base; i++) {
    if ((world.rival_dist[c->pos[dim_y] + all_dirs[i & 0x7][dim_y]]
                         [c->pos[dim_x] + all_dirs[i & 0x7][dim_x]] <
         min) &&
        !world.cur_map->cmap[c->pos[dim_y] + all_dirs[i & 0x7][dim_y]]
                            [c->pos[dim_x] + all_dirs[i & 0x7][dim_x]] &&
        c->pos[dim_x] + all_dirs[i & 0x7][dim_x] != 0 &&
        c->pos[dim_x] + all_dirs[i & 0x7][dim_x] != MAP_X - 1 &&
        c->pos[dim_y] + all_dirs[i & 0x7][dim_y] != 0 &&
        c->pos[dim_y] + all_dirs[i & 0x7][dim_y] != MAP_Y - 1) {
      dest[dim_x] = c->pos[dim_x] + all_dirs[i & 0x7][dim_x];
      dest[dim_y] = c->pos[dim_y] + all_dirs[i & 0x7][dim_y];
      min = world.rival_dist[dest[dim_y]][dest[dim_x]];
    }
    if (world.rival_dist[c->pos[dim_y] + all_dirs[i & 0x7][dim_y]]
                        [c->pos[dim_x] + all_dirs[i & 0x7][dim_x]] == 0) {
      io_battle(c, &world.pc);
      break;
    }
  }
}

static void move_pacer_func(character *c, pair_t dest)
{
  terrain_type_t t;
  npc *n = dynamic_cast<npc *> (c);
  
  dest[dim_x] = c->pos[dim_x];
  dest[dim_y] = c->pos[dim_y];

  if (!n->defeated &&
      world.cur_map->cmap[c->pos[dim_y] + n->dir[dim_y]]
                         [c->pos[dim_x] + n->dir[dim_x]] ==
      &world.pc) {
      io_battle(c, &world.pc);
      return;
  }

  t = world.cur_map->map[c->pos[dim_y] + n->dir[dim_y]]
                        [c->pos[dim_x] + n->dir[dim_x]];

  if ((t != ter_path && t != ter_grass && t != ter_clearing) ||
      world.cur_map->cmap[c->pos[dim_y] + n->dir[dim_y]]
                         [c->pos[dim_x] + n->dir[dim_x]]) {
    n->dir[dim_x] *= -1;
    n->dir[dim_y] *= -1;
  }

  if ((t == ter_path || t == ter_grass || t == ter_clearing) &&
      !world.cur_map->cmap[c->pos[dim_y] + n->dir[dim_y]]
                          [c->pos[dim_x] + n->dir[dim_x]]) {
    dest[dim_x] = c->pos[dim_x] + n->dir[dim_x];
    dest[dim_y] = c->pos[dim_y] + n->dir[dim_y];
  }
}

static void move_wanderer_func(character *c, pair_t dest)
{
  npc *n = dynamic_cast<npc *> (c);

  dest[dim_x] = c->pos[dim_x];
  dest[dim_y] = c->pos[dim_y];

  if (!n->defeated &&
      world.cur_map->cmap[c->pos[dim_y] + n->dir[dim_y]]
                         [c->pos[dim_x] + n->dir[dim_x]] ==
      &world.pc) {
      io_battle(c, &world.pc);
      return;
  }

  if ((world.cur_map->map[c->pos[dim_y] + n->dir[dim_y]]
                         [c->pos[dim_x] + n->dir[dim_x]] !=
       world.cur_map->map[c->pos[dim_y]][c->pos[dim_x]]) ||
      world.cur_map->cmap[c->pos[dim_y] + n->dir[dim_y]]
                         [c->pos[dim_x] + n->dir[dim_x]]) {
    rand_dir(n->dir);
  }

  if ((world.cur_map->map[c->pos[dim_y] + n->dir[dim_y]]
                         [c->pos[dim_x] + n->dir[dim_x]] ==
       world.cur_map->map[c->pos[dim_y]][c->pos[dim_x]]) &&
      !world.cur_map->cmap[c->pos[dim_y] + n->dir[dim_y]]
                          [c->pos[dim_x] + n->dir[dim_x]]) {
    dest[dim_x] = c->pos[dim_x] + n->dir[dim_x];
    dest[dim_y] = c->pos[dim_y] + n->dir[dim_y];
  }
}

static void move_sentry_func(character *c, pair_t dest)
{
  // Not a bug.  Sentries are non-aggro.
  dest[dim_x] = c->pos[dim_x];
  dest[dim_y] = c->pos[dim_y];
}

static void move_explorer_func(character *c, pair_t dest)
{
  npc *n = dynamic_cast<npc *> (c);

  dest[dim_x] = c->pos[dim_x];
  dest[dim_y] = c->pos[dim_y];

  if (!n->defeated &&
      world.cur_map->cmap[c->pos[dim_y] + n->dir[dim_y]]
                         [c->pos[dim_x] + n->dir[dim_x]] ==
      &world.pc) {
      io_battle(c, &world.pc);
      return;
  }

  if ((move_cost[char_other][world.cur_map->map[c->pos[dim_y] +
                                                n->dir[dim_y]]
                                               [c->pos[dim_x] +
                                                n->dir[dim_x]]] ==
       DIJKSTRA_PATH_MAX) || (world.cur_map->cmap[c->pos[dim_y] +
                                                  n->dir[dim_y]]
                              [c->pos[dim_x] + n->dir[dim_x]])) {
    rand_dir(n->dir);
  }

  if ((move_cost[char_other][world.cur_map->map[c->pos[dim_y] +
                                                n->dir[dim_y]]
                                               [c->pos[dim_x] +
                                                n->dir[dim_x]]] !=
       DIJKSTRA_PATH_MAX) &&
      !world.cur_map->cmap[c->pos[dim_y] + n->dir[dim_y]]
                          [c->pos[dim_x] + n->dir[dim_x]]) {
    dest[dim_x] = c->pos[dim_x] + n->dir[dim_x];
    dest[dim_y] = c->pos[dim_y] + n->dir[dim_y];
  }
}

static void move_swimmer_func(character *c, pair_t dest)
{
  map *m = world.cur_map;
  pair_t dir; 
  npc *n = dynamic_cast<npc *> (c);

  dest[dim_x] = c->pos[dim_x];
  dest[dim_y] = c->pos[dim_y];

  if (is_adjacent(world.pc.pos, ter_water) &&
      can_see(world.cur_map, c, &world.pc)) {
    /* PC is next to this body of water; swim to the PC */

    dir[dim_x] = world.pc.pos[dim_x] - c->pos[dim_x];
    if (dir[dim_x]) {
      dir[dim_x] /= abs(dir[dim_x]);
    }
    dir[dim_y] = world.pc.pos[dim_y] - c->pos[dim_y];
    if (dir[dim_y]) {
      dir[dim_y] /= abs(dir[dim_y]);
    }

    if ((m->map[dest[dim_y] + dir[dim_y]]
               [dest[dim_x] + dir[dim_x]] == ter_water) ||
        ((m->map[dest[dim_y] + dir[dim_y]]
                [dest[dim_x] + dir[dim_x]] == ter_path) &&
         is_adjacent(((pair_t){ (int16_t) (dest[dim_x] + dir[dim_x]),
                               (int16_t) (dest[dim_y] + dir[dim_y]) }),
           ter_water))) {
      dest[dim_x] += dir[dim_x];
      dest[dim_y] += dir[dim_y];
    } else if ((m->map[dest[dim_y]][dest[dim_x] + dir[dim_x]] == ter_water) ||
               ((m->map[dest[dim_y]][dest[dim_x] + dir[dim_x]] == ter_path) &&
                is_adjacent(((pair_t){ (int16_t) (dest[dim_x] + dir[dim_x]),
                                       (int16_t) (dest[dim_y]) }),
                  ter_water))) {
      dest[dim_x] += dir[dim_x];
    } else if ((m->map[dest[dim_y] + dir[dim_y]][dest[dim_x]] == ter_water) ||
               ((m->map[dest[dim_y] + dir[dim_y]][dest[dim_x]] == ter_path) &&
                is_adjacent(((pair_t){ (int16_t) (dest[dim_x]),
                                       (int16_t) (dest[dim_y] + dir[dim_y]) }),
                            ter_water))) {
      dest[dim_y] += dir[dim_y];
    }
  } else {
    /* PC is elsewhere.  Keep doing laps. */
    dir[dim_x] = n->dir[dim_x];
    dir[dim_y] = n->dir[dim_y];
    if ((m->map[dest[dim_y] + dir[dim_y]]
                    [dest[dim_x] + dir[dim_x]] != ter_water) ||
        !((m->map[dest[dim_y] + dir[dim_y]]
                 [dest[dim_x] + dir[dim_x]] == ter_path) &&
          is_adjacent(((pair_t) { (int16_t) (dest[dim_x] + dir[dim_x]),
                                  (int16_t) (dest[dim_y] + dir[dim_y]) }),
            ter_water))) {
      rand_dir(dir);
    }

    if ((m->map[dest[dim_y] + dir[dim_y]]
                    [dest[dim_x] + dir[dim_x]] == ter_water) ||
        ((m->map[dest[dim_y] + dir[dim_y]]
                [dest[dim_x] + dir[dim_x]] == ter_path) &&
         is_adjacent(((pair_t) { (int16_t) (dest[dim_x] + dir[dim_x]),
                                 (int16_t) (dest[dim_y] + dir[dim_y]) }),
           ter_water))) {
      dest[dim_x] += dir[dim_x];
      dest[dim_y] += dir[dim_y];
    }
  }

  if (m->cmap[dest[dim_y]][dest[dim_x]]) {
    /* Occupied.  Just be patient. */
    dest[dim_x] = c->pos[dim_x];
    dest[dim_y] = c->pos[dim_y];
  }
}

static void move_pc_func(character *c, pair_t dest)
{
  io_display();
  io_handle_input(dest);
}

void (*move_func[num_movement_types])(character *, pair_t) = {
  move_hiker_func,
  move_rival_func,
  move_pacer_func,
  move_wanderer_func,
  move_sentry_func,
  move_explorer_func,
  move_swimmer_func,
  move_pc_func,
};

int32_t cmp_char_turns(const void *key, const void *with)
{
  return ((((character *) key)->next_turn ==
           ((character *) with)->next_turn)  ?
          (((character *) key)->seq_num -
           ((character *) with)->seq_num)    :
          (((character *) key)->next_turn -
           ((character *) with)->next_turn));
}

void delete_character(void *v)
{
  if (v != &world.pc) {
    delete (character *) v;
  }
}

#define ter_cost(x, y, c) move_cost[c][m->map[y][x]]

static int32_t hiker_cmp(const void *key, const void *with) {
  return (world.hiker_dist[((path_t *) key)->pos[dim_y]]
                          [((path_t *) key)->pos[dim_x]] -
          world.hiker_dist[((path_t *) with)->pos[dim_y]]
                          [((path_t *) with)->pos[dim_x]]);
}

static int32_t rival_cmp(const void *key, const void *with) {
  return (world.rival_dist[((path_t *) key)->pos[dim_y]]
                          [((path_t *) key)->pos[dim_x]] -
          world.rival_dist[((path_t *) with)->pos[dim_y]]
                          [((path_t *) with)->pos[dim_x]]);
}

void pathfind(map *m)
{
  heap_t h;
  uint16_t x, y;
  static path_t p[MAP_Y][MAP_X], *c;
  static uint32_t initialized = 0;

  if (!initialized) {
    initialized = 1;
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        p[y][x].pos[dim_y] = y;
        p[y][x].pos[dim_x] = x;
      }
    }
  }

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      world.hiker_dist[y][x] = world.rival_dist[y][x] = DIJKSTRA_PATH_MAX;
    }
  }
  
  world.hiker_dist[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = 
    world.rival_dist[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = 0;

  heap_init(&h, hiker_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      /*
      if ((ter_cost(x, y, char_hiker) != DIJKSTRA_PATH_MAX) ||
          ((x == world.pc.pos[dim_x]) && (y == world.pc.pos[dim_y]))) {
      */
      if (ter_cost(x, y, char_hiker) != DIJKSTRA_PATH_MAX) {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      } else {
        p[y][x].hn = NULL;
      }
    }
  }

  while ((c = (path_t *) heap_remove_min(&h))) {
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn) &&
        (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn) &&
        (world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn) &&
        (world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn) &&
        (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);

  heap_init(&h, rival_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      /*
      if ((ter_cost(x, y, char_rival) != DIJKSTRA_PATH_MAX) ||
          ((x == world.pc.pos[dim_x]) && (y == world.pc.pos[dim_y]))) {
      */
      if (ter_cost(x, y, char_rival) != DIJKSTRA_PATH_MAX) {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      } else {
        p[y][x].hn = NULL;
      }
    }
  }

  while ((c = (path_t *) heap_remove_min(&h))) {
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn) &&
        (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn) &&
        (world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn) &&
        (world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn) &&
        (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);
}

void generate_pokemon(monster &p) {
  int minLvl, maxLvl, i;
  int manDist = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
                 abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)));
  if(manDist <= 1) {
    minLvl = 1;
    maxLvl = 1;
  }
  else if(manDist <= 200) {
    minLvl = 1;
    maxLvl = manDist / 2;
  } else {
    minLvl = (manDist - 200) / 2;
    maxLvl = 100;
  }
  int randomPkm = rand() % 1093;
  p.level = (rand() % (maxLvl - minLvl + 1)) + minLvl;
  p.name = pokemon[randomPkm].identifier;
  p.name[0] = toupper(p.name[0]);


  int spec_id = pokemon[randomPkm].species_id;
  std::vector<pokemon_move_db> potential_moves;
  std::vector<pokemon_move_db> selected_moves;

  for(i = 0; i < 528239; i++) {
    if(pokemon_moves[i].pokemon_id == spec_id       && 
       pokemon_moves[i].pokemon_move_method_id == 1 && 
       pokemon_moves[i].level <= p.level) {
      potential_moves.push_back(pokemon_moves[i]);
    }
  }
  std::default_random_engine rng(static_cast<unsigned int>(std::time(0)));
  std::shuffle(potential_moves.begin(), potential_moves.end(), rng);

  


  int move1ID = potential_moves[0].move_id;  
  int move2ID = potential_moves[1].move_id;
  move_db instances[2];
  for(i = 0; i < 845; i++) {
    if(moves[i].id == move1ID) {
      p.move1 = moves[i].identifier;
      break;
    }
  }
  for(i = 0; i < 845; i++) {
    if(moves[i].id == move2ID && move1ID != move2ID) {
      p.move2 = moves[i].identifier;
      break;
    }
  }


  

}