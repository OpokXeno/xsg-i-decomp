#include "common.h"

#include "shared.h"

/*
 * UwamonoMapUnit is this TU's local view of the same MapUnit[] record that
 * src/main/init_drill.c also touches; only the fields SendBrokenSignal,
 * GetUwamonoSignal and CreateUwamonoCommon read or write are named, and the
 * rest is unmodeled padding recovered as its exact byte span so the offsets
 * below stay exact.
 */

typedef struct UwamonoMapUnit {
    u32 flags;                        /* +0x00 */
    unsigned char unmodeled_04[0x0c];
    Vector4 position;                 /* +0x10 */
    unsigned char unmodeled_20[4];
    /*
     * MAP_updateUnitSymbol (VA 0x002bfbe0) adds the scaffold-owned .lit4
     * constant D_004D7EBC (value 0.05) to this field unconditionally
     * (`lwc1`/`add.s`/`swc1`, no branch in the original); no other role is
     * evidenced here.
     */
    float symbolPhase;                /* +0x24 */
    unsigned char unmodeled_28[0x79];
    signed char actionNo;             /* +0xA1 */
    signed char actionSub;            /* +0xA2 */
    unsigned char unmodeled_a3;
    short serial;                     /* +0xA4 */
    unsigned char unmodeled_a6[2];
    short sequenceNo;                 /* +0xA8 */
    short sequenceSub;                /* +0xAA */
    unsigned char unmodeled_ac[0xf8];
    signed char signal;               /* +0x1A4 */
    unsigned char unmodeled_1a5[0x28];
    signed char crossType;            /* +0x1CD */
} UwamonoMapUnit;

/* Runtime-loaded object message with a hexadecimal unit identifier. */

const char D_004CA028[24] = "\xBE\xEF\xC3\xF3\xBE\xE5\xCA\xAA\xA5\xED\xA1\xBC\xA5\xC9 %x\n";

int uwares_tbl[14] = { 0 };

int xtxres_tbl[14] = { 0 };

extern int printf(const char *format, ...);

/*
 * MAP_LoadUwamonoResource's own additive view of the same MapUnit[] record
 * UwamonoMapUnit above already partially names; only the fields
 * MAP_LoadUwamonoResource (VA 0x002be5f0) writes are named here: it stores
 * uwares_tbl[index] and xtxres_tbl[index], where index = resourceId -
 * 0x7001.
 */

typedef struct UwamonoResourceUnit {
    unsigned char unmodeled_00[0xe0];
    int uwaresId;                     /* +0xE0 */
    int xtxresId;                     /* +0xE4 */
} UwamonoResourceUnit;

/*
 * Valid resourceId values run 0x7001..0x700E (14 entries, the size of
 * uwares_tbl and xtxres_tbl); 0x7012 is excluded explicitly ahead of the
 * broader >= 0x1000 range check.
 */

/*
 * GetPartsPos's own additive view of the global game-loop record (TU-local
 * by canon, config/header-canon.json): only the map-parts resource root
 * pointer at +0x54 is read here.
 */

typedef struct MapPartsResource {
    unsigned char unmodeled_00[0x50];
    int arrayOffset;              /* +0x50, added to this pointer to form the entry array base */
} MapPartsResource;

typedef struct MapPartsRoot {
    unsigned char unmodeled_00[4];
    MapPartsResource *resource;   /* +0x04 */
} MapPartsRoot;

typedef struct UwamonoSoundActor {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
} UwamonoSoundActor;

typedef UwamonoSoundActor AimHeightReference;

typedef struct UwamonoGameLoopState {
    unsigned char unmodeled_00[4];
    UwamonoSoundActor *actor; /* +0x04 */
    unsigned char unmodeled_08[0x14];
    int enemyUpdateState;             /* +0x1C */
    unsigned int aimHeightFlags;      /* +0x20 */
    unsigned char unmodeled_24[0x30];
    MapPartsRoot *partsRoot;          /* +0x54 */
    unsigned char unmodeled_58[0x8c];
    short shootUnit[64];              /* +0xE4 */
    unsigned char unmodeled_164[0x29ddd];
    signed char aimMapUnitIndex;
    unsigned char unmodeled_29f42[2];
    float selectionDistance;      /* +0x29F41 */
} UwamonoGameLoopState;

extern UwamonoGameLoopState GameLoopState;

/*
 * GetPartsPos's own additive view of the same MapUnit[] record other
 * functions in this TU already partially name; only the fields it writes
 * are named here: position (+0x10, the full Vector4 copied from the
 * resource entry, w set to 1.0f) and rotation's three touched components
 * (+0x20/+0x24/+0x28, x and z always cleared, y set from atan2f); entry
 * (+0x80, the resource entry pointer stored for later use); serial (+0xA4,
 * this unit's index into the resource entry array).
 */

typedef struct GetPartsPosEntry {
    unsigned char unmodeled_00[0x20];
    float basisX;             /* +0x20 */
    unsigned char unmodeled_24[4];
    float basisZ;              /* +0x28 */
    unsigned char unmodeled_2c[4];
    float x;                    /* +0x30 */
    float y;                     /* +0x34 */
    float z;                      /* +0x38 */
    unsigned char unmodeled_3c[4];
} GetPartsPosEntry;

typedef struct GetPartsPosUnit {
    unsigned char unmodeled_00[0x10];
    Vector4 position;      /* +0x10, w set to 1.0f */
    float rotationX;         /* +0x20, always cleared */
    float rotationY;          /* +0x24, atan2f(entry->basisX, entry->basisZ) */
    float rotationZ;           /* +0x28, always cleared */
    unsigned char unmodeled_2c[0x54];
    GetPartsPosEntry *entry;      /* +0x80 */
    unsigned char unmodeled_84[0x20];
    short serial;                  /* +0xA4 */
} GetPartsPosUnit;

extern float atan2f(float y, float x);

extern void sefDeleteEffectCf(int effectId);

extern void xglSoundEffectStopID(int soundId, int channel);

