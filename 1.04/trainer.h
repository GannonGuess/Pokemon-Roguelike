#ifndef TRAINER_C
#define TRAINER_C

struct npc {
    int npc_x;
    int npc_y;
};

typedef struct {
    struct player *pc;
    struct npc *npc;
    int seqNum;
    char display;
}actor;

typedef struct {
    actor *actorMap[19][78];
} actorMap;




#endif
