#ifndef INCLUDE_OV01_ACTOR_H
#define INCLUDE_OV01_ACTOR_H

#include "ov01/battle_init.h"

/*
 * OV01's view of the actor record. include/main/act_1.h defines the same
 * record for MAIN; a TU includes one of the two.
 */
/* A UnitVector is a Vector4 whose 8-byte halves can be copied with ld/sd;
 * the components are read through `vector`, the whole is copied as `pair`. */
typedef union UnitVector {
    Vector4 vector;
    u64 pair[2];
} UnitVector;

/*
 * The battle actor object behind a unit: the ObjectTask `work` slot
 * (BattleUnit.work) and the unit's motion actor (BattleUnit.actor) are both
 * this record, and the weapon and model actors (BattleUnit.weapon) keep the
 * same head with model storage at +0x8D0. The members follow the offsets the
 * code reaches, one group per evidenced reader.
 */
typedef struct Actor {
    u32 flags;                                /* +0x00: ACTOR_FLAG_* */
    void (*update)(struct Actor *actor);      /* +0x04 */
    void (*draw)(struct Actor *actor);        /* +0x08 */
    u32 quadword_alignment_gap;               /* +0x0C */
    Vector4 position;                         /* +0x10 */
    Vector4 previous_position;                /* +0x20 */
    Vector4 velocity;                         /* +0x30 */
    Vector4 acceleration;                     /* +0x40 */
    Vector4 rotation;                         /* +0x50 */
    Vector4 scale;                            /* +0x60 */
    int runFrameCounter;                      /* +0x70: unitCmdRun tests it modulo 3
                                               * to space out its footstep decoEffCall. */
    int moveState;                            /* +0x74: unitCmdMove's own step; 0 issues
                                               * the move and advances to 1, which waits
                                               * for unitCmdJump/unitCmdWarp to report
                                               * arrival before calling unitCmdMoveNext. */
    int movePhase;                            /* +0x78: reset to 0 by unitCmdMove when a
                                               * move starts; unitCmdJump/unitCmdWarp
                                               * advance it through their phases. */
    int movePhaseTimer;                       /* +0x7C: unitCmdWarp zeroes it each time
                                               * it advances movePhase; unitTelIn/unitTelOut
                                               * use it as their first-step flag. */
    BattleUnit *targetUnit;                   /* +0x80: unitCmdDstSet's mode-0 path reads
                                               * the command target's CalcUnitParam/table
                                               * position through it. */
    struct CalcUnitParam *up;                 /* +0x84: calcUPGet reads the unit's
                                               * CalcUnitParam through the work actor. */
    unsigned char unmodeled_88[0x90 - 0x88];
    UnitVector cmdTarget;                     /* +0x90: unitCmdDirSet reads x/z against
                                               * the unit's actor->position; unitCmdDivSet
                                               * blends x/z toward that position. */
    unsigned char unmodeled_a0[0xC0 - 0xA0];
    /* +0xC0 opens the kind-specific block (the menu model actor's control
     * record in include/main/act_1.h, ACTOR_ARGS on a Chr peer). A battle unit's
     * motion actor keeps its transparency here; unitTelIn/unitTelOut reload
     * BattleUnit.actor after storing it, which a plain float member lets the
     * compiler merge into the first load (see unit_cmd.c). */
    union {
        float transparency;                   /* +0xC0: unitActdraw draws translucent below 1.0 */
        unsigned char args[0xD0 - 0xC0];
    };
    float posSnapX;                           /* +0xD0: unitCmdJump's arrival check */
    float hopLength;                          /* +0xD4: unitCmdMoveChaJump2 stores the
                                               * command distance divided by 3 and reloads
                                               * it to aim each intermediate hop. */
    float posSnapZ;                           /* +0xD8 */
    unsigned char unmodeled_dc[0xE0 - 0xDC];
    int motionFrame;                          /* +0xE0: unitActupdate advances it before
                                               * ACT_updateMotion */
    unsigned char unmodeled_e4[0xF0 - 0xE4];
    int motion;                               /* +0xF0: unitMotGet returns it plus one */
    float moveOriginY;                        /* +0xF4: unitCmdRun/unitCmdJump save
                                               * cmdTarget's y/z here around a stand or
                                               * attack phase. */
    float moveOriginZ;                        /* +0xF8 */
    unsigned char unmodeled_fc[0x100 - 0xFC];
    void *motionPackEntry[4];                 /* +0x100: dataMotAdrSet stores a motion
                                               * pack's second entry here by motion id */
    unsigned char unmodeled_110[0x698 - 0x110];
    int weaponLoadInfo;                       /* +0x698: dataUnitFileLoadWep logs it for
                                               * the unit's motion actor */
    unsigned char unmodeled_69c[0x6F0 - 0x69C];
    int moveFlags;                            /* +0x6F0: unitCmdRun/Hover/Float/Jump test
                                               * bit 0x1000 to detect the move's arrival. */
    float moveElapsed;                        /* +0x6F4: unitCmdJump's frame count source */
    float moveStepRate;                       /* +0x6F8: per-frame interpolation rate */
    unsigned char unmodeled_6fc[0x714 - 0x6FC];
    float interpTime;                         /* +0x714: unitSetInterpTime's write target */
    unsigned char unmodeled_718[0x760 - 0x718];
    float floatStepX;                         /* +0x760: unitCmdFloat adds it to the
                                               * motion actor's position.x each step. */
    unsigned char unmodeled_764[0x768 - 0x764];
    float floatStepZ;                         /* +0x768 */
    unsigned char unmodeled_76c[0x8D0 - 0x76C];
    /* +0x8D0: the model storage of a weapon, model or motion actor.
     * dataMotAdrSet's address needs the array form: the original adds the slot
     * constant to the scaled index before the actor base, while this array adds
     * the base first. This TU's loaders store file->animationAdr in the MATRIX
     * slot and file->textureAdr in MOVE. */
    void *resource[11];                       /* +0x8D0 */
    unsigned char unmodeled_8fc[0x9A0 - 0x8FC];
    unsigned int drawEnvFlags;                /* +0x9A0: the draw environment's flags;
                                               * unitClipSet sets and clears 0x200 (the
                                               * clipping bit), dataUnitFileLoadWep sets
                                               * 0x200 on a weapon actor. */
    unsigned int renderLevel;                 /* +0x9A4: unitCreate sets its 0x80 and 0x200 bits */
    unsigned char unmodeled_9a8[0x9D0 - 0x9A8];
    float cameraCullDistance;                 /* +0x9D0: unitCreate sets it for character 0xAB */
} Actor;

#endif /* INCLUDE_OV01_ACTOR_H */
