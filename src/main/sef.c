#include "common.h"
#include "main/xgl_2.h"
#include "shared.h"
#include "sef.h"
#include "main/srs.h"
#include "main/sef.h"
#include "main/act_1.h"
#include "main/act_2.h"
#include "main/exm_2.h"
#include "main/m_math.h"
#include "main/matrix_print.h"
#include "main/sdv.h"
#include "main/sef_print.h"
#include "main/sm_initilize.h"
#include "main/sv_get_image_item.h"
#include "main/xgl_sound.h"
#include "quadword.h"

/* Original-backed literal aliases; keep the accepted function bodies intact. */
#define circle_angle_scale_literal (0.01745329238474369f)

#define effect_scale_literal_0_1 (0.10000000149011612f)
#define DEG_TO_RAD (0.01745329238474369f)
#define lit4_004d8354 (0.01745329238474369f)
#define SEF_ACTOR_LIGHT_FLAG 0x00008000u
#define SEF_BATTLE_ACTOR_MAX 24
#define SEF_LINE_DATA_RECORD_SIZE 0x820
#define SEF_LINE_DATA_COUNT 0x80
#define SEF_DRAW_FLAG_PENDING 0x04000000

#define SEF_BATTLE_PHASE         0x00c

#define SEF_BATTLE_MATRIX_SOURCE 0x200
#define SEF_BATTLE_RUNNING       0x22c
#define SEF_LOCAL_OFFSET_Z       0x0f8
#define SEF_PARENT_DIR_MATRIX    0x120

#define SEF_LOCAL_DATA_PT_TABLE 0x600
#define SEF_SCHEDULER_REWIND_FLAG 1
#define SEF_RECORD_ANCHOR 0xa70
#define SEF_RECORD_LIVE (0x6b0 - SEF_RECORD_ANCHOR)
#define SEF_RECORD_CHARACTER_ID (0xa74 - SEF_RECORD_ANCHOR)
#define SEF_RECORD_EFFECT_NO (0xa78 - SEF_RECORD_ANCHOR)
#define SEF_SCHEDULER_RECORD_SIZE 0xab0
#define SEF_SCHEDULER_COUNT 0x80
#define SEF_TARGET_DAMAGE_NULL_A 527
#define SEF_TARGET_DAMAGE_NULL_B 528

/* _ptAlloc's per-record byte size, the same 640 sefCalcLocalMatrix already
 * multiplies _nowParentLocal by. */
#define SEF_PT_ALLOC_RECORD_SIZE 640

#define SEF_PT_ALLOCATOR ((SefPtAllocatorState *)(void *)_ptAlloc)
#define SEF_SAVE_PRM_COUNT 0x80
#define UNIFORM_SCALE_KEY_UNIT (0.009999999776482582f)
#define PARTICLE_SIZE_SCALE (0.009999999776482582f)
#define INV_32768 (3.0518509447574615e-05f)
#define RANDOM_JITTER_SCALE (3.0518509447574615e-05f)

typedef struct SefBossDeathEffect {
    short character_id;
    unsigned short effect_no;
} SefBossDeathEffect;

/*
 * sefKillEffect walks an interior cursor at SchedulerState
 * +0xa70 (request.character_id at +4, request.effect_no at +8, inUse at
 * -0x3c0); SchedulerState has no member at +0xa70, and every index or
 * SchedulerState-cursor form tried either moves that base or the function's
 * label count, so this partial view of the record tail stays local to it.
 */
typedef struct SefEffectControl {
    unsigned char unmodeled_00[4];
    short character_id;
    unsigned char unmodeled_06[2];
    short effect_no;
    unsigned char unmodeled_0a[0x40 - 0x0a];
} SefEffectControl;

typedef struct SefEffectScheduler {
    unsigned char unmodeled_000[0x6b0];
    int inUse;
    unsigned char unmodeled_6b4[0xa70 - 0x6b4];
    SefEffectControl effect;
} SefEffectScheduler;

typedef struct SefBattleActor {
    unsigned char unmodeled_000[0x20];
    Vector4 light_direction;         /* +0x20 */
    unsigned char unmodeled_030[0x20];
    Vector4 light_accumulator;       /* +0x50 */
    Vector4 light_position;          /* +0x60 */
    Vector4 actor_position;          /* +0x70 */
    struct Actor *actor;
    short excluded;
    short lightCount;
    short hitSignal;
    short schedulerLightCount;
    short seSignal;
    unsigned char unmodeled_08e[2];
} SefBattleActor;

typedef struct SefBattleActorTbl {
    SefBattleActor actors[SEF_BATTLE_ACTOR_MAX];
    Vector4 center;
    int count;
    unsigned char unmodeled_d94[0x0c];
} SefBattleActorTbl;

typedef struct SefLineDataEntry {
    unsigned char unmodeled_000[2];
    unsigned short inUse;
    unsigned char unmodeled_004[SEF_LINE_DATA_RECORD_SIZE - 4];
} SefLineDataEntry;

typedef struct SefLineDataTable {
    unsigned char unmodeled_000[0x810];
    SefLineDataEntry entries[SEF_LINE_DATA_COUNT];
} SefLineDataTable;

typedef struct GameLoopFlagsView {
    unsigned char unmodeled_00[0x10];
    int flags;
} GameLoopFlagsView;

typedef struct SefKey3 {
    short frame;
    short value;
    short jump;
} SefKey3;

/* The key cursor shared by every progress state: the key index and the frame. */
typedef struct SefProgressCursor {
    int key_index;
    int frame;
} SefProgressCursor;

typedef union SefIntFloatVector {
    int ints[4];
    Vector4 floats;
} SefIntFloatVector;

typedef struct SefProgressState {
    SefProgressCursor cursor;
    int value;
    unsigned char unmodeled_0c[4];
    SefIntFloatVector vector;
} SefProgressState;

/* The float-valued view of SefProgressState used by the line-data scale keys. */
typedef struct SefFloatProgressState {
    SefProgressCursor cursor;
    float value;
} SefFloatProgressState;

typedef struct SefIntegerProgressState {
    SefProgressCursor cursor;
    int value;
} SefIntegerProgressState;

/*
 * GNU EE native TI storage/copy type (docs/native-ti.md), used only for the
 * 16-byte aligned zero-fill blocks sefMemZero clears with por+sq; no wide
 * arithmetic is done on it.
 */
typedef unsigned int Quadword __attribute__((mode(TI)));

typedef union SefQuadBlock {
    Quadword quad;
    unsigned int words[4];
} SefQuadBlock;

/*
 * A 10-byte keyed-animation record (config/units, sefProgressKey5): the
 * end_frame this function tests against 1024 and the following record's own
 * jump index are the only fields any accepted function of this TU touches;
 * the six bytes between them are not yet evidenced.
 */
typedef struct SefKey5 {
    short end_frame;
    short vector[3];
    short jump;
} SefKey5;

typedef unsigned int SefPackedAnimation __attribute__((mode(TI)));

typedef struct SefKey5Prefix8 {
    unsigned int unmodeled_000[2];
    SefKey5 keys[1];
} SefKey5Prefix8;

typedef struct SefKey3Table {
    unsigned int unmodeled_000;
    SefKey3 keys[1];
} SefKey3Table;

typedef struct SefModeKeyTable {
    short mode;
    unsigned char unmodeled_002[6];
    SefKey5 keys[1];
} SefModeKeyTable;

typedef struct SefLightInput {
    int coordinates[3];
    int range;
} SefLightInput;

/*
 * SefParticleOffset (sefGetStartPos/sefGetTargetPos, main:0x002e2960 and
 * main:0x002e29b0): the only fields sefCreateParticle's effect record these
 * two functions evidence -- a start and a target position vector, and a
 * start and a target offset-table/index pair consumed opaquely by
 * sefGetOfsRange. The bytes between and around them are
 * not evidenced by any accepted function in this TU.
 */
typedef struct SefParticleOffset {
    unsigned char unmodeled_00[0x30];
    Vector4 start_position;                  /* +0x030 */
    unsigned char unmodeled_40[0x10];
    Vector4 target_position;                 /* +0x050 */
    unsigned char unmodeled_60[0x110];
    unsigned char start_offset_table[0x20];  /* +0x170 */
    unsigned char target_offset_table[0x20]; /* +0x190 */
    unsigned char unmodeled_1b0[0x24];
    short *start_index;                      /* +0x1D4 */
    unsigned char unmodeled_1d8[4];
    short *target_index;                     /* +0x1DC */
} SefParticleOffset;

/* One line-data record (SEF_LINE_DATA_RECORD_SIZE bytes): the particle-allocator
 * handle table sefFreeLocalData walks and the cursor sefAllocLocalData scans from. */
typedef struct SefLocalData {
    unsigned char *position_keys;                    /* +0x000 */
    short *spread_rotation_keys;                     /* +0x004 */
    unsigned char *size_keys;                        /* +0x008 */
    unsigned char *signal_keys;                      /* +0x00c */
    unsigned char unmodeled_010[0x14 - 0x10];
    unsigned char *rotation_keys;                    /* +0x014 */
    unsigned char unmodeled_018[0x20 - 0x18];
    unsigned short initial_frame;                    /* +0x020 */
    short frame_count;                               /* +0x022 */
    unsigned char unmodeled_024[0x1e0 - 0x24];
    short *color_keys;                               /* +0x1e0 */
    short *uniform_scale_keys;                       /* +0x1e4 */
    unsigned char unmodeled_1e8[SEF_LOCAL_DATA_PT_TABLE - 0x1e8];
    short pointHandles[0x100];                         /* +0x600 */
    unsigned char unmodeled_800[0x80e - 0x800];
    short cursor;                                      /* +0x80e */
} SefLocalData;

/* Only fields read or written by sefCreateParticle are named here. */
typedef struct SefParticle {
    Matrix4 local_matrix;                            /* +0x000 */
    Matrix4 transform;                               /* +0x040 */
    Matrix4 base_matrix;                              /* +0x080 */
    Vector4 start_position;                           /* +0x0c0 */
    Vector4 target_position;                           /* +0x0d0 */
    Vector4 random_rotation;                           /* +0x0e0 */
    Vector4 rotated_position;                          /* +0x0f0 */
    float animated_color_xyz[3];                       /* +0x100 */
    float one_initialized_scalar;                     /* +0x10c */
    unsigned char unmodeled_110[0x130 - 0x110];
    Vector4 random_translation;                        /* +0x130 */
    SefQuadBlock animated_scale;                      /* +0x140 */
    float random_scale;                               /* +0x150 */
    float battle_scale;                               /* +0x154 */
    int kind;                                         /* +0x158 */
    short frame;                                      /* +0x15c */
    short state;                                      /* +0x15e */
    short animate_advance;                            /* +0x160 */
    unsigned char signal;                             /* +0x162 */
    unsigned char unmodeled_163[0x188 - 0x163];
    short rotation_state;                             /* +0x188 */
    unsigned char unmodeled_18a[0x1a0 - 0x18a];
    int color[4];                                     /* +0x1a0 */
    unsigned char unmodeled_1b0[0x1c0 - 0x1b0];
    Vector4 rotation;                                 /* +0x1c0 */
    unsigned char unmodeled_1d0[0x1e0 - 0x1d0];
    union {
        Vector4 color_scale;                          /* +0x1e0 */
        SefPackedAnimation color_scale_bits;
    };
    unsigned char unmodeled_1f0[0x200 - 0x1f0];
    Vector4 color_offset;                             /* +0x200 */
    union {
        SefProgressState light_progress;              /* +0x210, sefLerpIVector2 */
        struct {
            unsigned char unmodeled_210[0x220 - 0x210];
            SefLightInput light;                      /* +0x220 */
        };
    };
    unsigned char unmodeled_230[0x240 - 0x230];
    Vector4 size;                                     /* +0x240 */
    SefIntegerProgressState first_integer_progress;                         /* +0x250 */
    SefIntegerProgressState second_integer_progress;                        /* +0x25c */
    SefFloatProgressState float_progress;             /* +0x268 */
} SefParticle;

typedef struct {
    unsigned char unmodeled_00[0x2c];
    void (*battleDrawCallback)(void);              /* +0x2c */
    unsigned char unmodeled_30[0x38 - 0x30];
    void (*cfDrawCallback)(void);                    /* +0x38 */
} SefRenderState;

typedef struct SefPtAllocatorState {
    unsigned char records[0xA0000];
    short freeSlots[0x400];
    unsigned int head;
    unsigned int tail;
    unsigned char unmodeled_a0808[8];
} SefPtAllocatorState;

typedef struct SefBattleParameters {
    short player_character_ids[3];
    short enemy_character_ids[3];
    short enemy_effect_numbers[3];
    short player_setup_parameters[3][3];
    short player_active[3];
    short enemy_active[3];
    int has_boss;
} SefBattleParameters;

typedef struct SefAnalyzeRecord {
    short time;
    short image_id;
    short alt_image_id;
    unsigned char unmodeled_06;
    signed char flag;
} SefAnalyzeRecord;

typedef struct SefImageItem {
    void *data;
    unsigned char unmodeled_04[0x08];
    unsigned short u12;
    unsigned char unmodeled_0e[4];
    unsigned short u18;
    unsigned short u20;
    unsigned short u22;
    unsigned short u24;
    unsigned short u26;
    unsigned short u28;
    unsigned short u30;
} SefImageItem;

typedef struct SefAnimRecord {
    short frame;
    unsigned char unmodeled_02[4];
    signed char jump;
    unsigned char unmodeled_07;
} SefAnimRecord;

typedef struct SefAnimBlock {
    int item_a_word;
    int item_b_word;
    unsigned long long packed_params;
    float scale_u;
    float scale_v;
    short item_u12;
    short total_time;
    int first_flag;
} SefAnimBlock;

typedef struct SefAnimCursor {
    int key_index;
    SefAnimBlock block;
} SefAnimCursor;

/* sefPushEffect's saved copy of a live scheduler, restored by sefPopEffect. */
typedef struct SefSavePrm {
    Vector4 position;
    Vector4 rotation;
    Vector4 scale;
    unsigned char *savedActive;
    struct Actor *caster;
    struct Actor *target;
    struct MapUnitRecord *casterUnit;
    struct MapUnitRecord *targetUnit;
    int flags;
    short effectNo;
    signed char display;
    unsigned char unmodeled_4b[5];
} SefSavePrm;

struct SefKey5Table {
    unsigned int unmodeled_000;
    SefKey5 keys[1];
};

typedef struct SefLineData {
    SefKey5Prefix8 *initial_vector_keys;
    SefModeKeyTable *rotation_vector_keys;
    SefKey5 *position_vector_keys;
    SefKey3Table *first_integer_keys_local;
    SefKey3 *second_integer_keys_local;
    SefKey5 *size_vector_keys;
    SefKey3Table *float_keys;
    SefKey5 *light_keys;
    unsigned short script_parameters[2];
    unsigned char unmodeled_024[0x0c];
    Vector4 global_position;                 /* +0x30 */
    unsigned char unmodeled_040[0x10];
    Vector4 parent_position;                 /* +0x50 */
    unsigned char unmodeled_060[0x40];
    Vector4 origin_offset;                   /* +0xa0 */
    Vector4 position;
    Vector4 position_delta;                  /* +0xc0 */
    unsigned char unmodeled_0D0[0x10];
    Vector4 scale_vector;
    Vector4 global_position_source;          /* +0xf0 */
    Vector4 rotation_vector;
    Vector4 parent_position_source;          /* +0x110 */
    Vector4 position_vector;
    Vector4 position_source[3];
    SefProgressState first_vector_progress;
    SefProgressState second_vector_progress;
    unsigned char unmodeled_1A0[0x0C];
    SefIntegerProgressState first_integer_progress;
    SefIntegerProgressState second_integer_progress;
    unsigned char unmodeled_1C4[0x0C];
    short *scale_keys;
    struct SefKey5Table *first_vector_keys;
    short *rotation_keys;
    struct SefKey5Table *second_vector_keys;
    SefModeKeyTable *key_1E0;
    unsigned char *key_1E4;
    struct SefKey3 *first_integer_keys;
    struct SefKey3 *second_integer_keys;
    unsigned char *key_1F0;
    short *position_keys;
    unsigned char unmodeled_1F8[0x400];
    float gravity;
    float height;
    short local_point_handles[0x100];                 /* +0x600 */
    unsigned char unmodeled_800[2];
    short global_result;
    short timer;
    unsigned short frame;
    short start_time;                        /* +0x808 */
    unsigned short loop_mode;
    unsigned short global_data_count;
    short field_80E;                         /* +0x80E */
    short link_index;                        /* +0x810 */
    unsigned short line_flags;               /* +0x812 */
    unsigned short local_result;
    short collision_progress;
    short global_data_state;                 /* +0x818 */
    unsigned char unmodeled_81A[6];
} SefLineData;

typedef union SefEffectRequestBlock SefEffectRequestBlock;

union SefEffectRequestBlock {
    SefEffectRequest request;
    unsigned long long words[4];
};

typedef struct SefLineGlobalView {
    Vector4 global_position;                 /* +0x00 */
    unsigned char unmodeled_010[0x10];
    Vector4 parent_position;                 /* +0x20 */
    unsigned char unmodeled_030[0x60];
    Vector4 position_delta;                  /* +0x90 */
    unsigned char unmodeled_0a0[0x20];
    Vector4 global_position_source;          /* +0xc0 */
    unsigned char unmodeled_0d0[0x10];
    Vector4 parent_position_source;          /* +0xe0 */
    unsigned char unmodeled_0f0[0x184 - 0xf0];
    int emit_count;                          /* +0x184 */
    unsigned char unmodeled_188[0x190 - 0x188];
    int particle_count;                      /* +0x190 */
} SefLineGlobalView;

typedef struct SefLightSet {
    unsigned long long words[30];
} SefLightSet;

/* Header of a line-script record: the two script table indices, then the
 * gravity, collision and height inputs of the line. */
typedef struct SefLineScriptHeader {
    unsigned short table_indices[2];
    short gravity;
    short collision;
    short height;
} SefLineScriptHeader;

typedef struct SefEffectDescriptor {
    short category;
    unsigned short script_offset;
    short setting;
} SefEffectDescriptor;

typedef struct {
    struct Actor *caster;         /* +0x00 */
    struct Actor *target;         /* +0x04 */
    struct Actor *weaponActor;    /* +0x08, read by sefGetPoint's weapon path */
    short phase;
    unsigned char unmodeled_0e[0x14 - 0x0e];
    short effectNo;                  /* +0x14 */
    short nusumeResult;              /* +0x16 */
    unsigned char unmodeled_18[0x70 - 0x18];
    struct MapUnitRecord *casterUnit; /* +0x70 */
    struct MapUnitRecord *targetUnit; /* +0x74 */
    unsigned char unmodeled_78[0x80 - 0x78];
    Matrix4 casterMatrix;            /* +0x80, identity, then the caster's transform */
    Matrix4 targetMatrix;            /* +0xc0, identity, then the target's transform */
    unsigned char unmodeled_100[0x180 - 0x100];
    Matrix4 particleTransform;       /* +0x180, copied into a particle's matrix at +0x40 */
    unsigned char unmodeled_1c0[0x200 - 0x1c0];
    Vector4 matrixSource;            /* +0x200, first vector sefGetVecMatrix reads */
    unsigned char unmodeled_210[0x214 - 0x210];
    int vectorActive;                /* +0x214 */
    unsigned char unmodeled_218[0x21c - 0x218];
    float effectSpeed;               /* +0x21c */
    float particleScale;             /* +0x220, copied into a particle's battle_scale */
    int flags;                       /* +0x224 */
    unsigned char unmodeled_228[0x22c - 0x228];
    int running;
} SefBattleStatus;

/* The shared 0x230-byte battle block: the battle status while a battle runs,
 * and an effect request while a script is being set up. */
typedef union SefBattleBlock {
    SefBattleStatus status;
    SefEffectRequest request;
    unsigned long long words[0x230 / 8];
    unsigned char bytes[0x230];
} SefBattleBlock;

typedef struct {
    unsigned char unmodeled_00[0xf0];
    unsigned long long offsetXYBits;
    float offsetZ;
    float offsetW;
} SefLocalOffset;

typedef struct {
    unsigned char unmodeled_00[0x6b0];
    int inUse;
    unsigned char unmodeled_6b4[0xa74 - 0x6b4];
    short characterId;
    unsigned char unmodeled_a76[2];
    short effectNo;
    unsigned char unmodeled_a7a[0xab0 - 0xa7a];
} SefSchedulerRecord;

/* One 12-byte command of an effect's script table, read by sefExecEffectData. */
typedef struct SefEffectCommand {
    short start_time;                /* +0x0 */
    short frame;                     /* +0x2 */
    unsigned short data_offset;      /* +0x4 */
    short link_index;                /* +0x6 */
    short count;                     /* +0x8 */
    short flags;                     /* +0xa */
} SefEffectCommand;

/* _scriptWork has 16 script records of 0x450 bytes; its active-task
 * counter is the halfword at +0x448. */
typedef struct SefScriptStatus {
    unsigned char unmodeled_00[8];
    short active_tasks;
    unsigned char unmodeled_0a[6];
} SefScriptStatus;

typedef struct SefScriptReference {
    unsigned char unmodeled_000[0x440];
    SefScriptStatus status;
} SefScriptReference;

extern unsigned char _lineData[];
extern GameLoopFlagsView GameLoopState;
extern const char addEnemyFailedFormat[];

/* _battleData + 0x200, still owned by the scaffold, so it keeps the original
 * object's own name for the address (docs/naming.md). The two callees below
 * materialise the same address differently, which is why both spellings are
 * here: the flag path reaches it from the block base it already holds. */
extern unsigned char localPointVector[];

extern SefRenderState sRender;
extern int entryBossFlags[];

extern const float sef_rand_unit_scale_literal;

extern short mapperSearchCharacterId[];
extern const float analyzeAnimScale;
extern unsigned int scanFlags[];
extern float randomRotationDegToRad;
extern const char characterIdMissingFormat[];
extern float collisionProgressScale;
extern unsigned char pointSourceVector[];
extern Matrix4 inverseParentMatrix;
extern float parentMatrixState[41];
extern float lightRangeScale;
extern const char badRangeParamFormat[];
extern const float offsetRadiusScale;
extern const float spherePosScale;
extern const float circlePosScale;
extern const float rotateXDegToRad;
extern const float rotateZDegToRad;
extern const float invShortMax;
extern const float offsetYScale;
extern float particleSizeScale;
extern const float gravityScale;
extern const float heightScale;
extern const float globalPositionScale;
extern const float parentPositionScale;
extern const float positionSourceScale;
extern const char lineEmptyMessage[];
extern const int _nowScript;
extern const int _nowEvent;
extern SefScriptReference _scriptWork[16];
extern unsigned char schedulerStates[];
extern unsigned char soundParameterRecords[];
extern unsigned char effectBlockRecords[];
extern Matrix4 parentMatrixSlot;

/* The freed-scheduler helper is defined later in this file. */
extern void sefFreeScheduler(unsigned int scheduler_index);

extern short *scAnalyzeScriptCf(int effect_no, int *count);
extern void scCreateScript(SefEffectRequest *request);
extern void sresFreeReloaderMemoryNo(int no);
extern void MMathDivVectorS(Vector4 *dest, Vector4 *source, float divisor);
float MMathCalcLength(float *vec);
extern Matrix4 *MMathRotateMatrixXYZ(Matrix4 *out, Matrix4 *matrix, const Vector4 *angles);
extern StudioCamera *xglStudioGetActiveCamera(void);
int sefSearchMapperIndex2(int effect_id, int number, int category);
extern void sefGetPoint(Vector4 *position, const unsigned char *source, int mode);

/*
 * sefGetCubePosBtm (main:0x002e23c8) is defined later in this TU;
 * declared here so sefGetCubePosTop (main:0x002e24c0) can call it. Both are
 * LOCAL in the original, so both stay static. `extent` is the cube's
 * integer extent vector (four ints, loaded with lqc2 and converted with
 * vitof0); the Top variant forwards it untouched.
 */
static void sefGetCubePosBtm(Vector4 *pos, const int *extent);

