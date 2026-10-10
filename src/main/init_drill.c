#include "common.h"

#include "shared.h"

float xglFRand(void);
float CheckDist2D(const Vector4 *first, const Vector4 *second);
int HitCheckContainerPosSize(Vector4 *position, int slot, float radius);
int HitCheckEnemy(Vector4 *position, float radius);

typedef struct DrillMapUnitInfo {
    unsigned char unmodeled_00[2];
    u16 drill_reset_value;
    signed char hit;
    u8 spawnedItemKind;
    unsigned char unmodeled_06[0x27];
    signed char containerType;
    unsigned char unmodeled_2e[0x72];
} DrillMapUnitInfo;

/*
 * MapUnit[] entries are game-wide unit records; only the fields this TU
 * touches are named. The rest of the struct is unmodeled padding recovered
 * as its exact byte span so the struct size and member offsets stay exact.
 */

typedef struct DrillMapUnit {
    u32 flags;
    void *update;
    unsigned char unmodeled_08[8];
    float position[4];
    unsigned char unmodeled_20[0x84];
    u16 serial;
    unsigned char unmodeled_a6[0x0a];
    float size[4];
    unsigned char unmodeled_c0[0xe0];
    /* DrillResetFlag tests this value against its cleanup range; other uses
     * of this field have not been identified. */
    DrillMapUnitInfo info;
    unsigned char unmodeled_240[0xc0];
} DrillMapUnit;

extern DrillMapUnit MapUnit[64];

/* Map input supplies the same pad record read by the drill camera. */

typedef struct DrillPadLayout {
    unsigned char unmodeled_00[0x28];
    u16 half_28;
    u16 half_2a;
    unsigned char unmodeled_2c[0x36];
    unsigned char pitchDeadZone;
    unsigned char yawDeadZone;
    unsigned char unmodeled_64[2];
    signed char pitchStick;
    signed char yawStick;
} DrillPadLayout;

extern DrillPadLayout PadData;

typedef union DrillVector {
    Vector4 vector;
    u64 words[2];
} DrillVector;

typedef struct Drill Drill;

typedef struct DrillEffectRef {
    unsigned char unmodeled_000[0x80];
    DrillVector offset;
    unsigned char unmodeled_090[0x634];
    Drill *owner;
    unsigned char unmodeled_6c8[0x3c4];
    u32 flags;
    unsigned char unmodeled_a90[0x0a];
    u8 hidden;
} DrillEffectRef;

typedef struct DrillCameraAngles {
    unsigned char unmodeled_00[4];
    float baseAngle;
    unsigned char unmodeled_08[8];
    float yaw;
    float pitch;
    float roll;
    unsigned char unmodeled_1c[4];
    float shakeOffsetX;
    float shakeOffsetY;
    float shakeOffsetZ;
    unsigned char unmodeled_2c[4];
    float shakeOffsetStepX;
    float shakeOffsetStepY;
    float shakeOffsetStepZ;
    unsigned char unmodeled_3c[4];
    float positionX;
    float positionY;
    float positionZ;
    unsigned char unmodeled_4c[4];
    float shakeVelocityX;
    float shakeVelocityY;
    float shakeVelocityZ;
    unsigned char unmodeled_5c[4];
    float shakeVelocityStepX;
    float shakeVelocityStepY;
    float shakeVelocityStepZ;
} DrillCameraAngles;

typedef struct DrillCamera {
    unsigned char unmodeled_000[4];
    u32 state;
    unsigned char unmodeled_008[0x88];
    DrillCameraAngles angles;
} DrillCamera;

typedef struct DrillSave {
    unsigned char unmodeled_00[4];
    signed char hit;
    signed char startState;
    signed char containerCount;
    signed char containerSetup;
    s16 cameraPreset;
    signed char clearRequest;
    signed char resetRequest;
    float xSpeed;
    float fallSpeed;
    float zSpeed;
    float xReturnSpeed;
    float riseSpeed;
    float zReturnSpeed;
    float hitRadius;
    float initialCameraAngle;
    signed char returnLimit;
    unsigned char runState;
    signed char kind;
    unsigned char unmodeled_2f[3];
    signed char command;
    signed char hitCount;
    int partIndexA;
    int partIndexB;
    int returnCount;
    int frameCount;
    unsigned char unmodeled_44[8];
    DrillEffectRef *effect1;
    int partIndexC;
    int partIndexD;
    u32 flags;
    int frameLimit;
} DrillSave;

struct Drill {
    unsigned char unmodeled_000[4];
    void *update;
    unsigned char unmodeled_008[8];
    DrillVector position;
    float rotationX;
    float rotationY;
    float rotationZ;
    unsigned char unmodeled_02c[0x54];
    float (*matrix)[4];
    unsigned char unmodeled_084[0x1c];
    u8 slot;
    unsigned char unmodeled_0a1;
    u8 phase;
    unsigned char unmodeled_0a3;
    s16 serial;
    unsigned char unmodeled_0a6[2];
    s16 crashTimer;
    s16 bounceCount;
    unsigned char unmodeled_0ac[0x3c];
    DrillEffectRef *effect3;
    unsigned char unmodeled_0ec[0xb4];
    DrillSave save;
    unsigned char unmodeled_200[0x20];
    float cameraYaw;
    float cameraPitch;
    unsigned char unmodeled_228[8];
    DrillEffectRef *effect2;
};

DrillCamera *xglStudioGetActiveCamera(void);

void *sefCreateEffectCf(int effect_id, const void *position, const void *orientation);

void sefRewindEffectCf(DrillEffectRef *effect);

void xglFontDebugPrintf(int x, int y, const char *fmt, ...);

void xglMatrixStackUnit(void);

void xglMatrixStackRotY(float angle);

void xglMatrixStackSave(float matrix[4][4]);

void xglSoundEffectStopID(int id, int channel);