/*
 * ClearUwamonoEffect's own additive view of the same MapUnit[] record
 * UwamonoMapUnit above already partially names; only the fields
 * ClearUwamonoEffect (VA 0x002be8c0) reads or writes are named here.
 */

typedef struct UwamonoEffectUnit {
    unsigned char unmodeled_00[0xa0];
    /* Read once (`lbu`), plus one, as the channel argument to
     * xglSoundEffectStopID; no other role is evidenced here. */
    unsigned char seChannel;           /* +0xA0 */
    unsigned char unmodeled_a1[0x47];
    /* Walked in order, deleting and zeroing each non-zero effect handle
     * with sefDeleteEffectCf. */
    int effectCf[3];                   /* +0xE8 */
    unsigned char unmodeled_f4[0xac];
    /*
     * The original computes this sub-pointer once, ahead of the effect
     * loop, and keeps it live (register evidence) across the whole
     * function; UwamonoTimers below names the same +0x1A0 block for
     * UwamonoCommonFunc's view.
     */
    struct UwamonoEffectTimers {
        unsigned char unmodeled_00[0x48];
        int bgmTimer;                   /* +0x48 (record +0x1E8) */
    } timers;                           /* +0x1A0 */
} UwamonoEffectUnit;

typedef struct InitMapPartsUnit {
    unsigned int flags;
    void (*update)(struct InitMapPartsUnit *);
    unsigned char unmodeled_08[0xd8];
    int uwaresId;
    unsigned char unmodeled_e4[0xbc];
    short mapType;
} InitMapPartsUnit;

extern void InitMapDoor(InitMapPartsUnit *unit);

extern void InitMapKoware(InitMapPartsUnit *unit);

extern void InitDrill(InitMapPartsUnit *unit);

const char D_004CA0D0[24] = "Illegal Parts Type %d\n";

#define UWAMONO_SYMBOL_PHASE_STEP 0.05f

/*
 * HitCheckMapUnitPos (main:0x002c02f0): a thin tail call into
 * HitCheckMapUnitPosSize with its distance threshold fixed at 0.0f (a
 * true `j`, no `jal`, so this function's own return value is whatever
 * HitCheckMapUnitPosSize returns).
 */

extern void HitCheckMapUnitPosSize(int unit, float threshold);

/*
 * *(UwamonoLinkedUnit.position below) is a separate record UwamonoCommonFunc
 * keeps synchronized with this unit's position; only the field it writes is
 * named.
 */

typedef struct UwamonoLinkedUnit {
    float direction[3];               /* +0x00 */
    unsigned char unmodeled_0c[0x24];
    Vector4 position; /* +0x30, set from the owning unit's position */
} UwamonoLinkedUnit;

typedef struct UwamonoTimers {
    unsigned char unmodeled_00[0xa];
    signed char projectSound;         /* +0x0A (record +0x1AA) */
    unsigned char unmodeled_0b[0x23];
    signed char heightCheckFlag; /* +0x2E (record +0x1CE) */
    unsigned char unmodeled_2f[0x19];
    int bgmTimer;                /* +0x48 (record +0x1E8) */
} UwamonoTimers;

typedef union UwamonoAlignedVector {
    Vector4 value;
    long long words[2];
} UwamonoAlignedVector;

typedef struct UwamonoCommonUnit {
    unsigned char unmodeled_00[0x10];
    UwamonoAlignedVector position;                 /* +0x10 */
    unsigned char unmodeled_20[0x60];
    UwamonoLinkedUnit *linkedUnit;     /* +0x80 */
    unsigned char unmodeled_84[0x1c];
    unsigned char seChannel;
    unsigned char unmodeled_a1[3];
    short serial;                     /* +0xA4 */
    unsigned char unmodeled_a6[0xfa];
    UwamonoTimers timers;              /* +0x1A0 */
} UwamonoCommonUnit;

/* Defined in src/main/map_create_unit_peer.c (main/tu270), still in asm. */

extern void MAP_updateUnitPartsSequence(void *unit);

extern void MAP_updateUnitSequence(void *unit);

/* Defined later in this TU (a local sibling still in asm). */

void CheckUwamonoHeight(UwamonoCommonUnit *unit);

/* Defined later in this TU (a local sibling still in asm). */

void UwamonoBgmFunc(UwamonoCommonUnit *unit);

/*
 * UwamonoBgmFadeOut's own additive view of the same MapUnit[] record the
 * structs above already partially name; only the fields it reads are named
 * here. flags (+0x00) is the same field UwamonoMapUnit and DrillMapUnit
 * name; bit 0x10000 is the created/active flag CreateUwamonoCommon sets.
 * seChannel (+0xA0) is the same field ClearUwamonoEffect's UwamonoEffectUnit
 * names, used the same way (+1 as a channel-shaped argument). bgmTimer
 * (+0x1E8) is the same field UwamonoTimers/UwamonoEffectTimers name; here
 * its high halfword is a SoundWork effect-bank index and its low halfword
 * a sub-id, combined below into the effect_id SsdFadeoutEffect takes.
 */

typedef struct UwamonoBgmFadeOutUnit {
    u32 flags;                        /* +0x00 */
    unsigned char unmodeled_04[0x9c];
    unsigned char seChannel;          /* +0xA0 */
    unsigned char unmodeled_a1[0x147];
    int bgmTimer;                     /* +0x1E8 */
    unsigned char unmodeled_1ec[0x114];
} UwamonoBgmFadeOutUnit;

typedef struct UwamonoMapSlot UwamonoMapSlot;


struct UwamonoMapSlot {
    unsigned int flags;
    void (*update)(UwamonoMapSlot *);
    unsigned char unmodeled_08[8];
    UwamonoAlignedVector position;
    unsigned char unmodeled_20[0x82];
    unsigned char actionSub;
    unsigned char unmodeled_a3;
    short serial;
    unsigned char unmodeled_a6[0x42];
    int effectCf[3];
    unsigned char unmodeled_f4[0xac];
    struct UwamonoMapCollisionState {
        unsigned char unmodeled_00[0x2d];
        signed char crossType;
        unsigned char unmodeled_2e[0x132];
    } state;
};

