#ifndef TRAINER_C
#define TRAINER_C

#include "heap.h"
#include "dijkstra.h"
#include <limits.h>

struct npc { // stores npc info
    int npc_x;
    int npc_y;
};

typedef struct {
    struct player *pc;
    struct npc *npc;
    int seqNum;
    int moveTime;
    char display;
}actor;

typedef struct { // stores map of actors
    actor *actorMap[19][78];
} actorMap;

typedef struct actor_time {
    int moveTime;
    int seqNum;
    char display;
    int x; // value between 0 and 77
    int y; // value between 0 and 18
} actor_time_t;

#include "world.h"

static int32_t time_cmp(const void *key, const void *with);
int newActor(actorMap *cmap, int y, int x, int n);
int generate_trainers(int numTrainers, actorMap *cmap, struct room *r);
actor_time_t* createActorTime(int moveTime, int seqNum, char display, int x, int y);
void max_grad_move(actor_time_t *a, distanceMap *map, int currentTime, struct room *r, actorMap *cmap);
static void move_loop(int numTrainers, actorMap *cmap, distanceMap *hikerMap, distanceMap *rivalMap, struct room *r);


#endif