void xglSoundEffectNormalID(int id, int channel);

void Vibration_Set_Strong(int left, int right, int duration);

void DrillHitCheck(Drill *drill);

void DrillCalcCameraPos(DrillCamera *camera);

void DrillResetCameraPos(Drill *drill, DrillCamera *camera);

void DrillCameraControlFunc(Drill *drill, DrillCamera *camera);

float xglSin(float angle);

float xglCos(float angle);

void *Unit_CreateUwamono(int identifier, int unit_id);

void InitItemSymbol(void *item);

void SetHideObject(void *unit);

int xglFlagsSet1(int bit_offset, int value);

typedef struct DrillItemInfo {
    u16 state;
    u16 value;
} DrillItemInfo;

typedef struct DrillItemUnit {
    unsigned char unmodeled_000[0xa0];
    u8 itemKind;
    unsigned char unmodeled_0a1[0xff];
    DrillItemInfo info;
    unsigned char unmodeled_1a4;
    u8 spawnedItemKind;
    unsigned char unmodeled_1a6[0x15a];
} DrillItemUnit;

/* Container records use the same 0x300-byte MapUnit slots. The work-area
 * fields below are read or written by the container creation/hit routines. */
typedef struct DrillContainerInfo {
    unsigned char unmodeled_00[4];
    signed char hit;
    u8 spawnedItemKind;
    unsigned char unmodeled_06[0x27];
    signed char containerType;
    u8 active;
    unsigned char unmodeled_2f;
    s16 containerSerial;
    u8 hitMode;
    unsigned char unmodeled_33[0x0d];
    int modelId;
    int extraId;
    unsigned char unmodeled_48[0x118];
} DrillContainerInfo;

typedef struct DrillContainerUnit {
    u32 flags;
    void *update;
    unsigned char unmodeled_08[8];
    Vector4 position;
    float rotationX;
    float rotationY;
    float rotationZ;
    unsigned char unmodeled_2c[0x14];
    float matrix[4][4];
    float (*matrixRef)[4];
    unsigned char unmodeled_84[0x1c];
    u8 slot;
    unsigned char unmodeled_a1[3];
    s16 serial;
    unsigned char unmodeled_a6[0x0a];
    DrillVector size;
    unsigned char unmodeled_c0[0xe0];
    DrillContainerInfo info;
} DrillContainerUnit;

typedef struct DrillContainerModel {
    u8 hitMode;
    unsigned char unmodeled_01[3];
    int modelId;
    int itemBase;
    int unmodeled_0c;
} DrillContainerModel;

typedef struct DrillContainerDef {
    u8 type;
    unsigned char unmodeled_01[0x0f];
    DrillVector size;
    DrillContainerModel model;
} DrillContainerDef;

typedef struct DrillItemBox {
    unsigned char unmodeled_00[3];
    signed char weight;
    unsigned char unmodeled_04[4];
} DrillItemBox;

typedef struct DrillMatrix {
    float m[4][4];
} DrillMatrix;

typedef struct DrillSceneBlock {
    unsigned char unmodeled_00[0x50];
    int matrixOffset;
} DrillSceneBlock;

typedef struct DrillSceneRoot {
    unsigned char unmodeled_00[4];
    DrillSceneBlock *block;
} DrillSceneRoot;

/* HitCheckEnemy's partial actor view keeps the original 0xa70 table stride. */
typedef struct DrillHitActor {
    u32 flags;
    unsigned char unmodeled_04[0x0c];
    Vector4 position;
    unsigned char unmodeled_20[0x60];
    u8 number;
    unsigned char unmodeled_81[5];
    s16 sound_effect_id;
    unsigned char unmodeled_88[0x84];
    int methodArg;
    unsigned char unmodeled_110[0x8d8];
    float body_radius;
    unsigned char unmodeled_9ec[0x84];
} DrillHitActor;

typedef DrillHitActor DrillMotionActor;

typedef struct DrillEnemyWork {
    unsigned char unmodeled_00[0x0c];
    float height;
    unsigned char unmodeled_10[0x38a0];
} DrillEnemyWork;

extern DrillContainerDef container[31];
extern DrillItemBox ItemBoxTbl[];
extern u32 *pDrillFlag;
extern DrillHitActor actor[64];
extern DrillEnemyWork enepc[16];

/* These unchanged data records are supplied by the original scaffolding. */
extern const char D_004CA940[];
extern const char D_004CA970[];
extern const char D_004CA9A0[];
extern const char D_004CA9B8[];
extern const char D_004CA9C8[];
extern char D_004DB7E0[];
extern char D_004DB7E8[];
extern const char D_004CAA00[];
extern const char D_004CAA10[];
extern const char D_004CAA20[];
extern const char D_004CAA30[];

int xglFlagsGet1(int bit_offset);
unsigned short xglSRand(void);
void xglMatrixStackTrans(const float translation[4]);
void xglMatrixStackRotX(float angle);
void xglMatrixStackRotZ(float angle);
void DrillClearContainer(void);
void SetContainer(int count);
void SetContainerItem(DrillItemUnit *unit, int value);
void CreateUwamonoCommon(DrillContainerUnit *unit);
void MAP_updateUnitTrap(void);
void SetContainerParam(DrillContainerUnit *unit);
int CalcPosContainer(DrillContainerUnit *unit);
void DrillPowerOffFunc(Drill *drill);
void DrillStandbyFunc(Drill *drill);
void DrillZMoveFunc(Drill *drill);
void DrillZStopFunc(Drill *drill);
void DrillXMoveFunc(Drill *drill);
void DrillCrashFunc(Drill *drill);
struct DrillReturnView;
void DrillReturnFunc(struct DrillReturnView *drill);
void MAP_updateUnitDrill(Drill *drill);
void DrillResetFlag(Drill *drill);
void GetPartsPos(Drill *drill);
int printf(const char *format, ...);
int HexToStr(int value, char *text);
void xglFontPrint(int x, int y, int colour, const char *text);
void xglFontPrintDirectOT(int ot, const char *text);
void Enemy_Command_Freeze(DrillHitActor *target, int frozen);
void CallMethod_I(const char *method, int argument);
void ACT_setMotion2(DrillMotionActor *actor, int motion, int mode);