extern UwamonoMapSlot MapUnit[];

typedef struct UwamonoSoundWorkView {
    unsigned char unmodeled_00[0x20];
    struct {
        unsigned short handle;        /* +0x20 + bank * 4 */
        unsigned char unmodeled_02[2];
    } effectBanks[16];
} UwamonoSoundWorkView;

extern UwamonoSoundWorkView SoundWork;

/* Defined in src/main/ssd_1.c (main/tu ssd_1), already C. */

extern void SsdFadeoutEffect(int effect_id, int source_id, int fade_time);


typedef struct UwamonoInitSymbolState {
    unsigned char unmodeled_00[0x2d];
    signed char state;                 /* +0x2d */
    unsigned char unmodeled_2e[0x16];
    int timer;                         /* +0x44 */
} UwamonoInitSymbolState;

typedef struct UwamonoInitSymbolUnit {
    int flags;                         /* +0x00 */
    void (*updateFunction)(UwamonoMapUnit *unit); /* +0x04 */
    unsigned char unmodeled_08[8];
    Vector4 position;                  /* +0x10 */
    float rotationX;                   /* +0x20 */
    float rotationY;                   /* +0x24 */
    float rotationZ;                   /* +0x28 */
    unsigned char unmodeled_2c[0x14];
    float matrix[4][4];                /* +0x40 */
    float (*savedMatrix)[4];           /* +0x80 */
    unsigned char unmodeled_84[0x5c];
    int uwaresId;                      /* +0xe0 */
    int xtxresId;                      /* +0xe4 */
    unsigned char unmodeled_e8[0xb8];
    UwamonoInitSymbolState timerState; /* +0x1a0 */
} UwamonoInitSymbolUnit;

const char D_004CA170[32] = "Create Save Symbol %f %f %f\n";

const char D_004CA190[32] = "Create Shop Symbol %f %f %f\n";

extern void MAP_updateUnitSaveSymbol(UwamonoMapUnit *unit);

extern void MAP_updateUnitShopSymbol(UwamonoMapUnit *unit);

extern void MAP_updateUnitSymbol(UwamonoMapUnit *unit);


typedef struct UwamonoCreatedState {
    unsigned char unmodeled_00[5];
    signed char signal;
    unsigned char unmodeled_06[0x1e];
    float height;
    unsigned char unmodeled_28[4];
    signed char state;
    unsigned char unmodeled_2d[1];
    signed char heightCheckFlag;
    unsigned char unmodeled_2f[3];
    signed char target;
    unsigned char unmodeled_33[0x11];
    int timer;
    unsigned char unmodeled_48[0x38];
} UwamonoCreatedState;

typedef struct UwamonoCreatedUnit {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    float rotationX;
    float rotationY;
    float rotationZ;
    unsigned char unmodeled_2c[0x174];
    UwamonoCreatedState state;
} UwamonoCreatedUnit;

extern const char D_004CA1B0[32];

extern UwamonoCreatedUnit *MAP_createUnit(int index, int resourceId);

extern int MAP_loadUnitResource(UwamonoCreatedUnit *unit, int resourceId);

extern void InitMapTrap(UwamonoCreatedUnit *unit);

extern void *memset(void *destination, int value, unsigned int bytes);

typedef struct UwamonoMapUnitState {
    unsigned char unmodeled_00[0x2d];
    signed char state;                 /* +0x2d */
    unsigned char unmodeled_2e[4];
    signed char target;                /* +0x32 */
    unsigned char unmodeled_33[0x11];
    int timer;                         /* +0x44 */
} UwamonoMapUnitState;

typedef struct UwamonoKowareDimensions {
    unsigned long long lanes[2];
} UwamonoKowareDimensions;

typedef struct UwamonoKowareKind {
    signed char shape;
    unsigned char unmodeled_01[0xf];
    UwamonoKowareDimensions dimensions;
    signed char target;
    unsigned char unmodeled_21[7];
    unsigned long long unmodeled_28;
} UwamonoKowareKind;

typedef struct UwamonoKowareUnit {
    unsigned int flags;
    void (*updateFunction)(void *unit);
    unsigned char unmodeled_08[8];
    unsigned long long unmodeled_10;
    unsigned char unmodeled_18[0x98];
    UwamonoKowareDimensions dimensions;
    unsigned char unmodeled_c0[0xe0];
    UwamonoMapUnitState state;         /* +0x1a0 */
} UwamonoKowareUnit;

extern UwamonoKowareKind KowareKind_t[];

extern const char D_004CA1D0[];

extern void MAP_updateUnitKoware(void *unit);

void CreateUwamonoCommon(UwamonoMapUnit *unit);

typedef struct UwamonoCollisionUnit {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    unsigned char unmodeled_20[0x90];
    float radius;
    float height;
    unsigned char unmodeled_b8[0xe8];
    UwamonoMapUnitState state;
} UwamonoCollisionUnit;

typedef struct UwamonoCollisionActor {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    unsigned char unmodeled_20[0x9c8];
    float radius;
    unsigned char unmodeled_9ec[0x84];
} UwamonoCollisionActor;

extern float CheckDist2D(const Vector4 *first, const Vector4 *second);

extern int HitCheckBoxCorner(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit);

unsigned int HitCheckMapUnitPosAt(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit);

typedef struct UwamonoScanFields {
    unsigned char channel;            /* +0x00 */
    unsigned char unmodeled_01[3];
    short resourceId;                 /* +0x04 */
    unsigned char unmodeled_06[0xfa];
    UwamonoMapUnitState state;         /* +0x100; state byte at +0x12D */
} UwamonoScanFields;

typedef struct UwamonoScanUnit {
    unsigned int flags;
    unsigned char unmodeled_04[0x0c];
    Vector4 position;
    unsigned char unmodeled_20[0x80];
    UwamonoScanFields scan;            /* +0xA0 */
    unsigned char unmodeled_1e8[0x118];
} UwamonoScanUnit;

struct UwamonoBoxUnit;

int HitCheckBox(UwamonoCollisionActor *actor, struct UwamonoBoxUnit *unit);

