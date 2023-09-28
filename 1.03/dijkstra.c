#include <math.h>
#include <limits.h>
#include "heap.h"
#include "heap.c"

typedef enum Actors{PLAYER = '@', HIKER = 'H', RIVAL = 'R'}actor_t;
enum Terrain{BOULDER = '%', TALL_GRASS = ':', TREE = '?', WATER = '~', PATH = '#',
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

static int32_t path_cmp(const void *key, const void *with) {
    return ((cell_cost_t *) key)->cost - ((cell_cost_t *) with)->cost;
}

int calculate_cost(char current_tile, char actor) {
    switch(actor) {
        case 'h':
            switch(current_tile) {
                case '#':
                case '.':
                case '@': {
                    return 10;
                }
                case ':':
                    return 15;
                case 'C':
                case 'M': {
                    return 50;
                }
                default:
                    return INT_MAX;
            }
        case 'r':
            switch(current_tile) {
                case 'C':
                case 'M': {
                    return 50;
                }
                case '#':
                case '.':
                case '@': {
                    return 10;
                }
                case ':':
                    return 20;
            }
    }
    return INT_MAX;
}


static void djikstra(struct room *r, int pc_x, int pc_y, char pc_terr, char actor) {
    static cell_cost_t costs[21][80], *c; // initilize array for holding cell costs
    int dist[21][80];
    static uint32_t initilized = 0;
    heap_t h;
    int x, y;
    if(!initilized) {
        for(y = 0; y < 21; y++) {
            for(x = 0; x < 80; x++) {
                costs[y][x].pos[dim_y] = y;
                costs[y][x].pos[dim_x] = x;
            }
        }
        initilized = 1;
    }

    heap_init(&h, path_cmp, NULL);
    
    for (y = 0; y < 21; y++) {
        for (x = 0; x < 80; x++) {
            costs[y][x].cost = INT_MAX;
        }
    }


    costs[pc_y][pc_x].cost = 0;
    costs[pc_y][pc_x].hn = heap_insert(&h, &costs[pc_y][pc_x]);

    char current_tile;
    char neighbor_tile;
    int tile_cost;
    int run = 0;

    while((c = heap_remove_min(&h))) { // set c = to h's min node and remove the min from the array
        //printf("%d\n", c->cost);
        int curr_x = c->pos[dim_x];
        int curr_y = c->pos[dim_y];
        //printf("%d %d\n", curr_x, curr_y);
        for(y = -1; y < 2; y++) {
            for(x = -1; x < 2; x++) { // check c's neighbors
                if(x == 0 && y == 0) { // skip center node
                    continue;
                }
                if(curr_x + x < 0 || curr_x + x > 79 || curr_y + y < 0 || curr_y + y > 20) { // skip if at border
                    continue;
                }
                
                current_tile = r->tiles[curr_y + 1][curr_x + 1];
                if(current_tile == '~' || current_tile == '?' || current_tile == '%') {
                    continue;
                }

                int current_cost = c->cost;
                
                neighbor_tile = r->tiles[curr_y + y + 1][curr_x + x + 1]; 

                tile_cost = calculate_cost(current_tile, actor);
                //printf("%d %d %c %c %d %d\n", x, y, neighbor_tile, current_tile, tile_cost, current_cost);

                if(tile_cost + current_cost < costs[curr_y + y][curr_x + x].cost && tile_cost + current_cost > 0) {
                    costs[curr_y + y][curr_x + x].cost = tile_cost + current_cost;
                    heap_insert(&h, &costs[curr_y + y][curr_x + x]);
                    int neighbor_cost = costs[curr_y + y][curr_x + x].cost;
                   // printf("new value: %d\n", neighbor_cost);
                }
                else {
                    //printf("not updated\n");
                }
                
                
                
            }
        }
        // run++;
        // if(run == 100) {
        //     break;
        // }
    }

    char check;
    for(y = 0; y < 19; y++) {
        for(x = 0; x < 80; x++) {
            check = r->tiles[costs[y][x].pos[dim_y] + 1][costs[y][x].pos[dim_x] + 1];
            if(check == '~' || check == '?' || check == '%') {
                printf("   ");
            }
            else {
                dist[y][x] = costs[y][x].cost;
                printf("%.2d ", dist[y][x] % 100);
            }
        }
        printf("\n");
    }
    


    ///END OF TRIAL
    /*
    for(y = 1; y < 20; y++) {
       for(x = 1; x < 79; x++) {
            costs[from_y][from_x].hn = heap_insert(&h, &costs[from_y][from_x]);
       }
    }

    int run = 0;
    char current_tile;
    int tile_cost;
    
    while((c = heap_remove_min(&h))) {
        c->hn = NULL;
        current_tile = r->tiles[c->pos[dim_y] - 1][c->pos[dim_x]];
        tile_cost = calculate_cost(current_tile, actor);

        if (costs[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn && (costs[c->pos[dim_y] - 1][c->pos[dim_x]    ].cost > c->cost)) { //west
            if(tile_cost != INT_MAX) {
                costs[c->pos[dim_y] - 1][c->pos[dim_x]    ].cost = c->cost + tile_cost;
            } 
            heap_decrease_key_no_replace(&h, costs[c->pos[dim_y] - 1]
                                           [c->pos[dim_x]    ].hn);
        }
        if (costs[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn && (costs[c->pos[dim_y]    ][c->pos[dim_x] - 1].cost > c->cost)) { //north
            if(tile_cost != INT_MAX) {
                costs[c->pos[dim_y]    ][c->pos[dim_x] - 1].cost = c->cost + tile_cost;
            }
            heap_decrease_key_no_replace(&h, costs[c->pos[dim_y]    ]
                                           [c->pos[dim_x] - 1].hn);
        }
        if (costs[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn && (costs[c->pos[dim_y]    ][c->pos[dim_x] + 1].cost > c->cost) && (c->pos[dim_x] + 1 != 79)) { //south
            if(tile_cost != INT_MAX) {
                costs[c->pos[dim_y]    ][c->pos[dim_x] + 1].cost = c->cost + tile_cost;
            }
            heap_decrease_key_no_replace(&h, costs[c->pos[dim_y]    ]
                                           [c->pos[dim_x] + 1].hn);
        }
        if (costs[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn && (costs[c->pos[dim_y] + 1][c->pos[dim_x]    ].cost > c->cost) && (c->pos[dim_y] + 1 != 20)) { //east
            if(tile_cost != INT_MAX) {
                costs[c->pos[dim_y] + 1][c->pos[dim_x]    ].cost = c->cost + tile_cost;
            }
            heap_decrease_key_no_replace(&h, costs[c->pos[dim_y] + 1]
                                           [c->pos[dim_x]    ].hn);
        }
        if (costs[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn && (costs[c->pos[dim_y] - 1][c->pos[dim_x] + 1].cost > c->cost) && (c->pos[dim_x] + 1 != 79)) { //south west
            if(tile_cost != INT_MAX) {
                costs[c->pos[dim_y] - 1][c->pos[dim_x] + 1].cost = c->cost + tile_cost;
            }
            heap_decrease_key_no_replace(&h, costs[c->pos[dim_y] - 1]
                                           [c->pos[dim_x] + 1].hn);
        }
        if (costs[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn && (costs[c->pos[dim_y] + 1][c->pos[dim_x] - 1].cost > c->cost) && (c->pos[dim_y] + 1 != 20)) { // north east
            if(tile_cost != INT_MAX) {
                costs[c->pos[dim_y] + 1][c->pos[dim_x] - 1].cost = c->cost + tile_cost;
            }
            heap_decrease_key_no_replace(&h, costs[c->pos[dim_y] + 1]
                                           [c->pos[dim_x] - 1].hn);
        }
        if (costs[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn && (costs[c->pos[dim_y] + 1][c->pos[dim_x] + 1].cost > c->cost) && (c->pos[dim_y] + 1 != 20) && (c->pos[dim_x] + 1 != 79)) { // south east
            if(tile_cost != INT_MAX) {
                costs[c->pos[dim_y] + 1][c->pos[dim_x] + 1].cost = c->cost + tile_cost;
            }
            heap_decrease_key_no_replace(&h, costs[c->pos[dim_y] + 1]
                                           [c->pos[dim_x] + 1].hn);
        }
        if (costs[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn && (costs[c->pos[dim_y] - 1][c->pos[dim_x] - 1].cost > c->cost)) { // north west
            if(tile_cost != INT_MAX) {
                costs[c->pos[dim_y] - 1][c->pos[dim_x] - 1].cost = c->cost + tile_cost;
            }
            heap_decrease_key_no_replace(&h, costs[c->pos[dim_y] - 1]
                                           [c->pos[dim_x] - 1].hn);
        }
        if(run == 2) {
        //    break;
        }
        run++;
    }
    
    for(y = 0; y < 20; y++) {
        for(x = 0; x < 79; x++) {
            check = r->tiles[costs[y][x].pos[dim_y] + 1][costs[y][x].pos[dim_x] + 1];
            if(check == '~' || check == '?' || check == '%') {
                printf("   ");
            }
            else {
                printf("%.2d ", dist[y][x] % 100);
            }
        }
        printf("\n");
    } */
}


/*Starting node has cost of zero. all else infinity.
Queue has @ with cost 0
@ inf
A inf
B inf
C inf
D inf
...

Pull out @ (lowest cost) and assign its neighbors their cost, mark start node (@) done. All checked nodes get a prev of @
Check next lowest (D) and set cost of its neighbors + cost D. All nodes get  prev of D. mark D done
Pick next lowest cost again (say, A) and find neighbors' cost and assign cost + A to them. mark previous as A and done
If a new cost is ever cheaper than an old cost, update the cost and the prev
If all neighbors are done, the node is done

remove edge_penalty
Remove */