const char D_004CA960[] = "SetContainer\n";

int CreateContainer(int, int);

typedef struct DrillReturnEffectRef {
    unsigned char unmodeled_000[0x80];
    Vector4 offset;                     /* +0x080: effect offset vector */
    unsigned char unmodeled_090[0x634];
    void *owner;                         /* +0x6c4: owning Drill */
    unsigned char unmodeled_6c8[0x3c4];
    u32 flags;                            /* +0xa8c */
    unsigned char unmodeled_a90[0x0a];
    u8 hidden;                            /* +0xa9a: visibility flag */
} DrillReturnEffectRef;

typedef struct DrillReturnSave {
    unsigned char unmodeled_00[4];
    signed char hit;                     /* +0x004 */
    unsigned char unmodeled_05[7];
    float xSpeed;                         /* +0x00c */
    float fallSpeed;                      /* +0x010 */
    float zSpeed;                         /* +0x014 */
    float xReturnSpeed;                   /* +0x018 */
    float riseSpeed;                      /* +0x01c */
    float zReturnSpeed;                   /* +0x020 */
    unsigned char unmodeled_24[8];
    signed char returnLimit;              /* +0x02c */
    unsigned char unmodeled_2d;
    signed char kind;                     /* +0x02e */
    unsigned char unmodeled_2f[4];
    signed char hitCount;                 /* +0x033 */
    unsigned char unmodeled_34[8];
    int returnCount;                      /* +0x03c */
    unsigned char unmodeled_40[0x0c];
    DrillReturnEffectRef *effect1;        /* +0x04c */
    unsigned char unmodeled_50[8];
    u32 flags;                            /* +0x058 */
} DrillReturnSave;

typedef struct DrillReturnView {
    unsigned char unmodeled_000[0x10];
    Vector4 position;                    /* +0x010: direct xyz view used by DrillReturnFunc */
    float rotationX;                      /* +0x020 */
    float rotationY;                      /* +0x024 */
    float rotationZ;                      /* +0x028 */
    unsigned char unmodeled_02c[0x54];
    float (*matrix)[4];                   /* +0x080 */
    unsigned char unmodeled_084[0x1e];
    u8 phase;                             /* +0x0a2 */
    unsigned char unmodeled_0a3[5];
    s16 crashTimer;                       /* +0x0a8 */
    unsigned char unmodeled_0aa[0x3e];
    DrillReturnEffectRef *effect3;        /* +0x0e8 */
    unsigned char unmodeled_0ec[0xb4];
    DrillReturnSave save;                 /* +0x1a0 */
    unsigned char unmodeled_1fc[0x24];
    float cameraYaw;                      /* +0x220 */
    float cameraPitch;                    /* +0x224 */
    unsigned char unmodeled_228[8];
    DrillReturnEffectRef *effect2;        /* +0x230 */
} DrillReturnView;

typedef struct DrillGameLoopState {
    unsigned char unmodeled_000[4];
    DrillMotionActor *playerActor;
    unsigned char unmodeled_008[8];
    u32 flags;
    unsigned char unmodeled_014[0x40];
    DrillSceneRoot *sceneRoot;
} DrillGameLoopState;

extern DrillGameLoopState GameLoopState;

/* The scene record contains a byte offset to its array of 4x4 matrices. */
#define DRILL_SCENE_MATRIX(index)                                           \
    (((DrillMatrix *)((u8 *)GameLoopState.sceneRoot->block                  \
        + GameLoopState.sceneRoot->block->matrixOffset))[index])

extern void CallMethod(const char *method);

typedef struct DrillMapUnitHitProbe {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    unsigned char unmodeled_20[0x9c8];
    float radius;
    unsigned char unmodeled_9ec[0x84];
} DrillMapUnitHitProbe;

int HitCheckMapUnitPosAt(DrillMapUnitHitProbe *probe, DrillMapUnit *unit);

void InitDrill(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillEffectRef *effect;

    printf((char *) D_004CA940, save->partIndexA, save->partIndexB, drill->serial);
    GetPartsPos(drill);
    save->runState = 0;
    drill->phase = 0;
    drill->update = MAP_updateUnitDrill;
    drill->crashTimer = 0;
    drill->bounceCount = 0;
    pDrillFlag = &save->flags;
    if (save->containerSetup != 0) {
        DrillResetFlag(drill);
        SetContainer(save->containerCount);
    }
    save->initialCameraAngle = xglStudioGetActiveCamera()->angles.baseAngle;

    effect = sefCreateEffectCf(1662, 0, 0);
    if (effect == 0) {
        effect = sefCreateEffectCf(1539, 0, 0);
    }
    drill->effect3 = effect;
    effect->offset.vector.x = -3.2f;
    effect->offset.vector.y = 6.67f;
    effect->offset.vector.z = -12.5f;
    effect->hidden = 0;

    effect = sefCreateEffectCf(1661, 0, 0);
    if (effect == 0) {
        effect = sefCreateEffectCf(1539, 0, 0);
    }
    drill->effect2 = effect;
    effect->offset.vector.z = -12.5f;
    effect->offset.vector.x = -3.03f;
    effect->offset.vector.y = 6.23f;
    effect->hidden = 0;

    effect = sefCreateEffectCf(1581, 0, 0);
    save->effect1 = effect;
    effect->hidden = 0;
    DrillResetFlag(drill);
}