static void sefGetOfsRange(Vector4 *position, const void *offset_table, int index);
extern Matrix4 *MMathRotateMatrixX(Matrix4 *out, Matrix4 *matrix, float angle);
extern Matrix4 *MMathRotateMatrixY(Matrix4 *out, Matrix4 *matrix, float angle);
extern Matrix4 *MMathRotateMatrixZ(Matrix4 *out, Matrix4 *matrix, float angle);
static void sevFreePtAllocator(unsigned int handle);
static void sefFreeLineData(int line_index);
static void sefFreeEffectData(EffectData *effect, unsigned int index);
static void sefDestroyEffectData(EffectData *effect);
extern void smInitilize(void *pool, unsigned int poolSize);
extern void sresInitMemoryRes(void);
extern void srsInitCdRead(void);
extern void svInitImageMapper(void);
void sefInitScheduler(void);
extern void scInitScript(void);
extern void sdvInitSpecialWork(void);
extern void sdvInitAmbient(void);
extern void sresLoadCommonMemory(void);
extern void sresLoadCfMemory(void);
void sefDrawEffect2D(void);
extern void sresLoadBattleData(void *battle_prm);
extern void scDestroyScriptAll(void);
static int sefCnvEtEffectNo(int effect_no, int character_id);
extern void svFileLoadScript(int mode, int effect_no);
extern int srsLeaveCdRead(void);
extern void scExecEffect(void);
extern void svDrawScheduler(void);
extern void svDrawScheduler3D(int flags);
extern void svDrawScheduler2D(int flags);
void sefFreeSchedulerCf(SchedulerState *scheduler);
extern int srsEffectNameToID(char *effect_name);
extern int srsMemoryLoadCf(void *data, int effect_no, int size);

static SchedulerState *sefCreateEffectCf2(int effectNo,
                                           const Vector4 *position,
                                           const Vector4 *orientation,
                                           int schedulerIndex);

void sefLerpIVectorA(const void *keys, void *vector);
extern int func_A33248(void);
extern Vector4 *MMathApplyMatrix(Vector4 *destination, const void *matrix, const Vector4 *point);
extern Vector4 *MMathSpecialVectorSub(Vector4 *destination, const Vector4 *first, const Vector4 *second, float parameter);
static int sefExecLineGlobalData(SefLineData *line_data, int frame);
void sefGetParentMatrix(SchedulerState *scheduler, int index, Matrix4 *matrix);
static int sefExecLineLocalData(SefLineData *line_data, unsigned short frame);
static void sefCnvWaitEffectNo(SefEffectRequest *request);
void sefDeleteEffect2(SchedulerState *scheduler);
extern short srsEftNoWeaponEffectID(int effectNo);
static void sefGetSpherePos(Vector4 *position, float radius);
static int sefAnimate(unsigned char *object, unsigned char *state, int advance);
extern int srsGetBossWaitEftNo(int character_id);
static void sefAddLightActor(const Vector4 *position, const SefLightInput *light);
extern void *MMathNormalizeVector2(void *destination, const Vector4 *source);
extern float MMathVectorDotProduct(const Vector4 *first, const Vector4 *second);
static int sefFreeLocalData(unsigned char *line_data, unsigned int index);
static void sefAnalyzeAnim(unsigned char *object);
extern void func_A2C408(int value);
extern void func_A2C430(int value);

static void sefInitLineData(SefLineData *line, int start_time, int frame,
                            SefLineScriptHeader *line_script, int link_index,
                            int count, int flags);

static void sefSetHitSignal(Actor *actor);
static void sefSetSeSignal(Actor *actor);
extern int sdvSelectCamera(void *choice, int script_base, int value);
extern int func_00A0CE30(void); /* nusumeGet (ov01) */
extern void sdvExecAmbient(void);
extern void sdvScheduleSound(void *sequence);
extern void sdvScheduleCamera(void *camera);
extern void sdvExecSpecial(void *special);
extern void sdvDrawSpecial(void *special);
extern void sdvScheduleAlter(void *alter, void *request);
void sefExecLoadQue(void);
static int sefSetReverseDir(int effect_id, int category);
static void sefInitBattlePrm(void);
static void sefSetLightFlag(void);

static SefBossDeathEffect _sefBossDeathPrm_4[36] ;

/*
 * These constants are backed by the original literal-pool bytes. Repeated
 * values stay repeated at separate use sites where the compiler emits them.
 */
/* Original TU-local scheduler parent index (small-data word, initialized -1). */
static int _parentLine = -1;

/* Original local floating-point tuning values used by the scheduler assembly. */
static float _gravity = 0.0f;

static float _colision = 0.0f;

static float _height = 0.0f;

/* Original local direction flag. */
static short _revDirZ = 0;

/* The original TU-local zero flag is present as a two-byte small-data object. */
static short _initialize = 0;

/* Original global signal flags shared with the scheduler assembly in this TU. */
short _hitFlag = 0;

short _hitSignal = 0;

short _seSignal = 0;

/* Original global battle-mode flag; the current C callers set and clear it. */
short _sefBattleMode = 0;

/* The original local pointer stores the KSEG0 alias of the named table
 * sefAtanTbl at 0x0040FAC0 (main symbol map: 0x0040FAC0, size 0x1004). */
static float *atanTbl_0 = (float *)0x2040FAC0u;

static int revEft_3[10] = {0x0b03, 0x0b04, 0x0afd, 0x0b07, 0x0b05, 0x0b32, 0x0b31, 0x0afb, 0x0afe, 0x0b06};

Matrix4 _invView = {0};

static unsigned int sefRandSeed = 0x12345678;

static unsigned short steftTbl[17] ;

static int _schridx_1;

static SefBossDeathEffect _sefBossDeathPrm_4[36] = { { .character_id = 151, .effect_no = 2683 }, { .character_id = 152, .effect_no = 2686 }, { .character_id = 153, .effect_no = 2689 }, { .character_id = 154, .effect_no = 2692 }, { .character_id = 155, .effect_no = 2695 }, { .character_id = 156, .effect_no = 2698 }, { .character_id = 157, .effect_no = 2701 }, { .character_id = 158, .effect_no = 2704 }, { .character_id = 159, .effect_no = 2707 }, { .character_id = 160, .effect_no = 2710 }, { .character_id = 161, .effect_no = 2713 }, { .character_id = 162, .effect_no = 2716 }, { .character_id = 163, .effect_no = 2719 }, { .character_id = 164, .effect_no = 2722 }, { .character_id = 165, .effect_no = 2725 }, { .character_id = 166, .effect_no = 2728 }, { .character_id = 167, .effect_no = 2731 }, { .character_id = 168, .effect_no = 2734 }, { .character_id = 169, .effect_no = 2737 }, { .character_id = 170, .effect_no = 2740 }, { .character_id = 171, .effect_no = 2743 }, { .character_id = 172, .effect_no = 2746 }, { .character_id = 173, .effect_no = 2749 }, { .character_id = 174, .effect_no = 2752 }, { .character_id = 175, .effect_no = 2755 }, { .character_id = 176, .effect_no = 2758 }, { .character_id = 177, .effect_no = 2761 }, { .character_id = 178, .effect_no = 2764 }, { .character_id = 179, .effect_no = 2767 }, { .character_id = 180, .effect_no = 2770 }, { .character_id = 181, .effect_no = 2773 }, { .character_id = 182, .effect_no = 2776 }, { .character_id = 183, .effect_no = 2779 }, { .character_id = 184, .effect_no = 2689 }, { .character_id = 185, .effect_no = 2692 }, { .character_id = 186, .effect_no = 2740 } };

static unsigned short steftTbl[17] = { 0, 2510, 2506, 2502, 2526, 2518, 2514, 2522, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

Vector4 _defLight[15] = { { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f }, { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f } };

Matrix4 sefDummy = { { 0.0f } };

Matrix4 sefDummy2 = { { 0.0f } };

static SefBattleActorTbl _battleActor[1];

/* Original TU-local scheduler-local index and effect-load queue. */
static int _nowParentLocal;

static int _sefLoadEftQue;

static SefBattleBlock _battleData;

static unsigned char _ptAlloc[0xA0810];

static unsigned char _eftBuffer[0xD4800];

static SefBattleParameters _battlePrm;

/* Original TU-local scheduler storage, kept named for its recovered users. */
static SchedulerState _scheduler[128];

static SefSavePrm _savePrm[128];

static int _nowScriptAdr;

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
static const unsigned char _zeroPos_004CBF00[16] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x80, 0x3f
};

static const int offset_2[5] = {0, 0, 1, 2, 2};

/* These values remain mutable objects so the recovered callers use storage. */
const char sefBoss033Name[16] = "boss_033";

const char sefBoss032Name[16] = "boss_032";

const char sefBoss031Name[16] = "boss_031";

const char sefBoss030Name[16] = "boss_030";

const char sefBoss029Name[16] = "boss_029";

const char sefBoss028Name[16] = "boss_028";

const char sefBoss027Name[16] = "boss_027";

const char sefBoss026Name[16] = "boss_026";

const char sefBoss025Name[16] = "boss_025";

const char sefBoss024Name[16] = "boss_024";

const char sefBoss023Name[16] = "boss_023";

const char sefBoss022Name[16] = "boss_022";

const char sefBoss021Name[16] = "boss_021";

const char sefBoss020Name[16] = "boss_020";

const char sefBoss019Name[16] = "boss_019";

const char sefBoss018Name[16] = "boss_018";

const char sefBoss017Name[16] = "boss_017";

const char sefBoss016Name[16] = "boss_016";

const char sefBoss015Name[16] = "boss_015";

const char sefBoss014Name[16] = "boss_014";

const char sefBoss013Name[16] = "boss_013";

const char sefBoss012Name[16] = "boss_012";

const char sefBoss011Name[16] = "boss_011";

const char sefBoss010Name[16] = "boss_010";

const char sefBoss009Name[16] = "boss_009";

const char sefBoss008Name[16] = "boss_008";

const char sefBoss007Name[16] = "boss_007";

const char sefBoss006Name[16] = "boss_006";

const char sefBoss005Name[16] = "boss_005";

const char sefBoss004Name[16] = "boss_004";

const char sefBoss003Name[16] = "boss_003";

const char sefBoss002Name[16] = "boss_002";

const char sefBoss001Name[16] = "boss_001";

const char sefUtso007Name[16] = "utso_007";

const char sefUtso006Name[16] = "utso_006";

const char sefUtso005Name[16] = "utso_005";

const char sefUtso004Name[16] = "utso_004";

const char sefUtso003Name[16] = "utso_003";

const char sefUtso002Name[16] = "utso_002";

const char sefUtso001Name[16] = "utso_001";

const char sefUtro007Name[16] = "utro_007";

const char sefUtro006Name[16] = "utro_006";

const char sefUtro005Name[16] = "utro_005";

const char sefUtro004Name[16] = "utro_004";

const char sefUtro003Name[16] = "utro_003";

const char sefUtro002Name[16] = "utro_002";

const char sefUtro001Name[16] = "utro_001";

const char sefUtre006Name[16] = "utre_006";

const char sefUtre005Name[16] = "utre_005";

const char sefUtre004Name[16] = "utre_004";

const char sefUtre003Name[16] = "utre_003";

const char sefUtre002Name[16] = "utre_002";

const char sefUtre001Name[16] = "utre_001";

const char sefUtma014Name[16] = "utma_014";

const char sefUtma013Name[16] = "utma_013";

const char sefUtma012Name[16] = "utma_012";

const char sefUtma011Name[16] = "utma_011";

const char sefUtma010Name[16] = "utma_010";

const char sefUtma009Name[16] = "utma_009";

const char sefUtma008Name[16] = "utma_008";

const char sefUtma007Name[16] = "utma_007";

const char sefUtma006Name[16] = "utma_006";

const char sefUtma005Name[16] = "utma_005";

const char sefUtma004Name[16] = "utma_004";

const char sefUtma003Name[16] = "utma_003";

const char sefUtma002Name[16] = "utma_002";

const char sefUtma001Name[16] = "utma_001";

const char sefFeso004Name[16] = "feso_004";

const char sefFeso003Name[16] = "feso_003";

const char sefFeso002Name[16] = "feso_002";

const char sefFeso001Name[16] = "feso_001";

const char sefFero001Name[16] = "fero_001";

const char sefFere003Name[16] = "fere_003";

const char sefFere002Name[16] = "fere_002";

const char sefFere001Name[16] = "fere_001";

const char sefFema006Name[16] = "fema_006";

const char sefFema005Name[16] = "fema_005";

const char sefFema004Name[16] = "fema_004";

const char sefFema003Name[16] = "fema_003";

const char sefFema002Name[16] = "fema_002";

const char sefFema001Name[16] = "fema_001";

const char sefGuno114Name[16] = "guno_114";

const char sefGuno025Name[16] = "guno_025";

const char sefGuno024Name[16] = "guno_024";

const char sefGuno023Name[16] = "guno_023";

const char sefGuno022Name[16] = "guno_022";

const char sefGuno021Name[16] = "guno_021";

const char sefGuno020Name[16] = "guno_020";

const char sefGuno019Name[16] = "guno_019";

const char sefGuno018Name[16] = "guno_018";

const char sefGuno017Name[16] = "guno_017";

const char sefGuno016Name[16] = "guno_016";

const char sefGuno015Name[16] = "guno_015";

const char sefGuno014Name[16] = "guno_014";

const char sefGuno013Name[16] = "guno_013";

const char sefGuno012Name[16] = "guno_012";

const char sefGuno011Name[16] = "guno_011";

const char sefGuno010Name[16] = "guno_010";

const char sefGuno009Name[16] = "guno_009";

const char sefGuno008Name[16] = "guno_008";

const char sefGuno007Name[16] = "guno_007";

const char sefGuno006Name[16] = "guno_006";

const char sefGuno005Name[16] = "guno_005";

const char sefGuno004Name[16] = "guno_004";

const char sefGuno003Name[16] = "guno_003";

const char sefGuno002Name[16] = "guno_002";

const char sefGuno001Name[16] = "guno_001";

const char sefCatherinName[16] = "catherin";

static const char addPlayerFailedMessage[24] = {
    (char)0xA5, (char)0xAD, (char)0xA5, (char)0xE3,
    (char)0xA5, (char)0xE9, (char)0xA4, (char)0xAC,
    (char)0xC4, (char)0xC9, (char)0xB2, (char)0xC3,
    (char)0xA4, (char)0xC7, (char)0xA4, (char)0xAD,
    (char)0xA4, (char)0xDE, (char)0xA4, (char)0xBB,
    (char)0xA4, (char)0xF3, 0, 0
};

float srsAtan2(float x, float y)
{
    int y_is_positive = y >= 0.0f;
    int x_is_positive = x >= 0.0f;
    float result = 0.0f;
    float angle;

    if (y == 0.0f) {
        if (x != 0.0f) {
            if (x_is_positive) {
                result = 1.5707964f;
            } else {
                result = -1.5707964f;
            }
        }
    } else if (x == 0.0f) {
        if (!y_is_positive) {
            result = 3.1415927f;
        }
    } else {
        if (!y_is_positive) {
            y = -y;
        }
        if (!x_is_positive) {
            x = -x;
        }

        if (x <= y) {
            angle = atanTbl_0[(int)(x * 1024.0f / y)];
            if (y_is_positive) {
                if (!x_is_positive) {
                    angle = -angle;
                }
            } else if (x_is_positive) {
                angle = 3.1415927f - angle;
            } else {
                angle += 3.1415927f;
            }
        } else {
            angle = atanTbl_0[(int)(y * 1024.0f / x)];
            if (y_is_positive) {
                if (x_is_positive) {
                    angle = 1.5707964f - angle;
                } else {
                    angle = -(1.5707964f - angle);
                }
            } else if (x_is_positive) {
                angle += 1.5707964f;
            } else {
                angle = 4.712389f - angle;
            }
        }
        result = angle;
    }
    return result;
}

static float sefRandf(void)
{
    unsigned int seed = sefRandSeed;
    unsigned int next_seed;
    int sample;

    next_seed = seed * 0x41C64E6Du;
    next_seed += 0x3039u;
    sefRandSeed = next_seed;
    sample = (next_seed >> 16) & 0x7FFFu;
    return (float)sample * sef_rand_unit_scale_literal;
}

/*
 * sefIsBossID (main:0x002e11e8): true when character_id falls in the boss ID
 * range [0x97, 0xba] (sefCnvDeathEffectNo, sefSetupEnemy, this TU).
 */
static int sefIsBossID(int character_id)
{
    return (unsigned int)(character_id - 0x97) < 0x24;
}

void sefMemZero(void *dest, int size)
{
    SefQuadBlock *blocks = dest;
    int block_count = size >> 4;
    int word_count = (size & 0xf) >> 2;
    SefQuadBlock *tail = blocks + block_count;
    int i;

    for (i = 0; i < block_count; i++) {
        blocks[i].quad = 0;
    }
    for (i = 0; i < word_count; i++) {
        tail->words[i] = 0;
    }
}

void sefCalcRotTransSMatrix(Vector4 *angles, Vector4 *translation, Vector4 *scale, Matrix4 *matrix)
{
    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(translation) : "memory");
    __asm__ __volatile__("vmove.w vf1, vf0" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf2, vf0" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf3, vf2" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf4, vf3" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 48(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf2, 32(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf3, 16(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf4, 0(%0)" : : "r"(matrix) : "memory");
    MMathRotateMatrixXYZ(matrix, matrix, angles);
    MMathScaleMatrix(matrix, matrix, scale);
}

static void sefCalcInvView(Matrix4 *matrix) {
    StudioCamera *camera = xglStudioGetActiveCamera();

    __asm__ __volatile__("vmr32.xyzw vf1, vf0" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf2, vf1" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf3, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf0, 48(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf1, 32(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf2, 16(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf3, 0(%0)" : : "r"(matrix) : "memory");
    MMathRotateMatrixYXZ(matrix, matrix, &camera->rotation.vector);
}

int sefSearchMapperIndex2(int effect_id, int number, int category) {
    int index;
    int column;
    int weapon_id;

    if (category == 14) {
        return 14;
    }
    if (category == 15) {
        return 15;
    }
    if ((unsigned int)(category - 16) < 24U) {
        for (index = 0; index < 24; index++) {
            if (_srsMemRes.effectData[index] != 0 &&
                _srsMemRes.no[16 + index] == effect_id) {
                return index + 16;
            }
        }
        return 0;
    }

    if ((unsigned int)(effect_id - 0xAF0) < 0xC8U) {
        weapon_id = srsEftNoWeaponEffectID(effect_id);
        if (weapon_id <= 0) {
            return 0;
        }
        for (index = 0; index < 3; index++) {
            for (column = 0; column < 3; column++) {
                if (_srsMemRes.weaponData[index][column] != 0 &&
                    _srsMemRes.no[index * 3 + column + 2] == weapon_id) {
                    return index * 3 + column + 2;
                }
            }
        }
        return 0;
    }

    if (((unsigned int)effect_id < 0x64U) ||
        ((unsigned int)(effect_id - 0xF0) < 0x9DU) ||
        ((unsigned int)(effect_id - 0x9C4) < 0x64U) ||
        ((unsigned int)(effect_id - 0xA78) < 0x6AU)) {
        {
            for (index = 0; index < 3; index++) {
                if (mapperSearchCharacterId[0] != 0 && _battlePrm.player_character_ids[index] == mapperSearchCharacterId[0]) {
                    return index * 3 + 2;
                }
            }
        }
        {
            for (index = 0; index < 3; index++) {
                if (mapperSearchCharacterId[0] != 0 && _battlePrm.enemy_character_ids[index] == mapperSearchCharacterId[0]) {
                    return index + 11;
                }
            }
        }
    }
    return 0;
}

int sefSearchMapperIndex(int effect_id) {
    int number;
    int category;

    srsAnalyzeEftNo(effect_id, &number, &category);
    return sefSearchMapperIndex2(effect_id, number, category);
}

/*
 * sevInitPtAllocator (main:0x002e1580): clears the whole _ptAlloc particle
 * allocator table and _battleData, then rebuilds _ptAlloc's own trailing
 * free-index queue (1024 entries, main VA 0x00794110's 0xa07fe-byte offset)
 * with the descending indices 0x3ff..0. sefInitEffect is its only caller.
 */
static void sevInitPtAllocator(void)
{
    short *slot;
    int index;

    memset(_ptAlloc, 0, 0xA0810);
    slot = (short *)(_ptAlloc + 0xA07FE);
    memset(_battleData.bytes, 0, 0x230);
    index = 0x3FF;
    do {
        *slot = (short)index;
        index--;
        slot--;
    } while (index >= 0);
}

static int sevAllocPtAllocator(void)
{
    unsigned int head = SEF_PT_ALLOCATOR->head;
    unsigned int next_head = (head + 1) & 0x3ff;

    if (next_head == SEF_PT_ALLOCATOR->tail)
        return -1;

    SEF_PT_ALLOCATOR->head = next_head;
    return SEF_PT_ALLOCATOR->freeSlots[head];
}

static void sevFreePtAllocator(unsigned int handle)
{
    unsigned int tail = SEF_PT_ALLOCATOR->tail;
    unsigned int next_tail;

    if (handle < 0x400u && SEF_PT_ALLOCATOR->head != tail) {
        SEF_PT_ALLOCATOR->freeSlots[tail] = (short)handle;
        next_tail = (tail + 1u) & 0x3ffu;
        SEF_PT_ALLOCATOR->tail = next_tail;
    }
}

static void sefProgressKey3(SefKey3 *keys, SefProgressCursor *state)
{
    int frame = state->frame + 1;

    state->frame = frame;
    if (frame < 1024) {
        SefKey3 *key = keys + state->key_index;

        if (key->frame != 1024 && frame >= key[1].frame) {
            if (key[1].jump >= 0) {
                state->key_index = key[1].jump;
                key = &keys[state->key_index];
                state->frame = key->frame;
            } else {
                state->key_index++;
            }
        }
    }
}

static SefKey5 *sefProgressKey5(SefKey5 *keys, SefProgressCursor *state)
{
    int key_index = state->key_index;
    int frame = state->frame + 1;
    SefKey5 *key = keys + key_index;
    short jump;

    state->frame = frame;
    if (frame < 1024) {
        if (key->end_frame != 1024 && frame >= key[1].end_frame) {
            key++;
            jump = key->jump;
            if (jump >= 0) {
                state->key_index = jump;
                key = keys + jump;
                state->frame = key->end_frame;
            } else {
                state->key_index = key_index + 1;
            }
        }
    }
    return key;
}

static void sefLerpVectorA(void *keys, Vector4 *dest)
{
    __asm__ __volatile__(
        "ldl $9, 7(%0)\n\t"
        "ldr $9, 0(%0)\n\t"
        "dsrl $9, $9, 0x10\n\t"
        "vmove.w $vf2w, $vf0w\n\t"
        "pcgth $10, $0, $9\n\t"
        "pextlh $10, $10, $9\n\t"
        "qmtc2 $10, $vf1\n\t"
        "vitof0.xyz $vf2xyz, $vf1xyz\n\t"
        "sqc2 $vf2, 0(%1)"
        :
        : "r"(keys), "r"(dest)
        : "$9", "$10", "memory"
    );
}

int sefLerpVector(SefKey5 *keys, void *state_data)
{
    SefProgressState *state = state_data;
    SefKey5 *key;
    Vector4 *vector = &state->vector.floats;
    short start_frame;
    short end_frame;

    key = keys + state->cursor.key_index;
    if (key[1].end_frame == 1024) {
        return 0;
    }

    key = sefProgressKey5(keys, &state->cursor);
    start_frame = key->end_frame;
    end_frame = key[1].end_frame;
    if (start_frame != end_frame) {
        float factor = (float)(state->cursor.frame - start_frame) /
                       (float)(end_frame - start_frame);
        SefKey5 *next_key = key + 1;

        __asm__ __volatile__(
            "mfc1 $9, %3\n\t"
            "qmtc2 $9, $vf3\n\t"
            "ldl $8, 7(%0)\n\t"
            "ldr $8, 0(%0)\n\t"
            "ldl $9, 7(%1)\n\t"
            "ldr $9, 0(%1)\n\t"
            "dsrl $8, $8, 16\n\t"
            "dsrl $9, $9, 16\n\t"
            "pcgth $10, $0, $8\n\t"
            "pcgth $11, $0, $9\n\t"
            "pextlh $10, $10, $8\n\t"
            "pextlh $11, $11, $9\n\t"
            "qmtc2 $10, $vf1\n\t"
            "qmtc2 $11, $vf2\n\t"
            "vitof0.xyzw $vf1, $vf1\n\t"
            "vitof0.xyzw $vf2, $vf2\n\t"
            "vsub.xyz $vf2, $vf2, $vf1\n\t"
            "vmulx.xyz $vf2, $vf2, $vf3x\n\t"
            "vadd.xyz $vf2, $vf2, $vf1\n\t"
            "sqc2 $vf2, 0(%2)"
            :
            : "r"(key), "r"(next_key), "r"(vector), "f"(factor)
            : "$8", "$9", "$10", "$11", "memory"
        );
        vector->w = 1.0f;
    } else {
        sefLerpVectorA(key, vector);
    }
    return 1;
}

