#include "common.h"
#include "shared.h"
#include "main/init_drill.h"
#include "main/act_1.h"
#include "main/exm_2.h"
#include "main/act_2.h"
#include "main/init_uwamono_sys.h"
#include "main/outer_product.h"
#include "main/sef.h"
#include "main/vibration.h"
#include "main/xgl_2.h"
#include "main/xgl_flags.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

/* The scene record contains a byte offset to its array of 4x4 matrices. */
#define DRILL_SCENE_MATRIX(index)                                           \
    (((DrillMatrix *)((u8 *)GameLoopState.sceneRoot->block                  \
        + GameLoopState.sceneRoot->block->matrixOffset))[index])

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
    AlignedVector4 position;
    unsigned char unmodeled_20[0x84];
    s16 serial;
    unsigned char unmodeled_a6[0x0a];
    float size[4];
    unsigned char unmodeled_c0[0xe0];
    /* DrillResetFlag tests this value against its cleanup range; other uses
     * of this field have not been identified. */
    DrillMapUnitInfo info;
    unsigned char unmodeled_240[0xc0];
} DrillMapUnit;

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

typedef struct Drill Drill;

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
    SchedulerState *effect1;
    int partIndexC;
    int partIndexD;
    int flags;
    int frameLimit;
} DrillSave;

struct Drill {
    unsigned char unmodeled_000[4];
    void *update;
    unsigned char unmodeled_008[8];
    AlignedVector4 position;
    float rotationX;
    float rotationY;
    float rotationZ;
    unsigned char unmodeled_02c[0x54];
    struct DrillMatrix *matrix;
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
    SchedulerState *effect3;
    unsigned char unmodeled_0ec[0xb4];
    DrillSave save;
    unsigned char unmodeled_200[0x20];
    float cameraYaw;
    float cameraPitch;
    unsigned char unmodeled_228[8];
    SchedulerState *effect2;
};

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

/* Partial view of MapUnitRecord (include/main/exm_2.h); kept: switching it changes
 * init_drill.raw .text (measured with tucheck; the size/matrixRef gap names and the
 * DrillContainerInfo cast on +0x1a0 were tried). */
typedef struct MapUnitRecordDrillView {
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
    DoublewordAlignedVector4 size;
    unsigned char unmodeled_c0[0xe0];
    DrillContainerInfo info;
} MapUnitRecordDrillView;

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

typedef struct DrillEnemyWork {
    unsigned char unmodeled_00[0x0c];
    float height;
    unsigned char unmodeled_10[0x38a0];
} DrillEnemyWork;

typedef struct DrillGameLoopState {
    unsigned char unmodeled_000[4];
    Actor *playerActor;
    unsigned char unmodeled_008[8];
    u32 flags;
    unsigned char unmodeled_014[0x40];
    DrillSceneRoot *sceneRoot;
} DrillGameLoopState;

typedef struct DrillMapUnitHitProbe {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    unsigned char unmodeled_20[0x9c8];
    float radius;
    unsigned char unmodeled_9ec[0x84];
} DrillMapUnitHitProbe;

extern DrillMapUnit MapUnit[64];
extern DrillPadLayout PadData;
extern DrillItemBox ItemBoxTbl[];
extern Actor actor[64];
extern DrillEnemyWork enepc[16];

/* These unchanged data records are supplied by the original scaffolding. */
extern const char initDrillDebugFormat[];