void SetContainer(int count)
{
    printf((char *) D_004CA960);
    DrillClearContainer();
    if (count > 0) {
        do {
            if (CreateContainer(-1, (xglSRand() % 20U) + 0x7014) == 0) {
                switch (xglSRand() & 3) {
                case 0:
                    CreateContainer(-1, 0x7014);
                    break;
                case 1:
                    CreateContainer(-1, 0x7015);
                    break;
                case 2:
                    CreateContainer(-1, 0x7016);
                    break;
                case 3:
                    CreateContainer(-1, 0x7017);
                    break;
                }
            }
        } while (--count != 0);
    }
}

int CreateContainer(int identifier, int unit_id)
{
    DrillContainerUnit *unit;
    DrillItemBox *table;
    u16 roll;
    int base;
    int weight;
    int tries;
    int slot;
    int pick;
    int item;

    unit = Unit_CreateUwamono(identifier, unit_id);
    if (unit == 0) {
        return 0;
    }
    unit->serial = unit_id;
    SetContainerParam(unit);
    if (CalcPosContainer(unit) == 0) {
        unit->update = 0;
        unit->serial = -1;
        return 0;
    }
    if ((u16)(unit->serial - 0x701C) < 4 && (xglSRand() & 0xF) == 0) {
        tries = 0;
        slot = (u16)(xglSRand() % 5U);
        do {
            pick = (slot + tries) % 5;
            tries++;
            slot = pick;
            if (!(*pDrillFlag & (0x10 << pick))) {
                *pDrillFlag |= 0x10 << pick;
                item = pick + 0x21D;
                if (xglFlagsGet1(pick + 0x7A0E4) == 0) {
                    SetContainerItem((DrillItemUnit *)unit, item);
                    printf((char *) D_004CA970, item);
                    return (int)unit;
                }
            }
        } while (tries < 5);
    }
    table = ItemBoxTbl;
    base = unit->serial - 0x7014;
    base = container[base].model.itemBase;
    weight = table[base].weight;
    roll = xglSRand() % 100U;
    if (roll < weight) {
        SetContainerItem((DrillItemUnit *)unit, base);
    } else {
        base++;
        weight += table[base].weight;
        if (roll < weight) {
            SetContainerItem((DrillItemUnit *)unit, base);
        }
    }
    return (int)unit;
}

void DrillClearContainer(void)
{
    int i;

    for (i = 0; i < 64; i++) {
        DrillMapUnit *unit = &MapUnit[i];

        if ((u32)unit->serial - 0x7014 < 0x14) {
            unit->serial = (u16)-1;
        } else if ((unit->flags & 0x100000) != 0) {
            unit->serial = (u16)-1;
        } else {
            continue;
        }
        unit->update = 0;
    }
}

void SetContainerParam(DrillContainerUnit *unit)
{
    DrillContainerInfo *info = &unit->info;
    int index;

    unit->flags = (unit->flags & ~4) | 0x200000;
    if (info->extraId == -1) {
        info->extraId = 0;
    }
    info->active = 1;
    index = unit->serial - 0x7014;
    info->containerType = container[index].type;
    info->hitMode = container[index].model.hitMode;
    unit->size = container[index].size;
    if (info->modelId == -1) {
        info->modelId = container[index].model.modelId;
    }
    CreateUwamonoCommon(unit);
    unit->update = MAP_updateUnitTrap;
    unit->matrixRef = unit->matrix;
    if (info->containerSerial == -1) {
        info->containerSerial = unit->serial;
    }
}

void SetContainerItem(DrillItemUnit *container, int value)
{
    DrillItemUnit *item;

    item = Unit_CreateUwamono(-1, 0x7000);
    if (item != 0) {
        DrillItemInfo *info = &item->info;

        info->value = value;
        info->state = 5;
        InitItemSymbol(item);
        container->spawnedItemKind = item->itemKind;
        SetHideObject(container);
    }
}

int CalcPosContainer(DrillContainerUnit *unit)
{
    DrillContainerInfo *info = &unit->info;
    float radius;
    float distance;
    int hit;
    int tries;

    tries = 0;
    unit->rotationX = 0.0f;
    unit->rotationY = xglFRand();
    unit->rotationZ = 0.0f;
    do {
        unit->position.x = -1.5f;
        unit->position.y = 0.01f;
        unit->position.z = -23.3f;
        unit->position.x += (xglSRand() % 500U) / 100.0f;
        unit->position.z += (xglSRand() % 1100U) / 100.0f;
        xglMatrixStackUnit();
        xglMatrixStackTrans(&unit->position.x);
        xglMatrixStackRotX(unit->rotationX);
        xglMatrixStackRotY(unit->rotationY);
        xglMatrixStackRotZ(unit->rotationZ);
        xglMatrixStackSave(unit->matrix);
        radius = unit->size.vector.x;
        if (info->containerType == 2) {
            radius *= 1.5f;
        }
        hit = HitCheckContainerPosSize(&unit->position, unit->slot, radius);
        if (hit == -1) {
            return 1;
        }
        distance = CheckDist2D(&unit->position, (const Vector4 *)MapUnit[hit].position);
        if (distance < MapUnit[hit].size[0] - radius) {
            unit->position.y = MapUnit[hit].position[1] + MapUnit[hit].size[1];
            hit = HitCheckContainerPosSize(&unit->position, unit->slot, radius);
            if (hit == -1) {
                return 1;
            }
            distance = CheckDist2D(&unit->position, (const Vector4 *)MapUnit[hit].position);
            if (distance < MapUnit[hit].size[0] - radius) {
                unit->position.y = MapUnit[hit].position[1] + MapUnit[hit].size[1];
                if (HitCheckContainerPosSize(&unit->position, unit->slot, radius) == -1) {
                    return 1;
                }
            }
        }
        tries++;
    } while (tries < 255);
    return 0;
}