extern const float D_004D7ED0;

extern const float D_004D7ED4;

extern const float D_004D7ED8;

extern const float D_004D7EDC;

typedef struct UwamonoBoxBasis {
    float x;
    unsigned char unmodeled_04[4];
    float z;
} UwamonoBoxBasis;

typedef struct UwamonoBoxCollisionState {
    unsigned char unmodeled_00[0x2d];
    signed char crossType;
} UwamonoBoxCollisionState;

typedef struct UwamonoBoxUnit {
    unsigned char unmodeled_00[0x10];
    Vector4 position;                 /* +0x10 */
    unsigned char unmodeled_20[0x60];
    UwamonoBoxBasis *basis;           /* +0x80 */
    unsigned char unmodeled_84[0x2c];
    Vector4 dimensions;               /* +0xB0 */
    unsigned char unmodeled_c0[0xe0];
    UwamonoBoxCollisionState state;   /* +0x1A0 */
} UwamonoBoxUnit;

extern int HitCheckCorner(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit);

typedef struct UwamonoCrossState {
    unsigned char unmodeled_00[0x2d];
    signed char crossType;
} UwamonoCrossState;

typedef struct UwamonoCrossUnit {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    unsigned char unmodeled_20[0x180];
    UwamonoCrossState state;
} UwamonoCrossUnit;

typedef Vector4 UwamonoNyuruVector;

typedef Vector4 UwamonoBoxVector;

typedef struct UwamonoNyuruActor {
    unsigned char unmodeled_00[0x10];
    UwamonoNyuruVector position;
    unsigned char unmodeled_20[0x10];
    UwamonoNyuruVector displacement;
} UwamonoNyuruActor;

extern int NyuruCircle(UwamonoNyuruActor *actor, UwamonoCollisionUnit *unit);

extern int NyuruMatrix(UwamonoNyuruActor *actor, UwamonoCollisionUnit *unit, int mode);

extern int HitCheckMapUnit(void *actor);

/*
 * Still assembler: it only reads `from` and `to`, and reads the unit's
 * position and radius (+0x10/+0x18, +0xB0), so it takes the collision view.
 */
extern int CrossCheckMapUnitCircle(const Vector4 *from, const Vector4 *to,
                                   UwamonoCollisionUnit *unit);

extern void xglVectorNormal(Vector4 *destination, const Vector4 *source);


extern int AreaCheckBox(const float *position, UwamonoCollisionUnit *unit, float radius);

extern int CrossCheckMapUnitAt(Vector4 *from, Vector4 *to, UwamonoCrossUnit *unit);

typedef struct UwamonoCrossActor {
    unsigned int flags;
    unsigned char unmodeled_04[0x0c];
    Vector4 position;
    unsigned char unmodeled_20[0x60];
    unsigned char actorIndex;
    unsigned char unmodeled_81[5];
    short resourceId;
    unsigned char unmodeled_88[0x848];
    void *collisionObject;
    unsigned char unmodeled_8d4[0x114];
    float radius;
    unsigned char unmodeled_9ec[0x84];
} UwamonoCrossActor;

extern UwamonoCrossActor actor[64];

extern const float D_004D7F18;

extern int CheckCrossCircle(float radius, const Vector4 *from, const Vector4 *to, const Vector4 *center);

static float CheckCornerDist(const Vector4 *position, UwamonoCollisionUnit *unit);

typedef struct BrokenMapUnitState {
    unsigned char unmodeled_00[4];
    signed char signal;
    unsigned char unmodeled_05[0x3f];
    int brokenId;
} BrokenMapUnitState;

typedef struct BrokenMapUnit {
    unsigned int flags;
    unsigned char unmodeled_04[0xa0];
    short serial;
    unsigned char unmodeled_a6[0xfa];
    BrokenMapUnitState brokenState;
} BrokenMapUnit;

extern int CheckBrokenMapUnit(BrokenMapUnit *unit);

extern void xglSoundEffectPosID(int soundId, const float *position, int enabled, int channel);

/* Defined in src/main/ssd_1.c (main/tu ssd_1), already C. */

extern int HitCheckUwamonoAt(UwamonoBoxUnit *actor, UwamonoBoxUnit *unit);

typedef struct UwamonoScreenPoint {
    int x;
    int y;
    int z;
    int w;
} UwamonoScreenPoint;

typedef struct UwamonoSpritePacket {
    unsigned char unmodeled_00[0x30];
    UwamonoScreenPoint color0;
    UwamonoScreenPoint position0;
    UwamonoScreenPoint color1;
    UwamonoScreenPoint position1;
} UwamonoSpritePacket;

typedef struct UwamonoLinePacket {
    unsigned char unmodeled_00[0x20];
    UwamonoScreenPoint color0;
    UwamonoScreenPoint position0;
    UwamonoScreenPoint color1;
    UwamonoScreenPoint position1;
} UwamonoLinePacket;

typedef struct UwamonoVectorWords {
    long long lower;
    long long upper;
} UwamonoVectorWords;

typedef struct UwamonoEnemyLink {
    unsigned char unmodeled_00[0x4b];
    unsigned char unitId;
    unsigned char unmodeled_4c[0x3864];
} UwamonoEnemyLink;

typedef struct UwamonoEnemyUnit UwamonoEnemyUnit;

struct UwamonoEnemyUnit {
    unsigned int flags;
    void (*update)(UwamonoEnemyUnit *);
    unsigned char unmodeled_08[8];
    UwamonoAlignedVector position;
    unsigned char unmodeled_20[0x34];
    float heading;
    unsigned char unmodeled_58[0x28];
    unsigned char enemyIndex;
    unsigned char unmodeled_81[0x963];
    float actorHeading;
};

extern UwamonoEnemyLink enepc[16];

extern void ACT_updateEnemy(UwamonoEnemyUnit *unit);

typedef struct UwamonoWind {
    unsigned char mode;
    unsigned char unmodeled_01[7];
    long long unmodeled_08;
    Vector4 position;
    float decay;
    float strength;
    int timer;
    unsigned char unmodeled_2c[4];
} UwamonoWind;