static int sefLerpVectorSC(void *keys, Vector4 *vectors)
{
    int status = sefLerpVector(keys, vectors);
    float scale = effect_scale_literal_0_1;

    __asm__ __volatile__("" : : "f"(scale));

    if (status != 0) {
        vectors++;

        __asm__ __volatile__(
            "lqc2 $vf1, 0(%0)\n\t"
            "mfc1 $8, %1\n\t"
            "qmtc2 $8, $vf2\n\t"
            "vmulx.xyz $vf1xyz, $vf1xyz, $vf2x\n\t"
            "sqc2 $vf1, 0(%0)"
            :
            : "r"(vectors), "f"(scale)
            : "$8", "memory"
        );
    }

    return status;
}

void sefLerpIVectorA(const void *keys, void *vector)
{
    /* Decode four signed halfwords from eight source bytes into sixteen
     * destination bytes. These are integer representations, not floats. */
    __asm__ __volatile__(
        "ldl $9, 7(%0)\n\t"
        "ldr $9, 0(%0)\n\t"
        "pcgth $10, $0, $9\n\t"
        "pextlh $10, $10, $9\n\t"
        "sq $10, 0(%1)"
        :
        : "r"(keys), "r"(vector)
        : "$9", "$10", "memory");
}

void sefLerpIVector(SefKey5 *keys, SefProgressState *state)
{
    SefKey5 *key = keys + state->cursor.key_index;
    Vector4 *vector = &state->vector.floats;
    short start_frame;
    short end_frame;
    float factor;
    SefKey5 *next_key;

    if (key[1].end_frame == 1024) {
        return;
    }

    key = sefProgressKey5(keys, &state->cursor);
    start_frame = key->end_frame;
    end_frame = key[1].end_frame;
    if (start_frame != end_frame) {
        factor = (float)(state->cursor.frame - start_frame) /
                 (float)(end_frame - start_frame);
        next_key = key + 1;

        __asm__ __volatile__(
            "ldl $8, 7(%0)\n\t"
            "ldr $8, 0(%0)\n\t"
            "ldl $9, 7(%1)\n\t"
            "ldr $9, 0(%1)\n\t"
            "mfc1 $10, %2\n\t"
            "qmtc2 $10, $vf3\n\t"
            "pcgth $10, $0, $8\n\t"
            "pcgth $11, $0, $9\n\t"
            "pextlh $10, $10, $8\n\t"
            "pextlh $11, $11, $9\n\t"
            "qmtc2 $10, $vf1\n\t"
            "qmtc2 $11, $vf2\n\t"
            "vitof0.xyzw $vf1, $vf1\n\t"
            "vitof0.xyzw $vf2, $vf2\n\t"
            "vsub.xyz $vf2, $vf2, $vf1\n\t"
            "vmulx.xyz $vf2, $vf2, $vf3x\n\t"
            "vadd.xyz $vf2, $vf2, $vf1\n\t"
            "vftoi0.xyzw $vf2, $vf2\n\t"
            "sqc2 $vf2, 0(%3)"
            :
            : "r"(key->vector), "r"(next_key->vector), "f"(factor),
              "r"(vector)
            : "$8", "$9", "$10", "$11", "memory"
        );
        return;
    }
    sefLerpIVectorA(key->vector, vector);
}

void sefLerpIVector2(SefKey5 *keys, SefProgressState *state)
{
    SefKey5 *key = keys + state->cursor.key_index;
    SefIntFloatVector *vector = &state->vector;
    int start_frame;
    int end_frame;
    int factor;

    if (key[1].end_frame != 1024) {
        key = sefProgressKey5(keys, &state->cursor);
        start_frame = key->end_frame;
        end_frame = key[1].end_frame;
        if (start_frame != end_frame) {
            factor = ((state->cursor.frame - start_frame) << 16) / (end_frame - start_frame);
            vector->ints[0] = (((key[1].vector[0] - key->vector[0]) * factor) >> 16) + key->vector[0];
            vector->ints[1] = (((key[1].vector[1] - key->vector[1]) * factor) >> 16) + key->vector[1];
            vector->ints[2] = (((key[1].vector[2] - key->vector[2]) * factor) >> 16) + key->vector[2];
            vector->ints[3] = -((((key[1].jump - key->jump) * factor) >> 16) + key->jump);
            return;
        }
    }
    vector->ints[0] = key->vector[0];
    vector->ints[1] = key->vector[1];
    vector->ints[2] = key->vector[2];
    vector->ints[3] = -key->jump;
}

void sefLerpFloat(SefKey3 *keys, SefFloatProgressState *state)
{
    SefKey3 *key;
    float fraction;

    key = &keys[state->cursor.key_index];
    if (key[1].frame != 1024) {
        sefProgressKey3(keys, &state->cursor);
        key = &keys[state->cursor.key_index];
        if (key->frame != key[1].frame) {
            fraction = (float)(state->cursor.frame - key->frame) / (float)(key[1].frame - key->frame);
            state->value = ((float)(key[1].value - key->value) * fraction + (float)key->value) * 0.01f;
            return;
        }
    }
    state->value = (float)key->value * 0.01f;
}

void sefLerpInt(SefKey3 *keys, SefIntegerProgressState *state)
{
    SefKey3 *key;
    int frame_delta;
    int frame_offset;
    int key_index;
    float fraction;

    key = &keys[state->cursor.key_index];
    if (key->frame == 1024) {
        state->value = key->value;
    } else if (key[1].frame == 1024) {
        state->value = key->value;
    } else {
        sefProgressKey3(keys, &state->cursor);
        key_index = state->cursor.key_index;
        key = &keys[key_index];
        if (key->frame != key[1].frame) {
            frame_delta = key[1].frame - key->frame;
            frame_offset = state->cursor.frame - key->frame;
            fraction = (float)frame_offset / (float)frame_delta;
            state->value = (short)((float)(key[1].value - key->value) * fraction + (float)key->value);
        } else {
            state->value = key->value;
        }
    }
}

static void sefProgressInt(SefKey3 *keys, SefIntegerProgressState *state)
{
    sefProgressKey3(keys, &state->cursor);
    state->value = keys[state->cursor.key_index].value;
}

static inline int sefIsNonZeroVector(Vector4 *vector)
{
    return (((int *)vector)[2] | *(unsigned long *)vector) != 0;
}

void sefGetMotionNullMatrix(Matrix4 *matrix, Actor *actor, int index)
{
    int joint;

    if (actor != 0) {
        joint = ACT_jointGetAccessories(actor, index + 8);
        if (joint >= 0 && actor->jointMatrices != 0) {
            __asm__ __volatile__(
                "lq $8, 0(%1)\n\t"
                "lq $9, 16(%1)\n\t"
                "lq $10, 32(%1)\n\t"
                "lq $11, 48(%1)\n\t"
                "sq $8, 0(%0)\n\t"
                "sq $9, 16(%0)\n\t"
                "sq $10, 32(%0)\n\t"
                "sq $11, 48(%0)"
                :
                : "r"(matrix), "r"((joint << 6) + (int)actor->jointMatrices)
                : "$8", "$9", "$10", "$11", "memory"
            );
            if (sefIsNonZeroVector((Vector4 *)(*matrix)[3])) {
                return;
            }
        }
        sefCalcRotTransSMatrix(&actor->rotation, &actor->position, &actor->scale, matrix);
        return;
    }
    __asm__ __volatile__("vmr32.xyzw vf1, vf0" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf2, vf1" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf3, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf0, 48(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf1, 32(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf2, 16(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf3, 0(%0)" : : "r"(matrix) : "memory");
}

void sefGetNullPosition(Vector4 *position, Actor *actor, unsigned int index,
                        int flag)
{
    int joint;
    Matrix4 *matrices;

    if (actor != 0 && index < 16) {
        joint = ACT_jointGetAccessories(actor, index + 8);
        matrices = actor->jointMatrices;
        if (joint < 0 || matrices == 0) {
            position->y = 20.0f;
        } else {
            __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)" : : "r"(position), "r"((joint << 6) + (int)matrices + 48) : "$8", "memory");
        }
    }
}

void sefGetWeaponPosition(Vector4 *position, Actor *actor, unsigned int index)
{
    int joint;
    Matrix4 *matrices;

    if (actor != 0 && index < 8) {
        joint = ACT_jointGetAccessories(actor, index);
        matrices = actor->jointMatrices;
        if (joint < 0 || matrices == 0) {
            position->y = 20.0f;
        } else {
            __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)" : : "r"(position), "r"((joint << 6) + (int)matrices + 48) : "$8", "memory");
        }
    }
}

void sefGetPoint(Vector4 *position, const unsigned char *source, int part)
{
    SefBattleStatus *battle;
    Matrix4 *point_block = 0;
    Matrix4 *copy_base;
    short has_source;
    int source_nonzero;
    Matrix4 point_copy;

    source_nonzero = (*(const int *)(source + 8) | *(const long long *)source) != 0;
    has_source = (short)source_nonzero;
    __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(position) : "memory");
    battle = &_battleData.status;

    if ((battle->flags & 2) != 0) {
        if ((unsigned int)(part - 257) < 16) {
            part += 1792;
        }
    }
    if (part == 529) {
        part = (battle->phase >= 2) ? 527 : 528;
    }

    if (part < 4 || part == 2048) {
        goto done;
    }
    if (part == 256) {
        if (battle->caster != 0) {
            point_block = &battle->casterMatrix;
        }
        goto done;
    }
    if (part == 512) {
        if (battle->target != 0) {
            point_block = &battle->targetMatrix;
        }
        goto done;
    }
    if ((part & 0x100) != 0) {
        sefGetNullPosition(position, battle->caster, part - 257, 1);
        copy_base = &battle->casterMatrix;
        goto copy_site;
    }
    if ((part & 0x200) != 0) {
        sefGetNullPosition(position, battle->target, part - 513, 0);
        copy_base = &battle->targetMatrix;
        goto copy_site;
    }
    if ((part & 0x1000) != 0) {
        goto done;
    }
    if (part == 4) {
        unsigned char *parent;

        if (_parentLine >= 0 && _nowParentLocal >= 0) {
            parent = _ptAlloc + _nowParentLocal * 640;
        } else {
            parent = 0;
        }
        if (parent != 0) {
            __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)" : : "r"(position), "r"(parent + 192) : "$8", "memory");
        }
        goto done;
    }
    if ((part & 0x400) != 0) {
        sefGetWeaponPosition(position, battle->weaponActor, part - 1024);
        copy_base = &battle->casterMatrix;
    } else {
        goto not_weapon;
    }

copy_site:
    __asm__ __volatile__(
        "lq $8, 0(%1)\n\t"
        "lq $9, 16(%1)\n\t"
        "lq $10, 32(%1)\n\t"
        "lq $11, 0(%2)\n\t"
        "sq $8, 0(%0)\n\t"
        "sq $9, 16(%0)\n\t"
        "sq $10, 32(%0)\n\t"
        "sq $11, 48(%0)"
        : : "r"(point_copy), "r"(copy_base), "r"(position), "r"(point_block) : "$8", "$9", "$10", "$11", "memory");
    point_block = &point_copy;
    goto done;

not_weapon:
    if (part == 5) {
        __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)" : : "r"(position), "r"(pointSourceVector) : "$8", "memory");
        goto done;
    }
    if ((part & 0x800) != 0) {
        sefGetNullPosition(position, battle->caster, part - 2049, 1);
        MMathApplyMatrix(position, battle->casterMatrix, position);
    }

done:
    if (point_block) {
        if (has_source) {
            MMathApplyMatrix(position, point_block, (const Vector4 *)source);
        } else {
            __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)" : : "r"(position), "r"((*point_block)[3]) : "$8", "memory");
        }
    } else {
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%0)\n\t"
            "lqc2 $vf2, 0(%1)\n\t"
            "vadd.xyz $vf1, $vf1, $vf2\n\t"
            "sqc2 $vf1, 0(%0)"
            : : "r"(position), "r"(source) : "memory");
    }
}

void sefGetPosition(Vector4 *position, void *source, int mode)
{
    const unsigned char *point = source;

    if (point == 0) {
        point = _zeroPos_004CBF00;
    }
    sefGetPoint(position, point, mode);
    position->w = 1.0f;
}

static void sefGetCirclePos(Vector4 *pos, float radius)
{
    float angle = sefRandf() * 360.0f * circle_angle_scale_literal;
    float x_unit;
    float z_unit;

    __asm__ __volatile__(
        "mfc1 $8, %1\n\t"
        "qmtc2 $8, $vf4\n\t"
        "vcallms 0xe8\n\t"
        "qmfc2.i $8, $vf1\n\t"
        "mtc1 $8, %0\n\t"
        : "=f"(x_unit)
        : "f"(angle)
        : "$8", "memory"
    );
    pos->x = x_unit * radius;

    __asm__ __volatile__(
        "mfc1 $8, %1\n\t"
        "qmtc2 $8, $vf4\n\t"
        "vcallms 0x20\n\t"
        "qmfc2.i $8, $vf1\n\t"
        "mtc1 $8, %0\n\t"
        : "=f"(z_unit)
        : "f"(angle)
        : "$8", "memory"
    );

    pos->y = 0.0f;
    pos->w = 1.0f;
    pos->z = z_unit * radius;
}

static void sefGetSpherePos(Vector4 *position, float radius)
{
    int random_vector[4];
    unsigned int seed_1;
    unsigned int seed_2;
    unsigned int seed_3;
    int random_x;
    int random_y;
    int random_z;
    float component_center;

    do {
        component_center = 16384.0f;
        seed_1 = sefRandSeed * 0x41C64E6Du + 0x3039u;
        seed_2 = seed_1 * 0x41C64E6Du + 0x3039u;
        seed_3 = seed_2 * 0x41C64E6Du + 0x3039u;
        sefRandSeed = seed_3;
        random_x = (seed_1 >> 16) & 0x7FFF;
        random_y = (seed_2 >> 16) & 0x7FFF;
        random_vector[0] = random_x;
        random_vector[1] = random_y;
        random_z = (seed_3 >> 16) & 0x7FFF;
        random_vector[2] = random_z;
    } while (0);

    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "mfc1 $2, %1\n\t"
        "vitof0.xyzw vf1, vf1\n\t"
        "qmtc2 $2, vf2"
        :
        : "r"(random_vector), "f"(component_center)
        : "$2", "memory");
    __asm__ __volatile__("vsubx.xyz vf1, vf1, vf2x" : : : "memory");
    __asm__ __volatile__("vmul.xyz vf2, vf1, vf1" : : : "memory");
    __asm__ __volatile__("vaddy.x vf2, vf2, vf2y" : : : "memory");
    __asm__ __volatile__("vaddz.x vf2, vf2, vf2z" : : : "memory");
    __asm__ __volatile__("vsqrt Q, vf2x" : : : "memory");
    __asm__ __volatile__("vwaitq" : : : "memory");
    __asm__ __volatile__("vaddq.x vf2, vf0, Q" : : : "memory");
    __asm__ __volatile__("vdiv Q, vf0w, vf2x" : : : "memory");
    __asm__ __volatile__("vmove.w vf2, vf0" : : : "memory");
    __asm__ __volatile__("vwaitq" : : : "memory");
    __asm__ __volatile__("vmulq.xyz vf2, vf1, Q" : : : "memory");
    __asm__ __volatile__("qmtc2.ni %0, vf1" : : "r"(radius) : "memory");
    __asm__ __volatile__("vmulx.xyz vf2, vf2, vf1x" : : : "memory");
    __asm__ __volatile__("sqc2 vf2, 0(%0)" : : "r"(position) : "memory");
}

void sefGetCubePosBtm(Vector4 *pos, const int *extent)
{
    Vector4 extent_vector;
    Vector4 random_vector;
    Vector4 *random_position;
    unsigned int seed_1;
    unsigned int seed_2;
    unsigned int seed_3;
    int random_x;
    int random_y;
    int random_z;
    float random_scale;
    float final_scale;
    float half_scale;

    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(extent) : "memory");
    __asm__ __volatile__("vitof0.xyzw vf1, vf1" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(&extent_vector) : "memory");
    random_vector.w = 0.0f;
    random_position = &random_vector;

    random_scale = 3.051851e-05f;
    seed_1 = sefRandSeed * 0x41C64E6Du + 0x3039u;
    seed_2 = seed_1 * 0x41C64E6Du + 0x3039u;
    seed_3 = seed_2 * 0x41C64E6Du + 0x3039u;
    sefRandSeed = seed_3;
    random_x = (seed_1 >> 16) & 0x7FFF;
    random_y = (seed_2 >> 16) & 0x7FFF;
    random_z = (seed_3 >> 16) & 0x7FFF;
    random_vector.x = (float)random_x * random_scale;
    random_vector.y = (float)random_y * random_scale;
    random_vector.z = (float)random_z * random_scale;

    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(random_position) : "memory");
    __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(&extent_vector) : "memory");
    __asm__ __volatile__("vmul.xyz vf1, vf1, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(random_position) : "memory");
    extent_vector.y = 0.0f;
    __asm__ __volatile__("" : : "m"(extent_vector.y));

    half_scale = 0.5f;
    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&extent_vector) : "memory");
    __asm__ __volatile__("mfc1 $8, %0\n\tqmtc2 $8, vf2" : : "f"(half_scale) : "$8", "memory");
    __asm__ __volatile__("vmulx.xyz vf1, vf1, vf2x" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(&extent_vector) : "memory");

    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(random_position) : "memory");
    __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(&extent_vector) : "memory");
    __asm__ __volatile__("vsub.xyz vf1, vf1, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(random_position) : "memory");

    final_scale = 0.1f;
    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(random_position) : "memory");
    __asm__ __volatile__("mfc1 $8, %0\n\tqmtc2 $8, vf2" : : "f"(final_scale) : "$8", "memory");
    __asm__ __volatile__("vmulx.xyz vf1, vf1, vf2x" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(pos) : "memory");
}

static void sefGetCubePosTop(Vector4 *pos, const int *extent)
{
    sefGetCubePosBtm(pos, extent);
    pos->y = -pos->y;
}

static void sefGetCubePos(Vector4 *pos, const int *extent)
{
    Vector4 extent_vector;
    Vector4 random_vector;
    unsigned int seed_1;
    unsigned int seed_2;
    unsigned int seed_3;
    int random_x;
    int random_y;
    int random_z;
    float random_scale;
    float final_scale;
    float half_scale;

    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(extent) : "memory");
    __asm__ __volatile__("vitof0.xyzw vf1, vf1" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(&extent_vector) : "memory");
    random_vector.w = 0.0f;

    random_scale = 3.051851e-05f;
    seed_1 = sefRandSeed * 0x41C64E6Du + 0x3039u;
    seed_2 = seed_1 * 0x41C64E6Du + 0x3039u;
    seed_3 = seed_2 * 0x41C64E6Du + 0x3039u;
    sefRandSeed = seed_3;
    random_x = (seed_1 >> 16) & 0x7FFF;
    random_y = (seed_2 >> 16) & 0x7FFF;
    random_z = (seed_3 >> 16) & 0x7FFF;
    random_vector.x = (float)random_x * random_scale;
    random_vector.y = (float)random_y * random_scale;
    random_vector.z = (float)random_z * random_scale;

    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&random_vector) : "memory");
    __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(&extent_vector) : "memory");
    __asm__ __volatile__("vmul.xyz vf1, vf1, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(&random_vector) : "memory");

    half_scale = 0.5f;
    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&extent_vector) : "memory");
    __asm__ __volatile__("mfc1 $8, %0\n\tqmtc2 $8, vf2" : : "f"(half_scale) : "$8", "memory");
    __asm__ __volatile__("vmulx.xyz vf1, vf1, vf2x" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(&extent_vector) : "memory");

    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&random_vector) : "memory");
    __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(&extent_vector) : "memory");
    __asm__ __volatile__("vsub.xyz vf1, vf1, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(&random_vector) : "memory");

    final_scale = 0.1f;
    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&random_vector) : "memory");
    __asm__ __volatile__("mfc1 $8, %0\n\tqmtc2 $8, vf2" : : "f"(final_scale) : "$8", "memory");
    __asm__ __volatile__("vmulx.xyz vf1, vf1, vf2x" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(pos) : "memory");
}

static void sefGetOfsRange(Vector4 *position, const void *offset_table, int index)
{
    const int *offsets = offset_table;

    __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(position) : "memory");

    if (index == 0) {
        float radius;

        if (!__builtin_expect(offsets[0], 0)) {
            return;
        }
        radius = (float)offsets[0] * offsetRadiusScale;
        sefGetSpherePos(position, radius * sefRandf());
        return;
    }
    if (index == 1) {
        if (!__builtin_expect(offsets[0], 0)) {
            return;
        }
        sefGetSpherePos(position, (float)offsets[0] * spherePosScale);
        return;
    }
    if (index == 2) {
        sefGetCubePos(position, offsets);
        return;
    }
    if (index == 3) {
        sefGetCubePosTop(position, offsets);
        return;
    }
    if (index == 4) {
        sefGetCubePosBtm(position, offsets);
        return;
    }
    if (index == 5) {
        Matrix4 rotation;
        Vector4 circle_position;

        sefGetCirclePos(&circle_position, (float)offsets[0] * circlePosScale);
        __asm__ __volatile__(
            "vmr32.xyzw vf1, vf0\n\t"
            "vmr32.xyzw vf2, vf1\n\t"
            "vmr32.xyzw vf3, vf2\n\t"
            "sqc2 vf0, 48(%0)\n\t"
            "sqc2 vf1, 32(%0)\n\t"
            "sqc2 vf2, 16(%0)\n\t"
            "sqc2 vf3, 0(%0)"
            :
            : "r"(&rotation)
            : "memory");

        if (offsets[1] != 0) {
            MMathRotateMatrixX(&rotation, &rotation, (float)offsets[1] * rotateXDegToRad);
        }
        if (offsets[2] != 0) {
            MMathRotateMatrixZ(&rotation, &rotation, (float)offsets[2] * rotateZDegToRad);
        }
        MMathApplyMatrix(position, &rotation, &circle_position);
        return;
    }
    if (index == 6) {
        unsigned int seed = sefRandSeed * 0x41C64E6Du;
        float range = (float)offsets[0];
        int sample;

        seed += 0x3039u;
        sample = (seed >> 16) & 0x7FFFu;
        sefRandSeed = seed;
        sefGetCirclePos(position,
                        (float)sample * invShortMax * range * offsetYScale);
        position->y = sefRandf() * (float)offsets[1] * offsetYScale;
        return;
    }
    tracePrint(badRangeParamFormat, index);
}

static void sefGetDirMatrix(Matrix4 *dest, Vector4 *dir)
{
    Vector4 flipped;
    Vector4 angle;
    float xz_length;
    int length_bits;

    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "vsub.xyz vf2, vf0, vf1\n\t"
        "vmul.xz vf4, vf2, vf2\n\t"
        "sqc2 vf0, 0(%1)\n\t"
        "vaddz.x vf3, vf4, vf4z\n\t"
        "vmove.y vf2, vf1\n\t"
        "vsqrt Q, vf3x\n\t"
        "sqc2 vf2, 0(%2)"
        : : "r"(dir), "r"(&angle), "r"(&flipped) : "memory");
    angle.y = srsAtan2(flipped.x, flipped.z);
    __asm__ __volatile__(
        "vwaitq\n\t"
        "vaddq.x vf1, vf0, Q\n\t"
        "qmfc2 %0, vf1\n\t"
        "mtc1 %0, %1"
        : "=r"(length_bits), "=f"(xz_length) : : "memory");
    angle.x = srsAtan2(flipped.y, xz_length);
    __asm__ __volatile__(
        "lqc2 vf7, 0(%0)\n\t"
        "vaddx.x vf4, vf0, vf7x\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.x vf5, vf0, vf1x\n\t"
        "vaddy.x vf4, vf0, vf7y\n\t"
        "vcallms 0xe8\n\t"
        "vaddx.y vf5, vf0, vf1x\n\t"
        "vaddx.x vf4, vf0, vf7x\n\t"
        "vcallms 0x20\n\t"
        "vaddx.x vf6, vf0, vf1x\n\t"
        "vaddy.x vf4, vf0, vf7y\n\t"
        "vcallms 0x20\n\t"
        "vaddx.y vf6, vf0, vf1x\n\t"
        "vmove.xyzw vf4, vf0\n\t"
        "vmr32.xyzw vf3, vf4\n\t"
        "vmr32.xyzw vf2, vf3\n\t"
        "vmr32.xyzw vf1, vf2\n\t"
        "vmulay.xyzw ACC, vf1, vf5y\n\t"
        "vmsuby.xyzw vf7, vf3, vf6y\n\t"
        "vmulay.xyzw ACC, vf3, vf5y\n\t"
        "vmaddy.xyzw vf3, vf1, vf6y\n\t"
        "vmulax.xyzw ACC, vf2, vf5x\n\t"
        "vmaddx.xyzw vf8, vf3, vf6x\n\t"
        "vmulax.xyzw ACC, vf3, vf5x\n\t"
        "vmsubx.xyzw vf3, vf2, vf6x\n\t"
        "sqc2 vf7, 0(%1)\n\t"
        "sqc2 vf8, 16(%1)\n\t"
        "sqc2 vf3, 32(%1)\n\t"
        "sqc2 vf4, 48(%1)"
        : : "r"(&angle), "r"(dest) : "memory");
}

