#include "common.h"
#include "shared.h"
#include "main/toolkit.h"
#include "main/exm_2.h"
#include "main/init_uwamono_sys.h"
#include "main/karakama_test.h"
#include "main/outer_product.h"
#include "main/sef.h"
#include "main/vibration.h"
#include "main/xgl_2.h"
#include "main/xgl_sound.h"

typedef struct MapUnitRecordTrapView MapUnitRecordTrapView;

typedef struct {
    unsigned int flags;
    unsigned char unmodeled_04[0x0c];
    DoublewordAlignedVector4 position;
    unsigned char unmodeled_20[0xa50];
} TrapDamageActor;

typedef struct {
    unsigned char unmodeled_00[0x20];
    DoublewordAlignedVector4 trapPosition;
    unsigned long long unmodeled_30;
    unsigned char unmodeled_38[0x3878];
} TrapEnemyWorkEntry;

typedef struct {
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    unsigned int mode;
    DoublewordAlignedVector4 sphere;
} TrapCollisionRecord;

struct SchedulerState;

typedef struct {
    unsigned char unmodeled_00[0xa9a];
    unsigned char trap_active;
} TrapLinkedObject;

/* Kept as a view: uwaresId/linkedObject sit on canonical union members
 * (model/effectCf). Only the trap work area at +0x1a0 is canonical (exm_2.h).
 * The unit's vectors are only 8-aligned and copied as two doublewords. */
struct MapUnitRecordTrapView {
    unsigned int flags;
    void (*update)(MapUnitRecordTrapView *);
    unsigned char unmodeled_08[0x08];
    DoublewordAlignedVector4 position;
    DoublewordAlignedVector4 effectOrientation;
    unsigned char unmodeled_30[0x10];
    float matrix[4][4];
    float (*matrixRef)[4];
    unsigned char unmodeled_84[0x1c];
    unsigned char trapSerial;
    unsigned char unmodeled_a1;
    unsigned char trapState;
    unsigned char unmodeled_a3;
    s16 trapId;
    unsigned char unmodeled_a6[2];
    s16 trapTimer;
    unsigned char unmodeled_aa[6];
    DoublewordAlignedVector4 dimensions;
    unsigned char unmodeled_c0[0x20];
    int uwaresId;
    int xtxresId;
    TrapLinkedObject *linkedObject;
    unsigned char unmodeled_ec[4];
    unsigned char unmodeled_f0[0xb0];
    TrapDamageProperties trap;
};

typedef unsigned int TrapGameLoopStateWords[0xa80c];

typedef struct {
    signed char windState;
    unsigned char unmodeled_01[0x0f];
    DoublewordAlignedVector4 dimensions;
    unsigned int flags;
    int effectId;
    unsigned char unmodeled_28[8];
} TrapKindEntry;

typedef struct {
    signed char shape;
    unsigned char unmodeled_01[0x0f];
    DoublewordAlignedVector4 dimensions;
    signed char target;
    unsigned char unmodeled_21[0x0f];
} KowareKindEntry;

extern TrapDamageActor actor[64];
extern TrapEnemyWorkEntry enepc[16];
extern const char actElectricFormat[24];
extern const char actExplosionFormat[24];
extern const char actFallFormat[16];
extern const char actSlotFormat[16];
extern const char actSealFormat[16];
extern TrapGameLoopStateWords GameLoopState;
extern const char unitTrapDeleteFormat[0x28];
extern char trapBrokenMethodName[];
extern KowareKindEntry KowareKind_t[127];
extern int uwares_tbl[14];
extern int xtxres_tbl[14];
extern const char setTrapSizeFormat[16];
extern const char trapSizeXyzFormat[16];
extern const char createTrapPosFormat[0x30];

extern int printf(const char *, ...);

extern void DrawActiveCursol(MapUnitRecordTrapView *unit);
extern void UwamonoCommonFunc(MapUnitRecordTrapView *unit);
extern void ClearUwamonoEffect(MapUnitRecordTrapView *unit);
extern void SetUwaWind(MapUnitRecordTrapView *unit);
extern void UwamonoBrokenSe(MapUnitRecordTrapView *unit);
extern void CallMethod_I(const char *method_name, int argument1);
extern void CreateUwamonoCommon(MapUnitRecordTrapView *unit);
extern void SetContainerParam(MapUnitRecordTrapView *unit);
void MAP_updateUnitTrap(MapUnitRecordTrapView *unit);
int GivesEnemyDamage(MapUnitRecordTrapView *unit);

