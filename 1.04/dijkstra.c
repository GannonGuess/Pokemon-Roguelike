#include "dijkstra.h"
#include "world.h"

static int32_t path_cmp(const void *key, const void *with) {
    return ((cell_cost_t *) key)->cost - ((cell_cost_t *) with)->cost;
}

int calculate_cost(char current_type, char npc_type) { // find the cost of the center nodes type
    switch(npc_type) { // switch based on npc
        case 'h':
            switch(current_type) { // switch based on center node type
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
            switch(current_type) {
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
                default:
                    return INT_MAX;
            }
    }
    return INT_MAX;
}


static distanceMap dijkstra(struct room *r, int pc_x, int pc_y, char npc_type) {
    static cell_cost_t costs[21][80], *c; // initilize array for holding cell costs
    distanceMap *dist; // inintilize array for storing final distances
    if(!(dist = malloc(sizeof(distanceMap)))) {
        printf("Failed to allocate space for new distance map");
    }
    static uint32_t initilized = 0;
    heap_t h;
    int x, y;
    if(!initilized) { // initilize cost array if not already initilized
        for(y = 0; y < 21; y++) {
            for(x = 0; x < 80; x++) {
                costs[y][x].pos[dim_y] = y;
                costs[y][x].pos[dim_x] = x;
            }
        }
        initilized = 1;
    }

    heap_init(&h, path_cmp, NULL);
    
    for (y = 0; y < 21; y++) { // fill cost array with max value
        for (x = 0; x < 80; x++) {
            costs[y][x].cost = INT_MAX;
        }
    }

    costs[pc_y][pc_x].cost = 0; // set player position to cost 0
    costs[pc_y][pc_x].hn = heap_insert(&h, &costs[pc_y][pc_x]); // insert player position into heap

    char current_type; // stores center node type
    int tile_cost; // cost of center node's type

    while((c = heap_remove_min(&h))) { // set c = to h's min node and remove the min from the array
        int curr_x = c->pos[dim_x]; // get center node x pos
        int curr_y = c->pos[dim_y]; // get center node y pos
        for(y = -1; y < 2; y++) {
            for(x = -1; x < 2; x++) { // check c's neighbors
                if(x == 0 && y == 0) { // skip center node
                    continue;
                }
                if(curr_x + x < 0 || curr_x + x > 79 || curr_y + y < 0 || curr_y + y > 20) { // skip if at border
                    continue;
                }
                
                current_type = r->tiles[curr_y + 1][curr_x + 1]; // get type of center node
                if(current_type == '~' || current_type == '?' || current_type == '%') { // skip if center node cannot be moved to
                    continue;
                }
                int current_cost = c->cost; // find cost of center node
                tile_cost = calculate_cost(current_type, npc_type); // get cost of central node

                if(tile_cost + current_cost < costs[curr_y + y][curr_x + x].cost && tile_cost + current_cost > 0) { // update cost if not sum not negative and is less than current stored cost
                    costs[curr_y + y][curr_x + x].cost = tile_cost + current_cost; // update cost of neighbor
                    heap_insert(&h, &costs[curr_y + y][curr_x + x]); // insert the neighbor into heap
                }
            }
        }
    }

    char check; // used to check type of tile
    for(y = 0; y < 19; y++) {
        for(x = 0; x < 78; x++) { // loop through cost array
            check = r->tiles[costs[y][x].pos[dim_y] + 1][costs[y][x].pos[dim_x] + 1];
            if(check == '~' || check == '?' || check == '%') { // skip printing of terrain that cannot be traversed
                dist->distances[y][x] = INT_MAX;
                //printf("   ");
            }
            else {
                dist->distances[y][x] = costs[y][x].cost;
                //if(dist[y][x] != INT_MAX) { //Removeable?
                //printf("%.2d ", ((dist->distances[y][x]) % 100));
                //}
            }
        }
        //printf("\n");
    }
    return *dist;
}


/* Notes
Starting node has cost of zero. all else infinity.
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