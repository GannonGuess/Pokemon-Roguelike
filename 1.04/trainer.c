#include "trainer.h"
#include "world.h"

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
            currentTile = r->tiles[y + 1][x + 1];
            printf("tile:%c\n", currentTile);
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




