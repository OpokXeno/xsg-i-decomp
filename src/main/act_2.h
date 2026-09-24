/*
 * TU-local declarations of main/tu255 (src/main/act_2.c).
 */

#ifndef SRC_MAIN_ACT_2_H
#define SRC_MAIN_ACT_2_H

#include "shared.h"

/*
 * The engine's actor record. `actor` is the 64-entry array at main
 * 0x0043c1e0 with a 0xa70 stride (readelf: 0043c1e0 OBJECT GLOBAL actor
 * size 0x29c00 = 64 * 0xa70). src/main/near_dir.h, src/main/chr.h and
 * src/main/set_motion.h each carry the same recovered +0x00..+0x70 head,
 * with the field-by-field evidence for it; it is repeated verbatim here.
 *
 * This TU extends the type past +0x70 with three more evidenced members and
 * the unmodeled spans between them:
 *
 *   +0x71c animData      ACT_animCheckData reads it and passes it unchanged
 *                        to FCV2_checkData (main:0x00306d88, lw a0,0x71c(a0);
 *                        tail call at 0x00306d90). FCV2_checkData itself
 *                        dereferences the pointer to compare a signature word
 *                        (main:0x0030d8c8, lw v1,0(a0), against 0x00564346
 *                        built at 0x0030d8cc/0x0030d8d0 - the bytes "FCV\0"
 *                        read little-endian), so this is a pointer.
 *   +0x720 animUserData  returned unchanged by ACT_animGetUserData
 *                        (main:0x00306d7c, lw v0,0x720(a0)).
 *   +0x7fc moveElementId the joint move-element index
 *                        ACT_jointGetMoveElementID caches. main:0x00306a30
 *                        reads it and the bgezl at 0x00306a34 returns it
 *                        unresolved once it is no longer negative;
 *                        0x00306a58 stores 0 for an absent move.
 *   +0x8d8 move          the actor's move (motion) resource pointer.
 *                        ACT_jointGetMoveElementID reads it (main:0x00306a3c,
 *                        lw v0,0x8d8(s0)) and passes it on to
 *                        JNT_getMoveElement, which dereferences its argument
 *                        (main:0x00313b50, addiu v1,a0,16; lhu a3,8(a0)), so
 *                        this is a pointer.
 *
 * Nothing between +0x70 and +0x71c, between +0x724 and +0x7fc, or between
 * +0x800 and +0x8d8 is recovered, so those spans stay unmodeled_XX
 * (docs/naming.md) rather than invented members.
 */

/*
 * +0x8dc animPackTables: eight per-category animation pack-table pointers,
 * right after move. ACT_animGetData selects one with bits 8-10 of its packed
 * id (main:0x003081c8, andi a1,a1,0x7ff; srl v0,a1,0x8; sll v0,v0,0x2; lw
 * a0,0x8dc(v0)) and tail calls PACK_getEntry(table, id & 0xff) on it, so
 * each slot is a pointer.
 */

/*
 * +0x6f0 animSlot: ACT_animGetCurrent takes its address (main:0x003081fc,
 * addiu a0,v1,0x6F0) and hands it to ANM_getEntry as the entry's owning
 * slot, so it is an embedded record, not a pointer. Only its +0x14 halfword
 * is read (main:0x00308200, lhu v0,20(a0)): bits 8-10 of that packed value
 * select an animPackTables slot the same way ACT_animGetData's dataId does
 * (srl v0,v0,0x6; andi v0,v0,0x1c is (currentDataId>>8&7)<<2 folded into one
 * shift), so it caches a dataId like the one ACT_animGetData is called with.
 * Nothing else in the slot is evidenced.
 */
typedef struct ActorAnimSlot {
    unsigned char unmodeled_0[0x14];
    unsigned short currentDataId;
} ActorAnimSlot;

typedef struct Actor {
    u32 flags;
    void (*update)(struct Actor *actor);
    void (*draw)(struct Actor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_70[0x6f0 - 0x70];
    ActorAnimSlot animSlot;
    unsigned char unmodeled_706[0x71c - 0x706];
    void *animData;
    void *animUserData;
    unsigned char unmodeled_724[0x7fc - 0x724];
    int moveElementId;
    unsigned char unmodeled_800[0x8d8 - 0x800];
    void *move;
    void *animPackTables[8];
} Actor;

int ACT_jointGetMoveElementID(Actor *actor);

void ACT_resetArms(Actor *actor, Actor *other, int acc_id);

int ACT_setFace(Actor *actor, Actor *parent, int faceId);

void *ACT_animGetUserData(Actor *actor);

int ACT_animCheckData(Actor *actor);

void ACT_initMTNResource(void);

void ACT_resourceInit(void);

void ACT_animGetCurrent(Actor *actor);

#endif /* SRC_MAIN_ACT_2_H */