void MAP_updateUnitDrill(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillMatrix *matrix;
    DrillMatrix *part;
    DrillCamera *camera;
    char text[64];
    u32 flags;
    int seconds;
    int length;

    switch (drill->phase) {
    case 0:
        DrillPowerOffFunc(drill);
        break;
    case 1:
        DrillStandbyFunc(drill);
        break;
    case 2:
        DrillZMoveFunc(drill);
        break;
    case 3:
        DrillZStopFunc(drill);
        break;
    case 4:
        DrillXMoveFunc(drill);
        break;
    case 5:
        DrillCrashFunc(drill);
        break;
    case 6:
        DrillReturnFunc((struct DrillReturnView *) drill);
        break;
    }

    if (drill->phase != 0) {
        memset(text, 0, sizeof(text));
        flags = save->flags;
        if ((flags & 1) != 0) {
            if ((flags & 2) != 0) {
                seconds = (int)((float)save->frameCount / 30.0f);
                seconds = seconds < 1000 ? seconds : 999;
                length = HexToStr(seconds, text);
                xglFontPrint(336, 24, 0xFFF0, D_004CA9A0);
                xglFontPrint(472 - length * 10, 24, 0xFFF0, text);
                xglFontPrintDirectOT(0xFFF0, D_004DB7E0);
            } else {
                seconds = (int)((float)(save->frameLimit - save->frameCount) / 30.0f);
                seconds = seconds < 1000 ? seconds : 999;
                if (seconds <= 0) {
                    if (!(flags & 0x80000000)) {
                        CallMethod(D_004CA9B8);
                        save->flags |= 0x80000000;
                        if (drill->phase != 5) {
                            DrillEffectRef *effect;

                            drill->phase = 6;
                            effect = drill->effect3;
                            effect->hidden = 0;
                            effect = save->effect1;
                            effect->hidden = 0;
                        }
                    }
                    seconds = 0;
                }
                length = HexToStr(seconds, text);
                xglFontPrint(328, 24, 0xFFF0, D_004CA9C8);
                xglFontPrint(480 - length * 10, 24, 0xFFF0, text);
                xglFontPrintDirectOT(0xFFF0, D_004DB7E0);
            }
        }
        if (save->command != 2) {
            save->frameCount++;
        }
    }

    matrix = (DrillMatrix *) drill->matrix;
    matrix->m[3][0] = drill->position.vector.x;
    matrix->m[3][1] = drill->position.vector.y;
    part = &DRILL_SCENE_MATRIX(save->partIndexA);
    part->m[3][2] = matrix->m[3][2] = drill->position.vector.z;
    part = &DRILL_SCENE_MATRIX(save->partIndexB);
    part->m[3][0] = matrix->m[3][0];
    part->m[3][2] = matrix->m[3][2];
    if (save->partIndexC != -1) {
        DrillMatrix *copy = &DRILL_SCENE_MATRIX(save->partIndexC);

        *copy = *matrix;
    }
    if (save->partIndexD != -1) {
        DrillMatrix *copy = &DRILL_SCENE_MATRIX(save->partIndexD);

        *copy = *matrix;
    }

    if (save->clearRequest != 0) {
        DrillClearContainer();
        save->clearRequest = 0;
    } else if (save->resetRequest != 0) {
        DrillResetFlag(drill);
        SetContainer(save->containerCount);
        save->resetRequest = 0;
    } else if (save->command == 1) {
        camera = xglStudioGetActiveCamera();
        GameLoopState.flags |= 0x80000;
        camera->state = 4;
        GameLoopState.flags &= ~0x10000;
        drill->phase = 0;
        save->hit = 0;
        save->command = 0;
    }
}

void DrillPowerOffFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillMotionActor *actor = GameLoopState.playerActor;
    DrillCamera *camera;
    float initialAngle;
    DrillEffectRef *effect;
    signed char hit;

    xglFontDebugPrintf(8, 16, D_004CAA00);
    drill->position.vector.x = -2.9f;
    drill->position.vector.y = 4.5f;
    drill->position.vector.z = -12.5f;
    save->returnCount = 0;
    save->startState = 0;
    save->hitCount = 0;
    save->command = 0;
    camera = xglStudioGetActiveCamera();
    initialAngle = save->initialCameraAngle;
    save->cameraPreset = 0;
    camera->angles.baseAngle = initialAngle;
    xglSoundEffectStopID(0x30096, 0);
    xglSoundEffectStopID(0x30098, 0);
    hit = save->hit;
    if (hit == 1) {
        drill->phase = hit;
        GameLoopState.flags |= 0x10000;
        ACT_setMotion2(actor, 0, 9);
        DrillResetCameraPos(drill, camera);
        save->returnCount = hit;
        save->frameCount = 0;
        GameLoopState.flags &= ~0x80000;
        save->flags &= 0x7fffffff;
    }
    effect = drill->effect2;
    effect->hidden = 0;
    effect = save->effect1;
    effect->hidden = 0;
}

void DrillStandbyFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillCamera *camera = xglStudioGetActiveCamera();

    if (save->command == 2) {
        save->cameraPreset = 1;
        xglFontDebugPrintf(8, 16, (char *) D_004CAA10);
        DrillResetCameraPos(drill, camera);
        return;
    } else if (save->command == 1) {
        GameLoopState.flags |= 0x80000;
        camera->state = 4;
        GameLoopState.flags &= ~0x10000;
        drill->phase = 0;
        save->hit = 0;
        save->command = 0;
    } else {
        xglFontDebugPrintf(8, 16, (char *) D_004CAA20);
        if ((int) save->flags >= 0) {
            DrillCameraControlFunc(drill, camera);
            drill->crashTimer = 0;
            drill->bounceCount = 0;
            if ((PadData.half_2a & 0x2) != 0) {
                DrillResetCameraPos(drill, camera);
                save->cameraPreset++;
                save->cameraPreset %= 3;
            }
            if ((PadData.half_2a & 0x80) != 0) {
                camera->angles.shakeVelocityX = 0.0f;
                camera->angles.shakeVelocityY = 0.0f;
                camera->angles.shakeVelocityZ = 0.0f;
                camera->angles.shakeOffsetX = 0.0f;
                camera->angles.shakeOffsetY = 0.0f;
                camera->angles.shakeOffsetZ = 0.0f;
                drill->phase = 2;
                xglSoundEffectNormalID(0x30096, 0);
                return;
            }
            if ((PadData.half_2a & 0x40) != 0
                && ((save->flags & 1) == 0 || save->frameLimit - save->frameCount >= 60)
                && save->command != 1) {
                save->command = 2;
                CallMethod((const char *) D_004CAA30);
            }
        }
    }
}