typedef struct UwamonoWindUnit {
    long long unmodeled_00[2];
    Vector4 position;
    unsigned char unmodeled_20[0x94];
    float height;
} UwamonoWindUnit;

UwamonoWind uwaWind = { 0 };

extern void EXM_SetWindStruct(UwamonoWind *wind);

#define D_004D7EC0 0.3f

#define D_004D7EC4 0.1f

typedef struct UwamonoBrokenSoundState {
    unsigned char unmodeled_00[0x32];
    signed char material;
    unsigned char unmodeled_33;
    int soundId;
} UwamonoBrokenSoundState;

typedef struct UwamonoBrokenSoundUnit {
    unsigned char unmodeled_00[0xa0];
    unsigned char seChannel;
    unsigned char unmodeled_a1[0xff];
    UwamonoBrokenSoundState state;
} UwamonoBrokenSoundUnit;

extern void xglSoundEffectNormalID(int soundId, int channel);

#define D_004D7F44 0.1f

int HitCheckBoxUwamono(UwamonoBoxUnit *actor, UwamonoBoxUnit *unit);

#define D_004D7F1C 0.6f

extern void nmlModelSetPartsVisible(void *resource, int partsId, int visible);

extern float Get_Angle(const Vector4 *first, const Vector4 *second);

extern float CheckDist3D(const Vector4 *first, const Vector4 *second);

#define D_004D7EB4 0.05f

#define D_004D7EB8 0.05f

void UnlockMapUnit(void);

/*
 * The hit and cross checks read a MapUnit[] slot through the collision view
 * (position, radius/height, shape byte); callers holding the slot as a
 * UwamonoMapSlot or UwamonoMapUnit pass it in that view, the same way
 * CheckNearMapUnit hands its scan slot to CheckCornerDist.
 */
extern int HitCheckMapUnitAt(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit);

void CalcNearCrossPointCircle(const Vector4 *first, const Vector4 *second,
                              UwamonoCollisionUnit *unit, Vector4 *result);

void CalcNearCrossPointBox(Vector4 *first, Vector4 *second,
                           UwamonoMapUnit *unit, void *result);

extern int CrossCheckMapUnitBox(Vector4 *first, Vector4 *second,
                                UwamonoCrossUnit *unit);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitUwamonoSys);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", Unit_CreateUwamono);

void MAP_LoadUwamonoResource(UwamonoResourceUnit *unit, int resourceId)
{
    int index;
    int uwaresId;
    int xtxresId;

    printf((const char *) D_004CA028);
    if (resourceId != 0x7012 && resourceId >= 0x1000) {
        index = resourceId - 0x7001;
        uwaresId = uwares_tbl[index];
        xtxresId = xtxres_tbl[index];
        unit->xtxresId = xtxresId;
        unit->uwaresId = uwaresId;
    }
}

void GetPartsPos(GetPartsPosUnit *unit)
{
    MapPartsRoot *root;
    MapPartsResource *resource;
    GetPartsPosEntry *table;
    GetPartsPosEntry *entry;

    root = GameLoopState.partsRoot;
    resource = root->resource;
    table = (GetPartsPosEntry *) ((char *) resource + resource->arrayOffset);
    entry = &table[unit->serial];

    unit->entry = entry;
    unit->position.x = entry->x;
    unit->position.y = entry->y;
    unit->position.z = entry->z;
    unit->position.w = 1.0f;
    unit->rotationX = 0.0f;
    unit->rotationY = atan2f(entry->basisX, entry->basisZ);
    unit->rotationZ = 0.0f;
}