void sefGetVecMatrix(Matrix4 *dest, Vector4 *source_a, Vector4 *source_b)
{
    Vector4 dir;

    __asm__ __volatile__(
        "lqc2 $vf1, 0(%0)\n\t"
        "lqc2 $vf2, 0(%1)\n\t"
        "vsub.xyz $vf1xyz, $vf1xyz, $vf2xyz\n\t"
        "sqc2 $vf1, 0(%2)\n\t"
        :
        : "r"(source_a), "r"(source_b), "r"(&dir)
        : "memory"
    );
    sefGetDirMatrix(dest, &dir);
}

static void sefGetDirVector(void *dest, void *source) {
    __asm__ __volatile__("lqc2 vf1, 0(%0)\n\tvsub.z vf1, vf0, vf1\n\tvmulax.xyzw ACC, vf7, vf1x\n\tvmadday.xyzw ACC, vf8, vf1y\n\tvmaddaz.xyzw ACC, vf3, vf1z\n\tvmaddw.xyzw vf1, vf4, vf1w\n\tsqc2 vf1, 0(%1)" :  : "r"(source), "r"(dest) : "memory");
}

static void sefGetStartPos(Vector4 *position, SefParticleOffset *record) {
    sefGetOfsRange(position, record->start_offset_table, *record->start_index);
    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&record->start_position) : "memory");
    __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(position) : "memory");
    __asm__ __volatile__("vadd.xyz vf1, vf1, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(position) : "memory");
}

static void sefGetTargetPos(Vector4 *position, SefParticleOffset *record) {
    sefGetOfsRange(position, record->target_offset_table, *record->target_index);
    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&record->target_position) : "memory");
    __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(position) : "memory");
    __asm__ __volatile__("vadd.xyz vf1, vf1, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(position) : "memory");
}

static void sefGetSpeed(Vector4 *position, void *particle, int mode, int option)
{
    unsigned char *record = (unsigned char *)particle;
    Vector4 vector;
    Vector4 *source;
    Vector4 *dest;
    unsigned char *parent;
    float speed;
    Vector4 *out;

    source = (Vector4 *)(record + 0x200);
    speed = *(float *)(record + 0x270) + *(float *)(record + 0x150);
    out = position;
    if (0.0f == speed) {
        __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(out) : "memory");
        return;
    }

    if (option != 0 && mode != 0) {
        MMathSpecialVectorSub(&vector, source, source, (float)option);
    } else {
        __asm__ __volatile__(
            "lq $8, 0(%0)\n\t"
            "sq $8, 0(%1)"
            : : "r"(source), "r"(&vector) : "$8", "memory");
    }

    if (mode >= 5) {
        sefGetVecMatrix((Matrix4 *)particle, (Vector4 *)(record + 0xd0), (Vector4 *)(record + 0xc0));
        sefGetDirVector(&vector, &vector);
    } else {
        if (mode == 4) {
            if (_parentLine >= 0 && _nowParentLocal >= 0) {
                parent = &_ptAlloc[_nowParentLocal * 640];
            } else {
                parent = 0;
            }
            if (parent != 0) {
                sefGetDirMatrix((Matrix4 *)particle, (Vector4 *)(parent + 0x120));
                sefGetDirVector(&vector, &vector);
            }
        } else if (mode == 3) {
            speed = 10.0f * speed;
            __asm__ __volatile__("lq $8, 0(%0)\n\tsq $8, 0(%0)" : : "r"(&vector) : "$8", "memory");
        } else if (mode == 1) {
            if (_battleData.status.caster != 0) {
                MMathApplyMatrix(&vector, _battleData.status.casterMatrix, &vector);
            }
        }
    }

    dest = (Vector4 *)(record + 0x120);
    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "mfc1 $8, %2\n\t"
        "qmtc2 $8, vf2\n\t"
        "sqc2 vf1, 0(%1)\n\t"
        "vmulx.xyz vf1xyz, vf1xyz, vf2x\n\t"
        "sqc2 vf1, 0(%3)"
        : : "r"(&vector), "r"(dest), "f"(speed), "r"(out) : "$8", "memory");
}

void sefMoveParticle(void *arg0, void *arg1, int arg2) {
    unsigned char *particle = (unsigned char *)arg0;
    Vector4 speed;
    int target;

    target = *(short *)*(void **)((unsigned char *)arg1 + 16);
    if (target == 529) {
        target = (_battleData.status.phase >= 2) ? 527 : 528;
    }
    if (_battleData.status.running != 0) {
        if ((unsigned int)(target - 257) < 16U) {
            target += 1792;
        }
        if ((_nowScheduler->flags & 0x1400) == 0x1000) {
            __asm__ __volatile__(
                "lq $8, 0(%1)\n\t"
                "lq $9, 16(%1)\n\t"
                "lq $10, 32(%1)\n\t"
                "lq $11, 48(%1)\n\t"
                "sq $8, 0(%0)\n\t"
                "sq $9, 16(%0)\n\t"
                "sq $10, 32(%0)\n\t"
                "sq $11, 48(%0)"
                : : "r"(particle + 0x40), "r"(_battleData.status.particleTransform)
                : "$8", "$9", "$10", "$11", "memory");
            *(float *)(particle + 0x154) = _battleData.status.particleScale;
        }
    }

    if (*(signed char *)(particle + 0x163) == 0) {
        sefGetSpeed(&speed, particle, target, arg2);
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%0)\n\t"
            "lqc2 $vf2, 0(%1)\n\t"
            "vadd.xyz $vf1xyz, $vf1xyz, $vf2xyz\n\t"
            "sqc2 $vf1, 0(%0)"
            : : "r"(particle + 0xc0), "r"(&speed) : "memory");
    }
    *(float *)(particle + 0x114) = *(float *)(particle + 0x114) - _gravity;
    __asm__ __volatile__(
        "lqc2 $vf1, 0(%0)\n\t"
        "lqc2 $vf2, 0(%1)\n\t"
        "vadd.xyz $vf1xyz, $vf1xyz, $vf2xyz\n\t"
        "sqc2 $vf1, 0(%0)"
        : : "r"(particle + 0xc0), "r"(particle + 0x110) : "memory");
    if (_colision != 0.0f) {
        if (*(float *)(particle + 0xc4) <= _height) {
            float colision;
            if (*(signed char *)(particle + 0x163) == 0) {
                __asm__ __volatile__(
                    "lqc2 $vf1, 0(%0)\n\t"
                    "lqc2 $vf2, 0(%1)\n\t"
                    "vadd.xyz $vf1xyz, $vf1xyz, $vf2xyz\n\t"
                    "sqc2 $vf1, 0(%1)"
                    : : "r"(&speed), "r"(particle + 0x110) : "memory");
            }
            *(particle + 0x163) = 1;
            colision = _colision;
            *(float *)(particle + 0xc4) = _height;
            __asm__ __volatile__(
                "lqc2 $vf1, 0(%0)\n\t"
                "mfc1 $8, %1\n\t"
                "qmtc2 $8, $vf2\n\t"
                "vmulx.xyz $vf1xyz, $vf1xyz, $vf2x\n\t"
                "sqc2 $vf1, 0(%0)"
                : : "r"(particle + 0x110), "f"(colision) : "$8", "memory");
            if (*(float *)(particle + 0x114) < 0.0f) {
                *(float *)(particle + 0x114) = -*(float *)(particle + 0x114);
            }
        }
    }
}

/*
 * sefCalcLocalMatrix (main:0x002e2d48), re-treated from the accepted assembly
 * group src/main/sef/sefCalcLocalMatrix.s toward readable C with constrained
 * ee-vu-cop2 inline assembly (route ee-vu-cop2, docs/ps2-capabilities.md).
 * The second function of that group, sefCalcRotChange, converted the same
 * way further down this file; the group now holds no tracked assembly.
 *
 * Names recovered from config/symbols/main.txt: _battleData (0x00794110),
 * _parentLine, _nowParentLocal, _ptAlloc. The battle state block and the
 * scheduler local record are read through narrowly evidenced scalar accesses
 * rather than through structs with invented members: only the fields below
 * are evidenced, the bytes between them are not, and docs/naming.md forbids
 * inventing padding members to close the gaps.
 *   _battleData + 0x00c  short  battle phase compared against 2
 *   _battleData + 0x22c  int    "battle is running" flag
 *   _battleData + 0x200         the matrix source both callees take
 *   record     + 0x000         the local matrix this call produces
 *   record     + 0x080         the base matrix it is multiplied by
 *   record     + 0x0c0         the rotation vector
 *   record     + 0x0f0/0x0f8   the offset the multiply is gated on
 *
 * record is the SefParticle whose local_matrix (+0x000) this call produces;
 * base_matrix, start_position and the rotated offset fields are the
 * particle's own members at the same offsets. sefGetDirMatrix/sefGetVecMatrix
 * take the destination matrix at offset 0 (src/main/sef.h).
 */
static void sefCalcLocalMatrix(SefParticle *record, int target, int previous_target)
{
    SefBattleStatus *battle = &_battleData.status;
    SefLocalOffset *local = (SefLocalOffset *)record;
    int in_battle = battle->running;

    target &= ~0xe000;
    if (in_battle != 0 && (unsigned int)(target - 257) < 16) {
        target += 1792;
    }
    if (target == 529) {
        target = (battle->phase >= 2) ? 527 : 528;
    }

    if (target == 4) {
        unsigned char *parent;

        if (_parentLine >= 0 && _nowParentLocal >= 0) {
            parent = &_ptAlloc[_nowParentLocal * 640];
        } else {
            parent = 0;
        }
        if (parent != 0 && target != previous_target) {
            sefGetDirMatrix(&record->local_matrix, (Vector4 *)(parent + SEF_PARENT_DIR_MATRIX));
        }
    } else if (target == 1) {
        /* The base matrix becomes the local one: the four quadword moves the
         * original performs inline through the EE's own scratch GPRs. */
        __asm__ __volatile__(
            "lq $8, 0(%0)\n\t"
            "lq $9, 16(%0)\n\t"
            "lq $10, 32(%0)\n\t"
            "lq $11, 48(%0)\n\t"
            "sq $8, 0(%1)\n\t"
            "sq $9, 16(%1)\n\t"
            "sq $10, 32(%1)\n\t"
            "sq $11, 48(%1)"
            :
            : "r"(&record->base_matrix), "r"(&record->local_matrix)
            : "$8", "$9", "$10", "$11", "memory");
        return;
    } else if (in_battle != 0 && (target & 0x200) != 0) {
        Vector4 direction;

        __asm__ __volatile__(
            "lqc2 $vf1, 0(%0)\n\t"
            "lqc2 $vf2, 0(%1)\n\t"
            "vadd.xyz $vf1xyz, $vf1xyz, $vf2xyz\n\t"
            "sqc2 $vf1, 0(%2)"
            :
            : "r"(&record->start_position), "r"(&_nowScheduler->position),
              "r"(&direction)
            : "memory");
        sefGetVecMatrix(&record->local_matrix, &battle->matrixSource, &direction);
    } else if (target >= 5) {
        sefGetVecMatrix(&record->local_matrix, (Vector4 *)localPointVector,
                        &record->start_position);
    } else {
        /* The identity matrix, built by rotating $vf0 = (0,0,0,1) one lane at
         * a time and storing the rows back to front. */
        __asm__ __volatile__(
            "vmr32.xyzw $vf1xyzw, $vf0xyzw\n\t"
            "vmr32.xyzw $vf2xyzw, $vf1xyzw\n\t"
            "vmr32.xyzw $vf3xyzw, $vf2xyzw\n\t"
            "sqc2 $vf0, 48(%0)\n\t"
            "sqc2 $vf1, 32(%0)\n\t"
            "sqc2 $vf2, 16(%0)\n\t"
            "sqc2 $vf3, 0(%0)"
            :
            : "r"(&record->local_matrix)
            : "memory");
    }

    if (local->offsetXYBits != 0 ||
            local->offsetZ != 0.0f) {
        MMathMulMatrix(&record->local_matrix, &record->local_matrix, &record->base_matrix);
    }
}

/*
 * sefCalcRotChange (main:0x002e2f00), re-treated from the accepted assembly
 * group src/main/sef/sefCalcRotChange.s (itself the re-cut remainder of
 * src/main/sef/sefCalcLocalMatrix.s once its sibling sefCalcLocalMatrix
 * converted, config/units/math-correction13-5.json) toward readable C with
 * constrained ee-vu-cop2 inline assembly (route ee-vu-cop2,
 * docs/ps2-capabilities.md). The group now holds no assembly of its own.
 *
 * particle's SefParticle fields hold what this function reads and writes
 * (the byte offsets sefCalcLocalMatrix's comment evidences are the same
 * fields, base_matrix at 0x080 among them):
 *   particle->transform (+0x040)   the source matrix MMathRotateMatrixXYZ
 *                    multiplies by when the masked mode selects it
 *   particle->base_matrix (+0x080) the matrix every path here rotates in
 *                    place and sefCalcLocalMatrix later reads
 *   particle->random_rotation (+0x0e0) the accumulated offset before this
 *                    frame's change
 *   particle->rotated_position (+0x0f0/0x0f4/0x0f8)
 *                    the combined offset this call produces and
 *                    sefCalcLocalMatrix gates its multiply on. The three
 *                    components are the x/y/z of one Vector4, so the
 *                    single-axis rotations below read them as its fields;
 *                    SEF_LOCAL_OFFSET_Z stays for sefCalcLocalMatrix, which
 *                    reads +0x0f8 on its own
 *   particle->rotation (+0x1c0) the per-frame rotation delta: degrees on
 *                    entry, converted to radians in place by the first block
 *
 * flags selects the rotation order: masking off bits 0x6000 and comparing to
 * 1 picks the XYZ path (Y/Z/X one axis at a time when bit 0x4000 is also set,
 * or one MMathRotateMatrixXYZ call over the source matrix otherwise); with
 * that not selected, bit 0x6000 clear takes MMathRotateMatrixYXZ, and
 * otherwise bit 0x4000 takes the same Y/Z/X order and bit 0x2000 takes
 * MMathRotateMatrixXYZ again, this time from the identity default and the
 * combined offset. update disables all of it: only the vector scale and
 * offset accumulation happen. MMathRotateMatrixX/Y/Z/XYZ/YXZ are main/tu219
 * (src/main/m_math.c); a null matrix argument here reaches their own
 * null-default idiom (docs/tu-worker.md), not a call site concern.
 *
 * The degrees-to-radians factor is DEG_TO_RAD, a second occurrence of the
 * same .lit4 constant sefDeg2RadVector reads as lit4_004d8354 above: EE GCC
 * 2.96 does not merge identical .lit4 words across call sites, so each user
 * gets its own address and its own extern (docs/naming.md, "Scaffold-owned
 * data keeps its splat name").
 *
 */
static void sefCalcRotChange(SefParticle *particle, int flags, int update)
{
    __asm__ __volatile__("lqc2 $vf2, 0(%0)" : : "r"(&particle->rotation) : "memory");

    {
        register float degToRad asm("$f8") = DEG_TO_RAD;

        __asm__ __volatile__(
            "mfc1 $8, %1\n\t"
            "qmtc2.ni $8, $vf1\n\t"
            "vmulx.xyz $vf2, $vf2, $vf1x\n\t"
            "sqc2 $vf2, 0(%0)"
            :
            : "r"(&particle->rotation), "f"(degToRad)
            : "$8", "memory");
    }

    __asm__ __volatile__(
        "lqc2 $vf1, 0(%1)\n\t"
        "lqc2 $vf2, 0(%2)\n\t"
        "vadd.xyz $vf1xyz, $vf1xyz, $vf2xyz\n\t"
        "sqc2 $vf1, 0(%0)"
        :
        : "r"(&particle->rotated_position), "r"(&particle->random_rotation),
          "r"(&particle->rotation)
        : "memory");

    if (update == 0) {
        return;
    }

    if ((flags & 0xffff1fff) == 1) {
        if (flags & 0x4000) {
            MMathRotateMatrixY(&particle->base_matrix, (Matrix4 *)0, particle->rotated_position.y);
            MMathRotateMatrixZ(&particle->base_matrix, &particle->base_matrix,
                               particle->rotated_position.z);
            MMathRotateMatrixX(&particle->base_matrix, &particle->base_matrix,
                               particle->rotated_position.x);
        } else {
            MMathRotateMatrixXYZ(&particle->base_matrix, &particle->transform,
                                 &particle->rotation);
        }
        return;
    }

    if ((flags & 0x6000) == 0) {
        MMathRotateMatrixYXZ(&particle->base_matrix, (Matrix4 *)0, &particle->rotated_position);
        return;
    }

    if (flags & 0x4000) {
        MMathRotateMatrixY(&particle->base_matrix, (Matrix4 *)0, particle->rotated_position.y);
        MMathRotateMatrixZ(&particle->base_matrix, &particle->base_matrix,
                           particle->rotated_position.z);
        MMathRotateMatrixX(&particle->base_matrix, &particle->base_matrix,
                           particle->rotated_position.x);
        return;
    }

    if (flags & 0x2000) {
        MMathRotateMatrixXYZ(&particle->base_matrix, (Matrix4 *)0, &particle->rotated_position);
        return;
    }
}

static int sefExecLineLocalData(SefLineData *line_data, unsigned short frame)
{
    SefParticle *particle;
    short *handle_pointer;
    int active_count = 0;
    int add_light = 0;
    SefKey5 *initial_vector_keys = line_data->initial_vector_keys != 0
        ? line_data->initial_vector_keys->keys : 0;
    SefKey5 *rotation_vector_keys = line_data->rotation_vector_keys != 0
        ? line_data->rotation_vector_keys->keys : 0;
    SefKey3 *float_keys = line_data->float_keys != 0
        ? line_data->float_keys->keys : 0;
    SefKey3 *first_integer_keys = line_data->first_integer_keys_local != 0
        ? line_data->first_integer_keys_local->keys : 0;
    SefKey5 *color_keys = line_data->key_1E0 != 0
        ? line_data->key_1E0->keys : 0;
    int loop_mode = line_data->rotation_vector_keys != 0
        ? line_data->rotation_vector_keys->mode : 0;
    int previous_mode = line_data->rotation_vector_keys != 0
        ? ((short *)line_data->rotation_vector_keys)[-1] : 0;
    int update_parent;
    int update_scaled_size;
    int update_rotation;
    int index;
    int component_result;
    int point_part;
    short reverse_direction;

    (void)frame;
    update_parent = (_nowScheduler->flags & 0x800) != 0 &&
                    (line_data->line_flags & 0x400) == 0;
    update_scaled_size = line_data->key_1E0->mode != 3 && line_data->size_vector_keys != 0;
    reverse_direction = _revDirZ;
    point_part = loop_mode & ~0xe000;
    if (point_part >= 5) {
        sefGetPoint((Vector4 *)localPointVector, _zeroPos_004CBF00, point_part);
    }

    index = 0;
    handle_pointer = line_data->local_point_handles;
    loop_mode = previous_mode != 0 ? loop_mode | 0x8000 : loop_mode;
    for (; index < 256; index++, handle_pointer++) {
        short handle = *handle_pointer;
        if (handle >= 0) {
            particle = (SefParticle *)&_ptAlloc[handle * 0x280];
            if (particle != 0) {
                particle->state++;
                if (particle->state >= 1024) {
                    particle->state = 1023;
                }
                if (particle->state <= particle->frame) {
                    component_result = 0;
                    update_rotation = particle->rotation_state != 4 || previous_mode != 0;
                    if (initial_vector_keys != 0) {
                        sefLerpIVector(initial_vector_keys,
                                       (SefProgressState *)((unsigned char *)particle + 0x190));
                    }
                    if (rotation_vector_keys != 0 &&
                        sefLerpVector(rotation_vector_keys,
                                      (unsigned char *)particle + 0x1b0)) {
                        sefCalcRotChange(particle, loop_mode, update_rotation);
                    }
                    if (line_data->position_vector_keys != 0) {
                        sefLerpIVector(line_data->position_vector_keys,
                                       (SefProgressState *)((unsigned char *)particle + 0x1d0));
                    }
                    if (first_integer_keys != 0) {
                        sefLerpInt(first_integer_keys,
                                   &particle->first_integer_progress);
                    }
                    if (line_data->size_vector_keys != 0 &&
                        sefLerpVector(line_data->size_vector_keys,
                                      (unsigned char *)particle + 0x230) && update_scaled_size) {
                        __asm__ __volatile__(
                            "lqc2 $vf1, 0(%0)\n\t"
                            "mfc1 $8, %1\n\t"
                            "qmtc2 $8, $vf2\n\t"
                            "vmulx.xyz $vf1xyz, $vf1xyz, $vf2x\n\t"
                            "sqc2 $vf1, 0(%0)"
                            : : "r"(&particle->size), "f"(particleSizeScale)
                            : "$8", "memory");
                    }
                    sefLerpFloat(float_keys, &particle->float_progress);
                    if (sefLerpVector(color_keys,
                                      (unsigned char *)particle + 0x1f0) && reverse_direction != 0) {
                        particle->color_offset.z = -particle->color_offset.z;
                    }
                    if (line_data->light_keys != 0 && add_light == 0) {
                        unsigned long long light_xy;
                        sefLerpIVector2(line_data->light_keys, &particle->light_progress);
                        __builtin_memcpy(&light_xy, particle->light.coordinates, sizeof(light_xy));
                        if (light_xy != 0 || particle->light.coordinates[2] != 0) {
                            sefAddLightActor(&line_data->position, &particle->light);
                            add_light = 1;
                        }
                    }

                    _nowParentLocal = particle->kind;
                    __asm__ __volatile__(
                        "lqc2 $vf21, 0(%0)\n\t"
                        "lqc2 $vf1, 0(%2)\n\t"
                        "lqc2 $vf3, 0(%3)\n\t"
                        "vmul.xyz $vf22, $vf21, $vf21\n\t"
                        "mfc1 $2, %4\n\t"
                        "qmtc2 $2, $vf2\n\t"
                        "vaddy.x $vf22x, $vf22x, $vf22y\n\t"
                        "vitof0.xyzw $vf1, $vf1\n\t"
                        "vaddz.x $vf22x, $vf22x, $vf22z\n\t"
                        "vmulx.xyz $vf1xyz, $vf1xyz, $vf2x\n\t"
                        "vsqrt Q, vf22x\n\t"
                        "vadd.xyz $vf3xyz, $vf1xyz, $vf3xyz\n\t"
                        "sqc2 $vf3, 0(%1)\n\t"
                        "vwaitq\n\t"
                        "vaddq.x vf22x, vf0x, Q\n\t"
                        "vdiv Q, vf0w, vf22x\n\t"
                        "vmove.w $vf22w, $vf0w"
                        :
                        : "r"(&particle->color_offset), "r"(particle->animated_color_xyz),
                          "r"(&particle->color), "r"(&particle->random_translation),
                          "f"(0.015625f)
                        : "$2", "memory");
                    /* Copy the packed integer scale lanes, then replace the
                     * fourth word with the animation value. */
                    particle->animated_scale.quad = particle->color_scale_bits;
                    __builtin_memcpy(&particle->animated_scale.words[3],
                                     &particle->first_integer_progress.value,
                                     sizeof(particle->first_integer_progress.value));
                    sefAnimate((unsigned char *)line_data, (unsigned char *)particle,
                               particle->animate_advance);
                    __asm__ __volatile__(
                        "vwaitq\n\t"
                        "vmulq.xyz vf22xyz, vf21xyz, Q\n\t"
                        "sqc2 $vf22, 0(%0)"
                        : : "r"(&particle->color_offset) : "memory");
                    if (line_data->second_integer_keys_local != 0) {
                        sefLerpInt(line_data->second_integer_keys_local,
                                   &particle->second_integer_progress);
                        component_result = particle->second_integer_progress.value;
                    }
                    if (update_parent) {
                        __asm__ __volatile__(
                            "lqc2 $vf1, 0(%1)\n\t"
                            "lqc2 $vf2, 0(%2)\n\t"
                            "vadd.xyz $vf1xyz, $vf1xyz, $vf2xyz\n\t"
                            "sqc2 $vf1, 0(%0)"
                            : : "r"(&particle->start_position),
                                "r"(&particle->start_position),
                                "r"(&line_data->position_delta)
                            : "memory");
                    }
                    sefMoveParticle(particle, &line_data->scale_keys,
                                    component_result);
                    if (update_rotation) {
                        sefCalcLocalMatrix(particle, loop_mode,
                                           line_data->key_1E0->mode);
                    }
                    active_count++;
                } else {
                    particle->frame = 0;
                    sefFreeLocalData((unsigned char *)line_data, index);
                }
            }
        }
    }
    return (scanFlags[0] & 0x100) == 0 ? active_count : 1;
}