const char D_004CAA40[] = "DRILL ZMOVE";

const char D_004CAA50[] = "DRILL ZSTANDBY";

const char D_004CAA60[] = "DRILL XMOVE";

const char D_004CAA70[] = "DRILL CRASH %2d";

const char D_004CAA80[] = "DRILL RETURN";

const char D_004CAA90[] = "drill_stanby";

const char D_004CAAA0[] = "drill_end";

const char D_004CAAB0[] = "CAMERA CONTROL";

void DrillZMoveFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillEffectRef *effect;

    xglFontDebugPrintf(8, 16, (char *) D_004CAA40);
    Vibration_Set_Strong(128, 128, 2);
    save->cameraPreset = 1;

    if ((PadData.half_28 & 0x80) != 0) {
        drill->position.vector.z -= save->zSpeed;
    } else {
        drill->phase = 3;
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectNormalID(0x30097, 0);
    }

    if (drill->position.vector.z < -23.3f) {
        drill->position.vector.z = -23.3f;
        drill->phase = 3;
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectNormalID(0x30097, 0);
    }

    effect = drill->effect2;
    sefRewindEffectCf(effect);
    effect->hidden = 1;
    effect->offset.vector.z = drill->position.vector.z;

    effect = save->effect1;
    sefRewindEffectCf(effect);
    effect->hidden = 1;
    effect->offset.vector.x = -4.0f;
    effect->offset.vector.y = 1.5f;
    effect->offset.vector.z = drill->position.vector.z + 0.5f;
}

void DrillZStopFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillCamera *camera;
    DrillEffectRef *effect;

    xglFontDebugPrintf(8, 16, (char *) D_004CAA50);
    camera = xglStudioGetActiveCamera();
    DrillCameraControlFunc(drill, camera);

    if ((PadData.half_2a & 0x2) != 0) {
        DrillResetCameraPos(drill, camera);
        save->cameraPreset++;
        save->cameraPreset %= 3;
    }

    if ((PadData.half_2a & 0x80) != 0) {
        camera->angles.shakeVelocityX = 0.0f;
        camera->angles.shakeVelocityY = 0.0f;
        camera->angles.shakeVelocityZ = 0.0f;
        camera->angles.shakeOffsetX = 0.0f;
        camera->angles.shakeOffsetY = 0.0f;
        camera->angles.shakeOffsetZ = 0.0f;
        camera->state = 0;
        drill->phase = 4;
        xglSoundEffectNormalID(0x30098, 0);
    }

    effect = drill->effect2;
    effect->hidden = 0;
    effect = save->effect1;
    effect->hidden = 0;
}

void DrillXMoveFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillCamera *camera;
    DrillEffectRef *effect;

    camera = xglStudioGetActiveCamera();
    xglFontDebugPrintf(8, 16, (char *) D_004CAA60);
    Vibration_Set_Strong(128, 128, 2);

    if ((PadData.half_28 & 0x80) != 0) {
        drill->position.vector.x += save->xSpeed;
    } else if (drill->position.vector.x < -1.9000001f) {
        drill->position.vector.x += save->xSpeed;
    } else {
        drill->phase = 5;

        effect = sefCreateEffectCf(1594, 0, 0);
        effect->offset = drill->position;
        effect->flags &= ~0x10;
        effect->offset.vector.y = 0.0f;

        effect = sefCreateEffectCf(1657, 0, 0);
        effect->flags &= ~0x400;
        effect->owner = drill;

        drill->cameraYaw = camera->angles.yaw;
        drill->cameraPitch = camera->angles.pitch;

        xglSoundEffectStopID(0x30098, 0);
        xglSoundEffectNormalID(0x30099, 0);
        return;
    }

    if (drill->position.vector.x > 4.3f) {
        drill->position.vector.x = 4.3f;
        drill->phase = 5;

        effect = sefCreateEffectCf(1594, 0, 0);
        effect->offset = drill->position;
        effect->flags &= ~0x10;
        effect->offset.vector.y = 0.0f;

        effect = sefCreateEffectCf(1657, 0, 0);
        effect->flags &= ~0x400;
        effect->owner = drill;

        drill->cameraYaw = camera->angles.yaw;
        drill->cameraPitch = camera->angles.pitch;

        xglSoundEffectStopID(0x30098, 0);
        xglSoundEffectNormalID(0x30099, 0);
    }

    effect = drill->effect3;
    sefRewindEffectCf(effect);
    effect->hidden = 1;
    effect->offset.vector.x = drill->position.vector.x;
    effect->offset.vector.z = drill->position.vector.z;

    effect = save->effect1;
    sefRewindEffectCf(effect);
    effect->hidden = 1;
    effect->offset.vector.x = drill->position.vector.x;
    effect->offset.vector.y = 6.0f;
    effect->offset.vector.z = drill->position.vector.z;
}

void DrillCrashFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillCamera *camera;
    DrillEffectRef *effect;

    effect = drill->effect3;
    effect->hidden = 0;
    effect = save->effect1;
    effect->hidden = 0;
    camera = xglStudioGetActiveCamera();
    xglFontDebugPrintf(8, 16, (char *) D_004CAA70, save->hitCount);

    drill->crashTimer++;
    drill->rotationY += drill->crashTimer * 0.1f;
    drill->rotationX = 0.0f;
    drill->rotationZ = 0.0f;
    xglMatrixStackUnit();
    xglMatrixStackRotY(drill->rotationY);
    xglMatrixStackSave(drill->matrix);

    camera->angles.positionX = drill->position.vector.x + 4.0f;
    camera->angles.positionY = drill->position.vector.y + 5.0f;
    camera->angles.positionZ = drill->position.vector.z - 4.0f;
    camera->angles.yaw = -0.78539819f;
    camera->angles.pitch = 2.3561945f;
    camera->angles.roll = 0.0f;

    if (drill->crashTimer >= 16) {
        drill->crashTimer = 16;
        drill->position.vector.y -= save->fallSpeed;
        DrillHitCheck(drill);
        if (drill->position.vector.y < 0.0f) {
            if (drill->bounceCount == 0) {
                sefCreateEffectCf(1721, &drill->position, 0);
            }
            drill->bounceCount++;
            drill->position.vector.y = 0.0f;
            if (drill->bounceCount >= 9) {
                camera->angles.yaw = drill->cameraYaw;
                camera->angles.pitch = drill->cameraPitch;
                DrillCalcCameraPos(camera);
                drill->phase = 6;
            }
        }
    }
}

void DrillReturnFunc(DrillReturnView *drill)
{
    DrillReturnSave *save = &drill->save;
    DrillReturnEffectRef *effect;
    DrillCamera *camera;

    xglFontDebugPrintf(8, 16, (const char *)D_004CAA80);
    if (drill->crashTimer > 0) {
        drill->crashTimer--;
        drill->rotationY += drill->crashTimer * 0.1f;
        drill->rotationX = 0.0f;
        drill->rotationZ = 0.0f;
        xglMatrixStackUnit();
        xglMatrixStackRotY(drill->rotationY);
        xglMatrixStackSave(drill->matrix);
        DrillHitCheck((Drill *)drill);
        return;
    }

    if (drill->position.y < 4.5f) {
        drill->position.y += save->riseSpeed;
        if (drill->position.y >= 4.5f) {
            drill->position.y = 4.5f;
            xglSoundEffectNormalID(0x30098, 0);
        }
        return;
    }

    drill->position.y = 4.5f;
    if (drill->position.x > -2.9000001f) {
        drill->position.x -= save->xReturnSpeed;
        if (drill->position.x <= -2.9000001f) {
            drill->position.x = -2.9000001f;
            xglSoundEffectStopID(0x30098, 0);
            xglSoundEffectNormalID(0x30096, 0);
        }
        effect = drill->effect3;
        sefRewindEffectCf((DrillEffectRef *)effect);
        effect->hidden = 1;
        effect->offset.x = drill->position.x;
        effect->offset.z = drill->position.z;
        effect = save->effect1;
        sefRewindEffectCf((DrillEffectRef *)effect);
        effect->hidden = 1;
        effect->offset.x = drill->position.x;
        effect->offset.y = 6.0f;
        effect->offset.z = drill->position.z;
        return;
    }

    drill->position.x = -2.9000001f;
    effect = drill->effect3;
    effect->hidden = 0;
    if (drill->position.z < -12.5f) {
        drill->position.z += save->zReturnSpeed;
        if (drill->position.z >= -12.5f) {
            drill->position.z = -12.5f;
            xglSoundEffectStopID(0x30096, 0);
            xglSoundEffectNormalID(0x30097, 0);
        }
        effect = drill->effect2;
        sefRewindEffectCf((DrillEffectRef *)effect);
        effect->hidden = 1;
        effect->offset.z = drill->position.z;
        effect = save->effect1;
        effect->offset.x = -4.0f;
        effect->offset.y = 1.5f;
        effect->offset.z = drill->position.z - 0.5f;
        return;
    }

    drill->phase = 1;
    drill->position.z = -12.5f;
    effect = drill->effect2;
    effect->hidden = 0;
    effect = save->effect1;
    effect->hidden = 0;
    save->returnCount++;
    if (save->returnLimit == -1) {
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectStopID(0x30098, 0);
        CallMethod((const char *)D_004CAA90);
        return;
    }
    if (save->returnLimit < save->returnCount) {
        camera = xglStudioGetActiveCamera();
        camera->state = 4;
        GameLoopState.flags &= ~0x10000;
        drill->phase = 0;
        GameLoopState.flags |= 0x80000;
        save->hit = 0;
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectStopID(0x30098, 0);
        CallMethod((const char *)D_004CAAA0);
    } else {
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectStopID(0x30098, 0);
        CallMethod((const char *)D_004CAA90);
    }
}

void DrillCameraControlFunc(Drill *drill, DrillCamera *camera)
{
    DrillSave *save;
    DrillCameraAngles *angles = &camera->angles;

    xglFontDebugPrintf(8, 32, (char *) D_004CAAB0);
    save = &drill->save;

    camera->state = 0;
    angles->shakeOffsetStepX = 0.0f;
    angles->shakeOffsetStepY = 0.0f;
    angles->shakeOffsetStepZ = 0.0f;
    angles->shakeVelocityStepX = 0.0f;
    angles->shakeVelocityStepY = 0.0f;
    angles->shakeVelocityStepZ = 0.0f;
    angles->shakeOffsetX = 0.0f;
    angles->shakeOffsetY = 0.0f;
    angles->shakeOffsetZ = 0.0f;
    angles->shakeVelocityX = 0.0f;
    angles->shakeVelocityY = 0.0f;
    angles->shakeVelocityZ = 0.0f;

    if (PadData.pitchStick < 0) {
        angles->pitch += (PadData.pitchStick + PadData.pitchDeadZone) * 0.00052359881f;
    }
    if (PadData.pitchStick > 0) {
        angles->pitch += (PadData.pitchStick - PadData.pitchDeadZone) * 0.00052359881f;
    }

    if (PadData.yawStick < 0) {
        angles->yaw += (PadData.yawStick + PadData.yawDeadZone) * 0.00030000001f;
    }
    if (PadData.yawStick > 0) {
        angles->yaw += (PadData.yawStick - PadData.yawDeadZone) * 0.00030000001f;
    }

    if (angles->yaw > -0.20943953f) {
        angles->yaw = -0.20943953f;
    }
    if (angles->yaw < -1.5707964f) {
        angles->yaw = -1.5707964f;
    }

    if (save->kind == 0) {
        if (angles->pitch < 0.0f) {
            angles->pitch = 0.0f;
        }
        if (angles->pitch > 1.5707964f) {
            angles->pitch = 1.5707964f;
        }
    }

    DrillCalcCameraPos(camera);
}

