#include "trainer.h"
#include "world.h"
#include "dijkstra.h"
#include <limits.h>

static int32_t time_cmp(const void *key, const void *with) {
    const actor_time_t *cost_1 = (const actor_time_t *)key;
    const actor_time_t *cost_2 = (const actor_time_t *)with;
    if(cost_1->moveTime != cost_2->moveTime) {
        return cost_1->moveTime - cost_2->moveTime;
    }
    return cost_1->seqNum - cost_2->seqNum;
}

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
    //all npcs at same address in memory may be an issue
    return 0;
}

int generate_trainers(int numTrainers, actorMap *cmap, struct room *r) {
    int n;
    for(n = 0; n < numTrainers; n++) {
        int x = rand() % 78; // select x and y value for location
        int y = rand() % 19;
        char currentTile = r->tiles[y + 1][x + 1]; 
        while(cmap->actorMap[y][x] || currentTile == '~' || currentTile == '%' || currentTile == '4') {
            x = rand() % 78;
            y = rand() % 19;
            printf("tile:%c\n", currentTile);
            currentTile = r->tiles[y + 1][x + 1];
        }
        printf("%d %d\n", y, x);
        
        newActor(cmap, y, x, n);

        
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

actor_time_t* createActorTime(struct player *pc, struct npc *npc, int moveTime, int seqNum, char display, int x, int y) {
    actor_time_t* actor_time = (actor_time_t*)malloc(sizeof(actor_time_t));
    if(actor_time == NULL) {
        printf("failed to allocate space for new actor");
        return NULL;
    }


    // actor_time->pc = pc;
    // actor_time->npc = npc;
    actor_time->display = display;
    actor_time->moveTime = moveTime;
    actor_time->seqNum = seqNum;
    actor_time->x = x;
    actor_time->y = y;

    return actor_time;
}

void max_grad_move(actor_time_t *a, distanceMap *map, int currentTime, struct room *r, actorMap *cmap) {
    printf("moving: seqNum: %d cost: %d dis: %c %d %d\n", a->seqNum, a->moveTime, a->display, a->x, a->y);
    int i, j;
    int moveToX = a->x; 
    int moveToY = a->y;
    int lowest = map->distances[a->y][a->x];
    int moved = 0;
    for(j = -1; j <= 1; j++) { //check cells around the actor
        for(i = -1; i <= 1; i++) {
            if(a->x + i < 0 || a->x + i > 77 || a->y + j < 0 || a->y + j > 18 || (i == 0 && j == 0)) { // continue if checking neighbor out of bounds nodes current tile
                printf("bounds\n");
                continue;
            }
            printf("%d ", map->distances[a->y +j][a->x +i]);
            if(cmap->actorMap[a->y + j][a->x + i]) { // if neighbor is occupied, dont move there
                printf("occupied\n");
                continue;
            }
            char neighborTile = r->tiles[a->y + j + 1][a->x + i + 1];
            if(map->distances[a->y + j][a->x + i] == INT_MAX || neighborTile == WATER || neighborTile == TREE || neighborTile == BOULDER) {
                continue;
            }
            //printf("%d\n", map->distances[a->y + j][a->x + i]);
            if(map->distances[a->y + j][a->x + i] <= lowest) { // <= will increase runtime marginally but prevent actors form prefering to move up
                lowest = map->distances[a->y + j][a->x + i];
                moveToX = a->x + i;
                moveToY = a->y + j;
                moved = 1;
            }
        }
        printf("\n");
    }
    //printf("move to: %d %d\n", moveToX, moveToY);
    
    char moveToTile = r->tiles[moveToY +1][moveToX + 1];
    int moveTimeCost = calculate_cost(moveToTile, a->display);
    // if(moveTimeCost == INT_MAX){
    //     printf("\n\nWEEWOOEOWOWOWOWOOW\n\n");
    // }
    //printf("cost to move to tile: %d + %d\n", moveTimeCost, currentTime);

    if(moved) {
    //move actor in cmap to new position
    actor *tmp = cmap->actorMap[a->y][a->x];
    cmap->actorMap[moveToY][moveToX] = tmp;
    cmap->actorMap[moveToY][moveToX]->moveTime = currentTime + moveTimeCost;
    cmap->actorMap[a->y][a->x] = NULL; // remove old cost map location
    a->x = moveToX;
    a->y = moveToY;
    
    printf("moving to: seqNum: %d cost: %d dis: %c %d %d %c\n", a->seqNum, a->moveTime, a->display, a->x, a->y, moveToTile);
    }
    a->moveTime = currentTime + moveTimeCost;

}


static void move_loop(int numTrainers, actorMap *cmap, distanceMap *hikerMap, distanceMap *rivalMap, struct room *r) { // should return the last actor to move
    heap_t actorHeap;
    actor_time_t *actor_store[numTrainers + 1], *a;
    int numActors = 0;
    int currentTime = 0; 
    heap_init(&actorHeap, time_cmp, NULL);

    int x, y;
    for(y = 0; y < 19; y++) {
        for(x = 0; x < 78; x++) {
            if(cmap->actorMap[y][x]) {
               // printf("%d %d \n");
                actor_store[numActors++] = createActorTime(NULL, NULL, cmap->actorMap[y][x]->moveTime, cmap->actorMap[y][x]->seqNum, cmap->actorMap[y][x]->display, x, y);
            }
        }
    }
    for(x = 0; x < numTrainers + 1; x++) {
        //printf("%d \n", actor_store[x]->seqNum);
        heap_insert(&actorHeap, &actor_store[x]->moveTime);
    }

    while((a = heap_remove_min(&actorHeap))) {
        currentTime = a->moveTime;
        if(a->display == 'h') {
            max_grad_move(a, hikerMap, currentTime, r, cmap);
        }
        if(a->display == 'r') {
            max_grad_move(a, rivalMap, currentTime, r, cmap); 
        }
        
        if(a->display == '@') {
            a->moveTime = currentTime + 10;
             
            room_print(r, cmap);
            usleep(250000);
        }
        //printf("after: seqNum: %d cost: %d dis: %c %d %d\n", a->seqNum, a->moveTime, a->display, a->x, a->y);
        heap_insert(&actorHeap, &a->moveTime);
        // currentTime = a->moveTime
    }
}

