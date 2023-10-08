#include "trainer.h"
#include "world.h"

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

actor_time_t* createActorTime(int moveTime, int seqNum) {
    actor_time_t* actor_time = (actor_time_t*)malloc(sizeof(actor_time_t));
    if(actor_time == NULL) {
        printf("failed to allocate space for new actor");
        return NULL;
    }

    actor_time->moveTime = moveTime;
    actor_time->seqNum = seqNum;

    return actor_time;
}


static int move(int numTrainers, actorMap *cmap, distanceMap *hikerMap, distanceMap *rivalMap) { // should return the last actor to move
    heap_t actorHeap;
    actor_time_t *actor_store[numTrainers + 1], *a;
    int numActors = 0;
    

    heap_init(&actorHeap, time_cmp, NULL);
    int x, y;
    for(y = 0; y < 19; y++) {
        for(x = 0; x < 78; x++) {
            if(cmap->actorMap[y][x]) {
                actor_store[numActors++] = createActorTime(cmap->actorMap[y][x]->moveTime, cmap->actorMap[y][x]->seqNum);
            }
        }
    }
    for(x = 0; x < numTrainers + 1; x++) {
        printf("%d \n", actor_store[x]->seqNum);
        heap_insert(&actorHeap, &actor_store[x]->moveTime);
    }

    while((a = heap_remove_min(&actorHeap))) {
        if(a->seqNum != 0) {
            printf("seqNum: %d cost: %d\n", a->seqNum, a->moveTime);
        } 
    }

    
    
    
    // if(!initilized) {
    //     for(y = 0; y < 19; y++) {
    //         for(x = 0; x < 78; x++) {
    //             if(cmap->actorMap[y][x]) {
    //                 printf("dis: %c seqNum: %d moveTime: %d\n", cmap->actorMap[y][x]->display, cmap->actorMap[y][x]->seqNum, cmap->actorMap[y][x]->moveTime);
    //             }
    //         }
    //     } 
    // }

    // for(y = 0; y < 19; y++) {
    //     for(x = 0; x < 78; x++) {
    //         if(cmap->actorMap[y][x]) {
    //             printf("dis: %c seqNum: %d moveTime: %d\n", cmap->actorMap[y][x]->display, cmap->actorMap[y][x]->seqNum, cmap->actorMap[y][x]->moveTime);
    //         }
    //     }
    // }


    return 0;
}

