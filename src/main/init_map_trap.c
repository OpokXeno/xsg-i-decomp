#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/init_map_trap", InitMapTrap);

INCLUDE_ASM("asm/main/nonmatchings/init_map_trap", MAP_updateUnitTrap);

#include "shared.h"

typedef struct {
    unsigned long long xy;
    float z;
    float w;
} TrapPosition;

typedef struct {
    unsigned char unmodeled_00[0x24];
    float damageRadius;
} TrapDamageProperties;

typedef struct {
    unsigned int flags;
    unsigned char unmodeled_04[0x0c];
    TrapPosition position;
    unsigned long long unmodeled_20;
    unsigned char unmodeled_28[0x178];
    TrapDamageProperties trap;
} TrapMapUnit;

typedef struct {
    unsigned int flags;
    unsigned char unmodeled_04[0x0c];
    TrapPosition position;
    unsigned char unmodeled_20[0xa50];
} TrapDamageActor;

typedef struct {
    unsigned char unmodeled_00[0x20];
    TrapPosition trapPosition;
    unsigned long long unmodeled_30;
    unsigned char unmodeled_38[0x3878];
} TrapEnemyWorkEntry;

typedef struct {
    volatile unsigned int red;
    volatile unsigned int green;
    volatile unsigned int blue;
    volatile unsigned int mode;
    TrapPosition sphere;
} TrapCollisionRecord;

extern TrapDamageActor actor[64];
extern TrapEnemyWorkEntry enepc[16];
extern signed char collflg;
extern signed char printflg;
extern const char D_004CA470[];
extern const char D_004CA488[];
extern const char D_004CA4A0[];
extern const char D_004CA4B0[];
extern const char D_004CA4C0[];
extern float CheckDist3D(const TrapPosition *, const TrapPosition *);
extern int printf(const char *, ...);

int GivesEnemyDamage(TrapMapUnit *unit)
{
    /* The collflg branch writes a color, mode and sphere to a local record;
     * its emitted stores remain present even though no reader is evidenced. */
    TrapCollisionRecord collisionRecord;
    volatile float *collisionRadius = &collisionRecord.sphere.w;
    int actorIndex;
    TrapDamageActor *target;
    TrapDamageProperties *trap = &unit->trap;

    if (unit->flags & 0x00200000u) {
        return 0;
    }
    if (collflg) {
        collisionRecord.blue = 255;
        collisionRecord.sphere = unit->position;
        *collisionRadius = trap->damageRadius;
        collisionRecord.mode = 128;
        collisionRecord.red = 255;
        collisionRecord.green = 255;
    }
    for (actorIndex = 0; actorIndex < 64; actorIndex++) {
        target = &actor[actorIndex];
        if (target->flags & 8) {
            continue;
        }
        if (!(target->flags & 0x00010000u)) {
            continue;
        }
        if (target->flags & 0x00800000u) {
            continue;
        }
        enepc[actorIndex].trapPosition = unit->position;
        if (!(CheckDist3D(&actor[actorIndex].position, &unit->position) < trap->damageRadius)) {
            continue;
        }
        target->flags |= 0x00800000u;
        if (unit->flags & 0x00400000u) {
            target->flags |= 0x02000000u;
            if (printflg) {
                printf(D_004CA470, actorIndex);
            }
        } else if (unit->flags & 0x00800000u) {
            target->flags |= 0x01000000u;
            if (printflg) {
                printf(D_004CA488, actorIndex);
            }
        } else if (unit->flags & 0x01000000u) {
            target->flags |= 0x04000000u;
            if (printflg) {
                printf(D_004CA4A0, actorIndex);
            }
        } else if (unit->flags & 0x02000000u) {
            if (printflg) {
                printf(D_004CA4B0, actorIndex);
            }
        } else if (unit->flags & 0x04000000u) {
            target->flags |= 0x08000000u;
            if (printflg) {
                printf(D_004CA4C0, actorIndex);
            }
        }
    }
    return 0;
}