void InitMapTrap(MapUnitRecordTrapView *unit)
{
    static TrapKindEntry TrapKind_t[5] = { { .windState = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 1.0f, 0.25f, 0.0f }, .flags = 4194304U, .effectId = 730, .unmodeled_28 = { 0, 0, 0, 0, 0, 0, 0, 0 } }, { .windState = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 1.0f, 0.25f, 0.0f }, .flags = 8388608U, .effectId = 731, .unmodeled_28 = { 0, 0, 0, 0, 0, 0, 0, 0 } }, { .windState = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 1.0f, 0.25f, 0.0f }, .flags = 67108864U, .effectId = 732, .unmodeled_28 = { 0, 0, 0, 0, 0, 0, 0, 0 } }, { .windState = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 0.8f, 0.5f, 0.0f }, .flags = 33554432U, .effectId = 733, .unmodeled_28 = { 0, 0, 0, 0, 0, 0, 0, 0 } }, { .windState = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 1.0f, 0.5f, 0.0f }, .flags = 2097152U, .effectId = 601, .unmodeled_28 = { 0, 0, 0, 0, 0, 0, 0, 0 } } };

    TrapDamageProperties *trap = &unit->trap;
    int index;
    int xtxres;
    int uwares;

    unit->update = MAP_updateUnitTrap;
    if ((u16)(unit->trapId - 0x7001) < 4) {
        index = unit->trapId - 0x7001;
        uwares = uwares_tbl[index];
        xtxres = xtxres_tbl[index];
        unit->uwaresId = uwares;
        unit->xtxresId = xtxres;
        if (trap->windState == -1) {
            trap->windState = TrapKind_t[index].windState;
        }
        unit->flags |= TrapKind_t[index].flags;
        unit->dimensions = TrapKind_t[index].dimensions;
        trap->damageRadius = 3.0f;
        if (trap->target == -1) {
            trap->target = 0;
        }
        if (trap->effectId == -1) {
            trap->effectId = TrapKind_t[index].effectId;
        }
    } else if ((u16)(unit->trapId - 0x7014) < 31) {
        SetContainerParam(unit);
    } else {
        unit->flags |= TrapKind_t[4].flags;
        unit->dimensions = TrapKind_t[4].dimensions;
        trap->damageRadius = 3.0f;
        if (trap->kindIndex != -1) {
            if (trap->windState == -1) {
                trap->windState = KowareKind_t[trap->kindIndex].shape;
            } else {
                trap->windState = TrapKind_t[4].windState;
            }
            unit->dimensions = KowareKind_t[trap->kindIndex].dimensions;
            if (trap->effectId == -1) {
                trap->effectId = TrapKind_t[trap->kindIndex].effectId;
            }
        } else if (trap->windState == -1) {
            trap->windState = TrapKind_t[4].windState;
        }
        if (trap->target == -1) {
            trap->target = KowareKind_t[trap->kindIndex].target;
        }
        if (trap->effectId == -1) {
            trap->effectId = TrapKind_t[4].effectId;
        }
    }
    if (trap->sizeX > 0.0f) {
        unit->dimensions.x = trap->sizeX * 0.5f;
        unit->dimensions.y = trap->sizeY;
        unit->dimensions.z = trap->sizeZ * 0.5f;
        if (printflg) {
            printf(setTrapSizeFormat);
            printf(trapSizeXyzFormat, trap->sizeX, trap->sizeY, trap->sizeZ);
        }
    }
    unit->dimensions.x += trap->offsetX * 0.5f;
    unit->dimensions.y += trap->offsetY;
    unit->dimensions.z += trap->offsetZ * 0.5f;
    if (trap->activeFlags == -1) {
        trap->activeFlags = 1;
    }
    if (unit->trapId == 0x7000) {
        unit->flags |= 4;
        trap->activeFlags = 0;
    }
    if (unit->trapId == 0x7012) {
        unit->flags |= 4;
        trap->activeFlags = 0;
        trap->windState = 0;
    }
    CreateUwamonoCommon(unit);
    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->position.x);
    xglMatrixStackRotX(unit->effectOrientation.x);
    xglMatrixStackRotY(unit->effectOrientation.y);
    xglMatrixStackRotZ(unit->effectOrientation.z);
    xglMatrixStackSave(unit->matrix);
    unit->matrixRef = unit->matrix;
    SetHideObject(unit);
    if (printflg) {
        printf(createTrapPosFormat, unit->position.x, unit->position.y, unit->position.z,
               unit->effectOrientation.y / 3.1415927f * 180.0f);
    }
}