void DrillCalcCameraPos(DrillCamera *camera)
{
    DrillCameraAngles *angles = &camera->angles;
    float radius;

    /*
     * 0.7f alone folds to 0x3f333333 under this compiler; the original
     * .lit4 constant at 0x004d7fe8 is 0x3f333334, so the literal is
     * spelled with the extra digit that rounds to that exact bit pattern.
     */
    camera->angles.positionX = 0.70000005f;
    camera->angles.positionY = 0.5f;
    camera->angles.positionZ = -17.9f;

    camera->angles.positionY -= xglSin(angles->yaw) * 20.0f;
    radius = xglCos(angles->yaw) * 15.0f;
    camera->angles.positionX += radius * xglSin(angles->pitch);
    camera->angles.positionZ += radius * xglCos(angles->pitch);
    camera->angles.positionX -= 1.2f;
}

void DrillResetCameraPos(Drill *drill, DrillCamera *camera)
{
    camera->state = 0;
    if (drill->save.cameraPreset == 0) {
        camera->angles.yaw = -0.20943953f;
        camera->angles.pitch = 1.5707964f;
        camera->angles.roll = 0.0f;
        DrillCalcCameraPos(camera);
    } else {
        if (drill->save.cameraPreset == 1) {
            camera->angles.pitch = 0.0f;
            camera->angles.yaw = -0.20943953f;
            camera->angles.roll = 0.0f;
            DrillCalcCameraPos(camera);
        } else {
            if (drill->save.cameraPreset == 2) {
                camera->angles.pitch = 0.0f;
                camera->angles.yaw = -1.5707964f;
                camera->angles.roll = 0.0f;
                DrillCalcCameraPos(camera);
            }
        }
    }
}

void DrillResetFlag(Drill *drill)
{
    DrillSave *save = &drill->save;
    int i;

    save->flags &= ~0x1F0;

    for (i = 501; i < 541; i++) {
        xglFlagsSet1(0x79ec7 + i, 0);
    }

    for (i = 0; i < 64; i++) {
        DrillMapUnit *unit = &MapUnit[i];

        if ((s16)unit->serial == 0x7000 || (s16)unit->serial == 0x7009) {
            if ((u32)(unit->info.drill_reset_value - 501) < 50) {
                unit->serial = (u16)-1;
                unit->update = 0;
            }
        }
    }
}

void DrillHitCheck(Drill *drill)
{
    DrillSave *save = &drill->save;
    Vector4 *position = &drill->position.vector;
    int hit;
    DrillMapUnitInfo *container;

    hit = HitCheckContainerPosSize(position, drill->slot, save->hitRadius);
    if (hit != -1) {
        container = &MapUnit[hit].info;
        container->hit = 1;
        save->hitCount++;
        if ((u16)((s16)MapUnit[hit].serial - 0x7014) < 4) {
            sefCreateEffectCf(1752, 0, 0);
        } else if ((u16)((s16)MapUnit[hit].serial - 0x7018) < 12) {
            sefCreateEffectCf(1753, 0, 0);
        } else if ((u16)((s16)MapUnit[hit].serial - 0x7024) < 4) {
            sefCreateEffectCf(1754, 0, 0);
        }
    } else {
        HitCheckEnemy(position, save->hitRadius);
    }
}

int HitCheckContainerPosSize(Vector4 *position, int slot, float radius)
{
    DrillMapUnitHitProbe probe;
    int i;
    DrillMapUnitInfo *container;

    probe.position = *position;
    probe.radius = radius;
    for (i = 0; i < 64; i++) {
        container = &MapUnit[i].info;
        if ((s16)MapUnit[i].serial == -1) {
            continue;
        }
        if (container->hit != 0) {
            continue;
        }
        if (i == slot) {
            continue;
        }
        if (container->containerType == 0) {
            continue;
        }
        if ((MapUnit[i].flags & 0x10000) == 0) {
            continue;
        }
        if (HitCheckMapUnitPosAt(&probe, &MapUnit[i]) != 0) {
            return i;
        }
    }
    return -1;
}

int HitCheckEnemy(Vector4 *position, float radius)
{
    DrillEnemyWork *work;
    int i;

    for (i = 0; i < 64; i++) {
        DrillHitActor *target = &actor[i];

        work = enepc;

        if (target->sound_effect_id == 0) {
            continue;
        }
        if ((target->flags & 8) != 0) {
            continue;
        }
        if (target->position.y + work[target->number].height < position->y) {
            continue;
        }
        if (target->position.y < position->y) {
            continue;
        }
        if (CheckDist2D(position, &target->position) < radius + target->body_radius) {
            target->flags |= 8;
            sefCreateEffectCf(635, &target->position, 0);
            xglSoundEffectNormalID(0x10003, 0);
            Enemy_Command_Freeze(target, 1);
            CallMethod_I(D_004DB7E8, target->methodArg);
            return i;
        }
    }
    return -1;
}
