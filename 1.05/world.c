#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "trainer.c"
#include "world.h"


#include "colors.c"
#include "heap.c"
#include "dijkstra.c"


typedef uint8_t pair_t[num_dims];
//define terrain types for easy modification and readability
//enum Terrain{BOULDER = '%', TALL_GRASS = ':', TREE = '?', WATER = '~', PATH = '#',
         //    SHORT_GRASS = '.', CENTER = 'C', MART = 'M'};

//define PC and NPC types
//typedef enum Actors{PLAYER = '@', HIKER = 'H', RIVAL = 'R'}actor_t;

//define min function for obtaining shortest path
int min(int num1, int num2) {
    return (num1 > num2) ? num2 : num1;
}

//initilize room with border and short grass
int room_init(struct room *r, int map_x, int map_y) {
     r->map_x = map_x;
     r->map_y = map_y;
     int y, x;
     for(y = 0; y < 21; y++) {
          for(x = 0; x < 80; x++) {
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
int room_print(struct room *r, actorMap *cmap) {
     int y, x;
     printf("\n");
     for(y = 0; y < 21; y++) {
          for(x = 0; x < 80; x++) {
               if(y != 0 && x != 0 && x != 79 && y != 20 && cmap->actorMap[y - 1][x - 1]) {
                    purple();
                    printf("%c", cmap->actorMap[y - 1][x - 1]->display);
               } else {
                    printf("%c", r->tiles[y][x]);
               }
               white();
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
     //add terrain of height * width to chosen random location
     for(y = y_index; y < y_index + height && y < 20; y++) {
          for(x = x_index; x < x_index + width && x < 79; x++) {
               if(r->tiles[y][x] != BOULDER) {
                    r->tiles[y][x] = terr_type;
               }
          }
     }
     
     return 0;
}

//Add between 3 and 12 trees to room
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

// Add between 1 and 4 boulders to room
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

// Add paths to room
int make_paths(struct room *r) {
     //generate turning points randomly
     int north_south = (rand() % 17) + 2;
     int east_west = (rand() % 76) + 2;

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

// Add building of type build_type to room
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
     //Choose smallest path
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

// Add terrain and buildings to the room parameter
int terraform(struct room *r) {
     //add terrains
     make_terrain(r, TALL_GRASS);
     make_terrain(r, TALL_GRASS);
     make_terrain(r, TALL_GRASS);
     make_trees(r);
     make_terrain(r, WATER);
     make_boulders(r);

     make_paths(r); //add paths

     //add buildings
     if(r->man_distance == 0) {
          make_building(r, CENTER);
          r->hasCenter = true;
          make_building(r, MART);
          r->hasMart = true;
     }

     float build_prob = (((-45.0 * r->man_distance) / 200.0) + 50.0) / 100.0;
     if(r->man_distance > 200) {
          build_prob = 0.05;
     }
     float roll = rand() * 1.0 / (1.0 * RAND_MAX);

     if(roll <= build_prob && !r->hasCenter) {
          make_building(r, CENTER);
          r->hasCenter = true;
     }
     roll = rand() * 1.0 / (1.0 * RAND_MAX);

     if(roll <= build_prob && !r->hasMart) {
          make_building(r, MART);
          r->hasMart = true;
     }
     

     return 0;
}

// Initilize a struct world
struct world world_init(int start_x, int start_y) {
     struct world *w;
     if(!(w = malloc(sizeof(struct world)))) { // Malloc space for world
          printf("FAILED TO CREATE WORLD");
     }
     
     //Create first room and assign its gates
     struct room *r;
     if(!(r = malloc(sizeof(struct room)))) { // Malloc space for room
          printf("FAILED TO CREATE ROOM");
     }
     r->north_gate = (rand() % 78) + 1; // Select gates
     r->south_gate = (rand() % 78) + 1;
     r->east_gate = (rand() % 19) + 1;
     r->west_gate = (rand() % 19) + 1;

     w->world_map[start_y][start_x] = r;
     r->man_distance = 0;
     room_init(r, start_x, start_y); // create room at 200, 200
     terraform(r); // add stuff to world map 0, 0

     return *w;
}

//initilize a 21x80 array to hold actors
actorMap actorMapInit() {
     actorMap *cmap;
     if(!(cmap = malloc(sizeof(actorMap)))) {
          printf("Failed to create character map");
     }
     return *cmap;
}

//initilizes a room at the input location
int expand(struct world *w, struct player *pc, int x, int y) {

     struct room *r; // define room
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

     w->world_map[y][x] = r; // Add the new room to the world_map
     r->man_distance = abs(x - 200) + abs(y - 200); //set room distance

     room_init(r, x, y); //Initilize the new room
     terraform(r); // Terraform the new room 
     player_place(pc, w->world_map[y][x]); // place player character in current map
     
     return 0;
}


int player_place(struct player *pc, struct room *r) { // for now, the player is placed at a random path location
     int x, y;
     bool isPath = false;
     while(!isPath) {
          x = rand() % 78 + 1;
          y = rand() % 19 + 1;
          if(r->tiles[y][x] == PATH) {
               isPath = true;
          }
     }
     
     pc->pc_x = x;
     pc->pc_y = y;
     return 0;
}


// Main method for running game
int main(int argc, char *argv[]) 
{
     srand(time(NULL)); //generate random room based on time
     int numTrainers;
     if(argc == 1) {
          numTrainers = 10;
     }
     else if(argc == 3) {
          if(atoi(argv[2]) < 0) {
               fprintf(stderr, "Number of trainser must be >= 0");
               return -1;
          }
          if(!strcmp(argv[1], "--numtrainers")) {
               numTrainers = atoi(argv[2]);
               if(numTrainers > 75) {
                    fprintf(stderr, "It looks like that many NPCs would be too difficult to play. Please choose a vlaue < 75");
                    return -1;
               }
          }
          else {
               fprintf(stderr, "Usage: --numtrainers <number of trainers>\n");
               return -1;
          }
     }
     else {
          fprintf(stderr, "Invalid number of arguments. <1, 3>\n");
          return -1;
     }
     
     // default room coordinates
     int x = 200;
     int y = 200;
          
     struct world w = world_init(x, y); //Initilize the world and room at (0,0) [200,200]
     actorMap cmap = actorMapInit(); //Initilize the map for storing actors

     //initilize an actor and store a player in it
     actor *player = malloc(sizeof(actor));
     struct player *pc = malloc(sizeof(struct player));
     player->pc = pc;
     player->seqNum = 0;
     player->display = '@';

     //place the player in the world and add it to the actor map
     player_place(pc, w.world_map[y][x]);
     cmap.actorMap[pc->pc_y - 1][pc->pc_x - 1] = player; 

     
     char user_in; // user input values
     int fly_x, fly_y; // flight coordinates
     char garbage[30]; // default garbage bin size. could be redone?
     bool canFly = true; // determines if flight can be done

     // main gameplay loop
     do {
     
          generate_trainers(numTrainers, &cmap, w.world_map[y][x]); // generate trainers and put them into cmap   
          room_print(w.world_map[y][x], &cmap); // print the room and actors using cmap
          printf("Current location (x, y): (%d, %d)\n", x - 200, y - 200); // display map info

          distanceMap hikerMap = dijkstra(w.world_map[y][x], pc->pc_x - 1, pc->pc_y - 1, 'h'); // compute hiker costmap
          distanceMap rivalMap = dijkstra(w.world_map[y][x], pc->pc_x - 1, pc->pc_y - 1, 'r'); // compute rival costmap
          printf("\n\n");

          move_loop(numTrainers, &cmap, &hikerMap, &rivalMap, w.world_map[y][x]); // begin movement of actors


          return 0;
          
    

          //Command input commented out for 1.04
          /*printf("\nCommand: ");
          int size = scanf(" %1c", &user_in);
          if(user_in == 'f') { // scan for <x, y> for flight
               size += scanf(" %4i %4i", &fly_x, &fly_y);
               if(size >= 3) { // get rid of excess input if any
                    scanf("%[^\n]", garbage);
               }
          }
          //printf("\n%c %d %d %d %s\n", user_in, fly_x, fly_y, size, garbage); //stats for printing
          if(size != 1 && size != 3) { // get rid of excess input if any. Dont allow flight if wrong format
               if(user_in == 'f') {
                    scanf("%[^\n]", garbage);
               }
               canFly = false;
          }   
          
          switch(user_in) { // choose option based on user input
               case 'n': // move north
                    y -= 1;
                    if(y < 0) {
                         y = 0;
                    }
                    break;
               case 's': // move south
                    y += 1;
                    if (y > 400) {
                         y = 400;
                    }
                    break;
               case 'e': // move east
                    x += 1;
                    if(x > 400) {
                         x = 400;
                    }
                    break;
               case 'w': // move west
                    x -= 1;
                    if(x < 0) {
                         x = 0;
                    }
                    break;
               case 'q': // quit game
                    printf("Exiting Gannomon");
                    break;
               case 'f': // fly to input <x, y>
                    if(fly_x + 200 ==x && fly_y + 200 == y) {
                         printf("Aborting Flight: You are already in this room\n");
                    } else if(fly_x > 200 || fly_x < -200 || fly_y > 200 || fly_y < -200) {
                         printf("Aborting flight: Selected room (%d, %d) outside of range [-200:200]\n", fly_x, fly_y);
                    } else if(!canFly){
                         printf("Aborting flight: the format is <f x y>\n");             
                    } else {
                         x = fly_x + 200;
                         y = fly_y + 200;
                    }
                    canFly = true; //reset fly to true
                    break;
               default: // if invalid input
                    printf("Input error. Please use valid input\n");
                    break;
          }
          if(!w.world_map[y][x]) {
               expand(&w, pc, x, y);
          }*/
     } while(user_in != 'q');
     return 0;
}

/* prioritize on two values. Cost and sequence number. PC is sequence number 0. Every new character gets a new sequence number
write comparator such that if the next turns are equal, then look at the sequence number


struct character {
     struct pc *pc;
     struct npc *npc
     //common data
}
in common data will be next turn and sequence number
prioity queue is filled with characters

Add a map of character pointers
struct character *cmap[21][80] : initilize all pointers to null and stick the created characters into this map
- check if place that character wants to move to in null or not in the character map
Displaying character in map is a matter of checking if null. If not null get symbol from character struct, otherwise print the terrain



1.05 INTERFACE
Dont print typed characters
raw()
noecho()
curs_set(0) to set cursor invisible
keypad(stdscr, TRUE) turn on number pad

void io_init_terminal(void) {
     initscr();
     raw();
     noecho();
     curs_set(0);
     keypad(stdscr, TRUE);
}

Deinitlizatio
int endwin(void) returns resources to the system
- if program crashes and corrupts the terminal: run "reset" command

reading input
int getch(void)
- reads single keystroke
- blocking call

Displaying
int clear(void)
- before redrawing potentially use
int mvaddch(int y, int x, const chtype ch)
- move cursor to (y, x) and write ch
int refresh(void)
- redraw the terminal: NEEDED
usr/include/ncurses.h/
*/