void MAP_updateUnitTrap(MapUnitRecordTrapView *unit)
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
                          &unit->position,
                          &unit->effectOrientation);
        unit->flags |= 0x00020004;
            ClearUwamonoEffect(unit);
            SetUwaWind(unit);
            if (trap->methodId != -1) {
                CallMethod_I(trapBrokenMethodName, trap->methodId);
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
                              &unit->position,
                              &unit->effectOrientation);
            unit->flags |= 0x00020004u;
            ClearUwamonoEffect(unit);
            SetUwaWind(unit);
            if (trap->methodId != -1) {
                GameLoopState[7] = 0;
                CallMethod_I(trapBrokenMethodName, trap->methodId);
            }
            Vibration_Set_Strong(0xff, 0x20, 0x1e);
        }
        break;
    }

    case 3: {
        unit->trapTimer = unit->trapTimer + 1;
        if (unit->trapTimer >= 5) {
            trap->windState = 0;
            EXM_ClearWindStruct();
        }
        if (unit->trapTimer < trap->activeDuration) {
            GivesEnemyDamage(unit);
        } else if (unit->trapTimer > trap->activeDuration) {
            if (printflg != 0) {
                printf(unitTrapDeleteFormat, unit->trapId, unit->trapSerial);
            }
            unit->update = 0;
            unit->trapId = -1;
            EXM_ClearWindStruct();
        }
        break;
    }

    case 1: {
        s16 elapsed;

        elapsed = unit->trapTimer = unit->trapTimer + 1;
        if (elapsed >= 5) {
            trap->windState = 0;
            if (printflg != 0) {
                printf(unitTrapDeleteFormat, unit->trapId, unit->trapSerial);
            }
            unit->update = 0;
            unit->trapId = -1;
            EXM_ClearWindStruct();
        }
        break;
    }
    }
}

int GivesEnemyDamage(MapUnitRecordTrapView *unit)
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
                printf(actElectricFormat, actorIndex);
            }
        } else if (unit->flags & 0x00800000u) {
            target->flags |= 0x01000000u;
            if (printflg) {
                printf(actExplosionFormat, actorIndex);
            }
        } else if (unit->flags & 0x01000000u) {
            target->flags |= 0x04000000u;
            if (printflg) {
                printf(actFallFormat, actorIndex);
            }
        } else if (unit->flags & 0x02000000u) {
            if (printflg) {
                printf(actSlotFormat, actorIndex);
            }
        } else if (unit->flags & 0x04000000u) {
            target->flags |= 0x08000000u;
            if (printflg) {
                printf(actSealFormat, actorIndex);
            }
        }
    }
    return 0;
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
char trapBrokenMethodName[8] = "broken";

const char CreateEvtItemGetDigit9Text[8] = "9";

const char CreateEvtItemGetDigit8Text[8] = "8";

const char CreateEvtItemGetDigit7Text[8] = "7";

const char CreateEvtItemGetDigit6Text[8] = "6";

const char CreateEvtItemGetDigit5Text[8] = "5";

const char CreateEvtItemGetDigit4Text[8] = "4";

const char CreateEvtItemGetDigit3Text[8] = "3";

const char CreateEvtItemGetDigit2Text[8] = "2";

const char CreateEvtItemGetDigit1Text[8] = "1";

const char CreateEvtItemGetDigit0Text[8] = "0";

const char setTrapSizeFormat[16] = "SetTrapSize\n";

const char trapSizeXyzFormat[16] = "x=%f y=%f z=%f\n";

const char createTrapPosFormat[48] = "Create Trap pos=x=%f, y=%f, z=%f, rotY=%f\n";

const char unitTrapDeleteFormat[40] = "unit trap delete id=0x%x serial=%d\n";

const char actElectricFormat[24] = "act[%d] Electric\n";

const char actExplosionFormat[24] = "act[%d] Explosion\n";

const char actFallFormat[16] = "act[%d] Fall\n";

const char actSlotFormat[16] = "act[%d] Slot\n";

const char actSealFormat[16] = "act[%d] Seal\n";