void SendBrokenSignal(UwamonoMapUnit *unit)
{
    unit->signal = 2;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", GetPartsSize);

void ClearUwamonoEffect(UwamonoEffectUnit *unit)
{
    int i;
    int effectId;
    struct UwamonoEffectTimers *timers = &unit->timers;

    for (i = 0; i < 3; i++) {
        effectId = unit->effectCf[i];
        if (effectId != 0) {
            sefDeleteEffectCf(effectId);
            unit->effectCf[i] = 0;
        }
    }
    if (timers->bgmTimer > 0) {
        xglSoundEffectStopID(timers->bgmTimer, unit->seChannel + 1);
        timers->bgmTimer = -1;
    }
}

signed char GetUwamonoSignal(UwamonoMapUnit *unit)
{
    return unit->signal;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitUwamono);

void InitMapParts(InitMapPartsUnit *unit)
{
    unit->uwaresId = 0;
    switch (unit->mapType) {
    case 2:
        InitMapKoware(unit);
        break;
    case 1:
        InitMapDoor(unit);
        break;
    case 20:
        InitDrill(unit);
        break;
    default:
        printf(D_004CA0D0, unit->mapType);
        unit->update = 0;
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitMapKoware);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitSaveSymbol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitShopSymbol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitEvsSymbol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", InitRetSymbol);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", SetHideObject);

void GameStateRestoreKoware(void)
{
    UwamonoGameLoopState *state;
    int index;
    index = 0;
    while (index < 64 && GameLoopState.shootUnit[index] != -1) {
        state = &GameLoopState;
        nmlModelSetPartsVisible(state->partsRoot->resource, state->shootUnit[index], 0);
        ++index;
    }
}

void SetMapUwamono(void)
{
    UwamonoGameLoopState *state = &GameLoopState;
    UwamonoMapSlot *unit = (UwamonoMapSlot *) MapUnit;
    short *shootUnit = state->shootUnit;
    int remaining = 63;

    do {
        *shootUnit++ = -1;
        if (unit->flags & 0x10000) {
            if (unit->update)
                unit->update(unit);
        }
        --remaining;
        ++unit;
    } while (remaining >= 0);
    UnlockMapUnit();
}

void ResetShootSys(void)
{
    UwamonoGameLoopState *state = &GameLoopState;
    short emptyShootUnitId = -1;
    int index = 0x3f;
    short *shootUnit = &state->shootUnit[0x3f];

    do {
        index--;
        *shootUnit = emptyShootUnitId;
        shootUnit--;
    } while (index >= 0);

    UnlockMapUnit();
}

UwamonoCreatedUnit *CreateUwamono(float x, float y, float z, float rotationY, int resourceId)
{
    UwamonoCreatedUnit *unit = MAP_createUnit(-1, resourceId);
    UwamonoCreatedState *state;
    int inactiveValue;

    if (unit == 0) {
        printf(D_004CA1B0);
        return 0;
    }
    MAP_loadUnitResource(unit, resourceId);
    state = &unit->state;
    memset(state, 0, sizeof(*state));
    inactiveValue = -1;
    state->height = 3.0f;
    state->heightCheckFlag = 1;
    state->signal = inactiveValue;
    state->state = inactiveValue;
    state->target = inactiveValue;
    state->timer = 1;
    unit->position.x = x;
    unit->position.y = y;
    unit->position.z = z;
    unit->position.w = 1.0f;
    unit->rotationY = rotationY;
    unit->rotationX = 0.0f;
    unit->rotationZ = 0.0f;
    InitMapTrap(unit);
    return unit;
}

UwamonoKowareUnit *CreateKowaremono(short resourceId, short kindIndex)
{
    UwamonoKowareUnit *unit = (UwamonoKowareUnit *)MAP_createUnit(-1, resourceId);
    UwamonoMapUnitState *state;
    if (unit == 0) {
        printf(D_004CA1D0);
        return 0;
    }
    GetPartsPos((GetPartsPosUnit *)unit);
    state = &unit->state;
    if (state->state == -1) {
        state->state = KowareKind_t[kindIndex].shape;
    }
    unit->dimensions = KowareKind_t[kindIndex].dimensions;
    if (state->target == -1) {
        state->target = KowareKind_t[kindIndex].target;
    }
    unit->flags = 0;
    CreateUwamonoCommon((UwamonoMapUnit *)unit);
    unit->updateFunction = MAP_updateUnitKoware;
    return unit;
}

void CreateUwamonoCommon(UwamonoMapUnit *unit)
{
    unit->actionNo = 0;
    unit->flags |= 0x10000;
    unit->sequenceNo = 0;
    unit->sequenceSub = 0;
    unit->actionSub = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitKoware);

void MAP_updateUnitEnemy(UwamonoEnemyUnit *unit)
{
    unsigned char linkedUnitId = enepc[unit->enemyIndex].unitId;
    UwamonoGameLoopState *state;

    if (!MapUnit[linkedUnitId].actionSub) {
        unit->flags |= 8;
    } else {
        unit->position = MapUnit[linkedUnitId].position;
        unit->flags &= ~8;
        unit->update = ACT_updateEnemy;
        state = &GameLoopState;
        state->enemyUpdateState = 0;
        unit->actorHeading = (unit->heading = Get_Angle(&unit->position.value, &state->actor->position));
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", MAP_updateUnitHide);

void MAP_updateUnitSaveSymbol(UwamonoMapUnit *unit)
{
    unit->symbolPhase += D_004D7EB4;
    if (CheckDist3D(&GameLoopState.actor->position, &unit->position) < 0.0f)
        return;
}

void MAP_updateUnitShopSymbol(UwamonoMapUnit *unit)
{
    unit->symbolPhase += D_004D7EB8;
    if (CheckDist3D(&GameLoopState.actor->position, &unit->position) < 0.0f)
        return;
}

void MAP_updateUnitSymbol(UwamonoMapUnit *unit)
{
    unit->symbolPhase = unit->symbolPhase + UWAMONO_SYMBOL_PHASE_STEP;
}

void SetUwaWind(UwamonoWindUnit *unit)
{
    UwamonoWind *wind = &uwaWind;
    float heightDelta;

    wind->mode = 2;
    __builtin_memcpy((UwamonoVectorWords *) &wind->position,
                     (const UwamonoVectorWords *) &unit->position, sizeof(Vector4));
    heightDelta = unit->height * 0.5f;
    wind->position.w = D_004D7EC0;
    wind->decay = D_004D7EC4;
    wind->strength = 1.0f;
    wind->timer = 0;
    wind->position.y += heightDelta;
    EXM_SetWindStruct(wind);
}

void UwamonoBrokenSe(UwamonoBrokenSoundUnit *unit)
{
    UwamonoBrokenSoundState *state = &unit->state;
    int soundId = state->soundId;
    int channel = unit->seChannel + 1;

    if (soundId != -1) {
        xglSoundEffectNormalID(soundId, channel);
        return;
    }
    switch (state->material) {
    case 0:
        xglSoundEffectNormalID(0x10003, channel);
        break;
    case 1:
        xglSoundEffectNormalID(0x10006, channel);
        break;
    case 2:
        xglSoundEffectNormalID(0x10007, channel);
        break;
    default:
        xglSoundEffectNormalID(0x10003, channel);
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", DrawActiveCursol);

int HitCheckMapUnit(void *actor)
{
    int index = 0;

    do {
        struct UwamonoMapCollisionState *state = &MapUnit[index].state;
        UwamonoMapSlot *unit = &MapUnit[index];
        if (unit->serial != -1) {
            unsigned int flags = unit->flags;
            if (!(flags & 0x100000) && state->crossType &&
                (flags & 0x10000) &&
                HitCheckMapUnitAt(actor, (UwamonoCollisionUnit *)unit)) {
                return index;
            }
        }
        ++index;
    } while (index < 64);
    return -1;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckMapUnitWithNyuru);

int HitCheckMapUnitReverse(void *actor)
{
    int index = 64;

    do {
        struct UwamonoMapCollisionState *state = &MapUnit[index].state;
        UwamonoMapSlot *unit = &MapUnit[index];
        if (unit->serial != -1) {
            unsigned int flags = unit->flags;
            if (!(flags & 0x100000) && state->crossType &&
                (flags & 0x10000) &&
                HitCheckMapUnitAt(actor, (UwamonoCollisionUnit *)unit))
                return index;
        }
        --index;
    } while (index > 0);
    return -1;
}

void HitCheckMapUnitPos(int unit) {
    HitCheckMapUnitPosSize(unit, 0.0f);
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckMapUnitPosSize);

int HitCheckMapUnitPosReverse(Vector4 *position)
{
    UwamonoCollisionActor probe;
    int index = 64;

    probe.position = *position;
    probe.radius = 0.0f;
    do {
        struct UwamonoMapCollisionState *state = &MapUnit[index].state;
        UwamonoMapSlot *unit = &MapUnit[index];
        if (unit->serial != -1 && state->crossType &&
            (unit->flags & 0x10000) &&
            HitCheckMapUnitAt(&probe, (UwamonoCollisionUnit *)unit))
            return index;
        --index;
    } while (index > 0);
    return -1;
}

int HitCheckMapUnitAt(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit)
{
    UwamonoMapUnitState *state;

    if (unit->position.y + unit->height * D_004D7ED0 < actor->position.y) {
        return 0;
    }

    state = &unit->state;
    if (actor->position.y < unit->position.y - D_004D7ED4) {
        return 0;
    }

    switch (state->state) {
    case 1: {
        float distance = CheckDist2D(&actor->position, &unit->position);
        float collisionRadius = actor->radius + unit->radius;
        if (!(distance < collisionRadius)) {
            return 0;
        }
        return 1;
    }
    case 2:
        return HitCheckBox(actor, (struct UwamonoBoxUnit *)unit);
    }
    return 0;
}

unsigned int HitCheckMapUnitPosAt(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit)
{
    UwamonoMapUnitState *state = &unit->state;
    float unitY = unit->position.y;
    float actorY = actor->position.y;
    signed char shape;

    if (unitY + unit->height * D_004D7ED8 < actorY) {
        return 0;
    }
    if (actorY < unitY - D_004D7EDC) {
        return 0;
    }
    shape = state->state;
    switch (shape) {
    case 1:
        if (CheckDist2D(&actor->position, &unit->position) <
            actor->radius + unit->radius) {
            return 1;
        }
        return 0;
    case 2:
        return HitCheckBoxCorner(actor, unit);
    default:
        return 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckBox);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckBoxCorner);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckCorner);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckBoxTest);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckNyuru);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", NyuruCircle);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", NyuruMatrix);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", NyuruBox);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", NyuruCorner);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", ShootMapUnit);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossPointUwamono);

