#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "world.h"

//define terrain types for easy modification and readability
enum Terrain{BOULDER = '%', TALL_GRASS = ':', TREE = '^', WATER = '~', PATH = '#',
             SHORT_GRASS = '.', CENTER = 'C', MART = 'M'};

//define min function for obtaining shortest path
int min(int num1, int num2)
{
    return (num1 > num2) ? num2 : num1;
}

//initilize room with border and short grass
int room_init(struct room *r, int map_x, int map_y) {
     r->height = 21;
     r->width = 80;
     r->map_x = map_x;
     r->map_y = map_y;
     int y, x;
     for(y = 0; y < r->height; y++) {
          for(x = 0; x < r->width; x++) {
               if(x == 0 || x == 79 || y == 0 || y == 20) {
                    r->tiles[y][x] = BOULDER;
               } else {
                    r->tiles[y][x] = SHORT_GRASS;
               }
          }
     }
     return 0;
}

//print rooms current state for user
int room_print(struct room *r) {
     int y, x;
     for(y = 0; y < r->height; y++) {
          for(x = 0; x < r->width; x++) {
               printf("%c", r->tiles[y][x]);
          }
          printf("\n");
     }
     return 0;
}

//Take input room and terrain type for adding terain
int make_terrain(struct room *r, char terr_type) {
     //generate grass at random index with random width and length

     int x_index = (rand() % 77) + 1;
     int y_index = (rand() % 18) + 1;
     int width = (rand() % 30) + 3;
     int height = (rand() % 15) + 3;

     int y, x;
     //add terrain of height x width to chosen random location
     for(y = y_index; y < y_index + height && y < 20; y++) {
          for(x = x_index; x < x_index + width && x < 79; x++) {
               if(r->tiles[y][x] != BOULDER) {
                    r->tiles[y][x] = terr_type;
               }
          }
     }
     
     return 0;
}

int make_trees(struct room *r) {
     int num_trees = rand() % 10 + 3;
     int x, y, i;
     for(i = 0; i < num_trees; i++) {
          x = rand() % 78 + 1;
          y = rand() % 19 + 1;
          r->tiles[y][x] = TREE;
     }
     return 0;
}

int make_boulders(struct room *r) {
     int num_boulders = rand() % 4 + 1;
     int x, y, i;
     for(i = 0; i < num_boulders; i++) {
          x = rand() % 78 + 1;
          y = rand() % 19 + 1;
          r->tiles[y][x] = BOULDER;
     }
     return 0;
}

int make_paths(struct room *r) {
     //generate turning points randomly
     int north_south = (rand() % 19) + 1;
     int east_west = (rand() % 78) + 1;

     //Fill in east-west path using r->gates
     int x, y;
     if(r->west_gate) {
          for(x = 0; x <= east_west; x++) {
               r->tiles[r->west_gate][x] = PATH;
          }
     }
     if(r->east_gate) {
          for(x = east_west; x <= 79; x++) {
               r->tiles[r->east_gate][x] = PATH;
          }
     }
     //connect east-west
     if(r->west_gate - r->east_gate < 0) {
          for(y = r->west_gate + 1; y < r->east_gate; y++) {
               r->tiles[y][east_west] = PATH;
          }
     }
     else if(r->west_gate - r->east_gate > 0) {
          for(y = r->west_gate - 1; y > r->east_gate; y--) {
               r->tiles[y][east_west] = PATH;
          }
     }
     //fill in north-south path
     for(y = 0; y <= north_south; y++) {
          r->tiles[y][r->north_gate] = PATH;
     }
     for(y = north_south; y <= 20; y++) {
          r->tiles[y][r->south_gate] = PATH;
     }
     //connect north-south
     if(r->north_gate - r->south_gate < 0) {
          for(x = r->north_gate + 1; x < r->south_gate; x++) {
               r->tiles[north_south][x] = PATH;
          }
     }
     else if(r->north_gate - r->south_gate > 0) {
          for(x = r->north_gate - 1; x > r->south_gate; x--) {
               r->tiles[north_south][x] = PATH;
          }
     }

     //Remove exits on world borders
     if(r->map_x == 0) {
          r->tiles[r->west_gate][0] = BOULDER;
     }
     if(r->map_x == 400) {
          r->tiles[r->east_gate][79] = BOULDER;
     }
     if(r->map_y == 0) {
          r->tiles[0][r->north_gate] = BOULDER;
     }
     if(r->map_y == 400) {
          r->tiles[20][r->south_gate] = BOULDER;
     }

     
     return 0;
}

