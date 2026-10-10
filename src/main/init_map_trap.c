#include "common.h"

#include "shared.h"

typedef struct {
    unsigned long long xy;
    float z;
    float w;
} TrapPosition;

typedef struct {
    unsigned char unmodeled_00[4];
    signed char state;
    unsigned char unmodeled_05[0x1f];
    float damageRadius;
    unsigned char unmodeled_28[5];
    signed char windState;
    unsigned char unmodeled_2e[2];
    s16 methodId;
    unsigned char unmodeled_32[0x0a];
    int activeDuration;
    int effectId;
    unsigned int activeFlags;
} TrapDamageProperties;

typedef struct TrapMapUnit TrapMapUnit;

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
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    unsigned int mode;
    TrapPosition sphere;
} TrapCollisionRecord;

extern TrapDamageActor actor[64];

extern TrapEnemyWorkEntry enepc[16];

extern signed char collflg;

extern signed char printflg;

const char D_004CA470[24] = "act[%d] Electric\n";

const char D_004CA488[24] = "act[%d] Explosion\n";

const char D_004CA4A0[16] = "act[%d] Fall\n";

const char D_004CA4B0[16] = "act[%d] Slot\n";

const char D_004CA4C0[16] = "act[%d] Seal\n";

extern float CheckDist3D(const TrapPosition *, const TrapPosition *);

extern int printf(const char *, ...);

#include "main/toolkit.h"

/* The effect API reads complete 16-byte position/orientation vectors and
 * returns the allocated scheduler, or null when allocation fails. */
struct SchedulerState;
extern struct SchedulerState *sefCreateEffectCf(int effectNo,
                                              const Vector4 *position,
                                              const Vector4 *orientation);


typedef struct {
    unsigned char unmodeled_00[0xa9a];
    unsigned char trap_active;
} TrapLinkedObject;

struct TrapMapUnit {
    unsigned int flags;
    void (*update)(TrapMapUnit *);
    unsigned char unmodeled_08[0x08];
    TrapPosition position;
    TrapPosition effectOrientation;
    unsigned char unmodeled_30[0x70];
    unsigned char trapSerial;
    unsigned char unmodeled_a1;
    unsigned char trapState;
    unsigned char unmodeled_a3;
    s16 trapId;
    unsigned char unmodeled_a6[2];
    s16 trapTimer;
    unsigned char unmodeled_aa[0x3e];
    TrapLinkedObject *linkedObject;
    unsigned char unmodeled_ec[4];
    unsigned char unmodeled_f0[0xb0];
    TrapDamageProperties trap;
};

typedef unsigned int TrapGameLoopStateWords[0xa80c];

extern TrapGameLoopStateWords GameLoopState;

extern const char D_004CA448[0x28];

extern char D_004DB728[];

extern void DrawActiveCursol(TrapMapUnit *unit);

extern void UwamonoCommonFunc(TrapMapUnit *unit);

extern void ClearUwamonoEffect(TrapMapUnit *unit);

extern void SetUwaWind(TrapMapUnit *unit);

extern void UwamonoBrokenSe(TrapMapUnit *unit);

extern void Vibration_Set_Strong(int a, int b, int c);

extern void xglSoundEffectNormalID(int soundId, int variant);

extern void CallMethod_I(const char *method_name, int argument1);

extern void EXM_ClearWindStruct();

int GivesEnemyDamage(TrapMapUnit *unit);

INCLUDE_ASM("asm/main/nonmatchings/init_map_trap", InitMapTrap);