extern const char rareItemPlacedFormat[];
extern const char elapsedTimeText[];
extern const char drillTimeoverMethodName[];
extern const char timeRemainingText[];
extern char drillTextControl[];
extern char enemyBrokenMethodName[];
extern const char drillPowerOffText[];
extern const char drillStopText[];
extern const char drillStandbyText[];
extern const char drillStopMethodName[];
extern const char setContainerFormat[];
extern DrillGameLoopState GameLoopState;
extern const char drillZMoveText[];
extern const char drillZStandbyText[];
extern const char drillXMoveText[];
extern const char drillCrashFormat[];
extern const char drillReturnText[];
extern const char drillStandbyMethodName[];
extern const char drillEndMethodName[];
extern const char cameraControlText[16];
int HitCheckContainerPosSize(Vector4 *position, int slot, float radius);
int HitCheckEnemy(Vector4 *position, float radius);
DrillCamera *xglStudioGetActiveCamera(void);
void DrillHitCheck(Drill *drill);
void DrillCalcCameraPos(DrillCamera *camera);
void DrillResetCameraPos(Drill *drill, DrillCamera *camera);
void DrillCameraControlFunc(Drill *drill, DrillCamera *camera);
void *Unit_CreateUwamono(int identifier, int unit_id);
void InitItemSymbol(void *item);
void DrillClearContainer(void);
void SetContainer(int count);
void SetContainerItem(MapUnitRecordDrillView *unit, int value);
void CreateUwamonoCommon(MapUnitRecordDrillView *unit);
void MAP_updateUnitTrap(void);
void SetContainerParam(MapUnitRecordDrillView *unit);
int CalcPosContainer(MapUnitRecordDrillView *unit);
void DrillPowerOffFunc(Drill *drill);
void DrillStandbyFunc(Drill *drill);
void DrillZMoveFunc(Drill *drill);
void DrillZStopFunc(Drill *drill);
void DrillXMoveFunc(Drill *drill);
void DrillCrashFunc(Drill *drill);
void DrillReturnFunc(Drill *drill);
void MAP_updateUnitDrill(Drill *drill);
void DrillResetFlag(Drill *drill);
void GetPartsPos(Drill *drill);
int printf(const char *format, ...);
int HexToStr(int value, char *text);
void xglFontPrintDirectOT(int ot, const char *text);
void Enemy_Command_Freeze(Actor *target, int frozen);
void CallMethod_I(const char *method, int argument);
int CreateContainer(int, int);
extern void CallMethod(const char *method);
int HitCheckMapUnitPosAt(DrillMapUnitHitProbe *probe, DrillMapUnit *unit);

DrillContainerDef container[31] = { { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.15000000596046448f, .y = 1.0f, .z = 0.15000000596046448f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 628, .itemBase = 501, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.15000000596046448f, .y = 1.0f, .z = 0.15000000596046448f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 628, .itemBase = 503, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.15000000596046448f, .y = 1.0f, .z = 0.15000000596046448f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 628, .itemBase = 505, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.15000000596046448f, .y = 1.0f, .z = 0.15000000596046448f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 628, .itemBase = 507, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.25f, .y = 1.5f, .z = 0.25f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 602, .itemBase = 509, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.25f, .y = 1.5f, .z = 0.25f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 602, .itemBase = 511, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.25f, .y = 1.5f, .z = 0.25f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 602, .itemBase = 513, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.25f, .y = 1.5f, .z = 0.25f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 602, .itemBase = 515, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.5f, .y = 1.0f, .z = 0.5f, .w = 0.0f }, .model = { .hitMode = 2, .unmodeled_01 = { 0, 0, 0 }, .modelId = 648, .itemBase = 517, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.5f, .y = 1.0f, .z = 0.5f, .w = 0.0f }, .model = { .hitMode = 2, .unmodeled_01 = { 0, 0, 0 }, .modelId = 648, .itemBase = 519, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.5f, .y = 1.0f, .z = 0.5f, .w = 0.0f }, .model = { .hitMode = 2, .unmodeled_01 = { 0, 0, 0 }, .modelId = 648, .itemBase = 521, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.5f, .y = 1.0f, .z = 0.5f, .w = 0.0f }, .model = { .hitMode = 2, .unmodeled_01 = { 0, 0, 0 }, .modelId = 648, .itemBase = 523, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.5f, .y = 1.5f, .z = 0.5f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 602, .itemBase = 525, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.5f, .y = 1.5f, .z = 0.5f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 602, .itemBase = 527, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.5f, .y = 1.5f, .z = 0.5f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 602, .itemBase = 529, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.5f, .y = 1.5f, .z = 0.5f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 602, .itemBase = 531, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =1.0f, .y = 1.5f, .z = 1.0f, .w = 0.0f }, .model = { .hitMode = 1, .unmodeled_01 = { 0, 0, 0 }, .modelId = 614, .itemBase = 533, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =1.0f, .y = 1.5f, .z = 1.0f, .w = 0.0f }, .model = { .hitMode = 1, .unmodeled_01 = { 0, 0, 0 }, .modelId = 614, .itemBase = 535, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =1.0f, .y = 1.5f, .z = 1.0f, .w = 0.0f }, .model = { .hitMode = 1, .unmodeled_01 = { 0, 0, 0 }, .modelId = 642, .itemBase = 537, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =1.0f, .y = 1.5f, .z = 1.0f, .w = 0.0f }, .model = { .hitMode = 1, .unmodeled_01 = { 0, 0, 0 }, .modelId = 642, .itemBase = 539, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.5f, .y = 1.0f, .z = 0.5f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 629, .itemBase = 1, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =1.0f, .y = 1.0f, .z = 1.0f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 632, .itemBase = 3, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =1.5f, .y = 1.0f, .z = 1.5f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 635, .itemBase = 5, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.5f, .y = 1.0f, .z = 0.5f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 601, .itemBase = 7, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =1.0f, .y = 1.0f, .z = 1.0f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 604, .itemBase = 9, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.15000000596046448f, .y = 1.5f, .z = 0.15000000596046448f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 601, .itemBase = 1, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.25f, .y = 1.5f, .z = 0.25f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 601, .itemBase = 3, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.40000000596046448f, .y = 1.5f, .z = 0.40000000596046448f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 601, .itemBase = 7, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.5f, .y = 1.5f, .z = 0.5f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 601, .itemBase = 8, .unmodeled_0c = 0 } }, { .type = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =1.0f, .y = 1.5f, .z = 1.0f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 604, .itemBase = 9, .unmodeled_0c = 0 } }, { .type = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .size = { .x =0.15000000596046448f, .y = 1.0f, .z = 0.15000000596046448f, .w = 0.0f }, .model = { .hitMode = 0, .unmodeled_01 = { 0, 0, 0 }, .modelId = 628, .itemBase = 10, .unmodeled_0c = 0 } } };