int make_building(struct room *r, char build_type) {
     int x_index = (rand() % 76) + 2;
     int y_index = (rand() % 17) + 2;
     int x, y;
     bool isValid = false;
     int len = 2;
     // determine if the randomly chosed x y coordinates for the building location 
     // are located on a path or another building. Find new coordinates if so
     while(!isValid) {
          isValid = true;
          for(y = 0; y < len; y++) {
               for(x = 0; x < len; x++) {
                    if(r->tiles[y_index + y][x_index + x] == PATH || r->tiles[y_index + y][x_index + x] == CENTER) {
                         isValid = false;
                    }
               }
          }
          if(!isValid) {
               x_index = (rand() % 77) + 1;
               y_index = (rand() % 18) + 1;
          }
     }
     
     //place building at the coordinates
     for(y = 0 ; y < len; y++) {
          for(x = 0; x < len; x++) {
               r->tiles[y_index + y][x_index + x] = build_type;
          }
     }

     //create path from building to closest existing path (weighted for left and right looking closer together)
     int up = 0, down = 0, left = 0, right = 0;
     //up
     for(y = y_index - 1; y >= 0; y--) {
          if(y == 0 || r->tiles[y][x_index] ==  CENTER || r->tiles[y][x_index + 1] ==  CENTER) {
               up = 100;
               break;
          }
          else if(r->tiles[y][x_index] == PATH || r->tiles[y][x_index + 1] ==  PATH) {
               break;
          }
          up++;
     }
     //down
     for(y = y_index + 2; y <= 20; y++) {
          if(y == 20 || r->tiles[y][x_index] ==  CENTER || r->tiles[y][x_index + 1] ==  CENTER) {
               down = 100;
               break;
          }
          else if(r->tiles[y][x_index] == PATH || r->tiles[y][x_index + 1] ==  PATH) {
               break;
          }
          down++;
     }
     //left
     for(x = x_index - 1; x >= 0; x--) {
          if(x == 0 || r->tiles[y_index][x] ==  CENTER || r->tiles[y_index + 1][x] == CENTER) {
               left = 100;
               break;
          }
          else if(r->tiles[y_index][x] == PATH || r->tiles[y_index + 1][x] == PATH) {
               break;
          }
          left++;
     }
     //right
     for(x = x_index + 2; x <= 79; x++) {
          if(x == 79 || r->tiles[y_index][x] ==  CENTER || r->tiles[y_index + 1][x] == CENTER) {
               right = 100;
               break;
          }
          else if(r->tiles[y_index][x] == PATH || r->tiles[y_index + 1][x] == PATH) {
               break;
          }
          right++;
     }
     
     int minimum = min(min(up * 2, down * 2), min(left, right));
     if(minimum == up * 2) {
          for(y = y_index - 1; y >= y_index - up - 1; y--) {
               r->tiles[y][x_index] = PATH;
               r->tiles[y][x_index + 1] = PATH;
          }
     }
     else if(minimum == down * 2) {
          for(y = y_index + 2; y <= y_index + down + 2; y++) {
               r->tiles[y][x_index] = PATH;
               r->tiles[y][x_index + 1] = PATH;
          }
     }
     else if(minimum == left) {
          for(x = x_index - 1; x >= x_index - left - 1; x--) {
               r->tiles[y_index][x] = PATH;
               r->tiles[y_index + 1][x] = PATH;
          }
     }
     else if(minimum == right) {
          for(x = x_index + 2; x <= x_index + right + 2; x++) {
               r->tiles[y_index][x] = PATH;
               r->tiles[y_index + 1][x] = PATH;
          }
     }

     return 0;
}

int terraform(struct room *r) {
     //add terrains
     make_terrain(r, TALL_GRASS);
     make_terrain(r, TALL_GRASS);
     make_trees(r);
     make_boulders(r);
     make_terrain(r, WATER);
     //add paths
     make_paths(r);

     //add buildings
     if(r->man_distance == 0) {
          make_building(r, CENTER);
          r->hasCenter = true;
          make_building(r, MART);
          r->hasMart = true;
     }
     //printf("dist: %d\n", r->man_distance);
     float build_prob = (((-45.0 * r->man_distance) / 200.0) + 50.0) / 100.0;
     if(r->man_distance > 200) {
          build_prob = 0.05;
     }
     float roll = rand() * 1.0 / (1.0 * RAND_MAX);
     //printf("build_prob: %f\ncenter roll: %f\n", build_prob, roll);
     if(roll <= build_prob && !r->hasCenter) {
          make_building(r, CENTER);
     }
     roll = rand() * 1.0 / (1.0 * RAND_MAX);
     //printf("build_prob: %f\nmart roll: %f\n", build_prob, roll);
     if(roll <= build_prob && !r->hasMart) {
          make_building(r, MART);
     }
     

     return 0;
}