static int sefFreeLocalData(unsigned char *line_data, unsigned int index) {
    int valid = index < 0x100;
    int offset = (int)(index * 2) + SEF_LOCAL_DATA_PT_TABLE;

    if (valid) {
        short *slot = (short *)(line_data + offset);

        sevFreePtAllocator(*slot);
        offset = -1;
        *slot = -1;
    }
    return offset;
}

static int sefAllocLocalData(SefLocalData *data)
{
    int index;
    int end;
    int handle;

    index = data->cursor;
    end = (index - 1) & 0xff;
    for (; index != end; index = (index + 1) & 0xff) {
        if (data->pointHandles[index] < 0) {
            break;
        }
    }
    if (index == end) {
        return -1;
    }
    handle = sevAllocPtAllocator();
    if (handle < 0) {
        return -1;
    }
    data->pointHandles[index] = handle;
    data->cursor = (index + 1) & 0xff;
    return index;
}

static void sefDestroyLocalData(SefLocalData *data)
{
    int index;

    for (index = 0; index < 0x100; index++) {
        if (data->pointHandles[index] >= 0) {
            sefFreeLocalData((unsigned char *)data, index);
        }
    }
}

static __inline__ float sefLineRandomOffset(short *range_word)
{
    int range = *range_word;
    unsigned int seed;
    int sample;

    if (range != 0) {
        seed = sefRandSeed * 0x41C64E6Du + 0x3039u;
        sefRandSeed = seed;
        sample = (seed >> 16) & 0x7fff;
        return (float)(sample % range - range / 2);
    }
    return 0.0f;
}

static void sefInitLineData(SefLineData *line_data, int start_time, int frame,
                            SefLineScriptHeader *line_script, int link_index,
                            int global_data_count, int has_line_flag)
{
    unsigned short *key_table;
    unsigned short *line_table;
    short *scale_words;
    short gravity_input;
    short height_input;
    short collision_input;
    Vector4 *initial_position;
    int source_mode;
    SchedulerState *const *current_scheduler = &_nowScheduler;
    float vector_scale;

    key_table = (unsigned short *)scGetImmAdrImmIdx2(_nowScriptAdr, line_script->table_indices, 0);
    line_table = (unsigned short *)scGetImmAdrImmIdx2(_nowScriptAdr, line_script->table_indices, 1);
    gravity_input = line_script->gravity;
    collision_input = line_script->collision;
    height_input = line_script->height;

    sefMemZero(line_data, 0x820);
    memset(line_data->local_point_handles, -1, 0x200);

    if (start_time == 0x1000) {
        start_time = 0;
    }
    if (has_line_flag != 0) {
        line_data->line_flags = 0x401;
    } else {
        line_data->line_flags = 1;
    }

    line_data->timer = frame - start_time;
    line_data->link_index = link_index;
    line_data->start_time = start_time;
    line_data->loop_mode = frame;
    line_data->global_data_count = global_data_count;
    line_data->gravity = (float)gravity_input * gravityScale;
    line_data->collision_progress = collision_input;
    line_data->height = (float)height_input * heightScale;
    line_data->field_80E = 0;
    line_data->local_result = 0;
    line_data->frame = 0;

    if ((*current_scheduler)->flags & 0x400) {
        line_data->line_flags |= 0x400;
    }

    line_data->scale_keys = (short *)scGetImmAdrImmIdx2(_nowScriptAdr, key_table, 0);
    line_data->first_vector_keys = (struct SefKey5Table *)scGetImmAdrImmIdx2(_nowScriptAdr, key_table, 1);
    line_data->rotation_keys = (short *)scGetImmAdrImmIdx2(_nowScriptAdr, key_table, 2);
    line_data->second_vector_keys = (struct SefKey5Table *)scGetImmAdrImmIdx2(_nowScriptAdr, key_table, 3);
    line_data->key_1E0 = (SefModeKeyTable *)scGetImmAdrImmIdx2(_nowScriptAdr, key_table, 4);
    line_data->key_1E4 = (unsigned char *)scGetImmAdrImmIdx2(_nowScriptAdr, key_table, 5);
    line_data->position_keys = (short *)scGetImmAdrImmIdx2(_nowScriptAdr, key_table, 6);
    line_data->first_integer_keys = (struct SefKey3 *)scGetImmAdrImmIdx2(_nowScriptAdr, key_table, 7);
    line_data->second_integer_keys = (struct SefKey3 *)scGetImmAdrImmIdx2(_nowScriptAdr, key_table, 8);
    line_data->key_1F0 = (unsigned char *)scGetImmAdrImmIdx2(_nowScriptAdr, key_table, 9);

    scale_words = line_data->scale_keys;
    line_data->origin_offset.x = sefLineRandomOffset(&scale_words[1]);
    line_data->origin_offset.y = sefLineRandomOffset(&scale_words[2]);
    line_data->origin_offset.z = sefLineRandomOffset(&scale_words[3]);

    if (line_data->first_vector_keys != 0 &&
        line_data->first_vector_keys->keys[1].end_frame == 0x400) {
        sefLerpIVectorA(line_data->first_vector_keys->keys[0].vector,
                        &line_data->first_vector_progress.vector.floats);
    }
    if (line_data->second_vector_keys != 0 &&
        line_data->second_vector_keys->keys[1].end_frame == 0x400) {
        sefLerpIVectorA(line_data->second_vector_keys->keys[0].vector,
                        &line_data->second_vector_progress.vector.floats);
    }

    if (line_data->scale_keys != 0 && line_data->scale_keys[9] == 0x400) {
        sefLerpVectorA(line_data->scale_keys + 4, &line_data->global_position_source);
        vector_scale = globalPositionScale;
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%0)\n\t"
            "mfc1 $8, %1\n\t"
            "qmtc2.ni $8, $vf2\n\t"
            "vmulx.xyz $vf1, $vf1, $vf2x\n\t"
            "sqc2 $vf1, 0(%0)"
            :
            : "r"(&line_data->global_position_source), "f"(vector_scale)
            : "$8", "memory");
    }
    if (line_data->rotation_keys != 0 && line_data->rotation_keys[9] == 0x400) {
        sefLerpVectorA(line_data->rotation_keys + 4, &line_data->parent_position_source);
        vector_scale = parentPositionScale;
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%0)\n\t"
            "mfc1 $8, %1\n\t"
            "qmtc2.ni $8, $vf2\n\t"
            "vmulx.xyz $vf1, $vf1, $vf2x\n\t"
            "sqc2 $vf1, 0(%0)"
            :
            : "r"(&line_data->parent_position_source), "f"(vector_scale)
            : "$8", "memory");
    }
    if (line_data->position_keys != 0 && line_data->position_keys[9] == 0x400) {
        sefLerpVectorA(line_data->position_keys + 4, line_data->position_source);
        vector_scale = positionSourceScale;
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%0)\n\t"
            "mfc1 $8, %1\n\t"
            "qmtc2.ni $8, $vf2\n\t"
            "vmulx.xyz $vf1, $vf1, $vf2x\n\t"
            "sqc2 $vf1, 0(%0)"
            :
            : "r"(line_data->position_source), "f"(vector_scale)
            : "$8", "memory");
    }

    line_data->script_parameters[0] = line_table[0];
    line_data->script_parameters[1] = line_table[1];
    line_data->initial_vector_keys = (SefKey5Prefix8 *)scGetImmAdrImmIdx2(_nowScriptAdr, line_table, 2);
    line_data->rotation_vector_keys = (SefModeKeyTable *)scGetImmAdrImmIdx2(_nowScriptAdr, line_table, 3);
    line_data->position_vector_keys = (SefKey5 *)scGetImmAdrImmIdx2(_nowScriptAdr, line_table, 4);
    line_data->first_integer_keys_local = (SefKey3Table *)scGetImmAdrImmIdx2(_nowScriptAdr, line_table, 5);
    line_data->size_vector_keys = (SefKey5 *)scGetImmAdrImmIdx2(_nowScriptAdr, line_table, 6);
    line_data->second_integer_keys_local = (SefKey3 *)scGetImmAdrImmIdx2(_nowScriptAdr, line_table, 7);
    line_data->light_keys = (SefKey5 *)scGetImmAdrImmIdx2(_nowScriptAdr, line_table, 8);
    line_data->float_keys = (SefKey3Table *)scGetImmAdrImmIdx2(_nowScriptAdr, key_table, 5);

    sefAnalyzeAnim((unsigned char *)line_data);

    initial_position = &line_data->global_position;
    source_mode = line_data->scale_keys[0];
    if (source_mode != 4) {
        sefGetPosition(initial_position,
                       &line_data->global_position_source,
                       source_mode);
    }
}

static int sefAllocLineData(void)
{
    SefLineDataTable *lineData = (SefLineDataTable *)_lineData;
    SefLineDataEntry *entry = lineData->entries;
    int line_index;

    for (line_index = 0; line_index < SEF_LINE_DATA_COUNT; line_index++) {
        if (entry->inUse == 0) {
            entry->inUse = 1;
            return line_index;
        }
        entry++;
    }
    return -1;
}

static void sefFreeLineData(int line_index)
{
    SefLocalData *data;

    if ((unsigned int)line_index < SEF_LINE_DATA_COUNT) {
        if (line_index < 0) {
            data = 0;
        } else {
            data = (SefLocalData *)&_lineData[line_index * SEF_LINE_DATA_RECORD_SIZE];
        }
        sefDestroyLocalData(data);
        ((SefLineDataTable *)_lineData)->entries[line_index].inUse = 0;
    }
}

static void sefAnalyzeAnim(unsigned char *object)
{
    SefAnalyzeRecord *records = *(SefAnalyzeRecord **)(object + 496);
    int accumulated = 0;
    int index = 0;
    float scale;

    if (records->image_id <= 0) {
        return;
    }
    scale = analyzeAnimScale;
    do {
        SefImageItem *item_a;
        SefImageItem *item_b;
        int type = _nowScheduler->mapperIndex;

        item_a = svGetImageListItem(type, records[index].image_id);
        item_b = svGetImageListItem(type, records[index].alt_image_id);
        if (item_a != 0 && item_b != 0) {
            int flag = records->flag;

            if (index < 32) {
                unsigned long long tex0;
                unsigned char *dest = (unsigned char *)((index << 5) + (int)object) + 512;
                int psm;

                *(float *)(dest + 8) = (float)item_a->u24 * scale;
                *(float *)(dest + 12) = (float)item_a->u26 * scale;
                *(short *)(dest + 16) = (short)item_a->u12;
                psm = (item_a->u20 == 8) ? 19 : 20;
                ((void **)dest)[-2] = item_a->data;
                ((void **)dest)[-1] = item_b->data;
                tex0 = item_a->u18 | ((unsigned long long)(item_a->u22 >> 6) << 14) | ((unsigned long long)psm << 20) | ((unsigned long long)item_a->u28 << 26) | ((unsigned long long)item_a->u30 << 30) | (1ULL << 34) | ((unsigned long long)item_b->u18 << 37) | (1ULL << 61);
                accumulated = (records[index].time != 1024) ? accumulated + records[index].time : 1024;
                *(unsigned long long *)(object + (index << 5) + 512) = tex0;
                *(int *)(dest + 20) = flag > 0;
                *(short *)(dest + 18) = (short)accumulated;
            }
        } else {
            memset(object + 504 + index * 32, 0, 32);
        }
        index++;
    } while (records[index].image_id > 0);
}

static int sefAnimate(unsigned char *object, unsigned char *state, int advance)
{
    SefAnimCursor *cursor = (SefAnimCursor *)(state + 360);
    SefAnimRecord *records = *(SefAnimRecord **)(object + 496);
    int key = cursor->key_index;
    SefAnimRecord *current = records + key;
    int advanced = 0;
    int found = 0;
    int result = 0;
    unsigned int scan = scanFlags[0];
    short value;
    unsigned short frame_bits;
    short first_frame;

    if (advance < 0 || (scan & 1) != 0) {
        cursor->block = *(SefAnimBlock *)(object + 504);
        cursor->key_index = 0;
        *(short *)(state + 352) = 0;
        return 0;
    }
    first_frame = current->frame;
    frame_bits = current->frame;
    if (first_frame == 1024) {
        return 0;
    }
    if ((scan & 8) != 0) {
        for (scan = key; ; scan++, current = records + scan, frame_bits = current->frame) {
            int scan_jump = current->jump;

            if ((short)frame_bits == 1024) {
                return 0;
            }
            if (scan_jump >= 0) {
                if (current[1].frame == 1024) {
                    continue;
                }
                return 0;
            }
            cursor->key_index = scan;
            found = 1;
            break;
        }
    }
    if (found) {
        cursor->key_index++;
        result = 1;
        advanced = 1;
    } else {
        value = cursor->block.total_time;
        if (advance >= value) {
            int jump = current->jump;

            if (jump >= 0) {
                cursor->key_index = jump;
                result = 4096;
                if (jump == 0) {
                    *(short *)(state + 352) = 0;
                } else {
                    *(short *)(state + 352) = *(short *)(object + 32 * jump + 498);
                }
            } else if (jump == -1) {
                if (current[1].frame != 1024) {
                    cursor->key_index++;
                    result = 1;
                }
            } else if (jump == -2) {
                result = 0;
            } else {
                if (current[1].frame == 1024) {
                    return 0;
                }
            }
            advanced = 1;
        }
    }
    if (advanced) {
        cursor->block = *(SefAnimBlock *)(object + 504 + 32 * cursor->key_index);
    }
    *(short *)(state + 352) = *(short *)(state + 352) + 1;
    return result;
}

int sefCreateParticle(SefLocalData *data, int kind)
{
    SefBattleStatus *battle;
    unsigned char *params;
    SefParticle *particle;
    short *rotation_source_keys;
    int key_flag;
    Vector4 spread;
    Vector4 jitter;
    int count;
    int index;
    int result;
    int update;
    short angle;
    short handle;
    unsigned int seed_1;
    unsigned int seed_2;
    unsigned int seed_3;
    float random_x;
    float random_y;
    float random_z;
    float uniform_jitter;

    index = sefAllocLocalData(data);
    params = (unsigned char *)data;
    result = -1;
    if (index >= 0) {
        handle = data->pointHandles[index];
        particle = (SefParticle *)(_ptAlloc + handle * 640);
        if (particle != 0) {
            sefMemZero(particle, 640);
            sefGetStartPos(&particle->start_position, (SefParticleOffset *)data);
            sefGetTargetPos(&particle->target_position, (SefParticleOffset *)data);
            particle->state = 0;
            particle->frame = data->initial_frame;
            count = data->frame_count;
            if (count > 0) {
                sefRandSeed = sefRandSeed * 0x41C64E6Du + 0x3039u;
                particle->frame += (short)((int)((sefRandSeed >> 16) & 0x7FFF) % count);
            }
            sefAnimate(params, (unsigned char *)particle, -1);

            if (data->spread_rotation_keys != 0) {
                spread.x = (float)data->spread_rotation_keys[1];
                spread.y = (float)data->spread_rotation_keys[2];
                spread.z = (float)data->spread_rotation_keys[3];
                jitter.x = sefRandf();
                jitter.y = sefRandf();
                jitter.z = sefRandf();
                jitter.w = 0.0f;
                __asm__ __volatile__(
                    "lqc2 vf1, 0(%0)\n\t"
                    "lqc2 vf2, 0(%1)\n\t"
                    "vmul.xyz vf1, vf1, vf2\n\t"
                    "sqc2 vf1, 0(%0)"
                    : : "r"(&jitter), "r"(&spread) : "memory");
                __asm__ __volatile__(
                    "lqc2 vf1, 0(%0)\n\t"
                    "mfc1 $8, %1\n\t"
                    "qmtc2 $8, vf2\n\t"
                    "vmulx.xyz vf1, vf1, vf2x\n\t"
                    "sqc2 vf1, 0(%0)"
                    : : "r"(&spread), "f"(0.5f) : "$8", "memory");
                __asm__ __volatile__(
                    "lqc2 vf1, 0(%0)\n\t"
                    "lqc2 vf2, 0(%1)\n\t"
                    "vsub.xyz vf1, vf1, vf2\n\t"
                    "sqc2 vf1, 0(%2)"
                    : : "r"(&jitter), "r"(&spread), "r"(&particle->random_rotation) : "memory");
                __asm__ __volatile__(
                    "lqc2 vf2, 0(%0)\n\t"
                    "lwc1 $f8, %1\n\t"
                    "mfc1 $8, $f8\n\t"
                    "qmtc2 $8, vf1\n\t"
                    "vmulx.xyz vf2, vf2, vf1x\n\t"
                    "sqc2 vf2, 0(%0)"
                    : : "r"(&particle->random_rotation), "m"(randomRotationDegToRad) : "$8", "$f8", "memory");
                battle = &_battleData.status;
                if (battle->running != 0) {
                    __asm__ __volatile__(
                        "lq $8, 0(%1)\n\t"
                        "lq $9, 16(%1)\n\t"
                        "lq $10, 32(%1)\n\t"
                        "lq $11, 48(%1)\n\t"
                        "sq $8, 0(%0)\n\t"
                        "sq $9, 16(%0)\n\t"
                        "sq $10, 32(%0)\n\t"
                        "sq $11, 48(%0)"
                        : : "r"(&particle->transform), "r"(battle->particleTransform) : "$8", "$9", "$10", "$11", "memory");
                    particle->battle_scale = battle->particleScale;
                } else if (data->spread_rotation_keys[0] == 1) {
                    MMathRotateMatrixYXZ(&particle->transform, 0, &particle->random_rotation);
                }
            }

            particle->random_scale = sefRandf() * (float)data->uniform_scale_keys[0] * UNIFORM_SCALE_KEY_UNIT;
            particle->kind = kind;

            if (data->position_keys == 0) {
                particle->color[2] = 64;
                particle->color[1] = 64;
                particle->color[0] = 64;
            } else if (*(short *)(data->position_keys + 18) == 1024) {
                sefLerpIVectorA(data->position_keys + 10, particle->color);
            }

            if (data->size_keys != 0 && *(short *)(data->size_keys + 10) == 1024) {
                sefLerpIVectorA(data->size_keys + 2, &particle->color_scale);
            }

            if (data->color_keys != 0 && data->color_keys[9] == 1024) {
                sefLerpVectorA(data->color_keys + 4, &particle->color_offset);
                if (_revDirZ != 0) {
                    particle->color_offset.z = -particle->color_offset.z;
                }
            }

            if (data->rotation_keys != 0 && *(short *)(data->rotation_keys + 10) == 1024) {
                sefLerpVectorA(data->rotation_keys, &particle->size);
                if (data->color_keys[0] != 3) {
                    __asm__ __volatile__(
                        "lqc2 vf1, 0(%3)\n\t"
                        "mfc1 $8, %2\n\t"
                        "qmtc2 $8, vf2\n\t"
                        "vmulx.xyz vf1, vf1, vf2x\n\t"
                        "sqc2 vf1, 0(%3)"
                        : "=m"(particle->size)
                        : "m"(particle->size), "f"(PARTICLE_SIZE_SCALE), "r"(&particle->size)
                        : "$8", "memory");
                }
            }

            if (data->spread_rotation_keys != 0 && data->spread_rotation_keys[9] == 1024) {
                sefLerpVectorA(data->spread_rotation_keys + 4, &particle->rotation);
                rotation_source_keys = data->spread_rotation_keys;
                update = 0;
                key_flag = rotation_source_keys[-1];
                angle = rotation_source_keys[0];
                if (particle->rotation_state != 4 || key_flag != 0) {
                    update = 1;
                }
                sefCalcRotChange(particle, angle, update);
            }
            __asm__ __volatile__(
                "lqc2 vf1, 0(%0)\n\t"
                "lqc2 vf2, 0(%1)\n\t"
                "vadd.xyz vf1, vf1, vf2\n\t"
                "sqc2 vf1, 0(%2)"
                : : "r"(&particle->random_rotation), "r"(&particle->rotation), "r"(&particle->rotated_position) : "memory");

            if (data->signal_keys != 0) {
                particle->signal = *data->signal_keys;
            }

            particle->one_initialized_scalar = 1.0f;
            particle->random_translation.w = 1.0f;
            if (data->position_keys != 0) {
                if (*(short *)(data->position_keys + 6) == 0) {
                    short range_x;
                    short range_y;
                    short range_z;
                    float scale_k;
                    range_x = *(short *)(data->position_keys + 0);
                    range_y = *(short *)(data->position_keys + 2);
                    range_z = *(short *)(data->position_keys + 4);
                    scale_k = INV_32768;
                    seed_1 = sefRandSeed * 0x41C64E6Du + 0x3039u;
                    random_x = (float)(int)((seed_1 >> 16) & 0x7FFFu);
                    seed_2 = seed_1 * 0x41C64E6Du + 0x3039u;
                    random_y = (float)(int)((seed_2 >> 16) & 0x7FFFu);
                    seed_3 = seed_2 * 0x41C64E6Du + 0x3039u;
                    random_z = (float)(int)((seed_3 >> 16) & 0x7FFFu);
                    sefRandSeed = seed_3;
                    particle->random_translation.x = random_x * (float)range_x * scale_k * 0.015625f;
                    particle->random_translation.y = random_y * (float)range_y * scale_k * 0.015625f;
                    particle->random_translation.z = random_z * (float)range_z * scale_k * 0.015625f;
                } else {
                    short range;
                    float rand_value;
                    unsigned int seed;

                    range = *(short *)(data->position_keys + 0);
                    seed = sefRandSeed * 0x41C64E6Du + 0x3039u;
                    sefRandSeed = seed;
                    rand_value = (float)(int)((seed >> 16) & 0x7FFFu);
                    uniform_jitter = rand_value * (float)range * RANDOM_JITTER_SCALE * 0.015625f;
                    particle->random_translation.x = uniform_jitter;
                    particle->random_translation.y = uniform_jitter;
                    particle->random_translation.z = uniform_jitter;
                }
            }
        }
        result = index;
    }
    return result;
}