static int *pDrillFlag;

void InitDrill(Drill *drill)
{
    DrillSave *save = &drill->save;
    SchedulerState *effect;

    printf(initDrillDebugFormat, save->partIndexA, save->partIndexB, drill->serial);
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
    effect->position.x = -3.2f;
    effect->position.y = 6.67f;
    effect->position.z = -12.5f;
    effect->display =0;

    effect = sefCreateEffectCf(1661, 0, 0);
    if (effect == 0) {
        effect = sefCreateEffectCf(1539, 0, 0);
    }
    drill->effect2 = effect;
    effect->position.z = -12.5f;
    effect->position.x = -3.03f;
    effect->position.y = 6.23f;
    effect->display =0;

    effect = sefCreateEffectCf(1581, 0, 0);
    save->effect1 = effect;
    effect->display =0;
    DrillResetFlag(drill);
}

void SetContainer(int count)
{
    printf(setContainerFormat);
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
    MapUnitRecordDrillView *unit;
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
        slot = xglSRand() % 5U;
        do {
            pick = (slot + tries) % 5;
            tries++;
            slot = pick;
            if (!(*pDrillFlag & (0x10 << pick))) {
                *pDrillFlag |= 0x10 << pick;
                item = pick + 0x21D;
                if (xglFlagsGet1(pick + 0x7A0E4) == 0) {
                    SetContainerItem(unit, item);
                    printf(rareItemPlacedFormat, item);
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
        SetContainerItem(unit, base);
    } else {
        base++;
        weight += table[base].weight;
        if (roll < weight) {
            SetContainerItem(unit, base);
        }
    }
    return (int)unit;
}

void DrillClearContainer(void)
{
    int i;

    for (i = 0; i < 64; i++) {
        DrillMapUnit *unit = &MapUnit[i];

        if ((u16)(unit->serial - 0x7014) < 0x14) {
            unit->serial = -1;
        } else if ((unit->flags & 0x100000) != 0) {
            unit->serial = -1;
        } else {
            continue;
        }
        unit->update = 0;
    }
}

void SetContainerParam(MapUnitRecordDrillView *unit)
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

void SetContainerItem(MapUnitRecordDrillView *container, int value)
{
    DrillItemUnit *item;

    item = Unit_CreateUwamono(-1, 0x7000);
    if (item != 0) {
        DrillItemInfo *info = &item->info;

        info->value = value;
        info->state = 5;
        InitItemSymbol(item);
        container->info.spawnedItemKind = item->itemKind;
        SetHideObject(container);
    }
}

int CalcPosContainer(MapUnitRecordDrillView *unit)
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
        radius = unit->size.x;
        if (info->containerType == 2) {
            radius *= 1.5f;
        }
        hit = HitCheckContainerPosSize(&unit->position, unit->slot, radius);
        if (hit == -1) {
            return 1;
        }
        distance = CheckDist2D(&unit->position, &MapUnit[hit].position);
        if (distance < MapUnit[hit].size[0] - radius) {
            unit->position.y = MapUnit[hit].position.y + MapUnit[hit].size[1];
            hit =HitCheckContainerPosSize(&unit->position, unit->slot, radius);
            if (hit == -1) {
                return 1;
            }
            distance = CheckDist2D(&unit->position, &MapUnit[hit].position);
            if (distance < MapUnit[hit].size[0] - radius) {
                unit->position.y = MapUnit[hit].position.y + MapUnit[hit].size[1];
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
        DrillReturnFunc(drill);
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
                xglFontPrint(336, 24, 0xFFF0, elapsedTimeText);
                xglFontPrint(472 - length * 10, 24, 0xFFF0, text);
                xglFontPrintDirectOT(0xFFF0, drillTextControl);
            } else {
                seconds = (int)((float)(save->frameLimit - save->frameCount) / 30.0f);
                seconds = seconds < 1000 ? seconds : 999;
                if (seconds <= 0) {
                    if (!(flags & 0x80000000)) {
                        CallMethod(drillTimeoverMethodName);
                        save->flags |= 0x80000000;
                        if (drill->phase != 5) {
                            SchedulerState *effect;

                            drill->phase = 6;
                            effect = drill->effect3;
                            effect->display =0;
                            effect = save->effect1;
                            effect->display =0;
                        }
                    }
                    seconds = 0;
                }
                length = HexToStr(seconds, text);
                xglFontPrint(328, 24, 0xFFF0, timeRemainingText);
                xglFontPrint(480 - length * 10, 24, 0xFFF0, text);
                xglFontPrintDirectOT(0xFFF0, drillTextControl);
            }
        }
        if (save->command != 2) {
            save->frameCount++;
        }
    }

    matrix = drill->matrix;
    matrix->m[3][0] = drill->position.x;
    matrix->m[3][1] = drill->position.y;
    part = &DRILL_SCENE_MATRIX(save->partIndexA);
    part->m[3][2] = matrix->m[3][2] = drill->position.z;
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
    Actor *actor = GameLoopState.playerActor;
    DrillCamera *camera;
    float initialAngle;
    SchedulerState *effect;
    signed char hit;

    xglFontDebugPrintf(8, 16, drillPowerOffText);
    drill->position.x = -2.9f;
    drill->position.y = 4.5f;
    drill->position.z = -12.5f;
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
    effect->display =0;
    effect = save->effect1;
    effect->display =0;
}

void DrillStandbyFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillCamera *camera = xglStudioGetActiveCamera();

    if (save->command == 2) {
        save->cameraPreset = 1;
        xglFontDebugPrintf(8, 16, drillStopText);
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
        xglFontDebugPrintf(8, 16, drillStandbyText);
        if (save->flags >= 0) {
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
                CallMethod(drillStopMethodName);
            }
        }
    }
}

void DrillZMoveFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    SchedulerState *effect;

    xglFontDebugPrintf(8, 16, drillZMoveText);
    Vibration_Set_Strong(128, 128, 2);
    save->cameraPreset = 1;

    if ((PadData.half_28 & 0x80) != 0) {
        drill->position.z -= save->zSpeed;
    } else {
        drill->phase = 3;
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectNormalID(0x30097, 0);
    }

    if (drill->position.z < -23.3f) {
        drill->position.z = -23.3f;
        drill->phase = 3;
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectNormalID(0x30097, 0);
    }

    effect = drill->effect2;
    sefRewindEffectCf(effect);
    effect->display =1;
    effect->position.z = drill->position.z;

    effect = save->effect1;
    sefRewindEffectCf(effect);
    effect->display =1;
    effect->position.x = -4.0f;
    effect->position.y = 1.5f;
    effect->position.z = drill->position.z + 0.5f;
}

void DrillZStopFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillCamera *camera;
    SchedulerState *effect;

    xglFontDebugPrintf(8, 16, drillZStandbyText);
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
    effect->display =0;
    effect = save->effect1;
    effect->display =0;
}

void DrillXMoveFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillCamera *camera;
    SchedulerState *effect;

    camera = xglStudioGetActiveCamera();
    xglFontDebugPrintf(8, 16, drillXMoveText);
    Vibration_Set_Strong(128, 128, 2);

    if ((PadData.half_28 & 0x80) != 0) {
        drill->position.x += save->xSpeed;
    } else if (drill->position.x < -1.9000001f) {
        drill->position.x += save->xSpeed;
    } else {
        drill->phase = 5;

        effect = sefCreateEffectCf(1594, 0, 0);
        effect->position = drill->position;
        effect->flags &= ~0x10;
        effect->position.y = 0.0f;

        effect = sefCreateEffectCf(1657, 0, 0);
        effect->flags &= ~0x400;
        effect->casterUnit = (MapUnitRecord *)drill;

        drill->cameraYaw = camera->angles.yaw;
        drill->cameraPitch = camera->angles.pitch;

        xglSoundEffectStopID(0x30098, 0);
        xglSoundEffectNormalID(0x30099, 0);
        return;
    }

    if (drill->position.x > 4.3f) {
        drill->position.x = 4.3f;
        drill->phase = 5;

        effect = sefCreateEffectCf(1594, 0, 0);
        effect->position = drill->position;
        effect->flags &= ~0x10;
        effect->position.y = 0.0f;

        effect = sefCreateEffectCf(1657, 0, 0);
        effect->flags &= ~0x400;
        effect->casterUnit = (MapUnitRecord *)drill;

        drill->cameraYaw = camera->angles.yaw;
        drill->cameraPitch = camera->angles.pitch;

        xglSoundEffectStopID(0x30098, 0);
        xglSoundEffectNormalID(0x30099, 0);
    }

    effect = drill->effect3;
    sefRewindEffectCf(effect);
    effect->display =1;
    effect->position.x = drill->position.x;
    effect->position.z = drill->position.z;

    effect = save->effect1;
    sefRewindEffectCf(effect);
    effect->display =1;
    effect->position.x = drill->position.x;
    effect->position.y = 6.0f;
    effect->position.z = drill->position.z;
}

void DrillCrashFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    DrillCamera *camera;
    SchedulerState *effect;

    effect = drill->effect3;
    effect->display =0;
    effect = save->effect1;
    effect->display =0;
    camera = xglStudioGetActiveCamera();
    xglFontDebugPrintf(8, 16, drillCrashFormat, save->hitCount);

    drill->crashTimer++;
    drill->rotationY += drill->crashTimer * 0.1f;
    drill->rotationX = 0.0f;
    drill->rotationZ = 0.0f;
    xglMatrixStackUnit();
    xglMatrixStackRotY(drill->rotationY);
    xglMatrixStackSave(drill->matrix->m);

    camera->angles.positionX = drill->position.x + 4.0f;
    camera->angles.positionY = drill->position.y + 5.0f;
    camera->angles.positionZ = drill->position.z - 4.0f;
    camera->angles.yaw = -0.78539819f;
    camera->angles.pitch = 2.3561945f;
    camera->angles.roll = 0.0f;

    if (drill->crashTimer >= 16) {
        drill->crashTimer = 16;
        drill->position.y -= save->fallSpeed;
        DrillHitCheck(drill);
        if (drill->position.y < 0.0f) {
            if (drill->bounceCount == 0) {
                sefCreateEffectCf(1721, &drill->position, 0);
            }
            drill->bounceCount++;
            drill->position.y = 0.0f;
            if (drill->bounceCount >= 9) {
                camera->angles.yaw = drill->cameraYaw;
                camera->angles.pitch = drill->cameraPitch;
                DrillCalcCameraPos(camera);
                drill->phase = 6;
            }
        }
    }
}

void DrillReturnFunc(Drill *drill)
{
    DrillSave *save = &drill->save;
    SchedulerState *effect;
    DrillCamera *camera;

    xglFontDebugPrintf(8, 16, drillReturnText);
    if (drill->crashTimer > 0) {
        drill->crashTimer--;
        drill->rotationY += drill->crashTimer * 0.1f;
        drill->rotationX = 0.0f;
        drill->rotationZ = 0.0f;
        xglMatrixStackUnit();
        xglMatrixStackRotY(drill->rotationY);
        xglMatrixStackSave(drill->matrix->m);
        DrillHitCheck(drill);
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
        sefRewindEffectCf(effect);
        effect->display =1;
        effect->position.x = drill->position.x;
        effect->position.z = drill->position.z;
        effect = save->effect1;
        sefRewindEffectCf(effect);
        effect->display =1;
        effect->position.x = drill->position.x;
        effect->position.y = 6.0f;
        effect->position.z = drill->position.z;
        return;
    }

    drill->position.x = -2.9000001f;
    effect = drill->effect3;
    effect->display =0;
    if (drill->position.z < -12.5f) {
        drill->position.z += save->zReturnSpeed;
        if (drill->position.z >= -12.5f) {
            drill->position.z = -12.5f;
            xglSoundEffectStopID(0x30096, 0);
            xglSoundEffectNormalID(0x30097, 0);
        }
        effect = drill->effect2;
        sefRewindEffectCf(effect);
        effect->display =1;
        effect->position.z = drill->position.z;
        effect = save->effect1;
        effect->position.x = -4.0f;
        effect->position.y = 1.5f;
        effect->position.z = drill->position.z - 0.5f;
        return;
    }

    drill->phase = 1;
    drill->position.z = -12.5f;
    effect = drill->effect2;
    effect->display =0;
    effect = save->effect1;
    effect->display =0;
    save->returnCount++;
    if (save->returnLimit == -1) {
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectStopID(0x30098, 0);
        CallMethod(drillStandbyMethodName);
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
        CallMethod(drillEndMethodName);
    } else {
        xglSoundEffectStopID(0x30096, 0);
        xglSoundEffectStopID(0x30098, 0);
        CallMethod(drillStandbyMethodName);
    }
}

