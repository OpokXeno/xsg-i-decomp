#ifndef INCLUDE_MAIN_RES_GET_PATH_H
#define INCLUDE_MAIN_RES_GET_PATH_H

#include "shared.h"

/*
 * The resource record RES_loadFile hands back for an actor (its `resource`
 * pointer, see struct Actor in src/main/res_get_path.c). Two views share the
 * first words of the record:
 *   - ACT_loadResource (src/main/act_2.c) reads the model, matrix and move
 *     pointers at +0, +4 and +8;
 *   - command_loadmot (src/main/res_get_path.c) writes and RES_GetFootStepNo
 *     reads the footstep number byte at +6, which lies inside the matrices
 *     word, so the two views are a union of the same bytes.
 */
typedef struct ActorResource {
    union {
        struct {
            void *model;    /* +0x0 */
            void *matrices; /* +0x4 */
            void *move;     /* +0x8 */
        };
        struct {
            unsigned char unmodeled_000[6];
            u8 foot_step_no; /* +0x6 */
        };
    };
} ActorResource;

/*
 * One entry of the runtime resource-block table (GameResource, 128 entries):
 * address/size describe the block's extent and handle is the random token
 * GameResourceAlloc (main:0x0024a9d8) assigns with xglSRand(); both it and
 * GameResourceReset (main:0x0024d8e8) clear it to 0. state is -1 for a free
 * block and 9 once GameResourceAlloc marks one in use.
 */
typedef struct GameResourceEntry {
    unsigned char *address;         /* +0x0 */
    int size;                       /* +0x4 */
    int handle;                     /* +0x8 */
    int state;                      /* +0xC, -1 = free */
} GameResourceEntry;

unsigned char *GameResourceWorkAlloc(int size);
void GameResourceWorkReload(void);

/* Functions and data used by other translation units. */
extern unsigned char ModelPath;
int RES_GetEnemySeBank(int sound_id);
void GameResourceReset(int slot);
void RES_GetMdlFileName(char *destination, int selector);
int RES_GetEnemySeType(int sound_id);
void GameResourceLoad(int scene_id);
extern void *GameResourceAlloc(int size);

#endif /* INCLUDE_MAIN_RES_GET_PATH_H */
