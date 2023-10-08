#ifndef TRAINER_C
#define TRAINER_C

#include "heap.h"

struct npc {
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

typedef struct {
    actor *actorMap[19][78];
} actorMap;

typedef struct actor_time {
    // struct player *pc;
    // struct npc *npc;
    int moveTime;
    int seqNum;
    char display;
    int x; // value between 0 and 77
    int y; // value between 0 and 18
} actor_time_t;



#endif
