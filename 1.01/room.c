#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "room.h"
#include <stdbool.h>

//define terrain types for easy modification and readability
enum terrain{BOULDER = '%', TALL_GRASS = ':', TREE = '^', WATER = '~', PATH = '#',
             SHORT_GRASS = '.', CENTER = 'C', MART = 'M'};

//define min function for obtaining shortest path
int min(int num1, int num2)
{
    return (num1 > num2) ? num2 : num1;
}

//initilize room with border and short grass
int room_init(struct room *r) {
     r->height = 21;
     r->width = 80;
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
     //select exits
     int north_exit = (rand() % 78) + 1;
     int south_exit = (rand() % 78) + 1;
     int east_exit = (rand() % 19) + 1;
     int west_exit = (rand() % 19) + 1;

     //generate turning points
     int north_south = (rand() % 19) + 1;
     int east_west = (rand() % 78) + 1;

     //Fill in east-west path
     int x, y;
     for(x = 0; x <= east_west; x++) {
          r->tiles[west_exit][x] = PATH;
     }
     for(x = east_west; x <= 79; x++) {
          r->tiles[east_exit][x] = PATH;
     }
     if(west_exit - east_exit < 0) {
          for(y = west_exit + 1; y < east_exit; y++) {
               r->tiles[y][east_west] = PATH;
          }
     }
     else if(west_exit - east_exit > 0) {
          for(y = west_exit - 1; y > east_exit; y--) {
               r->tiles[y][east_west] = PATH;
          }
     }
     
     //fill in north-south path
     for(y = 0; y <= north_south; y++) {
          r->tiles[y][north_exit] = PATH;
     }
     for(y = north_south; y <= 20; y++) {
          r->tiles[y][south_exit] = PATH;
     }
     if(north_exit - south_exit < 0) {
          for(x = north_exit + 1; x < south_exit; x++) {
               r->tiles[north_south][x] = PATH;
          }
     }
     else if(north_exit - south_exit > 0) {
          for(x = north_exit - 1; x > south_exit; x--) {
               r->tiles[north_south][x] = PATH;
          }
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

int main(int argc, char *argv[]) 
{
     //generate random room based on time
     srand(time(NULL));

     //initilize the room
     struct room r;
     room_init(&r);

     //add terrains
     make_terrain(&r, TALL_GRASS);
     make_terrain(&r, TALL_GRASS);
     make_trees(&r);
     make_boulders(&r);
     make_terrain(&r, WATER);

     //add paths
     make_paths(&r);

     //add buildings
     make_building(&r, CENTER);
     make_building(&r, MART);

     //print room
     room_print(&r);


     return 0;
}