static int sefExecLineGlobalData(SefLineData *line_data, int frame)
{
    SefBattleStatus *battle;
    Vector4 initial_position;
    Matrix4 parent_matrix;
    int scale_is_fixed;
    int rotation_is_fixed;
    int particle_count;
    int emit_count;
    int point_index;
    int count;
    int parent_line;
    short point_handle;
    unsigned int scale_mode;
    short **keys;
    SefLineGlobalView *global;

    keys = &line_data->scale_keys;
    global = (SefLineGlobalView *)&line_data->global_position;
    particle_count = global->particle_count;
    scale_is_fixed = keys[0][0] == 4;
    rotation_is_fixed = keys[2][0] == 4;
    line_data->global_data_state = 0;
    if (particle_count <= 0) {
        return 1;
    }
    battle = &_battleData.status;
    if ((battle->flags & 0x100) != 0) {
        return 1;
    }
    scale_mode = (unsigned short)keys[0][0];
    if ((scale_mode & 0x1000) && _nowScheduler != 0) {
        unsigned int scheduler_flags = _nowScheduler->flags;

        _nowScheduler->flags = scheduler_flags | 0x80;
        if (keys[0][0] >= 0x1001 &&
            (battle->caster != 0 || battle->casterUnit != 0)) {
            _nowScheduler->flags = scheduler_flags | 0x1080;
            sefGetParentMatrix(_nowScheduler, (short)scale_mode - 0x1000, &parent_matrix);
        }
    }
    __asm__ __volatile__(
        "lq $8, 0(%0)\n\t"
        "sq $8, 0(%1)"
        :
        : "r"(&global->global_position), "r"(&initial_position)
        : "$8", "memory");
    if (!scale_is_fixed) {
        sefGetPosition(&global->global_position, &global->global_position_source, keys[0][0]);
        __asm__ __volatile__(
            "lqc2 vf1, 0(%0)\n\t"
            "lqc2 vf2, 0(%1)\n\t"
            "vadd.xyz vf1, vf1, vf2\n\t"
            "sqc2 vf1, 0(%0)"
            :
            : "r"(&global->global_position), "r"(&line_data->origin_offset)
            : "memory");
    }
    if (!rotation_is_fixed) {
        sefGetPosition(&global->parent_position, &global->parent_position_source, keys[2][0]);
    }
    if (frame % particle_count == 0) {
        emit_count = global->emit_count;
        if (_parentLine >= 0) {
            SefLocalData *parent_data = (SefLocalData *)&_lineData[_parentLine * SEF_LINE_DATA_RECORD_SIZE];

            if (parent_data == 0) {
                __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(&global->position_delta) : "memory");
                return 1;
            }
            for (parent_line = 0; parent_line < 0x100; parent_line++) {
                point_handle = parent_data->pointHandles[parent_line];
                if (point_handle >= 0) {
                    _nowParentLocal = point_handle;
                    if (scale_is_fixed) {
                        sefGetPosition(&global->global_position, &global->global_position_source, keys[0][0]);
                        __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&global->global_position) : "memory");
                        __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(&line_data->origin_offset) : "memory");
                        __asm__ __volatile__("vadd.xyz vf1, vf1, vf2");
                        __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(&global->global_position) : "memory");
                    }
                    if (rotation_is_fixed) {
                        sefGetPosition(&global->parent_position, &global->parent_position_source, keys[2][0]);
                    }
                    if (emit_count > 0) {
                        count = emit_count;
                        do {
                            sefCreateParticle((SefLocalData *)line_data, point_handle);
                        } while (--count != 0);
                    }
                }
            }
        } else {
            if (emit_count > 0) {
                parent_line = emit_count;
                do {
                    sefCreateParticle((SefLocalData *)line_data, -1);
                } while (--parent_line != 0);
            }
        }
    }
    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "vsub.xyz vf1, vf1, vf2\n\t"
        "sqc2 vf1, 0(%2)"
        :
        : "r"(&global->global_position), "r"(&initial_position), "r"(&global->position_delta)
        : "memory");
    return 1;
}

static int sefExecLineData(SefLineData *line_data)
{
    unsigned short frame = line_data->frame;

    line_data->global_result = -1;
    if (line_data->loop_mode != 0) {
        line_data->timer--;
        if (line_data->timer < 0 &&
            (line_data->global_data_count == 0 || line_data->local_result == 0)) {
            line_data->timer = 0;
            return 0;
        }
    }
    if (line_data->timer < 0) {
        line_data->timer = -1;
    }

    _gravity = line_data->gravity;
    _height = line_data->height;
    _colision = (float)line_data->collision_progress * collisionProgressScale;
    sefLerpVectorSC(line_data->scale_keys + 4, &line_data->scale_vector);
    sefLerpVectorSC(line_data->rotation_keys + 4, &line_data->rotation_vector);
    if (line_data->position_keys != 0) {
        ((void (*)(void *, Vector4 *))sefLerpVectorSC)(line_data->position_keys + 4, &line_data->position_vector);
        sefGetPosition(&line_data->position, line_data->position_source, *line_data->position_keys);
    }
    sefLerpIVector(line_data->first_vector_keys->keys,
                   &line_data->first_vector_progress);
    sefLerpIVector(line_data->second_vector_keys->keys,
                   &line_data->second_vector_progress);
    sefProgressInt(line_data->first_integer_keys,
                   &line_data->first_integer_progress);
    sefProgressInt(line_data->second_integer_keys,
                   &line_data->second_integer_progress);

    if (line_data->timer < 0 || sefExecLineGlobalData(line_data, frame) != 0) {
        line_data->local_result = sefExecLineLocalData(line_data, frame);
        line_data->frame++;
        if (line_data->frame == 0x3FF) {
            if (line_data->loop_mode == 0) {
                line_data->frame -= line_data->second_integer_progress.value;
                if (line_data->frame == 0) {
                    line_data->frame = 1;
                }
            } else {
                line_data->frame = 0x3FE;
            }
        }
    }
    return 1;
}

static void sefInitEffectData(EffectData *effect, int owner, int effect_no, int category)
{
    memset(effect, -1, 0x20);
    effect->owner = owner;
    effect->effect_no = (short)effect_no;
    effect->category = (short)category;
    effect->frame = 0;
    effect->flags = 0;
}

static void sefDestroyEffectData(EffectData *effect)
{
    int index;

    if (effect->owner != 0) {
        for (index = 0; index < 0x20; index++) {
            sefFreeEffectData(effect, index);
        }
    }
    effect->owner = 0;
}

static SefLineData *sefAllocEffectData(EffectData *effect)
{
    int index = effect->flags;
    signed char *slot;
    int line_index;

    if ((unsigned int)index >= 0x20) {
        return 0;
    }
    slot = effect->lineHandles + index;
    if (*slot >= 0) {
        return 0;
    }
    line_index = sefAllocLineData();
    if (line_index < 0) {
        return 0;
    }
    *slot = line_index;
    effect->active = 1;
    effect->flags++;
    return (SefLineData *)&_lineData[line_index * SEF_LINE_DATA_RECORD_SIZE];
}

static void sefFreeEffectData(EffectData *effect, unsigned int index)
{
    if (index < 0x20) {
        signed char *handle = &effect->lineHandles[index];

        if (*handle >= 0) {
            sefFreeLineData(*handle);
            *handle = -1;
        }
    }
}

static void sefCheckFinish(int effect_no, int condition) {
    if (condition & 0x4000) {
        _nowScheduler->flags |= 0x40;
    }
}

int sefExecEffectData(EffectData *effect, int frame, short *active_count)
{
    SefEffectRequest *battle;
    SefLineData *line;
    SefLineData *linked;
    int running;
    int index;
    int j;
    int link_index;
    unsigned int category_word;
    int category;
    short finish_condition;
    unsigned char battle_active;

    effect->frame = 0;
    if (effect->owner == 0) {
        return 0;
    }

    if (frame == (effect->category & 0x3ff)) {
        if ((effect->active & 0x10) != 0) {
            func_A2C408(0);
        } else if ((effect->active & 0x20) != 0) {
            func_A2C430(0);
            _nowScheduler->flags |= 0x4000;
        }
    }

    if ((effect->category & 0x1000) != 0) {
        if ((unsigned char)_battleData.request.special == 0) {
            sefDestroyEffectData(effect);
            effect->frame = 1;
            return 0;
        }
        if (*active_count != 0) {
            sefCheckFinish(frame, effect->category);
            effect->category = (short)frame;
            *active_count = 0;
        }
    }

    category = effect->category;
    finish_condition = category;
    if ((effect->category & 0x2800) != 0 && frame >= (category & 0x3ff)) {
        battle = &_battleData.request;
        battle_active = (unsigned char)battle->special;
        sefCheckFinish(frame, finish_condition);
        if ((effect->category & 0x2000) != 0) {
            _hitSignal = 1;
            sefSetHitSignal(battle->target);
        }
        if ((effect->category & 0x800) != 0) {
            _seSignal = 1;
            sefSetSeSignal(battle->target);
        }
        if (battle_active == 0) {
            sefDestroyEffectData(effect);
            effect->frame = 1;
            return 0;
        }
        effect->category = (short)frame;
        *active_count = 0;
    }

    category_word = *(unsigned int *)&effect->category;
    if ((category_word & 0x7000) == 0x4000 && frame == (effect->category & 0x3ff)) {
        effect->category = (short)frame;
        _nowScheduler->flags |= 0x40;
    }
    if (frame >= effect->category) {
        frame -= effect->category;
        if ((unsigned int)effect->owner < 3U) {
            sefDestroyEffectData(effect);
            effect->frame = 1;
            return 0;
        }

        running = 0;
        for (;;) {
            SefEffectCommand *command;
            SefLineScriptHeader *line_script;
            int script_index;
            int start_time;
            int command_frame;
            short count;
            short command_flags;

            script_index = effect->flags;
            command = (SefEffectCommand *)effect->owner + script_index;
            start_time = command->start_time;
            command_frame = command->frame;
            count = command->count;
            command_flags = command->flags;
            link_index = command->link_index;

            if (start_time < 1024) {
                if (frame >= start_time) {
                    if (script_index < 32) {
                        line_script = (SefLineScriptHeader *)scGetImmAdrImmIdx2(_nowScriptAdr,
                            (unsigned short *)effect->owner, script_index * 6 + 2);
                        if (line_script != 0) {
                            line = sefAllocEffectData(effect);
                            if (line != 0) {
                                sefInitLineData(line, start_time, command_frame, line_script,
                                                link_index, count, command_flags);
                                if (start_time != 0x1000) {
                                    continue;
                                }
                            } else {
                                tracePrint(lineEmptyMessage);
                            }
                        }
                    }
                } else if (start_time != 0x400) {
                    running = 1;
                }
            }
            break;
        }

        effect->frame = 1;
        for (index = 0; index < 32; index++) {
            int handle;

            handle = effect->lineHandles[index];
            if (handle < 0) {
                continue;
            }
            line = (SefLineData *)&_lineData[handle * SEF_LINE_DATA_RECORD_SIZE];
            link_index = line->link_index;
            if (link_index >= 0) {
                link_index = effect->lineHandles[link_index];
            }
            _parentLine = link_index;
            if (((int (*)(SefLineData *, int))sefExecLineData)(line, frame) != 0) {
                running++;
                continue;
            }
            sefFreeEffectData(effect, index);
            if (line->global_data_count != 0) {
                continue;
            }
            for (j = 0; j < 32; j++) {
                int handle;

                if (j == index) {
                    continue;
                }
                handle = effect->lineHandles[j];
                if (handle < 0) {
                    continue;
                }
                linked = (SefLineData *)&_lineData[handle * SEF_LINE_DATA_RECORD_SIZE];
                if (linked->link_index == index) {
                    sefFreeEffectData(effect, j);
                }
            }
        }

        if (running == 0) {
            sefDestroyEffectData(effect);
            effect->frame = 1;
            return 0;
        }
        return effect->owner != 0;
    }
    return 1;
}

void sefInitScheduler(void)
{
    SchedulerState *scheduler = _scheduler;
    ScriptBinding *binding;
    EffectData *effects = scheduler->effects;
    EffectData *effect;
    short invalid = -1;
    int index;
    int count;

    sefMemZero(scheduler, 0x55800);
    for (index = 0; index < SEF_SCHEDULER_COUNT; index++) {
        binding = &scheduler[index].scriptBinding;
        binding->script_id = invalid;
        binding->task_id = invalid;
        binding->link_id = invalid;
        effect = (EffectData *)(index * SEF_SCHEDULER_RECORD_SIZE + (int)effects);
        for (count = 0; count < 0x20; count++) {
            sefInitEffectData(effect, 0, -1, 0);
            effect++;
        }
    }
}

static int sefAllocScheduler(void *table)
{
    int start = _schridx_1;
    int end = (start + 127) % 128;
    int index = start;

    while (index != end) {
            int record_offset;
            unsigned int marker_offset;
            int binding_offset;
            int *inUse;
            ScriptBinding *binding;

            index %= 128;
            record_offset = index * SEF_SCHEDULER_RECORD_SIZE;
            marker_offset = record_offset + offsetof(SchedulerState, inUse);
            inUse = (int *)(marker_offset + (int)_scheduler);
            if (*inUse == 0) {
                binding_offset = record_offset + offsetof(SchedulerState, scriptBinding);
                binding = (ScriptBinding *)(binding_offset + (int)_scheduler);
                binding->link_id = -1;
                binding->script_id = _nowScript;
                binding->task_id = _nowEvent;
                *inUse = (int)table;
                _schridx_1 = (index + 1) % 128;
                binding->state = 0;
                return index;
            }
            index++;
    }
    return -1;
}

void sefFreeSchedulerCf(SchedulerState *scheduler)
{
  int soundId;
  int i;

  if (scheduler != 0 && scheduler->inUse != 0)
  {
    soundId = scheduler->soundId;
    if (soundId > 0)
    {
      xglSoundEffectStopID(soundId, 0);
      scheduler->soundId = 0;
    }
    for (i = 0; i < 32; i++)
    {
      sefDestroyEffectData(&scheduler->effects[i]);
    }
    scheduler->scriptBinding.state = 0;
    scheduler->inUse = 0;
    scheduler->scriptBinding.script_id = -1;
    scheduler->scriptBinding.task_id = -1;
  }
}

void sefFreeScheduler(unsigned int scheduler_index)
{
    enum {
        scheduler_stride = sizeof(SchedulerState),
        binding_offset = 0xa90
    };
    int byte_offset;
    int script_id;
    int i;
    EffectData *effect;
    SefScriptStatus *status;
    ScriptBinding *binding;
    SchedulerState *scheduler;
    SchedulerState *entry;

    if (scheduler_index >= 128) {
        return;
    }
    if (_scheduler[scheduler_index].inUse == 0) {
        return;
    }
    scheduler = &_scheduler[scheduler_index];
    if ((scheduler->flags & 0x202) != 0) {
        sefFreeSchedulerCf(scheduler);
        return;
    }
    effect = _scheduler[scheduler_index].effects;
    for (i = 31; i >= 0; i--, effect++) {
        sefDestroyEffectData(effect);
    }
    byte_offset = scheduler_index * scheduler_stride;
    binding = (ScriptBinding *)((unsigned int)_scheduler
        + (byte_offset + binding_offset));
    script_id = binding->script_id;
    if (script_id >= 0) {
        status = &_scriptWork[script_id].status;
        --status->active_tasks;
    }
    entry = (SchedulerState *)((unsigned int)_scheduler + byte_offset);
    entry->inUse = 0;
    binding->script_id = -1;
    binding->task_id = -1;
    binding->state = 0;
}

void sefDestroyScriptScheduler(int script_index)
{
    int index;

    index = 0;
    do {
        if (_scheduler[index].scriptBinding.script_id == script_index) {
            sefFreeScheduler(index);
        }
        index++;
    } while (index < 0x80);
}

void sefDestroyScriptScheduler2(int script_index, int task_index)
{
    int index;

    index = 0;
    do {
        if (_scheduler[index].scriptBinding.script_id == script_index
            && _scheduler[index].scriptBinding.task_id == task_index) {
            sefFreeScheduler(index);
        }
        index++;
    } while (index < 0x80);
}

static inline void sefInitializeDescriptorEffect(EffectData *effect,
                                                  SefEffectDescriptor *entry,
                                                  int scheduler_index,
                                                  ScriptBinding *binding,
                                                  SefEffectDescriptor *descriptors)
{
    int owner;
    owner = scGetImmAdrImmIdx2(_nowScriptAdr, (unsigned short *)entry, 1);

    if (owner == 0) {
        owner = 1;
    }
    sefInitEffectData(effect, owner, scheduler_index, entry->category);
    if (entry->setting >= 0) {
        binding->link_id = descriptors[entry->setting].category;
    }
}

static inline void sefApplyDescriptorCommand(EffectData *effect, short command)
{
    if (command == -0x200) {
        effect->active |= 0x10;
    } else if (command == -0x201) {
        effect->active |= 0x20;
    }
}

static void sefInitEffectTbl(SefEffectDescriptor *entry, int scheduler_index, int init_flags)
{
    int index = 0;
    EffectData *effects = ((SchedulerState *)_scheduler)[scheduler_index].effects;
    SefEffectDescriptor *descriptors = entry;
    int preserve_setting = -0x100;

    if (entry->category >= 0) {
        SchedulerState *scheduler = &_scheduler[scheduler_index];
        EffectData *effect = effects;
        ScriptBinding *binding = &scheduler->scriptBinding;
        EffectData *cursor = effects;

process_descriptor:
        {
            int next_category;
            int has_capacity;
            if (entry->setting != preserve_setting || (init_flags & 1) != 0) {
                sefInitializeDescriptorEffect(effect, entry, scheduler_index, binding, descriptors);
            }
            sefApplyDescriptorCommand(effect, entry->setting);
            entry++;
            cursor++;
            next_category = entry->category;
            has_capacity = ++index < 32;
            effect = cursor;
            if (next_category >= 0 && has_capacity) {
                goto process_descriptor;
            }
        }
    }
    if (index < 32) {
        do {
            effects[index].owner = 0;
            index++;
        } while (index < 32);
    }
}

int sefCreateScheduler2(int effect_no, void *parameters, void *position, int table, int flags, int index)
{
    int number;
    int category;
    int scheduler_index;
    unsigned char *saved_script_adr;
    int value0;
    int value1;
    int value2;
    int value3;
    void *origin;
    SefScriptStatus *work;
    int *param_block;
    int *camera_block;
    int *effect_block;
    int *tail_block;
    SefEffectRequest *request;

    value0 = scGetImmAdrImmIdx2(table, (unsigned short *)effect_no, 0);
    value1 = scGetImmAdrImmIdx2(table, (unsigned short *)effect_no, 1);
    value2 = scGetImmAdrImmIdx2(table, (unsigned short *)effect_no, 2);
    value3 = scGetImmAdrImmIdx2(table, (unsigned short *)effect_no, 3);
    effect_no += 8;

    if (index >= 0) {
        scheduler_index = index;
        _scheduler[scheduler_index].inUse = effect_no;
        _scheduler[scheduler_index].scriptBinding.link_id = -1;
        _scheduler[scheduler_index].scriptBinding.script_id = _nowScript;
        _scheduler[scheduler_index].scriptBinding.task_id = _nowEvent;
        _scheduler[scheduler_index].scriptBinding.state = 0;
    } else {
        scheduler_index = sefAllocScheduler((void *)effect_no);
    }
    if (scheduler_index < 0) {
        return -1;
    }

    origin = position;
    saved_script_adr = *(unsigned char **)&_nowScriptAdr;
    _nowScriptAdr = table;
    request = &_scheduler[scheduler_index].request;
    *request = *((SefEffectRequest *)parameters);
    _scheduler[scheduler_index].caster = ((SefEffectRequest *)parameters)->source;
    _scheduler[scheduler_index].targetUnit = 0;
    _scheduler[scheduler_index].casterUnit = 0;
    _scheduler[scheduler_index].target = ((SefEffectRequest *)parameters)->target;
    if (origin == 0) {
        origin = (void *)_zeroPos_004CBF00;
    }
    __asm__ __volatile__("lq $8, 0(%0)\n\t"
                         "sq $8, 0(%1)"
                         :
                         : "r"(origin), "r"(&_scheduler[scheduler_index].position)
                         : "$8", "memory");
    QuadwordZero(&_scheduler[scheduler_index].rotation);
    _scheduler[scheduler_index].savedActive = 0;
    _scheduler[scheduler_index].scriptAddress = (unsigned char *)table;
    _scheduler[scheduler_index].delayFrames = (short)flags;
    _scheduler[scheduler_index].flags = 0;
    _scheduler[scheduler_index].signalByte = 0;
    _scheduler[scheduler_index].schedulerIndex = (short)scheduler_index;
    param_block = (int *)&_scheduler[scheduler_index].soundParameters;
    memset(param_block, 0, 0x18);
    camera_block = (int *)&_scheduler[scheduler_index].cameraChoice;
    memset(camera_block, 0, 0x44);
    effect_block = (int *)&_scheduler[scheduler_index].effectValue;
    memset(effect_block, 0, 0x320);
    tail_block = &_scheduler[scheduler_index].alterValue;
    memset(tail_block, 0, 0x14);
    param_block[0] = value0;
    param_block[4] = 4;
    tail_block[4] = 18;
    effect_block[4] = 2;
    tail_block[0] = value3;
    effect_block[1] = table;
    effect_block[0] = value2;
    sdvSelectCamera(camera_block, table, value1);
    __asm__ __volatile__("vmaxw.xyzw vf1, vf0, vf0w\n\t"
                         "sqc2 vf1, 0(%0)"
                         : : "r"(&_scheduler[scheduler_index].scale) : "memory");
    _scheduler[scheduler_index].display = 1;
    _scheduler[scheduler_index].effectSpeed = 3.0f;
    work = &_scriptWork[_scheduler[scheduler_index].scriptBinding.script_id].status;
    work->active_tasks += 1;
    sefInitEffectTbl((SefEffectDescriptor *)effect_no, scheduler_index, ((SefEffectRequest *)parameters)->initFlags);
    _nowScriptAdr = (int)saved_script_adr;
    srsAnalyzeEftNo(((SefEffectRequest *)parameters)->effect_no, &number, &category);
    _scheduler[scheduler_index].effectNumber = (short)number;
    _scheduler[scheduler_index].category = (short)category;
    _scheduler[scheduler_index].mapperIndex = sefSearchMapperIndex(((SefEffectRequest *)parameters)->effect_no);
    return scheduler_index;
}

int sefCreateScheduler(int effect_no, void *parameters, void *position, int table, int index) {
    return sefCreateScheduler2(effect_no, parameters, position, table, 0, index);
}

static int sefGetSizeOffset(int index) {
    return offset_2[index];
}

void sefCreateReactionEffect(void)
{
    SefEffectRequestBlock block;
    unsigned long long *source_words;
    unsigned short special;
    SefEffectRequest *battle;
    int kind;
    int mode;
    unsigned int reaction;
    int index;
    int created;
    int bit;
    unsigned short character;
    SefEffectRequest *current;

    battle = &_battleData.request;
    special = battle->special;
    kind = (unsigned char)special;
    reaction = battle->reaction_mask;
    if (!kind) {
        return;
    }
    mode = special >> 8;
    created = 0;
    source_words = _battleData.words;
    block.words[0] = source_words[0];
    block.words[1] = source_words[1];
    block.words[2] = source_words[2];
    block.words[3] = source_words[3];
    block.request.param = battle->param;
    block.request.special = 0;
    *(unsigned short *)&block.request.reaction_mask = 0;
    if (block.request.target == 0) {
        block.request.target = block.request.source;
        block.request.character_id_copy = block.request.character_id;
        block.request.size_copy = block.request.size;
    }
    if (mode & 0x22) {
        block.request.effect_no = 0xa34 + sefGetSizeOffset(block.request.size_copy);
        scCreateScript(&block.request);
    }
    if (mode & 0x10) {
        block.request.effect_no = sefGetSizeOffset(block.request.size_copy) + 0xa37;
        scCreateScript(&block.request);
    }
    if (reaction != 0) {
        bit = 1;
        for (index = 0; index < 8; index++) {
            if (reaction & bit) {
                created++;
                block.request.effect_no = 0xa5b + index;
                scCreateScript(&block.request);
            }
            bit <<= 1;
        }
    }
    if ((mode & 1) && !created) {
        block.request.effect_no = 0xa64;
        scCreateScript(&block.request);
    }
    if (kind != 3) {
        if (kind != 1) {
            return;
        }
    }
    current = &_battleData.request;
    if ((current->reactionGate & 1) == 0) {
        return;
    }
    character = block.request.character_id_copy;
    if ((unsigned int)character - 0x22u < 3u || (unsigned int)character - 0x99u < 2u) {
        block.request.effect_no = 0xa63;
        scCreateScript(&block.request);
    }
}

float sefGetMatrixScale(float *matrix)
{
    return MMathCalcLength(matrix);
}

