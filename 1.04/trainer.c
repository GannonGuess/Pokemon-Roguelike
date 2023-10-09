#include "trainer.h"


// comparator for finding smallest moveTime
static int32_t time_cmp(const void *key, const void *with) {
    const actor_time_t *cost_1 = (const actor_time_t *)key;
    const actor_time_t *cost_2 = (const actor_time_t *)with;
    if(cost_1->moveTime != cost_2->moveTime) {
        return cost_1->moveTime - cost_2->moveTime;
    }
    return cost_1->seqNum - cost_2->seqNum; //if move times are identical, get based off seqNum
}

/** Creates a new actor populates it, placing it into the cmap for future reference
 * 
*/
int newActor(actorMap *cmap, int y, int x, int n) {
    actor *a;
    if(!(a = malloc(sizeof(a)))) {
        printf("Failed to allocate storage for new actor");
    }
    struct npc *npc;
    if(!(npc = malloc(sizeof(struct npc)))) {
        printf("Failed to allocate storage for new npc"); 
    }
    a->npc = npc;
    
    cmap->actorMap[y][x] = a;
    a->npc->npc_x = x;
    a->npc->npc_y = y;
    cmap->actorMap[y][x]->seqNum = n + 1;
    cmap->actorMap[y][x]->npc = npc;
    cmap->actorMap[y][x]->moveTime = 0;
    return 0;
}

/**
 * Function to generate a given number of trainers and put them into the cmap
*/
int generate_trainers(int numTrainers, actorMap *cmap, struct room *r) {
    int n;
    for(n = 0; n < numTrainers; n++) {
        int x = rand() % 78; // select x and y value for location
        int y = rand() % 19;
        char currentTile = r->tiles[y + 1][x + 1]; //find tile type of where actor will be placed
        while(cmap->actorMap[y][x] || currentTile == '~' || currentTile == '%' || currentTile == '4') { //if cannot be placed there or actor is already present
            x = rand() % 78;
            y = rand() % 19;
            currentTile = r->tiles[y + 1][x + 1];
        }
        
        newActor(cmap, y, x, n); // create new actor based off information

        
        if(n == 0) { // first actor added is a hiker
            cmap->actorMap[y][x]->display = 'h';
        }
        else if(n == 1) { // second actor added is a rival
            cmap->actorMap[y][x]->display = 'r';
        }
        else { // randomly choose other actors
            int type = rand() % 2;
            switch(type) {
                case 0:
                    cmap->actorMap[y][x]->display = 'h'; 
                    break;
                case 1:
                    cmap->actorMap[y][x]->display = 'r';
                    break;
            }
        }
    }
    return 0;
}

/** Creates an actor_time_t node to be put into the prioty queue
 * 
*/
actor_time_t* createActorTime(int moveTime, int seqNum, char display, int x, int y) {
    actor_time_t* actor_time = (actor_time_t*)malloc(sizeof(actor_time_t)); //allocate space
    if(actor_time == NULL) {
        printf("failed to allocate space for new actor");
        return NULL;
    }
    //fill in values
    actor_time->display = display;
    actor_time->moveTime = moveTime;
    actor_time->seqNum = seqNum;
    actor_time->x = x;
    actor_time->y = y;
    //return the actor_time_t
    return actor_time;
}

/**
 * computes movement of actor_time_t using the provided distance map, room, and time
*/
void max_grad_move(actor_time_t *a, distanceMap *map, int currentTime, struct room *r, actorMap *cmap) {
    int i, j;
    int moveToX = a->x; //actor wont move if not updated
    int moveToY = a->y;
    int lowest = map->distances[a->y][a->x]; // looking for distance lower than actors current position
    int moved = 0; // check if actor has valid moved
    for(j = -1; j <= 1; j++) { //check cells around the actor
        for(i = -1; i <= 1; i++) {
            if(a->x + i < 0 || a->x + i > 77 || a->y + j < 0 || a->y + j > 18 || (i == 0 && j == 0)) { // continue if checking neighbor out of bounds nodes current tile
                continue;
            }
            if(cmap->actorMap[a->y + j][a->x + i]) { // if neighbor is occupied, dont move there
                continue;
            }
            char neighborTile = r->tiles[a->y + j + 1][a->x + i + 1]; // check neighbor tiles type
            if(map->distances[a->y + j][a->x + i] == INT_MAX || neighborTile == WATER || neighborTile == TREE || neighborTile == BOULDER) {
                //dont check to move into a tile that has a value of INTM_MAX or a type that cannot be traverseds
                continue;
            }
            if(map->distances[a->y + j][a->x + i] <= lowest) { // <= will increase runtime marginally but prevent actors form prefering to move up
                lowest = map->distances[a->y + j][a->x + i];
                moveToX = a->x + i;
                moveToY = a->y + j;
                moved = 1;
            }
        }
    }
    
    char moveToTile = r->tiles[moveToY +1][moveToX + 1]; // get tile type being moved into
    int moveTimeCost = calculate_cost(moveToTile, a->display); // calculate cost of the tile being moved into

    if(moved) { // only update actor position if it is moving
        actor *tmp = cmap->actorMap[a->y][a->x];
        cmap->actorMap[moveToY][moveToX] = tmp;
        cmap->actorMap[a->y][a->x] = NULL; // remove old cost map location
        a->x = moveToX;
        a->y = moveToY;
    }
    //update costs of actor passed in for its requeuing
    cmap->actorMap[moveToY][moveToX]->moveTime = currentTime + moveTimeCost;
    a->moveTime = currentTime + moveTimeCost;

}

/**
 * Main loop for actor movement
*/
static void move_loop(int numTrainers, actorMap *cmap, distanceMap *hikerMap, distanceMap *rivalMap, struct room *r) {
    heap_t actorHeap; // heap for storing actors
    actor_time_t *actor_store[numTrainers + 1], *a; // array of actor_time_t and an actor reference
    int numActors = 0;
    int currentTime = 0; // time starts at 0
    heap_init(&actorHeap, time_cmp, NULL); // initilize heap

    int x, y;
    for(y = 0; y < 19; y++) {
        for(x = 0; x < 78; x++) {
            if(cmap->actorMap[y][x]) { // if there is an actor at cmap location, add it to actor_store
                actor_store[numActors++] = createActorTime(cmap->actorMap[y][x]->moveTime, cmap->actorMap[y][x]->seqNum, cmap->actorMap[y][x]->display, x, y);
            }
        }
    }
    for(x = 0; x < numTrainers + 1; x++) { // insert actors from actor_store into heap
        heap_insert(&actorHeap, &actor_store[x]->moveTime);
    }

    while((a = heap_remove_min(&actorHeap))) { // remove lowest time actor from heap
        currentTime = a->moveTime; // current time is the time of the most recently dequeued node
        if(a->display == 'h') { // if selected actor is a hiker, compute based off that
            max_grad_move(a, hikerMap, currentTime, r, cmap);
        }
        if(a->display == 'r') { // for rival
            max_grad_move(a, rivalMap, currentTime, r, cmap); 
        }
        
        if(a->display == '@') { // if Player is selected, increment its time and print the map
            a->moveTime = currentTime + 10;
            room_print(r, cmap);
            usleep(250000); // 4fps
        }
        heap_insert(&actorHeap, &a->moveTime); // reinsert node with new moveTime back into heap
    }
}