void DrillCameraControlFunc(Drill *drill, DrillCamera *camera)
{
    DrillSave *save;
    DrillCameraAngles *angles = &camera->angles;

    xglFontDebugPrintf(8, 32, cameraControlText);
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

        if (unit->serial == 0x7000 || unit->serial == 0x7009) {
            if ((u32)(unit->info.drill_reset_value - 501) < 50) {
                unit->serial = -1;
                unit->update = 0;
            }
        }
    }
}

void DrillHitCheck(Drill *drill)
{
    DrillSave *save = &drill->save;
    Vector4 *position = &drill->position;
    int hit;
    DrillMapUnitInfo *container;

    hit = HitCheckContainerPosSize(position, drill->slot, save->hitRadius);
    if (hit != -1) {
        container = &MapUnit[hit].info;
        container->hit = 1;
        save->hitCount++;
        if ((u16)(MapUnit[hit].serial - 0x7014) < 4) {
            sefCreateEffectCf(1752, 0, 0);
        } else if ((u16)(MapUnit[hit].serial - 0x7018) < 12) {
            sefCreateEffectCf(1753, 0, 0);
        } else if ((u16)(MapUnit[hit].serial - 0x7024) < 4) {
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
        if (MapUnit[i].serial == -1) {
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
        Actor *target = &actor[i];

        work = enepc;

        if (target->inUseId == 0) {
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
            CallMethod_I(enemyBrokenMethodName, target->args.methodArg);
            return i;
        }
    }
    return -1;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char initDrillDebugFormat[32] = "InitDrill D1=%d D2=%d D3=%d\n";

const char setContainerFormat[] = "SetContainer\n";

/* JP: うーくんボールに%dのレアアイテムが入った\n
 * EN: Rare item %d was placed in the U-kun Ball.\n */
const char rareItemPlacedFormat[48] = "\244\246\241\274\244\257\244\363\245\334\241\274\245\353\244\313%d\244\316\245\354\245\242\245\242\245\244\245\306\245\340\244\254\306\376\244\303\244\277\n";

/* Text: {effect 0}{style 3}Elapsed Time{proportional 1} */
const char elapsedTimeText[24] = "\r\000\031\003Elapsed Time\025\002\004\001";

const char drillTimeoverMethodName[16] = "drill_timeover";

/* Text: {effect 0}{style 3}Time Remaining{proportional 1} */
const char timeRemainingText[24] = "\r\000\031\003Time Remaining\025\002\004\001";

const char drillPowerOffText[16] = "DRILL POWEROFF";

const char drillStopText[16] = "DRILL STOP";

const char drillStandbyText[16] = "DRILL STANDBY";

const char drillStopMethodName[16] = "drill_stop";

const char drillZMoveText[] = "DRILL ZMOVE";

const char drillZStandbyText[] = "DRILL ZSTANDBY";

const char drillXMoveText[] = "DRILL XMOVE";

const char drillCrashFormat[] = "DRILL CRASH %2d";

const char drillReturnText[] = "DRILL RETURN";

const char drillStandbyMethodName[] = "drill_stanby";

const char drillEndMethodName[] = "drill_end";

const char cameraControlText[16] = "CAMERA CONTROL";

/* Text: {proportional 0} */
char drillTextControl[8] = "\025\002\004";

char enemyBrokenMethodName[8] = "broken";

short Times = 0;

short NowTester = 0;