//Not allowed must remove
struct world world_init(int start_x, int start_y) {
     struct world *w;
     if(!(w = malloc(sizeof(struct world)))) {
          printf("FAILED TO CREATE");
     }

     w->length = 401;
     w->x = start_x;
     w->y = start_y;
     
     //Create first room and assign its gates
     struct room *r;
     if(!(r = malloc(sizeof(struct room)))) {
          printf("FAILED TO CREATE");
     }
     r->north_gate = (rand() % 78) + 1;
     r->south_gate = (rand() % 78) + 1;
     r->east_gate = (rand() % 19) + 1;
     r->west_gate = (rand() % 19) + 1;

     w->world_map[start_y][start_x] = r;
     r->man_distance = 0;
     printf("man_dist: %d\n", r->man_distance);
     room_init(r, start_x, start_y);
     terraform(r);

     return *w;
}

int expand(struct world *w, int x, int y) {

     struct room *r;
     if(!(r = malloc(sizeof(struct room)))) {
          printf("failed to create room");
          return -1;
     }
     //Add North gate if it can exist
     if(y - 1 != -1) {
          if(!w->world_map[y - 1][x]) {
               r->north_gate = (rand() % 78) + 1;
          } else {
               r->north_gate = w->world_map[y - 1][x]->south_gate;
          }
     } else {
          r->north_gate = (rand() % 78) + 1; 
     }
     //Add South gate if it can exist
     if(y + 1 != 401) {
          if(!w->world_map[y + 1][x]) {
               r->south_gate = (rand() % 78) + 1;
          } else {
               r->south_gate = w->world_map[y + 1][x]->north_gate;
          }     
     } else {
          r->south_gate = (rand() % 78) + 1;
     }
     //Add East gate if it can exist
     if(x + 1 != 401) {
          if(!w->world_map[y][x + 1]) {
               r->east_gate = (rand() % 19) + 1;
          } else {
               r->east_gate = w->world_map[y][x + 1]->west_gate;
          }
     } else {
          r->east_gate = (rand() % 19) + 1;
     }
     //Add West gate if it can exist
     if(x - 1 != -1) {
          if(!w->world_map[y][x - 1]) {
               r->west_gate = (rand() % 19) + 1;
          } else {
               r->west_gate = w->world_map[y][x - 1]->east_gate;
          }
     } else {
          r->west_gate = (rand() % 19) + 1;
     }

     w->world_map[y][x] = r;
     int dist = abs(x - 200) + abs(y - 200);
     r->man_distance = dist;
     printf("dist: %d", r->man_distance);

     room_init(r, x, y);
     terraform(r);
     

     return 0;
}


//Prints out the room of world coordinates (x, y)
int room_output(struct world *w, int x, int y) {
     struct room *r;
     printf("user_x: %d user_y: %d world_mapX: %d world_mapY: %d\n", x - 200, y - 200, x, y);
     

     //set room
     r = w->world_map[y][x];
     room_print(r);
     return 0;
}



int main(int argc, char *argv[]) 
{
     int x = 200;
     int y = 200;
     
     //generate random room based on time
     srand(time(NULL));
     
     //Initilize the world and (0,0)
     struct world w = world_init(x, y);
     int i, j;
     
     char user_in;
     int fly_x, fly_y;

     do {
          room_output(&w, x, y);
          printf("Command: char ");
          scanf(" %1c", &user_in);
          if(user_in == 'f') {
               scanf(" %d %d", &fly_x, &fly_y);
          }
          printf("fx: %d fy: %d\n", fly_x, fly_y);
          switch(user_in) {
               case 'n':
                    y -= 1;
                    if(y < 0) {
                         y = 0;
                    }
                    break;
               case 's':
                    y += 1;
                    if (y > 400) {
                         y = 400;
                    }
                    break;
               case 'e':
                    x += 1;
                    if(x > 400) {
                         x = 400;
                    }
                    break;
               case 'w':
                    x -= 1;
                    if(x < 0) {
                         x = 0;
                    }
                    break;
               case 'q':
                    printf("Exiting Gannomon");
                    break;
               case 'f':
                    if(fly_x + 200 ==x && fly_y + 200 == y) {
                         printf("You are already in this room\n");
                    } else if(fly_x > 200 || fly_x < -200 || fly_y > 200 || fly_y < -200) {
                         printf("Selected room outside of range [-200:200]\n");
                    } else {
                         x = fly_x + 200;
                         y = fly_y + 200;
                    }
                    break;
               default:
                    printf("Input error. Please use valid input\n");
                    break;
          }
          if(w.world_map[y][x] == NULL) {
               expand(&w, x, y);
          }
     } while(user_in != 'q');
     
          
          
     return 0;
}