void sefGetParentMatrix(SchedulerState *scheduler, int index, Matrix4 *matrix)
{
    Actor *target;
    MapUnitRecord *parent;
    Actor *child;
    Matrix4 accessoryMatrix;
    Matrix4 selected;
    Matrix4 base;
    int matrixIndex;
    int matrixReady;

    if ((scheduler->flags & 2) == 0) {
        return;
    }

    target = scheduler->caster;
    matrixReady = 1;
    {
        if (target != 0) {
            if (scheduler->flags & 0x80) {
                sefGetMotionNullMatrix(matrix, target, index);
            } else {
                sefCalcRotTransSMatrix(&target->rotation, &target->position,
                                       &target->scale, matrix);
            }
            if (scheduler->caster->parent != 0) {
                MATRIX_convert4(accessoryMatrix, *(Matrix4 *)scheduler->caster->jointMatrices);
                MMathMulMatrix(matrix, &accessoryMatrix, matrix);
            }
        } else {
            if (scheduler->casterUnit == 0) {
                goto identity;
            }
            parent = scheduler->casterUnit;
            sefCalcRotTransSMatrix(&parent->rotation, &parent->position,
                                   &parent->scale, matrix);
            child = scheduler->casterUnit->parent_peer;
            if (child != 0) {
                Matrix4 *accessory;
                Matrix4 *selected_source;

                matrixIndex = scheduler->casterUnit->parentMatrixIndex;
                accessory = child->jointMatrices;
                if (matrixIndex >= 0) {
                if (matrixIndex & 0x8000) {
                    selected_source = (Matrix4 *)(((matrixIndex & 0x7fff) << 6) + (int)accessory);
                    __asm__ __volatile__(
                        "lq $8, 0(%0)\n\t"
                        "lq $9, 16(%0)\n\t"
                        "lq $10, 32(%0)\n\t"
                        "lq $11, 48(%0)\n\t"
                        "sq $8, 0(%1)\n\t"
                        "sq $9, 16(%1)\n\t"
                        "sq $10, 32(%1)\n\t"
                        "sq $11, 48(%1)"
                        :
                        : "r"(*selected_source), "r"(selected)
                        : "$8", "$9", "$10", "$11", "memory");
                    selected[0][3] = selected[1][3] = selected[2][3] = 0.0f;
                    xglMatrixStackLoad(selected);
                } else {
                    __asm__ __volatile__(
                        "lq $8, 0(%0)\n\t"
                        "lq $9, 16(%0)\n\t"
                        "lq $10, 32(%0)\n\t"
                        "lq $11, 48(%0)\n\t"
                        "sq $8, 0(%1)\n\t"
                        "sq $9, 16(%1)\n\t"
                        "sq $10, 32(%1)\n\t"
                        "sq $11, 48(%1)"
                        :
                        : "r"(*accessory), "r"(base)
                        : "$8", "$9", "$10", "$11", "memory");
                    selected_source = (Matrix4 *)(((scheduler->casterUnit->parentMatrixIndex & 0x7fff) << 6) + (int)accessory);
                    __asm__ __volatile__(
                        "lq $8, 0(%0)\n\t"
                        "lq $9, 16(%0)\n\t"
                        "lq $10, 32(%0)\n\t"
                        "lq $11, 48(%0)\n\t"
                        "sq $8, 0(%1)\n\t"
                        "sq $9, 16(%1)\n\t"
                        "sq $10, 32(%1)\n\t"
                        "sq $11, 48(%1)"
                        :
                        : "r"(*selected_source), "r"(selected)
                        : "$8", "$9", "$10", "$11", "memory");
                    base[0][3] = base[1][3] = base[2][3] = selected[0][3] = selected[1][3] = selected[2][3] = 0.0f;
                    xglMatrixStackLoad(base);
                    xglMatrixStackMul(selected);
                }
                xglMatrixStackMul(*matrix);
                xglMatrixStackSave(*matrix);
                }
            }
        }
        __asm__ __volatile__(
            "lq $8, 0(%0)\n\t"
            "lq $9, 16(%0)\n\t"
            "lq $10, 32(%0)\n\t"
            "lq $11, 48(%0)\n\t"
            "sq $8, 0(%1)\n\t"
            "sq $9, 16(%1)\n\t"
            "sq $10, 32(%1)\n\t"
            "sq $11, 48(%1)"
            :
            : "r"(*matrix), "r"(parentMatrixState)
            : "$8", "$9", "$10", "$11", "memory");
        parentMatrixState[40] = sefGetMatrixScale(parentMatrixState);
        }
    goto published;
identity:
    {
        matrixReady = 0;
        __asm__ __volatile__("vmr32.xyzw vf1, vf0\n\t"
                             "vmr32.xyzw vf2, vf1\n\t"
                             "vmr32.xyzw vf3, vf2\n\t"
                             "sqc2 vf0, 48(%0)\n\t"
                             "sqc2 vf1, 32(%0)\n\t"
                             "sqc2 vf2, 16(%0)\n\t"
                             "sqc2 vf3, 0(%0)"
                             :
                             : "r"(matrix)
                             : "memory");
        __asm__ __volatile__(
            "lq $8, 0(%0)\n\t"
            "lq $9, 16(%0)\n\t"
            "lq $10, 32(%0)\n\t"
            "lq $11, 48(%0)\n\t"
            "sq $8, 0(%1)\n\t"
            "sq $9, 16(%1)\n\t"
            "sq $10, 32(%1)\n\t"
            "sq $11, 48(%1)"
            :
            : "r"(*matrix), "r"(parentMatrixState)
            : "$8", "$9", "$10", "$11", "memory");
        parentMatrixState[40] = 1.0f;
    }
published:
    if (matrixReady) {
        xglMatrixInverse(inverseParentMatrix, *matrix);
    }
}

/* sefExecScheduler walks the slots through a cursor that points 0xa80 bytes into
 * the current slot (the original's base register): SLOT(field) is that slot's
 * field read off the cursor, BATTLE the shared battle status block. */
#define SLOT(f) (*(__typeof__(((SchedulerState *)0)->f) *)(control_address + (offsetof(SchedulerState, f) - 0xa80)))

#define BATTLE (&_battleData.status)

int sefExecScheduler(void)
{
    SchedulerState *scheduler;
    SchedulerState *table;
    short hit_flag;
    int active_count = 0;
    int index;
    unsigned int control_address;
    unsigned int record_offset = 0;

    table = _scheduler;

    sdvExecAmbient();
    sefInitBattlePrm();
    sdvClearSpecialWork();
    sefExecLoadQue();

    scheduler = table;
    control_address = (unsigned int)table + 0xa80;
    for (index = 0; index < 128; control_address += sizeof(SchedulerState), record_offset += sizeof(SchedulerState), scheduler++, index++) {
        int effect_alive = 0;
        int running;
        int k;
        SchedulerState *expired;
        unsigned int binding_offset;
        ScriptBinding *exec_binding;
        SchedulerState *record;

        binding_offset = index * sizeof(SchedulerState) + offsetof(SchedulerState, scriptBinding);
        running = (unsigned short)SLOT(flags) & 0x2;
        hit_flag = _hitFlag;

        if (SLOT(inUse) != 0 && SLOT(display) != 0) {
            if (func_A33248() != 0) {
                active_count += 1;
                continue;
            }
            if (SLOT(delayFrames) > 0) {
                SLOT(delayFrames) -= 1;
                active_count += 1;
                continue;
            }
            if (running != 0) {
                if (SLOT(casterUnit) != 0 && SLOT(casterUnit)->serial < 0) {
                    sefFreeScheduler(index);
                    continue;
                }
                if (SLOT(caster) != 0 && SLOT(caster)->inUseId <= 0) {
                    sefFreeScheduler(index);
                    continue;
                }
            }

            _battleData.request = SLOT(request);
            BATTLE->caster = SLOT(caster);
            BATTLE->target = SLOT(target);
            BATTLE->casterUnit = SLOT(casterUnit);
            BATTLE->targetUnit = SLOT(targetUnit);
            BATTLE->effectNo = SLOT(effectNo);
            BATTLE->flags = SLOT(flags);
            BATTLE->running = running;

            if (SLOT(effectNo) == 55 && hit_flag != 0) {
                SLOT(nusumeResult) = BATTLE->nusumeResult = func_00A0CE30();
            }

            *(int *)&_nowScript = SLOT(scriptBinding.script_id);
            *(int *)&_nowEvent = SLOT(scriptBinding.task_id);
            _nowScriptAdr = (int)SLOT(scriptAddress);
            _nowEftNo = SLOT(effectNo);
            /* Non-zero when rotation (x and y read as one doubleword, then z) is not zero. */
            BATTLE->vectorActive = (((long long)*(int *)&SLOT(rotation.z)
                                     | *(unsigned long long *)&SLOT(rotation)) != 0);
            _nowScheduler = scheduler;
            sefCalcRotTransSMatrix(&scheduler->rotation, &scheduler->position,
                                   &scheduler->scale, (Matrix4 *)&scheduler->localMatrix);

            __asm__ __volatile__("vmr32.xyzw vf1, vf0\n\tvmr32.xyzw vf2, vf1\n\tvmr32.xyzw vf3, vf2\n\tsqc2 vf0, 48(%0)\n\tsqc2 vf1, 32(%0)\n\tsqc2 vf2, 16(%0)\n\tsqc2 vf3, 0(%0)" : : "r"(&BATTLE->casterMatrix) : "memory");
            __asm__ __volatile__("vmr32.xyzw vf1, vf0\n\tvmr32.xyzw vf2, vf1\n\tvmr32.xyzw vf3, vf2\n\tsqc2 vf0, 48(%0)\n\tsqc2 vf1, 32(%0)\n\tsqc2 vf2, 16(%0)\n\tsqc2 vf3, 0(%0)" : : "r"(&BATTLE->targetMatrix) : "memory");

            if (running != 0) {
                sefGetParentMatrix(scheduler, 0, &scheduler->worldMatrix);
            } else {
                __asm__ __volatile__("vmr32.xyzw vf1, vf0\n\tvmr32.xyzw vf2, vf1\n\tvmr32.xyzw vf3, vf2\n\tsqc2 vf0, 48(%0)\n\tsqc2 vf1, 32(%0)\n\tsqc2 vf2, 16(%0)\n\tsqc2 vf3, 0(%0)" : : "r"(&scheduler->worldMatrix) : "memory");
                if (SLOT(caster) != 0) {
                    sefCalcRotTransSMatrix(&SLOT(caster)->rotation, &SLOT(caster)->position,
                                           &SLOT(caster)->scale, &BATTLE->casterMatrix);
                }
            }
            if (SLOT(target) != 0) {
                sefCalcRotTransSMatrix(&SLOT(target)->rotation, &SLOT(target)->position,
                                       &SLOT(target)->scale, &BATTLE->targetMatrix);
            }

            BATTLE->effectSpeed = SLOT(effectSpeed);
            sefSetReverseDir(SLOT(effectNo), (short)SLOT(requestParam));
            for (k = 0; k < 32; k++) {
                exec_binding = (ScriptBinding *)((unsigned int)_scheduler + binding_offset);
                if (sefExecEffectData((EffectData *)(schedulerStates + index * sizeof(SchedulerState) + k * sizeof(scheduler->effects[0])),
                                      exec_binding->state, &hit_flag) != 0) {
                    effect_alive += 1;
                }
            }

            /* soundParameterRecords is _scheduler[0].soundParameters (+0x6cc): +0x18 cameraChoice,
             * +0x64 the effect block (+0x730, effectBlockRecords), +0x384/+0x398 alter block and request. */
            sdvScheduleSound(soundParameterRecords + record_offset);
            sdvScheduleCamera(soundParameterRecords + 0x18 + record_offset);
            sdvExecSpecial(soundParameterRecords + 0x64 + record_offset);
            sdvScheduleAlter(soundParameterRecords + 0x384 + record_offset, soundParameterRecords + 0x398 + record_offset);

            if (_hitFlag != 0 || _hitSignal != 0) {
                unsigned int reaction_offset = record_offset + offsetof(SchedulerState, scriptBinding);
                signed char *binding = (signed char *)(reaction_offset + (unsigned int)table);

                if (binding[11] == 0) {
                    sefCreateReactionEffect();
                }
                binding[11] = binding[11] + 1;
            }
            if (!(GameLoopState.flags & SEF_DRAW_FLAG_PENDING)) {
                sdvDrawSpecial(effectBlockRecords + record_offset);
            }

            if (effect_alive == 0) {
                if (_nowScheduler->flags & 0x4000) {
                    _nowScheduler->flags &= ~0x4000;
                    func_A2C408(0);
                }
                expired = (SchedulerState *)(record_offset + (unsigned int)_scheduler);
                if (expired->scriptBinding.link_id < 0) {
                    /* Rebased through the negated table address, as the original computes it. */
                unsigned int back_base = 0 - (unsigned int)_scheduler;

                    record = (SchedulerState *)(record_offset - back_base);
                    if ((record->flags & 0x20) == 0) {
                        sefFreeScheduler(index);
                        continue;
                    }
                }
                for (k = 0; k < 32; k++) {
                    sefDestroyEffectData((EffectData *)(schedulerStates + index * sizeof(SchedulerState) + k * sizeof(scheduler->effects[0])));
                }
                record_offset -= sizeof(SchedulerState);
                SLOT(scriptBinding.state) = SLOT(scriptBinding.link_id);
                {
                    SefEffectDescriptor *descriptor = (SefEffectDescriptor *)SLOT(inUse);
                    int init_flags = SLOT(initFlags);

                    control_address -= sizeof(SchedulerState);
                    scheduler--;
                    sefInitEffectTbl(descriptor, index, init_flags);
                }
                index -= 1;
                continue;
            } else {
                unsigned int state_offset = record_offset + offsetof(SchedulerState, scriptBinding);
                ScriptBinding *binding;
                SchedulerState *state;

                binding = (ScriptBinding *)(state_offset + (unsigned int)_scheduler);

                binding->state++;
                if (binding->state == 0x3ff) {
                    binding->state = 0x3fe;
                }
                state = (SchedulerState *)(record_offset + (unsigned int)_scheduler);
                if ((state->flags & 0x40) == 0) {
                    active_count += 1;
                }
            }
        }
        SLOT(flags) &= ~9;
    }

    sefSetLightFlag();
    return active_count > 0;
}

#undef SLOT
#undef BATTLE

int sefIsDeadSchduler(unsigned int scheduler_index)
{

    if (scheduler_index >= 0x80) {
        return 1;
    }
    return _scheduler[scheduler_index].inUse == 0;
}

static int sefSetReverseDir(int effect_id, int category)
{
    int normalized_id;
    unsigned int index;

    if ((unsigned int)(category - 0x18) < 8) {
        normalized_id = effect_id;
        if ((unsigned int)(effect_id - 0xB54) < 0x63) {
            normalized_id = effect_id - 0x64;
        }

        for (index = 0; index < 10; index++) {
            if (normalized_id == revEft_3[index]) {
                _revDirZ = 1;
                return 1;
            }
        }
    }

    _revDirZ = 0;
    return 0;
}

void sefCreateBattleActorTbl(int effect_no)
{
    SchedulerState *scheduler;
    int scheduler_index;
    int actor_index;

    if ((unsigned int)(effect_no - 0x9c4) < 0x11f) {
        return;
    }

    memset(_battleActor, 0, 0xda0);
    for (scheduler_index = 0; scheduler_index < 0x80; scheduler_index++) {
        scheduler = &_scheduler[scheduler_index];
        if (scheduler->inUse == 0) {
            continue;
        }
        if (scheduler->flags & 0x202) {
            continue;
        }
        if (scheduler->effectNo != effect_no) {
            continue;
        }

        if (scheduler->caster != 0) {
            for (actor_index = 0;
                 actor_index < _battleActor->count
                 && _battleActor->actors[actor_index].actor != scheduler->caster;
                 actor_index++) {
            }
            if (actor_index == _battleActor->count && actor_index < 24) {
                _battleActor->actors[actor_index].actor = scheduler->caster;
                _battleActor->actors[actor_index].schedulerLightCount = scheduler->targetLightCount;
                _battleActor->actors[actor_index].excluded = 1;
                _battleActor->count = actor_index + 1;
            }
        }

        if (scheduler->target != 0) {
            for (actor_index = 0;
                 actor_index < _battleActor->count
                 && _battleActor->actors[actor_index].actor != scheduler->target;
                 actor_index++) {
            }
            if (actor_index == _battleActor->count && actor_index < 24) {
                _battleActor->actors[actor_index].schedulerLightCount = scheduler->sourceLightCount;
                _battleActor->actors[actor_index].actor = scheduler->target;
                _battleActor->actors[actor_index].excluded = 0;
                _battleActor->count = actor_index + 1;
            }
        }
    }
}

static void sefInitBattlePrm(void)
{
    SefBattleActor *record;
    Actor *actor;
    Vector4 *position;
    int count = 0;
    int i;

    __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(&_battleActor->center) : "memory");
    for (i = 0; i < _battleActor->count; i++) {
        record = &_battleActor->actors[i];
        actor = record->actor;
        record->lightCount = 0;
        record->hitSignal = 0;
        record->seSignal = 0;
        if (actor != 0 && record->excluded == 0) {
            position = &actor->position;
            __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&_battleActor->center) : "memory");
            __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(position) : "memory");
            __asm__ __volatile__("vadd.xyz vf1, vf1, vf2" : : : "memory");
            __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(&_battleActor->center) : "memory");
            count++;
        }
    }
    if (count != 0) {
        MMathDivVectorS(&_battleActor->center, &_battleActor->center, (float)count);
    }
}

void sefCaclAllTarget(Vector4 *target)
{
    SefBattleActor *record;
    Actor *actor;
    Vector4 *position;
    int count = 0;
    int i;

    __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(target) : "memory");
    for (i = 0; i < _battleActor->count; i++) {
        record = &_battleActor->actors[i];
        actor = record->actor;
        if (actor != 0 && record->excluded == 0) {
            position = &actor->position;
            __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(target) : "memory");
            __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(position) : "memory");
            __asm__ __volatile__("vadd.xyz vf1, vf1, vf2" : : : "memory");
            __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(target) : "memory");
            count++;
        }
    }
    if (count != 0) {
        MMathDivVectorS(target, target, (float)count);
    }
}

static void sefSetLightFlag(void)
{
    SefBattleActor *record;
    Actor *actor;
    int i;

    for (i = 0; i < _battleActor->count; i++) {
        record = &_battleActor->actors[i];
        actor = record->actor;
        if (actor != 0 && record->lightCount > 0) {
            actor->flags |= SEF_ACTOR_LIGHT_FLAG;
        } else {
            actor->flags &= ~SEF_ACTOR_LIGHT_FLAG;
        }
    }
}

static void sefSetHitSignal(Actor *actor)
{
    SefBattleActor *record;
    int i;

    if (actor != 0) {
        for (i = 0; i < _battleActor->count; i++) {
            record = &_battleActor->actors[i];
            if (record->actor == actor) {
                record->hitSignal++;
                break;
            }
        }
    }
}

int sefIsHitActor(Actor *actor)
{
    SefBattleActor *record;
    int i;

    if (actor != 0) {
        for (i = 0; i < _battleActor->count; i++) {
            record = &_battleActor->actors[i];
            if (record->actor == actor) {
                return record->hitSignal != 0;
            }
        }
    }
    return 0;
}

static void sefSetSeSignal(Actor *actor)
{
    SefBattleActor *record;
    int i;

    if (actor != 0) {
        for (i = 0; i < _battleActor->count; i++) {
            record = &_battleActor->actors[i];
            if (record->actor == actor) {
                record->seSignal++;
                break;
            }
        }
    }
}

int sefIsSeSignal(Actor *actor)
{
    SefBattleActor *record;
    int i;

    if (actor != 0) {
        for (i = 0; i < _battleActor->count; i++) {
            record = &_battleActor->actors[i];
            if (record->actor == actor) {
                return record->seSignal != 0;
            }
        }
    }
    return 0;
}

static void sefAddLightActor(const Vector4 *position, const SefLightInput *light)
{
    float range_squared = (float)light->range * lightRangeScale;
    int actor_index;
    SefBattleActor *battle_actor;
    Vector4 *lights;
    Vector4 actor_position;
    Vector4 difference;
    float distance_squared;
    float weight;

    range_squared *= range_squared;

    for (actor_index = 0; actor_index < _battleActor->count; actor_index++) {
        battle_actor = &_battleActor->actors[actor_index];
        if (battle_actor->actor == 0 || battle_actor->lightCount != 0) {
            continue;
        }

        sefGetNullPosition(&actor_position, battle_actor->actor, 0, 1);
        __asm__ __volatile__(
            "lqc2 vf1, 0(%0)\n\t"
            "lqc2 vf2, 0(%1)\n\t"
            "vsub.xyz vf1, vf1, vf2\n\t"
            "sqc2 vf1, 0(%2)"
            :
            : "r"(position), "r"(&actor_position), "r"(&difference)
            : "memory");
        distance_squared = MMathVectorDotProduct(&difference, &difference);
        if (!(distance_squared < range_squared)) {
            continue;
        }

        lights = battle_actor->actor->light.parameters;
        weight = (range_squared - distance_squared) / range_squared * 0.0078125f;
        if (battle_actor->lightCount == 0) {
            *(SefLightSet *)lights = *(SefLightSet *)_defLight;
            __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(&battle_actor->light_accumulator) : "memory");
            MMathNormalizeVector2(&battle_actor->light_direction, &difference);
            __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)" : : "r"(&lights[6]), "r"(&battle_actor->light_direction) : "$8", "memory");
            battle_actor->actor->flags |= SEF_ACTOR_LIGHT_FLAG;
            __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)" : : "r"(&battle_actor->light_position), "r"(position) : "$8", "memory");
            __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)" : : "r"(&battle_actor->actor_position), "r"(&actor_position) : "$8", "memory");
        }

        __asm__ __volatile__(
            "mfc1 $2, %2\n\t"
            "lqc2 vf1, 0(%0)\n\t"
            "lqc2 vf3, 0(%1)\n\t"
            "vitof0.xyzw vf1, vf1\n\t"
            "qmtc2 $2, vf2\n\t"
            "vmulx.xyz vf1, vf1, vf2x\n\t"
            "vadd.xyz vf3, vf1, vf3\n\t"
            "sqc2 vf3, 0(%1)"
            :
            : "r"(light), "r"(&battle_actor->light_accumulator), "f"(weight)
            : "$2", "memory");
        battle_actor->actor->flags |= SEF_ACTOR_LIGHT_FLAG;
        __asm__ __volatile__(
            "lqc2 vf1, 0(%0)\n\t"
            "lqc2 vf2, 0(%1)\n\t"
            "vadd.xyz vf1, vf1, vf2\n\t"
            "sqc2 vf1, 0(%2)"
            :
            : "r"(&lights[5]), "r"(&battle_actor->light_accumulator), "r"(&lights[5]));
        battle_actor->lightCount++;
    }
}

void sefInitEffect(void)
{
    smInitilize(_eftBuffer, 0xD4800);
    sresInitMemoryRes();
    srsInitCdRead();
    sevInitPtAllocator();
    svInitImageMapper();
    sefInitScheduler();
    scInitScript();
    memset(&_battlePrm, 0, 0x34);
    memset(_battleData.bytes, 0, 0x230);
    sdvInitSpecialWork();
    sdvInitAmbient();
    sresLoadCommonMemory();
    _sefLoadEftQue = 0;
    _initialize = 1;
}

void sefInitEffectBattle(void)
{
    _sefBattleMode = 1;
    sdvInitAlters();
    sefDestroyEffect();

    sRender.battleDrawCallback = sefDrawEffect2D;
    sRender.cfDrawCallback = 0;
    if (_srsMemRes.battleImage == 0) {
        _srsMemRes.no[SRS_RES_BATTLE_IMAGE] = -1;
        _srsMemRes.battleImage = smAlloc(0x40000);
    }
}

void sefInitEffectCf(void)
{
    _sefBattleMode = 0;
    sefDestroyEffectCf();
    sresLoadCfMemory();

    sRender.battleDrawCallback = 0;
    sRender.cfDrawCallback = sefDrawEffect2D;
}

/* _battlePrm is the 0x34-byte battle parameter block read as SefBattleParameters. */
static inline SefBattleParameters *sefBattleParameters(void)
{
    return &_battlePrm;
}

void sefSetupPlayer(int character_id, int setup_value_1,
                    int setup_value_2, int setup_value_3)
{
    int index;

    if (character_id == 0xbd) {
        return;
    }
    for (index = 0; index < 3; index++) {
        if (sefBattleParameters()->player_character_ids[index] == character_id) {
            return;
        }
    }
    for (index = 0; index < 3; index++) {
        if (sefBattleParameters()->player_character_ids[index] == 0) {
            SefBattleParameters *parameters = sefBattleParameters();

            parameters->player_character_ids[index] = character_id;
            parameters->player_setup_parameters[index][0] = setup_value_1;
            parameters->player_setup_parameters[index][1] = setup_value_2;
            parameters->player_setup_parameters[index][2] = setup_value_3;
            parameters->player_active[index] = 1;
            sefSetupEffect();
            return;
        }
    }
    tracePrint(addPlayerFailedMessage, character_id);
}