void MAP_updateUnitTrap(TrapMapUnit *unit)
{
    TrapDamageProperties *trap = &unit->trap;

    if (trap->activeFlags != 0) {
        DrawActiveCursol(unit);
    }
    UwamonoCommonFunc(unit);
    switch (unit->trapState) {
    case 0: {
        if (unit->linkedObject != 0 && !(unit->flags & 0x00200000u)) {
            if (trap->activeFlags & 1u) {
                unit->linkedObject->trap_active = 1;
            } else {
                unit->linkedObject->trap_active = 0;
            }
        }

        if (trap->state == 1) {
            unsigned int flags;

            if (unit->trapId == 0x7012) {
                unit->update = 0;
                unit->trapId = -1;
                ClearUwamonoEffect(unit);
                return;
            }

            unit->trapState = 3;
            flags = unit->flags;
            if (flags & 0x00400000u) {
                xglSoundEffectNormalID(0x10004, unit->trapSerial + 1);
            } else if (flags & 0x00800000u) {
                xglSoundEffectNormalID(0x10005, unit->trapSerial + 1);
            } else if (flags & 0x01000000u) {
                xglSoundEffectNormalID(0x10003, unit->trapSerial + 1);
            } else if (flags & 0x02000000u) {
                xglSoundEffectNormalID(0x10003, unit->trapSerial + 1);
            } else if (unit->flags & 0x04000000u) {
                xglSoundEffectNormalID(0x1000a, unit->trapSerial + 1);
            } else if (unit->flags & 0x00200000u) {
                UwamonoBrokenSe(unit);
            }

            if (unit->flags & 0x00200000u) {
                Vibration_Set_Strong(0xff, 0x20, 0x1e);
            } else {
                Vibration_Set_Strong(0xff, 0x20, 0x5a);
        }
        sefCreateEffectCf(trap->effectId,
                          (const void *)&unit->position,
                          (const void *)&unit->effectOrientation);
        unit->flags |= 0x00020004;
            ClearUwamonoEffect(unit);
            SetUwaWind(unit);
            if (trap->methodId != -1) {
                CallMethod_I(D_004DB728, trap->methodId);
            }
        }
        if (trap->state == 2) {
            if (unit->trapId == 0x7012) {
                unit->update = 0;
                unit->trapId = -1;
                ClearUwamonoEffect(unit);
                return;
            }
            unit->trapState = 1;
            UwamonoBrokenSe(unit);
            sefCreateEffectCf(0x259,
                              (const void *)&unit->position,
                              (const void *)&unit->effectOrientation);
            unit->flags |= 0x00020004u;
            ClearUwamonoEffect(unit);
            SetUwaWind(unit);
            if (trap->methodId != -1) {
                GameLoopState[7] = 0;
                CallMethod_I(D_004DB728, trap->methodId);
            }
            Vibration_Set_Strong(0xff, 0x20, 0x1e);
        }
        break;
    }

    case 3: {
        unit->trapTimer = (s16)(unit->trapTimer + 1);
        if (unit->trapTimer >= 5) {
            trap->windState = 0;
            EXM_ClearWindStruct();
        }
        if (unit->trapTimer < trap->activeDuration) {
            GivesEnemyDamage(unit);
        } else if (unit->trapTimer > trap->activeDuration) {
            if (printflg != 0) {
                printf(D_004CA448, unit->trapId, unit->trapSerial);
            }
            unit->update = 0;
            unit->trapId = -1;
            EXM_ClearWindStruct();
        }
        break;
    }

    case 1: {
        s16 elapsed;

        elapsed = unit->trapTimer = (s16)(unit->trapTimer + 1);
        if (elapsed >= 5) {
            trap->windState = 0;
            if (printflg != 0) {
                printf(D_004CA448, unit->trapId, unit->trapSerial);
            }
            unit->update = 0;
            unit->trapId = -1;
            EXM_ClearWindStruct();
        }
        break;
    }
    }
}

int GivesEnemyDamage(TrapMapUnit *unit)
{
    /* Prepare the collision sphere and its display color when collision
     * display is enabled. No reader of this local record is evidenced here. */
    TrapCollisionRecord collisionRecord;
    int actorIndex;
    TrapDamageActor *target;
    TrapDamageProperties *trap = &unit->trap;

    if (unit->flags & 0x00200000u) {
        return 0;
    }
    if (collflg) {
        collisionRecord.sphere = unit->position;
        collisionRecord.sphere.w = trap->damageRadius;
        collisionRecord.red = 255;
        collisionRecord.green = 255;
        collisionRecord.blue = 255;
        collisionRecord.mode = 128;
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