void CrossPointUwamonoAt(Vector4 *first, Vector4 *second, UwamonoMapUnit *unit, void *result)
{
    switch (unit->crossType) {
    case 1:
        CalcNearCrossPointCircle(first, second, (UwamonoCollisionUnit *)unit, result);
        return;
    case 2:
        CalcNearCrossPointBox(first, second, unit, result);
        break;
    }
}

void CalcNearCrossPointCircle(const Vector4 *from, const Vector4 *to,
                              UwamonoCollisionUnit *unit, Vector4 *out)
{
    Vector4 delta;
    Vector4 direction;
    float distance;
    if (CrossCheckMapUnitCircle(from, to, unit)) {
        delta = *from;
        delta.y = unit->position.y;
        __asm__ __volatile__(
            "lqc2 vf3,0(%0)\n\t"
            "lqc2 vf2,0(%1)\n\t"
            "vsub.xyz vf2xyz,vf2xyz,vf3xyz\n\t"
            "sqc2 vf2,0(%2)"
            :
            : "r"(from), "r"(&unit->position), "r"(&delta)
            : "memory");
        xglVectorLength(&distance, &delta);
        xglVectorNormal(&direction, &delta);
        distance -= unit->radius;
        if (distance < 0.0f) {
            *out = *from;
        } else {
            out->x = from->x + distance * direction.x;
            out->y = from->y;
            out->z = from->z + distance * direction.z;
        }
    } else {
        out->y = -1000.0f;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CalcNearCrossPointBox);

void CheckNearPoint(const Vector4 *origin, const Vector4 *candA, const Vector4 *candB, Vector4 *out)
{
    Vector4 delta;
    float lenA;
    float lenB;

    __asm__ __volatile__(
        "lqc2 vf3,0(%1)\n\t"
        "lqc2 vf2,0(%0)\n\t"
        "vsub.xyz vf2xyz,vf2xyz,vf3xyz\n\t"
        "sqc2 vf2,0(%2)"
        :
        : "r"(origin), "r"(candA), "r"(&delta)
        : "memory"
    );
    xglVectorLength(&lenA, &delta);

    __asm__ __volatile__(
        "lqc2 vf3,0(%1)\n\t"
        "lqc2 vf2,0(%0)\n\t"
        "vsub.xyz vf2xyz,vf2xyz,vf3xyz\n\t"
        "sqc2 vf2,0(%2)"
        :
        : "r"(origin), "r"(candB), "r"(&delta)
        : "memory"
    );
    xglVectorLength(&lenB, &delta);

    if (lenA < lenB)
        *out = *candA;
    else
        *out = *candB;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", UwamonoAreaCheck);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", AreaCheckBox);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossCheckMapUnitAim);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossCheckMapUnit);

int CrossCheckActor(UwamonoCrossActor *source, const Vector4 *to)
{
    int index;
    for (index = 0; index < 64; ++index) {
        if (!(D_004D7F18 < __builtin_fabsf(source->position.y - actor[index].position.y)) &&
            actor[index].resourceId >= 0 && !(actor[index].flags & 8) &&
            actor[index].collisionObject != 0 && index != source->actorIndex &&
            CheckCrossCircle(actor[index].radius, &source->position, to, &actor[index].position)) {
            return index;
        }
    }
    return -1;
}

int CrossCheckMapUnitAt(Vector4 *first, Vector4 *second, UwamonoCrossUnit *unit)
{
    UwamonoCrossState *state = &unit->state;

    if (D_004D7F1C < __builtin_fabsf(first->y - unit->position.y))
        return 0;
    switch (state->crossType) {
    case 1:
        return CrossCheckMapUnitCircle(first, second, (UwamonoCollisionUnit *)unit);
    case 2:
        return CrossCheckMapUnitBox(first, second, unit);
    default:
        return 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossCheckMapUnitCircle);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CrossCheckMapUnitBox);

