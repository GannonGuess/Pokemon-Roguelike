#ifndef TRAINER_C
#define TRAINER_C


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