void sefSetupEnemy(int character_id)
{
    int index;

    if (character_id <= 0) {
        return;
    }
    for (index = 0; index < 3; index++) {
        if (sefBattleParameters()->enemy_effect_numbers[index] == character_id ||
            sefBattleParameters()->enemy_character_ids[index] == character_id) {
            sefBattleParameters()->enemy_character_ids[index] = character_id;
            return;
        }
    }
    for (index = 0; index < 3; index++) {
        if (sefBattleParameters()->enemy_character_ids[index] == 0) {
            if (sefBattleParameters()->enemy_effect_numbers[index] > 0) {
                sresFreeReloaderMemoryNo(index + 3);
            }
            if (sefIsBossID(character_id)) {
                sefBattleParameters()->has_boss = 1;
            }
            sefBattleParameters()->enemy_character_ids[index] = character_id;
            sefBattleParameters()->enemy_effect_numbers[index] = character_id;
            sefBattleParameters()->enemy_active[index] = 1;
            sefSetupEffect();
            return;
        }
    }
    tracePrint(addEnemyFailedFormat, character_id);
}

void sefReleaseID(int effect_id)
{
    int index;

    for (index = 0; index < 3; index++) {
        if (sefBattleParameters()->player_character_ids[index] == effect_id) {
            sresFreeReloaderMemoryNo(index);
            sefBattleParameters()->player_character_ids[index] = 0;
            sefBattleParameters()->player_setup_parameters[index][0] = 0;
            sefBattleParameters()->player_setup_parameters[index][1] = 0;
            sefBattleParameters()->player_setup_parameters[index][2] = 0;
            sefBattleParameters()->player_active[index] = 0;
            return;
        }
    }

    for (index = 0; index < 3; index++) {
        if (sefBattleParameters()->enemy_character_ids[index] == effect_id) {
            sefBattleParameters()->enemy_effect_numbers[index] = effect_id;
            sefBattleParameters()->enemy_character_ids[index] = 0;
            sefBattleParameters()->enemy_active[index] = 0;
            return;
        }
    }

    tracePrint(characterIdMissingFormat, effect_id);
}

void sefSetupEffect(void) {
    sresLoadBattleData(&_battlePrm);
}

int sefIsEntryBoss(void) {
    return entryBossFlags[0];
}

void sefDestroyEffect(void)
{
    _sefLoadEftQue = 0;
    sefKillEffect(-1);
    sdvInitAmbient();
    scDestroyScriptAll();
    sresFreeReloaderMemory(1);
    memset(&_battlePrm, 0, 0x34);
    memset(_battleData.bytes, 0, 0x230);
    memset(_battleActor, 0, 0xDA0);
    sdvDestroyAlters();
}

void sefDestroyEffectCf(void)
{
    _sefLoadEftQue = 0;
    sefKillEffect(-1);
    sdvInitAmbient();
    scDestroyScriptAll();
    sresFreeReloaderMemory(0);
    memset(&_battlePrm, 0, 0x34);
    memset(_battleData.bytes, 0, 0x230);
    memset(_battleActor, 0, 0xDA0);
    sdvDestroyAlters();
}

void sefLoadEffect(int effect_no, int character_id)
{
    if (effect_no > 0) {
        svFileLoadScript(0, sefCnvEtEffectNo(effect_no, character_id));
    }
}

int sefCheckLoad(void)
{
    if (_sefLoadEftQue != 0) {
        return 1;
    }
    return srsLeaveCdRead() > 0;
}

void sefExecEffect(void)
{
    scExecEffect();
    sdvExecAlters();
    _hitFlag = 0;
    _hitSignal = 0;
    _seSignal = 0;
}

void sefProgressEffect(int frame_count)
{
    int remaining;

    if (frame_count > 0) {
        remaining = frame_count;
        do {
            sefExecEffect();
            remaining--;
        } while (remaining != 0);
    }
}

void sefDrawEffect(void)
{
    if (_initialize != 0) {
        if (!(GameLoopState.flags & SEF_DRAW_FLAG_PENDING)) {
            sefCalcInvView(&_invView);
            svDrawScheduler();
        }
    }
}

void sefDrawEffect3D(void) {
    if (_initialize != 0) {
        if (!(GameLoopState.flags & SEF_DRAW_FLAG_PENDING)) {
            svDrawScheduler3D(SEF_DRAW_FLAG_PENDING);
        }
    }
}

void sefDrawEffect2D(void) {
    if (_initialize != 0) {
        if (!(GameLoopState.flags & SEF_DRAW_FLAG_PENDING)) {
            svDrawScheduler2D(SEF_DRAW_FLAG_PENDING);
        }
    }
}

static void sefCnvDeathEffectNo(SefEffectRequest *request)
{
    int character_id;
    int kind;
    unsigned int i;

    if (request->effect_no == 0x4000) {
        character_id = request->character_id;
        kind = 0;
        if (sefIsBossID(character_id)) {
            for (i = 0; i < 36; i++) {
                if (request->character_id == _sefBossDeathPrm_4[i].character_id) {
                    request->effect_no = _sefBossDeathPrm_4[i].effect_no;
                    return;
                }
            }
        } else {
            if (request->flags & 0x2000) {
                kind = 1;
            } else if (request->flags & 0x1000) {
                kind = 2;
            } else {
                kind = 0;
            }
        }
        request->effect_no = kind * 3 + sefGetSizeOffset(request->size) + 2603;
        request->target = request->source;
        request->character_id_copy = request->character_id;
        request->size_copy = request->size;
    }
}

static void sefCnvWaitEffectNo(SefEffectRequest *request)
{
    int character_id;

    if (request->effect_no == 0x4001) {
        character_id = (unsigned short)request->character_id;
        if ((unsigned int)(character_id - 1) < 16) {
            character_id = (short)character_id;
            request->effect_no = steftTbl[character_id];
        } else if ((unsigned short)(character_id - 151) < 36) {
            character_id = (short)character_id;
            request->effect_no = srsGetBossWaitEftNo(character_id);
        } else {
            request->effect_no = 0;
        }
    }
}

void sefDeleteEffectWait(int character_id)
{
    SefEffectRequest request;
    int index;

    memset(&request, 0, sizeof(request));
    request.character_id = character_id;
    request.effect_no = 0x4001;
    sefCnvWaitEffectNo(&request);
    if (request.effect_no > 0) {
        index = 0;
        for (;;) {
            if (_scheduler[index].inUse != 0
                && _scheduler[index].effectNo == request.effect_no) {
                sefDeleteEffect2(&_scheduler[index]);
            }
            if (++index >= SEF_SCHEDULER_COUNT) {
                break;
            }
        }
    }
}

static int sefCnvEtEffectNo(int effect_no, int character_id)
{
    if ((unsigned int)effect_no - 0x7ebu < 3u) {
        if (character_id != 2) {
            effect_no -= 3;
        }
    } else if ((unsigned int)effect_no - 0x7e8u < 3u) {
        if (character_id == 2) {
            effect_no += 3;
        }
    } else if ((unsigned int)effect_no - 0x7f8u < 2u) {
        if (character_id != 1) {
            effect_no -= 2;
        }
    } else if ((unsigned int)effect_no - 0x7f6u < 2u && character_id == 1) {
        effect_no += 2;
    }
    return effect_no;
}

void sefCreateEffect(SefEffectRequest *request)
{
    SefEffectRequest *script_request;

    if (request->effect_no > 0) {
        if (request->effect_no == 55) {
            request->special = 1;
        }
        script_request = &_battleData.request;
        *script_request = *request;
        __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(_battleData.bytes + 0x30) : "memory");
        sefCnvDeathEffectNo(script_request);
        scCreateScript(script_request);
    }
}

SchedulerState *sefCreateEffect2(SefEffectRequest *request)
{
    SefEffectRequest *script_request;
    short *script_data;
    int script_index;
    int scheduler_index;
    SchedulerState *result = 0;

    if (request->effect_no <= 0
        || (script_request = &_battleData.request,
            *script_request = *request,
            sefCnvWaitEffectNo(script_request),
            request->effect_no <= 0)) {
        return 0;
    }
    script_data = scAnalyzeScriptCf(script_request->effect_no,
                                    &script_index);
    if (script_data == 0) {
        return 0;
    }
    scheduler_index = sefCreateScheduler((int)(script_data + script_index),
                                         script_request, 0,
                                         (int)script_data, -1);
    if (scheduler_index >= 0) {
        result = (SchedulerState *)_scheduler + scheduler_index;
        result->flags |= 0x240;
        if ((unsigned int)((unsigned short)request->effect_no - 2630) < 7) {
            result->caster = request->target;
            result->flags = (result->flags & ~0x400) | 2;
        }
    }
    return result;
}

void sefDeleteEffect2(SchedulerState *scheduler)
{
    sefFreeSchedulerCf(scheduler);
}

int sefLoadEffectCf(int cf_id, int effect_no)
{
    srsFileLoadCf(cf_id, effect_no);
    return 0;
}

int sefLoadEffectCfName(int cf_id, char *effect_name)
{
    int effect_no = srsEffectNameToID(effect_name);

    if (effect_no > 0) {
        srsFileLoadCf(cf_id, effect_no);
    }
    return 0;
}

int sefLoadMemoryEffectCf(void *data, int effect_no, int size)
{
    srsMemoryLoadCf(data, effect_no, size);
    return 0;
}

int sefLoadMemoryEffectCfName(void *data, char *effect_name, int size)
{
    int effect_no = srsEffectNameToID(effect_name);

    if (effect_no > 0) {
        srsMemoryLoadCf(data, effect_no, size);
    }
    return 0;
}

static SchedulerState *sefCreateEffectCf2(int effectNo,
                                           const Vector4 *position,
                                           const Vector4 *orientation,
                                           int schedulerIndex)
{
    SchedulerState *scheduler = 0;
    short *script;
    int count;
    SefEffectRequest request;
    int index;

    if (effectNo <= 0) {
        return 0;
    }
    script = scAnalyzeScriptCf(effectNo, &count);
    if (script == 0) {
        return 0;
    }
    memset(&request, 0, sizeof(request));
    request.effect_no = effectNo;
    index = sefCreateScheduler((int)(script + count), &request, (void *)position,
                               (int)script, schedulerIndex);
    if (index >= 0) {
        scheduler = &((SchedulerState *)_scheduler)[index];
        scheduler->flags |= 0x412;
        if (orientation != 0) {
            __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)" : : "r"(&scheduler->rotation), "r"(orientation) : "$8", "memory");
        }
    }
    return scheduler;
}

SchedulerState *sefCreateEffectCf(int effectNo, const Vector4 *position,
                                const Vector4 *orientation)
{
    return sefCreateEffectCf2(effectNo, position, orientation, -1);
}

void sefDeleteEffectCf(SchedulerState *scheduler)
{
    sefFreeSchedulerCf(scheduler);
}

void sefRewindEffectCf(SchedulerState *scheduler)
{
    if (scheduler != 0) {
        scheduler->flags |= SEF_SCHEDULER_REWIND_FLAG;
    }
}

void sefClearEffectCf(SchedulerState *scheduler)
{
    EffectData *effect;
    unsigned char *lines;
    SefLocalData *line;
    int handle;
    int i;
    int j;

    if (scheduler != 0) {
        effect = scheduler->effects;
        for (i = 0; i < 0x20; i++) {
            if (effect->owner != 0) {
                for (j = 0, lines = _lineData; j < 0x20; j++) {
                    handle = effect->lineHandles[j];
                    if (handle >= 0) {
                        line = (SefLocalData *)(handle * SEF_LINE_DATA_RECORD_SIZE + (int)lines);
                        if (line != 0) {
                            sefDestroyLocalData(line);
                        }
                    }
                }
            }
            effect++;
        }
    }
}

int sefIsFinishEffect(int effect_no)
{
    unsigned int scheduler_address;
    int *scheduler_cursor;
    int active_count = 0;
    int scheduler_index;

    if (func_A33248() != 0) {
        return 0;
    }

    scheduler_index = 0x7f;
    scheduler_address = (unsigned int)_scheduler;
    scheduler_cursor = (int *)(scheduler_address + 0xa70);
    do {
        if (scheduler_cursor[-0xf0] != 0) {
            if ((scheduler_cursor[7] & 0x40) == 0) {
                if (effect_no == 0 || ((short *)scheduler_cursor)[4] == effect_no) {
                    active_count++;
                }
            }
        }
        scheduler_index--;
        scheduler_cursor += 0x2ac;
    } while (scheduler_index >= 0);

    return active_count == 0;
}

int sefIsFinishEffect2(int effect_no)
{
    SchedulerState *scheduler;
    int index;
    int running;

    running = 0;
    if (effect_no <= 0) {
        return 1;
    }
    for (index = 0; index < SEF_SCHEDULER_COUNT; index++) {
        if (_scheduler[index].inUse != 0
            && sefCnvEtEffectNo(_scheduler[index].request.effect_no,
                                _scheduler[index].request.character_id)
                   == effect_no) {
            running++;
        }
    }
    return running == 0;
}

void sefSetLoadQue(int effect_no)
{
    _sefLoadEftQue = effect_no;
}

int *sefGetLoadQue(void)
{
    return &_sefLoadEftQue;
}

void sefExecLoadQue(void)
{
    if (_sefLoadEftQue > 0) {
        svFileLoadScript(0, _sefLoadEftQue);
    }
}

void sefKillEffect(int effect_no)
{
    int record_offset;
    SefEffectScheduler *schedulers = (void *)_scheduler;
    SefEffectScheduler *record;
    SefEffectControl *control;
    int index;

    record_offset = offsetof(SefEffectScheduler, effect);
    control = (void *)((unsigned char *)_scheduler + record_offset);
    index = 0;
    do {
        record = (void *)((unsigned char *)control - record_offset);
        if (record->inUse != 0
            && (effect_no < 0 || control->effect_no == effect_no)) {
            sefFreeScheduler(index);
        }
        index++;
        if (index < SEF_SCHEDULER_COUNT) {
            control = &schedulers[index].effect;
        }
    } while (index < 0x80);
}

void sefHitEffect(void) {
    _hitFlag = 1;
}

void sefPushEffect(void)
{
    SefEffectRequest *request;
    int index;

    sefMemZero(_savePrm, sizeof(_savePrm));
    for (index = 0; index < SEF_SAVE_PRM_COUNT; index++) {
        if (_scheduler[index].inUse != 0) {
            __asm__ __volatile__(
                "lq $8, 0(%1)\n\t"
                "sq $8, 0(%0)"
                : : "r"(&_savePrm[index].position), "r"(&_scheduler[index].position) : "$8", "memory");
            __asm__ __volatile__(
                "lq $8, 0(%1)\n\t"
                "sq $8, 0(%0)"
                : : "r"(&_savePrm[index].rotation), "r"(&_scheduler[index].rotation) : "$8", "memory");
            __asm__ __volatile__(
                "lq $8, 0(%1)\n\t"
                "sq $8, 0(%0)"
                : : "r"(&_savePrm[index].scale), "r"(&_scheduler[index].scale) : "$8", "memory");
            _savePrm[index].savedActive = _scheduler[index].savedActive;
            _savePrm[index].caster = _scheduler[index].caster;
            _savePrm[index].target = _scheduler[index].target;
            _savePrm[index].casterUnit = _scheduler[index].casterUnit;
            _savePrm[index].targetUnit = _scheduler[index].targetUnit;
            _savePrm[index].flags = _scheduler[index].flags;
            /* Read through the request record, not the SchedulerState union
             * member: a union member access is alias set 0 in GCC 2.96 and
             * would order this load before every store of the block. */
            request = &_scheduler[index].request;
            _savePrm[index].effectNo = request->effect_no;
            _savePrm[index].display = _scheduler[index].display;
        }
    }
}

void sefPopEffect(void)
{
    SefSavePrm *save;
    int index;

    sefKillEffect(-1);
    for (index = 0; index < SEF_SAVE_PRM_COUNT;) {
        if (_savePrm[index].savedActive != 0) {
            sefCreateEffectCf2(_savePrm[index].effectNo, &_savePrm[index].position,
                               &_savePrm[index].rotation, index);
            __asm__ __volatile__(
                "lq $8, 0(%1)\n\t"
                "sq $8, 0(%0)"
                : : "r"(&_scheduler[index].scale), "r"(&_savePrm[index].scale) : "$8", "memory");
            save = &_savePrm[index];
            _scheduler[index].savedActive = save->savedActive;
            _scheduler[index].caster = save->caster;
            _scheduler[index].target = save->target;
            _scheduler[index].casterUnit = save->casterUnit;
            _scheduler[index].targetUnit = save->targetUnit;
            _scheduler[index].flags = save->flags;
            _scheduler[index].display = save->display;
        }
        index++;
    }
    sefMemZero(_savePrm, 0x2800);
    sefProgressEffect(10);
}

unsigned char *sefGetBattleData(void)
{
    return _battleData.bytes;
}

int sefGetDmgNull(void)
{
    return (sefDmgNullCount[0] >= 2) ? SEF_TARGET_DAMAGE_NULL_A : SEF_TARGET_DAMAGE_NULL_B;
}

unsigned char *sevGetPtAllocator(int allocator_index)
{
    return &_ptAlloc[allocator_index * SEF_PT_ALLOC_RECORD_SIZE];
}

void *sevGetPtAllocator2(int allocator_index)
{
    return &_ptAlloc[allocator_index * SEF_PT_ALLOC_RECORD_SIZE];
}

void *sefGetLineAdr(int line_index)
{
    if (line_index < 0) {
        return 0;
    }
    return &_lineData[line_index * SEF_LINE_DATA_RECORD_SIZE];
}

SchedulerState *sefGetScheduler(void)
{
    return _scheduler;
}

SchedulerState *sefGetNowScheduler(void)
{
    return _nowScheduler;
}

unsigned char *sefGetParentLine(int line, int parent_local)
{
    if (line >= 0 && parent_local >= 0) {
        return &_ptAlloc[parent_local * SEF_PT_ALLOC_RECORD_SIZE];
    }
    return 0;
}

void sefScaleIVectorAdd(void *dest, void *scale_source, void *addend, float scale) {
    __asm__ __volatile__("lqc2 vf1, 0(%0)\n\tlqc2 vf3, 0(%1)\n\tvitof0.xyzw vf1, vf1\n\tqmtc2.ni %2, vf2\n\tvmulx.xyz vf1, vf1, vf2x\n\tvadd.xyz vf3, vf1, vf3\n\tsqc2 vf3, 0(%3)" :  : "r"(scale_source), "r"(addend), "r"(scale), "r"(dest) : "memory");
}

void sefDeg2RadVector(Vector4 *dst, Vector4 *src)
{
    __asm__ __volatile__("lqc2 $vf2, 0(%0)" : : "r"(src) : "memory");

    {
        register float degToRad asm("$f8") = lit4_004d8354;

        __asm__ __volatile__(
            "mfc1 $8, %1\n\t"
            "qmtc2.ni $8, $vf1\n\t"
            "vmulx.xyz $vf2, $vf2, $vf1x\n\t"
            "sqc2 $vf2, 0(%0)"
            :
            : "r"(dst), "f"(degToRad)
            : "$8", "memory"
        );
    }
}

void sefMergeMatrixPos(Matrix4 *dest, Matrix4 *matrix, Vector4 *position)
{
    __asm__ __volatile__(
        "lq $8, 0(%1)\n\t"
        "lq $9, 16(%1)\n\t"
        "lq $10, 32(%1)\n\t"
        "lq $11, 0(%2)\n\t"
        "sq $8, 0(%0)\n\t"
        "sq $9, 16(%0)\n\t"
        "sq $10, 32(%0)\n\t"
        "sq $11, 48(%0)"
        :
        : "r"(dest), "r"(matrix), "r"(position)
        : "$8", "$9", "$10", "$11", "memory"
    );
}

void sefLerpVectorB(void *first, Vector4 *dest, float factor)
{
    void *second = (char *)first + 10;

    __asm__ __volatile__(
        "mfc1 $9, %3\n\t"
        "qmtc2.ni $9, $vf3\n\t"
        "ldl $8, 7(%0)\n\t"
        "ldr $8, 0(%0)\n\t"
        "ldl $9, 7(%1)\n\t"
        "ldr $9, 0(%1)\n\t"
        "dsrl $8, $8, 16\n\t"
        "dsrl $9, $9, 16\n\t"
        "pcgth $10, $0, $8\n\t"
        "pcgth $11, $0, $9\n\t"
        "pextlh $10, $10, $8\n\t"
        "pextlh $11, $11, $9\n\t"
        "qmtc2.ni $10, $vf1\n\t"
        "qmtc2.ni $11, $vf2\n\t"
        "vitof0.xyzw $vf1, $vf1\n\t"
        "vitof0.xyzw $vf2, $vf2\n\t"
        "vsub.xyz $vf2, $vf2, $vf1\n\t"
        "vmulx.xyz $vf2, $vf2, $vf3x\n\t"
        "vadd.xyz $vf2, $vf2, $vf1\n\t"
        "sqc2 $vf2, 0(%2)"
        :
        : "r"(first), "r"(second), "r"(dest), "f"(factor)
        : "$8", "$9", "$10", "$11", "memory"
    );
}

void sefLerpIVectorB(void *first, Vector4 *dest, float factor)
{
    void *second = (char *)first + 10;

    __asm__ __volatile__(
        "ldl $8, 7(%0)\n\t"
        "ldr $8, 0(%0)\n\t"
        "ldl $9, 7(%1)\n\t"
        "ldr $9, 0(%1)\n\t"
        "mfc1 $10, %3\n\t"
        "qmtc2.ni $10, $vf3\n\t"
        "pcgth $10, $0, $8\n\t"
        "pcgth $11, $0, $9\n\t"
        "pextlh $10, $10, $8\n\t"
        "pextlh $11, $11, $9\n\t"
        "qmtc2.ni $10, $vf1\n\t"
        "qmtc2.ni $11, $vf2\n\t"
        "vitof0.xyzw $vf1, $vf1\n\t"
        "vitof0.xyzw $vf2, $vf2\n\t"
        "vsub.xyz $vf2, $vf2, $vf1\n\t"
        "vmulx.xyz $vf2, $vf2, $vf3x\n\t"
        "vadd.xyz $vf2, $vf2, $vf1\n\t"
        "vftoi0.xyzw $vf2, $vf2\n\t"
        "sqc2 $vf2, 0(%2)"
        :
        : "r"(first), "r"(second), "r"(dest), "f"(factor)
        : "$8", "$9", "$10", "$11", "memory"
    );
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char badRangeParamFormat[24] = "bad range prm : %d";

const char lineEmptyMessage[16] = "line empty!!";

/* JP: 敵キャラが追加できません : %d
 * EN: Cannot add enemy character : %d */
const char addEnemyFailedFormat[32] = "\305\250\245\255\245\343\245\351\244\254\304\311\262\303\244\307\244\255\244\336\244\273\244\363 : %d";

/* JP: 指定ＩＤのキャラ(%d)は存在しません!!
 * EN: Character with the given ID (%d) does not exist!! */
const char characterIdMissingFormat[40] = "\273\330\304\352\243\311\243\304\244\316\245\255\245\343\245\351(%d)\244\317\302\270\272\337\244\267\244\336\244\273\244\363!!";

const float sef_rand_unit_scale_literal = 3.0518509447574615e-05f;

const float offsetRadiusScale = 0.10000000149011612f;

const float spherePosScale = 0.10000000149011612f;

const float circlePosScale = 0.10000000149011612f;

const float rotateXDegToRad = 0.01745329238474369f;

const float rotateZDegToRad = 0.01745329238474369f;

const float invShortMax = 3.0518509447574615e-05f;

const float offsetYScale = 0.10000000149011612f;

float particleSizeScale = 0.0099999997764825821f;

const float gravityScale = 0.0010000000474974513f;

const float heightScale = 0.0099999997764825821f;

const float globalPositionScale = 0.10000000149011612f;

const float parentPositionScale = 0.10000000149011612f;

const float positionSourceScale = 0.10000000149011612f;

const float analyzeAnimScale = 0.004999999888241291f;

float randomRotationDegToRad = 0.01745329238474369f;

float collisionProgressScale = 0.0099999997764825821f;

float lightRangeScale = 0.0099999997764825821f;

static int _schridx_1 = 0;

int _nowEftNo = 0;

static const Vector4 _zeroAng = { 0.0f, 0.0f, 0.0f, 0.0f };