void UnlockMapUnit(void)
{
    int unitIndex = 63;
    int *effectCf = &MapUnit[0].effectCf[0];

    do {
        MapUnit[63 - unitIndex].flags &= ~0x40000u;
        if (effectCf[1] != 0) {
            sefDeleteEffectCf(effectCf[1]);
            effectCf[1] = 0;
        }
        if (effectCf[2] != 0) {
            sefDeleteEffectCf(effectCf[2]);
            effectCf[2] = 0;
        }
        unitIndex--;
        if (unitIndex >= 0)
            effectCf = &MapUnit[63 - unitIndex].effectCf[0];
    } while (unitIndex >= 0);

    GameLoopState.aimMapUnitIndex = -1;
}

int CheckNearMapUnit(void)
{
    UwamonoScanUnit *unit = (UwamonoScanUnit *)MapUnit;
    struct UwamonoSoundActor *actor = GameLoopState.actor;
    float nearest = GameLoopState.selectionDistance + 1000.0f;
    int selected = -1;
    int index = 0;

    do {
        if (CheckBrokenMapUnit((BrokenMapUnit *)unit) & 1) {
            if (unit->flags & 0x40000) {
                float distance = CheckCornerDist(&actor->position, (UwamonoCollisionUnit *)unit);
                if (distance < nearest) {
                    nearest = distance;
                    selected = index;
                }
            }
        }
        ++index;
        ++unit;
    } while (index < 64);
    if (nearest < GameLoopState.selectionDistance) {
        return selected;
    }
    return -1;
}

int CheckBrokenMapUnit(BrokenMapUnit *unit)
{
    BrokenMapUnitState *state = &unit->brokenState;
    int flags;
    int brokenId;

    if (unit->serial == -1)
        return 0;
    flags = unit->flags;
    if (!(flags & 0x10000))
        return 0;
    if (flags & 4)
        return 0;
    if (state->signal == 1)
        return 0;
    brokenId = state->brokenId;
    return brokenId == -1 ? 0 : brokenId;
}

int AimHeightCheck(UwamonoMapUnit *unit)
{
    UwamonoGameLoopState *state = &GameLoopState;
    AimHeightReference *heightReference = state->actor;
    if (!(state->aimHeightFlags & 2))
        return 1;
    return __builtin_fabsf(heightReference->position.y - unit->position.y) < 0.3f;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", AimMapUnit);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", AimMapUnitLookCheck);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CheckCornerDist);

void UwamonoCommonFunc(UwamonoCommonUnit *unit)
{
    UwamonoTimers *timers = &unit->timers;

    if (unit->serial < 0x1000) {
        MAP_updateUnitPartsSequence(unit);
    } else {
        MAP_updateUnitSequence(unit);
    }

    if (timers->heightCheckFlag != 0) {
        CheckUwamonoHeight(unit);
    }

    unit->linkedUnit->position.x = unit->position.value.x;
    unit->linkedUnit->position.y = unit->position.value.y;
    unit->linkedUnit->position.z = unit->position.value.z;

    if (timers->bgmTimer > 0) {
        UwamonoBgmFunc(unit);
    }
}

void UwamonoBgmFunc(UwamonoCommonUnit *unit)
{
    UwamonoTimers *timers = &unit->timers;
    UwamonoAlignedVector soundPosition;

    if (timers->projectSound == 0) {
        soundPosition = unit->position;
    } else {
        float x = unit->position.value.x;
        float y = unit->position.value.y;
        float z = unit->position.value.z;
        UwamonoLinkedUnit *linkedUnit = unit->linkedUnit;
        float projection =
            (GameLoopState.actor->position.x - x) * linkedUnit->direction[0] +
            (GameLoopState.actor->position.y - y) * linkedUnit->direction[1] +
            (GameLoopState.actor->position.z - z) * linkedUnit->direction[2];

        soundPosition.value.x = x + projection * linkedUnit->direction[0];
        soundPosition.value.y = y + projection * linkedUnit->direction[1];
        soundPosition.value.z = z + projection * linkedUnit->direction[2];
        soundPosition.value.w = 1.0f;
    }
    xglSoundEffectPosID(timers->bgmTimer, &soundPosition.value.x, 1, unit->seChannel + 1);
}

void UwamonoBgmFadeOut(void)
{
    UwamonoBgmFadeOutUnit *unit;
    int i;
    int bgmTimer;
    int bank;
    unsigned short handle;

    unit = (UwamonoBgmFadeOutUnit *) MapUnit;
    for (i = 0; i < 0x40; i++, unit++) {
        if (!(unit->flags & 0x10000)) {
            continue;
        }
        bgmTimer = unit->bgmTimer;
        if (bgmTimer < 0) {
            continue;
        }
        if (bgmTimer > 0xFFFFF) {
            continue;
        }
        bank = bgmTimer >> 16;
        handle = SoundWork.effectBanks[bank].handle;
        if (handle == 0xFFFF) {
            continue;
        }
        SsdFadeoutEffect((handle << 16) + (bgmTimer & 0xFFFF), 0x12C, unit->seChannel + 1);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", CheckUwamonoHeight);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckUwamono);

int HitCheckUwamonoAt(UwamonoBoxUnit *actor, UwamonoBoxUnit *unit)
{
    UwamonoBoxCollisionState *state = &unit->state;
    float actorHeight = actor->position.y;
    float baseHeight = unit->position.y;

    if (!(actorHeight < baseHeight) && !(baseHeight + unit->dimensions.y < actorHeight)) {
        switch (state->crossType) {
        case 1:
            if (CheckDist2D(&actor->position, &unit->position) < unit->dimensions.x + D_004D7F44)
                return 1;
            return 0;
        case 2:
            return HitCheckBoxUwamono(actor, unit);
        default:
            return 0;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", HitCheckBoxUwamono);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", DrawRadar);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", DrawSprite);

INCLUDE_ASM("asm/main/nonmatchings/init_uwamono_sys", SortLine);
