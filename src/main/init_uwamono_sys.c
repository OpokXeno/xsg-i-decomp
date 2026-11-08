#include "common.h"
#include "main/xgl_dma.h"
#include "main/xgl_2.h"
#include "main/sef.h"
#include "main/enemy_1.h"
#include "main/map_2.h"
#include "main/exm_2.h"
#include "shared.h"
#include "main/init_uwamono_sys.h"
#include "main/Undulate.h"
#include "main/act_1.h"
#include "main/act_2.h"
#include "main/get.h"
#include "main/karakama_test.h"
#include "main/outer_product.h"
#include "main/pause_menu_page_uwamono.h"
#include "main/vibration.h"
#include "main/xgl_font.h"
#include "vu0_macro.h"

#define UWAMONO_SYMBOL_PHASE_STEP 0.05f
#define WIND_POSITION_W 0.3f
#define WIND_DECAY 0.1f
#define UWAMONO_HIT_RADIUS_PADDING 0.1f
#define CROSS_CHECK_MAP_UNIT_HEIGHT 0.6f
#define SAVE_SYMBOL_PHASE_STEP 0.05f
#define SHOP_SYMBOL_PHASE_STEP 0.05f

/* uwamono_dat capacity (resident muobj models and textures) and the karakama tables read at start-up. */
#define UWAMONO_DAT_SIZE 0x50000
#define ITEM_BOX_TABLE_SIZE 0x12C8
#define EVT_ITEM_TABLE_SIZE 0x7F80

/* Actors collide against a unit at 1.5 times their radius. */
#define UWAMONO_RADIUS_SCALE 1.5f
#define UWAMONO_BOX_HIT_MARGIN 0.15f
#define UWAMONO_AIM_HEIGHT_RANGE 0.3f
#define UWAMONO_CROSS_HEIGHT_RANGE 0.3f
#define UWAMONO_BOUNCE_DAMPING 0.25f

/* Segment/unit crossing results: no crossing, and the "far away" start of the nearest search. */
#define CROSS_POINT_NONE (-1000.0f)
#define CROSS_POINT_FAR 65535.0f

/* Actor.flags: the actor is hidden (inactive). */
#define UWAMONO_ACTOR_HIDDEN 0x8


/* A breakable part is hidden and flagged broken on its third frame after the hit. */
#define UWAMONO_KOWARE_HIDE_COUNT 3

/* Sound effect ids (xglSoundEffectNormalID) and the effect-resource ids (sefCreateEffectCf) used here. */
enum {
    UWAMONO_SE_CURSOR_MOVE = 3,
    UWAMONO_SE_AIM_LOCK = 0x10001,
    UWAMONO_SE_SHOOT = 0x10002,
    UWAMONO_SE_BREAK_MATERIAL0 = 0x10003,   /* also the fallback */
    UWAMONO_SE_BREAK_MATERIAL1 = 0x10006,
    UWAMONO_SE_BREAK_MATERIAL2 = 0x10007
};

enum {
    UWAMONO_EFFECT_SHOT = 0x2de,
    UWAMONO_EFFECT_CURSOR = 0x2e2,          /* aim cursor on a unit that is not the current target */
    UWAMONO_EFFECT_CURSOR_AIMED = 0x2e3     /* aim cursor on the current target */
};

/* GameLoopState.aimHeightFlags */
enum {
    UWAMONO_AIM_HEIGHT_CHECK = 0x2,         /* require the unit within UWAMONO_AIM_HEIGHT_RANGE of the actor's height */
    UWAMONO_AIM_LOOK_GROUND = 0x4,          /* test the line of sight against the ground (undulation) */
    UWAMONO_AIM_LOOK_UNITS = 0x8            /* test the line of sight against other map units */
};

/* GameLoopState.systemFlags while the shoot motion plays. */
enum {
    UWAMONO_SYSTEM_SHOOT_MOTION = 0x4000,
    UWAMONO_SYSTEM_SHOOT_BUSY = 0x8000      /* blocks AimMapUnit */
};
#define UWAMONO_SHOOT_MOTION 14

/* Sound id layout: bank in bits 16..19, entry in the low 16 bits; 0xFFFF marks an unloaded bank. */
#define SOUND_ID_MAX 0xFFFFF
#define SOUND_ID_ENTRY_MASK 0xFFFF
#define SOUND_BANK_EMPTY 0xFFFF
#define UWAMONO_BGM_FADE_FRAMES 0x12C

/* DrawRadar: dot centre and range in GS fixed point, dot half size, radar units per world unit. */
#define RADAR_CENTER_X 0x8B00
#define RADAR_CENTER_Y 0x8900
#define RADAR_DEPTH 0xFFFFFF
#define RADAR_DOT_HALF_SIZE 0x20
#define RADAR_RANGE 0x3E1
#define RADAR_SCALE 64.0f

typedef struct UwamonoScreenPoint {
    int x;
    int y;
    int z;
    int w;
} UwamonoScreenPoint;

typedef union UwamonoAlignedScreenPoint {
    UwamonoScreenPoint value;
    int coordinates[4];
    long long words[2];
} UwamonoAlignedScreenPoint;

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

/*
 * The unit's 4x4 world matrix (MapUnitRecord.basis) read as rows: x axis,
 * y axis, z axis and anchor (translation); the checks copy it whole.
 */
typedef struct UwamonoCrossBasis {
    union {
        struct {
            float xAxisX;
            unsigned char unmodeled_04[4];
            float xAxisZ;
            unsigned char unmodeled_0c[0x14];
            float zAxisX;
            unsigned char unmodeled_24[4];
            float zAxisZ;
            unsigned char unmodeled_2c[4];
            float anchor[4];
        };
        float rows[4][4]; /* the same matrix as MapUnitRecord.basis rows */
    };
} UwamonoCrossBasis;

/*
 * GetPartsPos's own view of the game-loop record: only the map-parts resource
 * root pointer at +0x54 is read here.
 */
typedef struct MapPartsResource {
    unsigned char unmodeled_00[0x40];
    int partCount;                /* +0x40 */
    unsigned char unmodeled_44[0x0c];
    int arrayOffset;              /* +0x50, added to this pointer to form the entry array base */
} MapPartsResource;

typedef struct MapPartsRoot {
    unsigned char unmodeled_00[4];
    struct MapPartsResource *resource;   /* +0x04 */
} MapPartsRoot;

typedef struct UwamonoGameLoopState {
    unsigned char unmodeled_00[4];
    Actor *actor; /* +0x04 */
    unsigned char unmodeled_08[8];
    unsigned int systemFlags;             /* +0x10 */
    unsigned char unmodeled_14[8];
    int enemyUpdateState;             /* +0x1C */
    unsigned int aimHeightFlags;      /* +0x20 */
    unsigned char unmodeled_24[0x30];
    struct MapPartsRoot *partsRoot;          /* +0x54 */
    unsigned char unmodeled_58[0x8c];
    short shootUnit[64];              /* +0xE4 */
    unsigned char unmodeled_164[0x29ddd];
    signed char aimMapUnitIndex;
    unsigned char unmodeled_29f42[2];
    float selectionDistance;      /* +0x29F41 */
} UwamonoGameLoopState;

/*
 * ClearUwamonoEffect's view of a MapUnit[] record: only the fields it reads
 * or writes are named here.
 */
typedef struct UwamonoEffectUnit {
    unsigned char unmodeled_00[0xa0];
    /* Read once, plus one, as the channel argument to
     * xglSoundEffectStopID; no other role is evidenced here. */
    unsigned char seChannel;           /* +0xA0 */
    unsigned char unmodeled_a1[0x47];
    /* Walked in order, deleting and zeroing each non-zero effect handle
     * with sefDeleteEffectCf. */
    int effectCf[3];                   /* +0xE8 */
    unsigned char unmodeled_f4[0xac];
    /*
     * The sub-pointer is computed once, ahead of the effect loop, and
     * stays live across the whole function; UwamonoTimers below names the
     * same +0x1A0 block for UwamonoCommonFunc's view.
     */
    struct UwamonoEffectTimers {
        unsigned char unmodeled_00[0x48];
        int bgmTimer;                   /* +0x48 (record +0x1E8) */
    } timers;                           /* +0x1A0 */
} UwamonoEffectUnit;

typedef struct InitMapPartsUnit {
    unsigned int flags;
    void (*update)(struct InitMapPartsUnit *);
    unsigned char unmodeled_08[0x98];
    unsigned char hideMode;          /* +0xa0 */
    unsigned char unmodeled_a1[3];
    short serial;                    /* +0xa4 */
    unsigned char unmodeled_a6[0x3a];
    int uwaresId;
    unsigned char unmodeled_e4[0xbc];
    short mapType;
} InitMapPartsUnit;

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
    signed char heightBounce;    /* +0x2F (record +0x1CF) */
    unsigned char unmodeled_30[0x18];
    int bgmTimer;                /* +0x48 (record +0x1E8) */
} UwamonoTimers;

typedef struct UwamonoCommonUnit {
    unsigned int flags;
    unsigned char unmodeled_04[0x0c];
    AlignedVector4 position;                 /* +0x10 */
    float rotationX;
    float rotationY;
    float rotationZ;
    unsigned char unmodeled_2c[0x54];
    struct UwamonoLinkedUnit *linkedUnit;     /* +0x80 */
    unsigned char unmodeled_84[0x1c];
    unsigned char seChannel;
    unsigned char unmodeled_a1[3];
    short serial;                     /* +0xA4 */
    unsigned char unmodeled_a6[0xfa];
    UwamonoTimers timers;              /* +0x1A0 */
    unsigned char unmodeled_1ec[0x38];
    float heightVelocity; /* +0x224 */
} UwamonoCommonUnit;


typedef struct UwamonoMapSlot UwamonoMapSlot;

typedef struct UwamonoBoxBasis {
    float x;
    unsigned char unmodeled_04[4];
    float z;
} UwamonoBoxBasis;

struct UwamonoMapSlot {
    unsigned int flags;
    void (*update)(void *);
    void (*draw)(void *);
    unsigned char unmodeled_0c[4];
    AlignedVector4 position;
    float rotationX, rotationY, rotationZ;
    unsigned char unmodeled_2c[0x14];
    Matrix4 localMatrix;
    union {
        struct UwamonoBoxBasis *basis;
        Matrix4 *matrixView;
    };
    unsigned char unmodeled_84[0x1e];
    unsigned char actionSub;
    unsigned char linkedUnitIndex;
    short serial;
    unsigned char unmodeled_a6[0x0a];
    AlignedVector4 dimensions;
    unsigned char unmodeled_c0[0x28];
    int effectCf[3];
    unsigned char unmodeled_f4[0xac];
    struct UwamonoMapCollisionState {
        short kindIndex;
        unsigned char unmodeled_02[2];
        signed char signal;
        unsigned char unmodeled_05[0x28];
        signed char crossType;
        signed char hideOverride;
        unsigned char unmodeled_2f[0x131];
    } state;
};

typedef struct UwamonoSoundWorkView {
    unsigned char unmodeled_00[0x20];
    struct {
        unsigned short handle;        /* +0x20 + bank * 4 */
        unsigned char unmodeled_02[2];
    } effectBanks[16];
} UwamonoSoundWorkView;

typedef struct UwamonoKowareKind {
    signed char shape;
    unsigned char unmodeled_01[0xf];
    DoublewordAlignedVector4 dimensions; /* +0x10 */
    signed char target;
    unsigned char unmodeled_21[3];
    int kindParameter;                 /* +0x24 */
    unsigned long long unmodeled_28;
} UwamonoKowareKind;

typedef struct UwamonoKowareUnit {
    unsigned int flags;
    void (*updateFunction)(void *unit);
    unsigned char unmodeled_08[8];
    Vector4 position;                  /* +0x10 */
    Vector4 rotation;                  /* +0x20 */
    unsigned char unmodeled_30[0x70];
    unsigned char sequenceIndex;       /* +0xA0 */
    unsigned char actionNo;            /* +0xA1 */
    unsigned char actionSub;           /* +0xA2 */
    unsigned char unmodeled_a3;
    short serial;                      /* +0xA4 */
    unsigned char unmodeled_a6[2];
    short breakCount;                  /* +0xA8 */
    unsigned char unmodeled_aa[6];
    DoublewordAlignedVector4 dimensions;
    unsigned char unmodeled_c0[0xe0];
    UwamonoMapUnitState state;         /* +0x1a0 */
} UwamonoKowareUnit;

/*
 * The MapUnit[] vectors the box checks copy are DoublewordAlignedVector4: copied one
 * 64-bit word at a time, high word first (ld/sd pairs at +8 then +0).
 */
typedef struct UwamonoCollisionUnit {
    unsigned char unmodeled_00[0x10];
    DoublewordVector4 position; /* copied word by word (words[1] first) in HitCheckBoxCorner */
    float rotationX;                  /* +0x20 */
    float rotationY;                  /* +0x24 */
    float rotationZ;                  /* +0x28 */
    unsigned char unmodeled_2c[0x14];
    Matrix4 matrix;                   /* +0x40 */
    struct UwamonoCrossBasis *basis;
    unsigned char unmodeled_84[0x2c];
    AlignedVector4 dimensions;
    unsigned char unmodeled_c0[0xe0];
    UwamonoMapUnitState state;
    unsigned char unmodeled_1e8[0x118];
} UwamonoCollisionUnit;

typedef struct UwamonoAimQuery {
    unsigned char unmodeled_00[8];
    unsigned short attributeMask;
    unsigned char unmodeled_0a[0x0e];
    void *header;
    unsigned char unmodeled_1c[0x24];
} UwamonoAimQuery;

typedef struct UwamonoCollisionActor {
    unsigned char unmodeled_00[0x10];
    AlignedVector4 position;
    unsigned char unmodeled_20[0x10];
    AlignedVector4 displacement;       /* +0x30 */
    unsigned char unmodeled_40[0x488];
    UwamonoAimQuery undulation; /* +0x4c8 */
    unsigned char unmodeled_508[0x4e0];
    float radius;
    unsigned char unmodeled_9ec[0x84];
} UwamonoCollisionActor;

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

typedef struct UwamonoBoxCollisionState {
    unsigned char unmodeled_00[0x2d];
    signed char crossType;
} UwamonoBoxCollisionState;

typedef struct UwamonoBoxUnit {
    unsigned char unmodeled_00[0x10];
    Vector4 position;                 /* +0x10 */
    unsigned char unmodeled_20[0x60];
    struct UwamonoBoxBasis *basis;           /* +0x80 */
    unsigned char unmodeled_84[0x2c];
    Vector4 dimensions;               /* +0xB0 */
    unsigned char unmodeled_c0[0xe0];
    UwamonoBoxCollisionState state;   /* +0x1A0 */
} UwamonoBoxUnit;

struct UwamonoBoxUnit;

typedef struct UwamonoCrossState {
    unsigned char unmodeled_00[0x2d];
    signed char crossType;
} UwamonoCrossState;

typedef struct UwamonoCrossUnit {
    unsigned char unmodeled_00[0x10];
    AlignedVector4 position;
    unsigned char unmodeled_20[0x60];
    struct UwamonoCrossBasis *basis;
    unsigned char unmodeled_84[0x2c];
    DoublewordVector4 dimensions; /* copied word by word (words[1] first) in CrossCheckMapUnitBox */
    unsigned char unmodeled_c0[0xe0];
    UwamonoCrossState state;
} UwamonoCrossUnit;

typedef Vector4 UwamonoBoxVector;

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

typedef struct UwamonoVectorWords {
    long long lower;
    long long upper;
} UwamonoVectorWords;

typedef struct UwamonoEnemyLink {
    unsigned char unmodeled_00[0x40];
    union {
        unsigned char linkState[12];
        struct {
            unsigned char unmodeled_link_40[8];
            unsigned char unmodeled_48[3];
            unsigned char unitId; /* +0x4b */
        };
    };
    unsigned char unmodeled_4c[0x3864];
} UwamonoEnemyLink;

typedef struct UwamonoEnemyUnit UwamonoEnemyUnit;

struct UwamonoEnemyUnit {
    unsigned int flags;
    void (*update)(UwamonoEnemyUnit *);
    unsigned char unmodeled_08[8];
    AlignedVector4 position;
    unsigned char unmodeled_20[0x30];
    float rotationX;                   /* +0x50 */
    float heading;                     /* +0x54 */
    float rotationZ;                   /* +0x58 */
    unsigned char unmodeled_5c[0x24];
    unsigned char actorIndex;          /* +0x80: this actor's actor[] index */
    unsigned char unmodeled_81[5];
    short resourceId;                  /* +0x86: negative for a free actor */
    unsigned char unmodeled_88[0x848];
    void *collisionObject;             /* +0x8d0 */
    unsigned char unmodeled_8d4[0x110];
    float actorHeading;                /* +0x9e4 */
    float radius;                      /* +0x9e8 */
    unsigned char unmodeled_9ec[0x84];
};

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

typedef struct UwamonoSequenceRecord {
    int initialValues00[2];
    unsigned char unmodeled_08[4];
    int initialValues0c[10];
    unsigned char unmodeled_34[0x20c];
    int initialValues240[3];
    float scaleX;
    float scaleY;
    int initialValues254[2];
    float scaleZ;
} UwamonoSequenceRecord;

typedef struct UwamonoKowareSizeUnit {
    unsigned int flags;
    void (*updateFunction)(void *unit);
    unsigned char unmodeled_08[8];
    Vector4 position;
    float rotationX;
    float rotationY;
    float rotationZ;
    unsigned char unmodeled_2c[0x74];
    unsigned char hideMode;
    unsigned char unmodeled_a1[3];
    short serial;
    unsigned char unmodeled_a6[0x0a];
    DoublewordAlignedVector4 dimensions;
    unsigned char unmodeled_c0[0xe0];
    UwamonoMapUnitState state;
} UwamonoKowareSizeUnit;

typedef struct UwamonoProbeParameters {
    unsigned char unmodeled_00[8];
    unsigned short flags;              /* +0x08 */
    unsigned char unmodeled_0a[0x0e];
    void *dataHeader;                   /* +0x18 */
    unsigned char unmodeled_1c[0x24];
} UwamonoProbeParameters;

typedef struct KowarePartsSizeState {
    unsigned char unmodeled_00[2];
    short kindIndex;
    unsigned char unmodeled_04[8];
    float baseSizeX;
    float baseSizeY;
    float baseSizeZ;
    float sizeOffsetX;
    float sizeOffsetY;
    float sizeOffsetZ;
    unsigned char unmodeled_24[8];
    unsigned char unmodeled_2c[1];
    signed char state;
    unsigned char unmodeled_2e[4];
    signed char target;
    unsigned char unmodeled_33[0x0d];
    int kindParameter;
    int timer;
} KowarePartsSizeState;

typedef union KowarePartsSizeUnitDimensions {
    DoublewordAlignedVector4 raw;
    Vector4 vector;
} KowarePartsSizeUnitDimensions;

typedef struct KowarePartsSizeUnit {
    unsigned char unmodeled_00[0xb0];
    KowarePartsSizeUnitDimensions dimensions;
    unsigned char unmodeled_c0[0xe0];
    KowarePartsSizeState state;
} KowarePartsSizeUnit;

typedef struct KowareInitState {
    unsigned char unmodeled_00[2];
    short kindIndex;
    unsigned char unmodeled_04[0x28];
    unsigned char unmodeled_2c;
    signed char state;
    unsigned char unmodeled_2e[4];
    signed char target;
    unsigned char unmodeled_33[0x11];
    int timer;
} KowareInitState;

typedef struct KowareInitUnit {
    unsigned int flags;
    void (*updateFunction)(void *unit);
    unsigned char unmodeled_08[8];
    Vector4 position;
    unsigned char unmodeled_20[0x80];
    unsigned char hideMode;
    unsigned char unmodeled_a1[3];
    short serial;
    unsigned char unmodeled_a6[0x0a];
    DoublewordAlignedVector4 dimensions;
    unsigned char unmodeled_c0[0xe0];
    KowareInitState state;
} KowareInitUnit;


typedef struct UwamonoHideEffect {
    unsigned char unmodeled_00[0xa9a];
    signed char enabled;
} UwamonoHideEffect;

typedef struct UwamonoCreatedPosition {
    unsigned char unmodeled_00[0x10];
    AlignedVector4 position;
} UwamonoCreatedPosition;

typedef struct DrawActiveCursorUnit {
    unsigned int flags;
    unsigned char unmodeled_04[0x9c];
    unsigned char aimIndex;
    unsigned char unmodeled_a1[0x0f];
    float cursorScale[3];
    unsigned char unmodeled_bc[0xe8];
    signed char signal;
} DrawActiveCursorUnit;

typedef struct UwamonoCornerCollisionUnit {
    unsigned char unmodeled_00[0x10];
    DoublewordVector4 position; /* copied word by word (words[1] first) in HitCheckBox */
    unsigned char unmodeled_20[0x60];
    const float (*basis)[4];
    unsigned char unmodeled_84[0x2c];
    AlignedVector4 halfSize;
} UwamonoCornerCollisionUnit;

typedef struct UwamonoBoxDimensions {
    float radius;
    float height;
    float boxDepth;
    unsigned char unmodeled_0c[4];
} UwamonoBoxDimensions;

typedef union UwamonoAlignedBoxDims {
    UwamonoBoxDimensions value;
    long long words[2];
} UwamonoAlignedBoxDims;


typedef struct UwamonoShotEffect {
    unsigned char unmodeled_000[0x6bc];
    Actor *owner;
    unsigned char unmodeled_6c0[0x3cc];
    unsigned int flags;
} UwamonoShotEffect;

typedef union UwamonoCollisionCopy {
    UwamonoCollisionUnit value;
    unsigned long long words[0x300 / 8];
} UwamonoCollisionCopy;

typedef struct UwamonoAimCandidate {
    int unitIndex;
    unsigned char unmodeled_04[0x0c];
    UwamonoAlignedScreenPoint screenPoint;
    float distance;
    unsigned char unmodeled_24[0x0c];
} UwamonoAimCandidate;

typedef struct NativeUnitPeer NativeUnitPeer;

extern UwamonoGameLoopState GameLoopState;
extern UwamonoMapSlot MapUnit[];
extern UwamonoSoundWorkView SoundWork;
extern const char createUwamonoUnitOverflowFormat[32];
extern UwamonoKowareKind KowareKind_t[];
extern const char createKowaremonoUnitOverflowFormat[];
extern const float mapUnitHitHeightScale;
extern const float mapUnitHitBelowMargin;
extern const float mapUnitPosHitHeightScale;
extern const float mapUnitPosHitBelowMargin;
extern UwamonoEnemyUnit actor[ACTOR_COUNT];
extern const float crossActorHeightLimit;
extern UwamonoEnemyLink enepc[16];
extern const char fieldResidentOverflowFormat[];
extern const char uwamonoDatUsageFormat[];
extern const char karakamaItemboxPath[];
extern const char karakamaEvtItemPath[];
extern const char lexFileExtension[];
extern const char xtxFileExtension[];
extern unsigned char ItemBoxTbl[ITEM_BOX_TABLE_SIZE];
extern unsigned char EvtItemTbl[EVT_ITEM_TABLE_SIZE];
extern const char alreadyMapUnitFormat[];
extern const char createUwamonoOverflowFormat[];
extern UwamonoSequenceRecord unitSequence[];
extern const char setSizeFormat[];
extern const char xyzVectorFormat[];
extern const char illegalUwaNullFormat[];
extern const char illegalUwamonoTypeFormat[];
extern const char hitCheckMatrixFormat[];
extern const char hitCheckPosFormat[];
extern const char hitCheckLenRadiusFormat[];
extern const char matrixHitLabel[];
extern const float uwamonoUnusedFloat;
extern const float uwamonoUnusedFloatNext;
extern const char hitCheckLenFormat[];
extern const char hitCheckXFormat[];
extern const char hitCheckYFormat[];
extern const char hitCheckZFormat[];
extern const char hitCheckUFormat[];
extern const char hitCheckVFormat[];
extern const char hitCheckWFormat[];
extern const char illegalKowareKindFormat[];
extern const char breakableCreateErrorFormat[];
extern const char createKowaremonoFormat[];
extern const char brokenMethodName[];
extern const char kowareDeleteFormat[];
extern float activeCursolMinScale;
extern float activeCursolScaleFloor;
extern float nyuruCircleRadiusMargin;
extern float nyuruMatrixStillThreshold;
extern float nyuruMatrixStillEpsilon;
extern const char shootMapUnitFormat[];
extern float shootMotionSpeed;
extern float shootProgressThreshold;
extern const float uwamonoAreaHeightScale;
extern const float uwamonoAreaLowerMargin;
extern float crossCheckMapUnitPadding;
extern float uwamonoMaxFallVelocity;
extern float uwamonoGravity;
extern float uwamonoFloorOffset;
extern float uwamonoLandStopVelocity;
extern float uwamonoBounceStopVelocity;
extern float uwamonoHitHeightPadding;
extern float aimMapUnitLookEpsilon;
extern float nyuruCornerRadius;

extern int printf(const char *format, ...);
extern float atan2f(float y, float x);
extern void sefDeleteEffectCf(int effectId);
extern void xglSoundEffectStopID(int soundId, int channel);
extern void InitMapDoor(InitMapPartsUnit *unit);
void InitMapKoware(InitMapPartsUnit *mapUnit);
extern void InitDrill(InitMapPartsUnit *unit);

/* Defined in src/main/map_create_unit_peer.c. */
extern void MAP_updateUnitPartsSequence(void *unit);

extern void MAP_updateUnitSequence(void *unit);

/* Defined later in this TU. */
void CheckUwamonoHeight(UwamonoCommonUnit *unit);

/* Defined later in this TU. */
void UwamonoBgmFunc(UwamonoCommonUnit *unit);

/* Defined in src/audio/ssd/ssd_sec.c. */
extern void SsdFadeoutEffect(int effect_id, int source_id, int fade_time);

void MAP_updateUnitSaveSymbol(MapUnitRecord *unit);
void MAP_updateUnitShopSymbol(MapUnitRecord *unit);
void MAP_updateUnitSymbol(MapUnitRecord *unit);
extern int MAP_loadUnitResource(MapUnitRecord *unit, int resourceId);
extern void InitMapTrap(MapUnitRecord *unit);
extern void *memset(void *destination, int value, unsigned int bytes);
void MAP_updateUnitKoware(void *argument);
void CreateUwamonoCommon(MapUnitRecord *unit);
int HitCheckBoxCorner(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit);
unsigned int HitCheckMapUnitPosAt(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit);
int HitCheckBox(UwamonoCollisionActor *actor, struct UwamonoBoxUnit *boxUnit);
int HitCheckCorner(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit);
static int NyuruCircle(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit);
static int NyuruMatrix(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit, int mode);
/*
 * Reads only `from` and `to`, plus the unit's position and radius
 * (+0x10/+0x18, +0xB0), so it takes the collision view.
 */
int CrossCheckMapUnitCircle(const Vector4 *from, const Vector4 *to,
                            UwamonoCollisionUnit *unit);

int AreaCheckBox(const Vector4 *position, UwamonoMapSlot *unit, float radius);
int CrossCheckMapUnitAt(Vector4 *first, Vector4 *second, UwamonoCrossUnit *unit);
extern int CheckCrossCircle(float radius, const Vector4 *from, const Vector4 *to, const Vector4 *center);
static float CheckCornerDist(const Vector4 *position, UwamonoCollisionUnit *unit);
int CheckBrokenMapUnit(BrokenMapUnit *unit);
extern void xglSoundEffectPosID(int soundId, const float *position, int enabled, int channel);

int HitCheckUwamonoAt(UwamonoBoxUnit *actor, UwamonoBoxUnit *unit);

extern void ACT_updateEnemy(UwamonoEnemyUnit *unit);
extern void EXM_SetWindStruct(UwamonoWind *wind);
extern void xglSoundEffectNormalID(int soundId, int channel);
int HitCheckBoxUwamono(UwamonoBoxUnit *actor, UwamonoMapSlot *unit);
extern void nmlModelSetPartsVisible(void *resource, int partsId, int visible);
/*
 * The hit and cross checks read a MapUnit[] slot through the collision view
 * (position, radius/height, shape byte); callers holding the slot as a
 * UwamonoMapSlot or MapUnitRecord pass it in that view, the same way
 * CheckNearMapUnit hands its scan slot to CheckCornerDist.
 */
int HitCheckMapUnitAt(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit);

void CalcNearCrossPointCircle(const Vector4 *from, const Vector4 *to,
                              UwamonoCollisionUnit *unit, Vector4 *out);

void CalcNearCrossPointBox(const Vector4 *first, const Vector4 *second,
                           MapUnitRecord *unit, AlignedVector4 *result);

int CrossCheckMapUnitBox(Vector4 *first, Vector4 *second,
                         UwamonoCrossUnit *unit);

extern char *strcpy(char *destination, const char *source);
void InitUwamono(MapUnitRecord *unit);
extern double fptodp(float value);
extern void UnduParamInit(void *parameters);
extern float UnduCheck(Vector4 *position, Vector4 *displacement, void *parameters);
extern void InitItemBox(MapUnitRecord *unit);
extern void InitItemSymbol(MapUnitRecord *unit);
void InitSaveSymbol(MapUnitRecord *unit);
void InitShopSymbol(MapUnitRecord *unit);
void InitEvsSymbol(MapUnitRecord *unit);
void InitRetSymbol(MapUnitRecord *unit);
extern void InitSpecialSymbol(MapUnitRecord *unit);
void MAP_updateUnitHide(void *argument);
void DrawActiveCursol(MapUnitRecord *argument);
extern void MAP_updateUnitTrap(void *unit);
extern void MAP_updateUnitItemBox(void *unit);
extern void MAP_drawUnitItemBox(void *unit);
extern void MAP_updateUnitItemSymbol(void *unit);
int AimMapUnit(void);
extern void DispCollUwamono(void);
extern void Enemy_FindByEar(Actor *actor, Vector4 *position);
int UwamonoAreaCheck(const Vector4 *position, UwamonoMapSlot *unit, float radius);

extern int CheckCrossLine(const Vector4 *first, const Vector4 *second,
                          const Vector4 *edgeStart, const Vector4 *edgeEnd);

extern void CalcCrossPoint(const Vector4 *first, const Vector4 *second,
                           const Vector4 *edgeStart, const Vector4 *edgeEnd, Vector4 *out);

float xglRotTransPers(Vector4 *out, Matrix4 matrix,
                      Vector4 *point, int camera_id);

void InitMapParts(InitMapPartsUnit *unit);
int HitCheckNyuru(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit);
void MAP_updateUnitEnemy(UwamonoEnemyUnit *unit);
void UwamonoBrokenSe(UwamonoBrokenSoundUnit *unit);

void DrawSprite(const UwamonoAlignedScreenPoint *position,
                const UwamonoScreenPoint *color, int width, int height);

static void NyuruCorner(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit);

void CrossPointUwamonoAt(const Vector4 *first, const Vector4 *second,
                         MapUnitRecord *unit, void *result);

void CheckNearPoint(const Vector4 *origin, const Vector4 *candA,
                    const Vector4 *candB, Vector4 *out);

int AimMapUnitLookCheck(UwamonoScanUnit *target);

void SortLine(const UwamonoAlignedScreenPoint *from,
              const UwamonoAlignedScreenPoint *to, int mode);

int HitCheckUwamono(UwamonoScanUnit *actor);
extern const signed char printflgReadOnly __asm__("printflg");
void SetUwaWind(UwamonoWindUnit *unit);
void UwamonoCommonFunc(UwamonoCommonUnit *unit);

/* Loading resident field objects, followed by their hexadecimal identifier. */
const char loadResidentObjectFormat[24] = "\xBE\xEF\xC3\xF3\xBE\xE5\xCA\xAA\xA5\xED\xA1\xBC\xA5\xC9 %x\n";

const char illegalPartsTypeFormat[24] = "Illegal Parts Type %d\n";

const char createSaveSymbolFormat[32] = "Create Save Symbol %f %f %f\n";

const char createShopSymbolFormat[32] = "Create Shop Symbol %f %f %f\n";

static UwamonoSpritePacket sPoint_0;

static UwamonoLinePacket sSortLine_1;

int uwares_tbl[UWAMONO_RESIDENT_COUNT] = { 0 };

int xtxres_tbl[UWAMONO_RESIDENT_COUNT] = { 0 };

UwamonoWind uwaWind = { 0 };

static UwamonoSpritePacket sPoint_0 = { .unmodeled_00 = { 1, 128, 0, 0, 0, 64, 35, 96, 238, 81, 81, 0, 0, 0, 0, 0, 3, 16, 3, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0 }, .color0 = { .x = 0, .y = 0, .z = 255, .w = 0 }, .position0 = { .x = 0, .y = 0, .z = 0, .w = 0 }, .color1 = { .x = 0, .y = 0, .z = 255, .w = 0 }, .position1 = { .x = 0, .y = 0, .z = 0, .w = 0 } };

static UwamonoLinePacket sSortLine_1 = { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 81, 1, 128, 0, 0, 0, 64, 1, 64, 81, 81, 0, 0, 0, 0, 0, 0 }, .color0 = { .x = 0, .y = 0, .z = 255, .w = 0 }, .position0 = { .x = 0, .y = 0, .z = 0, .w = 0 }, .color1 = { .x = 0, .y = 0, .z = 255, .w = 0 }, .position1 = { .x = 0, .y = 0, .z = 0, .w = 0 } };

UwamonoKowareKind KowareKind_t[KOWARE_KIND_COUNT] = { { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 1.0f, 0.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 601, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 2.0f, 0.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 602, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 3.0f, 0.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 603, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 604, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 605, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 606, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 1.0f, 1.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 607, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 2.0f, 1.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 608, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 3.0f, 3.0f, 1.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 609, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 1.0f, 0.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 628, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 2.0f, 0.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 629, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 3.0f, 0.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 630, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 631, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 632, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 633, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 1.0f, 1.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 634, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 2.0f, 1.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 635, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 3.0f, 1.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 636, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 2.5f, 2.0f, 2.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 637, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 1.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 658, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 2.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 659, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 3.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 660, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 1.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 661, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 2.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 662, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 3.0f, 1.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 663, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 1.0f, 1.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 664, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 2.0f, 1.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 665, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 3.0f, 1.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 666, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.25f, 2.0f, 2.0f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 667, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 0.25f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 688, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 0.25f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 689, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 0.25f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 690, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 0.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 691, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 0.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 692, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 0.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 693, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 1.0f, 0.75f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 694, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 2.0f, 0.75f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 695, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 3.0f, 0.75f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 696, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 2.0f, 2.0f, 1.25f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 697, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.125f, 2.25f, 0.75f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 718, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 2.25f, 0.125f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 719, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 4.0f, 0.5f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 601, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.875f, 4.0f, 0.3f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 601, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.375f, 2.25f, 0.125f, 0.0f }, .target = 0, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 601, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 1.0f, 0.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 610, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 2.0f, 0.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 611, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 3.0f, 0.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 612, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 613, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 614, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 615, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 1.0f, 1.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 616, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 2.0f, 1.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 617, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 3.0f, 3.0f, 1.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 618, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 1.0f, 0.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 638, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 2.0f, 0.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 639, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 3.0f, 0.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 640, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 641, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 642, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 643, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 1.0f, 1.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 644, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 2.0f, 1.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 645, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 3.0f, 1.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 646, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 2.5f, 2.0f, 2.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 647, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 1.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 668, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 2.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 669, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 3.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 670, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 1.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 671, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 2.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 672, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 3.0f, 1.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 673, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 1.0f, 1.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 674, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 2.0f, 1.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 675, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 3.0f, 1.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 676, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.25f, 2.0f, 2.0f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 677, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 0.25f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 698, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 0.25f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 699, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 0.25f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 700, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 0.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 701, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 0.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 702, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 0.5f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 703, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 1.0f, 0.75f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 704, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 2.0f, 0.75f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 705, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 3.0f, 0.75f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 706, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 2.0f, 2.0f, 1.25f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 707, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 1.0f, 0.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 619, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 2.0f, 0.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 620, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 3.0f, 0.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 621, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 622, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 623, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 624, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 1.0f, 1.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 625, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 2.0f, 1.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 626, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 3.0f, 3.0f, 1.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 627, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 1.0f, 0.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 648, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 2.0f, 0.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 649, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 3.0f, 0.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 650, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 651, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 652, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 653, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 1.0f, 1.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 654, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 2.0f, 1.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 655, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 3.0f, 1.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 656, .unmodeled_28 = 0ULL }, { .shape = 1, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 2.5f, 2.0f, 2.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 657, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 1.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 678, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 2.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 679, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.25f, 3.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 680, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 1.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 681, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 2.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 682, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.5f, 3.0f, 1.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 683, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 1.0f, 1.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 684, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 2.0f, 1.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 685, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 3.0f, 1.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 686, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.25f, 2.0f, 2.0f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 687, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 0.25f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 708, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 0.25f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 709, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 0.25f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 710, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 1.0f, 0.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 711, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 2.0f, 0.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 712, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.0f, 3.0f, 0.5f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 713, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 1.0f, 0.75f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 714, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 2.0f, 0.75f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 715, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 1.5f, 3.0f, 0.75f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 716, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 2.0f, 2.0f, 1.25f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 717, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.125f, 2.25f, 0.75f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 601, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 2.25f, 0.125f, 0.0f }, .target = 1, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 601, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.125f, 2.25f, 0.75f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 601, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 0.75f, 2.25f, 0.125f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 601, .unmodeled_28 = 0ULL }, { .shape = 2, .unmodeled_01 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .dimensions = { 2.866f, 2.859f, 0.025f, 0.0f }, .target = 2, .unmodeled_21 = { 0, 0, 0 }, .kindParameter = 601, .unmodeled_28 = 0ULL } };

static unsigned char uwamono_dat[UWAMONO_DAT_SIZE];

static inline float UwamonoProjectBoxBasisXZ(float x, float z,
                                           const struct UwamonoBoxBasis *basis)
{
    return x * basis->x + z * basis->z;
}

/* Reads the resident model and texture pairs into uwamono_dat, then the karakama item tables. */
void InitUwamonoSys(void)
{
    char path[128];
    const char *paths[UWAMONO_RESIDENT_COUNT] = {
        "data\\obj\\muobj\\MU_001",
        "data\\obj\\muobj\\MU_002",
        "data\\obj\\muobj\\MU_003",
        "data\\obj\\muobj\\MU_004",
        "data\\obj\\muobj\\Tre_001",
        "data\\obj\\muobj\\Save_P",
        "data\\obj\\muobj\\Shop_P",
        "data\\obj\\muobj\\Tre_002",
        "data\\obj\\muobj\\MU_009",
        "data\\obj\\muobj\\MU_010",
        "data\\obj\\muobj\\MU_011",
        "data\\obj\\muobj\\MU_012",
        "data\\obj\\muobj\\MU_013",
        "data\\obj\\muobj\\MU_014"
    };
    unsigned char *dataEnd = uwamono_dat;
    int index;
    int size;

    for (index = 0; index < UWAMONO_RESIDENT_COUNT; index++) {
        memset(path, 0, sizeof(path));
        strcpy(path, paths[index]);
        strcat(path, lexFileExtension);
        uwares_tbl[index] = (int)dataEnd;
        dataEnd += xglCdReadFile(path, dataEnd, 0, 1);
        dataEnd = (unsigned char *) (((unsigned int) dataEnd + 15) & ~15);

        memset(path, 0, sizeof(path));
        strcpy(path, paths[index]);
        strcat(path, xtxFileExtension);
        xtxres_tbl[index] = (int)dataEnd;
        dataEnd += xglCdReadFile(path, dataEnd, 0, 1);
        dataEnd = (unsigned char *) (((unsigned int) dataEnd + 15) & ~15);
    }

    size = (int) (dataEnd - uwamono_dat);
    if (size > UWAMONO_DAT_SIZE) {
        printf(fieldResidentOverflowFormat, size / 1024);
    } else {
        printf(uwamonoDatUsageFormat, size / 1024, index);
    }

    memset(ItemBoxTbl, 0, sizeof(ItemBoxTbl));
    memset(EvtItemTbl, 0, sizeof(EvtItemTbl));
    {
        unsigned char *workEnd = WorkEnd;

        xglCdReadFile(karakamaItemboxPath, workEnd, 0, 1);
        __builtin_memcpy(ItemBoxTbl, workEnd, ITEM_BOX_TABLE_SIZE);
        xglCdReadFile(karakamaEvtItemPath, workEnd, 0, 1);
        __builtin_memcpy(EvtItemTbl, workEnd, EVT_ITEM_TABLE_SIZE);
    }

    GameLoopState.aimHeightFlags |= 3;
    GameLoopState.selectionDistance = 3.0f;
}

/*
 * Returns the unit for a resource id. Serials below UWAMONO_SERIAL_RESOURCE_BASE
 * reuse an active unit with the same serial; otherwise a new unit is created.
 * The uwamono state is reset to its defaults.
 */
MapUnitRecord *Unit_CreateUwamono(int index, int resourceId)
{
    int slot;
    float identityScale;
    MapUnitRecord *unit = 0;
    UwamonoCreatedState *state;

    if (resourceId < UWAMONO_SERIAL_RESOURCE_BASE) {
        for (slot = 0; slot < MAP_UNIT_COUNT; slot++) {
            if (resourceId != MapUnit[slot].serial) {
                continue;
            }
            if ((MapUnit[slot].flags & MAP_UNIT_ACTIVE) == 0) {
                continue;
            }
            if (printflg != 0) {
                printf(alreadyMapUnitFormat, resourceId);
            }
            unit = (MapUnitRecord *)&MapUnit[slot];
            break;
        }
    }

    if (unit == 0) {
        unit = MAP_createUnit(index, resourceId);
        if (unit == 0) {
            printf(createUwamonoOverflowFormat);
            return 0;
        }
    }

    /* Load the resource unless the id is in the special or symbol ranges, or is 0x7013. */
    if (resourceId >= UWAMONO_SERIAL_RESOURCE_BASE + 1 &&
        ((unsigned int)(resourceId - UWAMONO_ID_SPECIAL_FIRST) >= UWAMONO_ID_SPECIAL_COUNT) &&
        ((unsigned int)(resourceId - UWAMONO_ID_SYMBOL_FIRST) >= UWAMONO_ID_SYMBOL_COUNT) &&
        (resourceId != 0x7013)) {
        MAP_loadUnitResource(unit, resourceId);
    }

    identityScale = 1.0f;
    unit->flags = MAP_UNIT_ACTIVE | MAP_UNIT_NO_DRAW;
    unit->basis = unit->matrix;
    unit->actionNo = 0;
    unit->sequenceNo = 0;
    unit->sequenceSub = 0;
    unit->actionSub = 0;
    unit->position.x = 0.0f;
    unit->position.y = 0.0f;
    unit->position.z = 0.0f;
    unit->position.w = identityScale;

    state = &unit->uwamono;
    memset(state, 0, sizeof(*state));
    state->signal = -1;
    state->initialUnitId = -1;
    state->state = -1;
    state->initialState = -1;
    state->heightCheckFlag = -1;
    state->target = -1;
    state->initialValue48 = -1;
    state->timer = -1;
    state->initialValue34 = -1;
    state->initialValue38 = -1;
    state->initialValue40 = -1;
    state->initialPartId = -1;
    unit->update = InitUwamono;
    {
        UwamonoSequenceRecord *sequence = &unitSequence[unit->slot];

        sequence->initialValues00[0] = 0;
        sequence->initialValues00[1] = 0;
        sequence->initialValues240[0] = 0;
        sequence->initialValues240[1] = 0;
        sequence->initialValues240[2] = 0;
        sequence->scaleX = identityScale;
        sequence->scaleY = identityScale;
        sequence->initialValues254[0] = 0;
        sequence->initialValues254[1] = 0;
        sequence->initialValues0c[0] = 0;
        sequence->initialValues0c[2] = 0;
        sequence->initialValues0c[3] = 0;
        sequence->initialValues0c[4] = 0;
        sequence->initialValues0c[5] = 0;
        sequence->initialValues0c[6] = 0;
        sequence->initialValues0c[7] = 0;
        sequence->initialValues0c[8] = 0;
        sequence->scaleZ = identityScale;
        sequence->initialValues0c[9] = 0;
    }

    return unit;
}

/*
 * Valid resourceId values run 0x7001..0x700E (14 entries, the size of
 * uwares_tbl and xtxres_tbl); 0x7012 is excluded explicitly ahead of the
 * broader >= 0x1000 range check.
 */
void MAP_LoadUwamonoResource(MapUnitRecord *unit, int resourceId)
{
    int index;
    int uwaresId;
    int xtxresId;

    printf(loadResidentObjectFormat);
    if (resourceId != UWAMONO_ID_SOUND && resourceId >= UWAMONO_SERIAL_RESOURCE_BASE) {
        index = resourceId - UWAMONO_ID_RESIDENT_FIRST;
        uwaresId = uwares_tbl[index];
        xtxresId = xtxres_tbl[index];
        unit->xtxresId = xtxresId;
        unit->uwaresId = uwaresId;
    }
}

/* Copies the parts matrix for unit->serial from the map-parts resource into the unit's basis, position and Y rotation. */
void GetPartsPos(MapUnitRecord *unit)
{
    MapPartsRoot *root;
    MapPartsResource *resource;
    Matrix4 *table;
    float (*entry)[4];

    root = GameLoopState.partsRoot;
    resource = root->resource;
    table = (Matrix4 *) ((char *) resource + resource->arrayOffset);
    entry = table[unit->serial];

    unit->basis = entry;
    unit->position.x = entry[3][0];
    unit->position.y = entry[3][1];
    unit->position.z = entry[3][2];
    unit->position.w = 1.0f;
    unit->rotation.x = 0.0f;
    unit->rotation.y = atan2f(entry[2][0], entry[2][2]);
    unit->rotation.z = 0.0f;
}

void SendBrokenSignal(MapUnitRecord *unit)
{
    unit->trap.state = 2;
}

/*
 * Sets a breakable part's dimensions from its base size, or from its kind
 * table entry when no base size is set, then applies the size offsets.
 */
void GetPartsSize(UwamonoKowareSizeUnit *unit)
{
    UwamonoMapUnitState *state;
    short kindIndex;

    state = &unit->state;
    kindIndex = state->kindIndex;
    if (state->state == -1) {
        state->state = KowareKind_t[kindIndex].shape;
    }
    if (state->baseSizeX > 0.0f) {
        unit->dimensions.x = state->baseSizeX * 0.5f;
        unit->dimensions.y = state->baseSizeY;
        unit->dimensions.z = state->baseSizeZ * 0.5f;
        if (printflg != 0) {
            const char *format = xyzVectorFormat;
            printf(setSizeFormat);
            printf(format, fptodp(state->baseSizeX),
                   fptodp(state->baseSizeY), fptodp(state->baseSizeZ));
        }
    } else {
        unit->dimensions = KowareKind_t[kindIndex].dimensions;
    }
    unit->dimensions.x += state->sizeOffsetX * 0.5f;
    unit->dimensions.y += state->sizeOffsetY;
    unit->dimensions.z += state->sizeOffsetZ * 0.5f;
    if (state->target == -1) {
        state->target = KowareKind_t[kindIndex].target;
    }
    if (state->kindParameter == -1) {
        state->kindParameter = KowareKind_t[kindIndex].kindParameter;
    }
}

/* Deletes the unit's effect handles, and stops its BGM sound when a BGM timer is running. */
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

signed char GetUwamonoSignal(MapUnitRecord *unit)
{
    return unit->trap.state;
}

/* Initializes a map unit: probes its ground height, then dispatches to the parts or symbol init for its map type. */
void InitUwamono(MapUnitRecord *unit)
{
    UwamonoCreatedState *map = &unit->uwamono;
    UwamonoProbeParameters parameters;

    unit->flags &= ~MAP_UNIT_NO_DRAW;
    if (map->mapType == 0) {
        printf(illegalUwaNullFormat, unit->serial, unit->slot);
        unit->serial = -1;
        unit->update = 0;
        return;
    }

    if (map->heightCheckFlag == -1) {
        map->heightCheckFlag = 0;
    }
    UnduParamInit(&parameters);
    parameters.dataHeader = UnduDataGetHeader(0, UNDU_HEADER_DIRECT);
    if (map->unmodeled_2f != 0) {
        parameters.flags |= UNDU_QUERY_HEIGHT_TABLE;
    }
    if (UnduCheck(&unit->position, 0, &parameters) == UNDU_NO_HEIGHT && map->unmodeled_2f == 0) {
        map->heightCheckFlag = 0;
    }

    if (unit->serial < UWAMONO_SERIAL_RESOURCE_BASE) {
        InitMapParts((InitMapPartsUnit *)unit);
        return;
    }

    switch (map->mapType) {
    case UWAMONO_TYPE_ITEM_BOX:
        InitItemBox(unit);
        return;
    case UWAMONO_TYPE_ITEM_SYMBOL:
        InitItemSymbol(unit);
        return;
    case UWAMONO_TYPE_TRAP:
        InitMapTrap(unit);
        return;
    case UWAMONO_TYPE_SAVE_SYMBOL:
        InitSaveSymbol(unit);
        return;
    case UWAMONO_TYPE_SHOP_SYMBOL:
        InitShopSymbol(unit);
        return;
    case UWAMONO_TYPE_EVS_SYMBOL:
        InitEvsSymbol(unit);
        return;
    case UWAMONO_TYPE_RET_SYMBOL:
        InitRetSymbol(unit);
        return;
    case UWAMONO_TYPE_SPECIAL_SYMBOL:
        InitSpecialSymbol(unit);
        return;
    default:
        printf(illegalUwamonoTypeFormat, map->mapType);
        unit->update = 0;
        return;
    }
}

/* Dispatches a map parts unit to its init by map type; an unknown type disables the unit. */
void InitMapParts(InitMapPartsUnit *unit)
{
    unit->uwaresId = 0;
    switch (unit->mapType) {
    case UWAMONO_TYPE_KOWARE:
        InitMapKoware(unit);
        break;
    case UWAMONO_TYPE_DOOR:
        InitMapDoor(unit);
        break;
    case UWAMONO_TYPE_DRILL:
        InitDrill(unit);
        break;
    default:
        printf(illegalPartsTypeFormat, unit->mapType);
        unit->update = 0;
        break;
    }
}

/*
 * Initializes a breakable map part: reads its position and size, sets its update
 * and makes its model part visible. An out-of-range serial is reported and
 * disables the part.
 */
void InitMapKoware(InitMapPartsUnit *mapUnit)
{
    KowareInitUnit *unit = (KowareInitUnit *)mapUnit;
    MapPartsResource *resource = GameLoopState.partsRoot->resource;
    MapPartsResource *resourceCount = (MapPartsResource *)resource;
    KowareInitState *state = &unit->state;
    short kindIndex = state->kindIndex;
    if (kindIndex < 0) {
        printf(illegalKowareKindFormat, kindIndex);
    }
    GetPartsPos((MapUnitRecord *)unit);
    GetPartsSize((UwamonoKowareSizeUnit *)unit);
    if (state->timer == -1) {
        state->timer = 1;
    }
    unit->flags = 0;
    unit->updateFunction = MAP_updateUnitKoware;
    CreateUwamonoCommon((MapUnitRecord *)unit);
    SetHideObject(unit);
    if (unit->serial >= 0 && unit->serial < resourceCount->partCount) {
        nmlModelSetPartsVisible(resource, unit->serial, 1);
    } else {
        printf(breakableCreateErrorFormat, unit->serial);
        unit->serial = -1;
        unit->updateFunction = 0;
    }
    if (printflg != 0) {
        printf(createKowaremonoFormat, unit->serial, kindIndex,
               fptodp(unit->position.x), fptodp(unit->position.y),
               fptodp(unit->position.z));
    }
}

/* Initializes a save point symbol: sets its resource ids and transform, then its update function. */
void InitSaveSymbol(MapUnitRecord *unit)
{
    UwamonoInitSymbolState *timerState = &unit->symbol;
    float (*matrix)[4] = unit->matrix;
    Vector4 *position = &unit->position;
    int uwaresId = uwares_tbl[5];

    unit->xtxresId = xtxres_tbl[5];
    unit->uwaresId = uwaresId;

    xglMatrixStackUnit();
    xglMatrixStackTrans(&position->x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackSave(matrix);
    unit->basis = matrix;

    if (timerState->state == -1) {
        timerState->state = 0;
    }
    /* Stored through the timer's address (a plain int store, not a member
     * store), so the printflg load below stays behind it. */
    *&timerState->timer = 0;
    unit->update = MAP_updateUnitSaveSymbol;
    unit->flags |= MAP_UNIT_ACTIVE;
    unit->flags |= MAP_UNIT_SAVE_SYMBOL;

    if (printflg != 0) {
        printf(createSaveSymbolFormat,
                      fptodp(position->x),
                      fptodp(unit->position.y),
                      fptodp(unit->position.z));
    }
}

/* Initializes a shop symbol with the shop resource ids and transform, then its update function. */
void InitShopSymbol(MapUnitRecord *unit)
{
    UwamonoInitSymbolState *timerState = &unit->symbol;
    float (*matrix)[4] = unit->matrix;
    Vector4 *position = &unit->position;
    int uwaresId = uwares_tbl[6];

    unit->xtxresId = xtxres_tbl[6];
    unit->uwaresId = uwaresId;

    xglMatrixStackUnit();
    xglMatrixStackTrans(&position->x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackSave(matrix);
    unit->basis = matrix;

    if (timerState->state == -1) {
        timerState->state = 0;
    }
    /* As in InitSaveSymbol: a plain int store through the timer's address. */
    *&timerState->timer = 0;
    unit->update = MAP_updateUnitShopSymbol;
    unit->flags |= MAP_UNIT_ACTIVE;
    unit->flags |= MAP_UNIT_SHOP_SYMBOL;

    if (printflg != 0) {
        printf(createShopSymbolFormat,
                      fptodp(position->x),
                      fptodp(unit->position.y),
                      fptodp(unit->position.z));
    }
}

/* Initializes an EVS symbol: builds its transform and sets its update function. */
void InitEvsSymbol(MapUnitRecord *unit)
{
    UwamonoInitSymbolState *timerState = &unit->symbol;

    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->position.x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackSave(&unit->matrix[0]);
    unit->basis = &unit->matrix[0];

    if (timerState->state == -1) {
        timerState->state = 0;
    }
    timerState->timer = 0;
    unit->update = MAP_updateUnitSymbol;
    unit->flags = unit->flags | MAP_UNIT_ACTIVE;
    if (printflgReadOnly == 0) return;
    printf(createShopSymbolFormat,
           fptodp(unit->position.x),
           fptodp(unit->position.y),
           fptodp(unit->position.z));
}

/* Initializes a return symbol: builds its transform and sets its update function. */
void InitRetSymbol(MapUnitRecord *unit)
{
    UwamonoInitSymbolState *timerState = &unit->symbol;
    float (*matrix)[4] = unit->matrix;
    Vector4 *position = &unit->position;

    xglMatrixStackUnit();
    xglMatrixStackTrans(&position->x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackSave(matrix);
    unit->basis = matrix;

    if (timerState->state == -1) {
        timerState->state = 0;
    }
    timerState->timer = 0;
    unit->update = MAP_updateUnitSymbol;
    unit->flags |= MAP_UNIT_ACTIVE;

    if (printflgReadOnly != 0) {
        printf(createShopSymbolFormat,
               fptodp(position->x),
               fptodp(unit->position.y),
               fptodp(unit->position.z));
    }
}

/*
 * Links a new breakable part to its enemy actor and hidden map unit at the same
 * position, and hides the part's model.
 */
void SetHideObject(void *argument)
{
    MapUnitRecord *created = argument;
    UwamonoCreatedPosition *createdPosition = argument;
    UwamonoCreatedState *state = &created->uwamono;
    signed char actorIndex = state->state;
    signed char mapUnitIndex;

    if (actorIndex != -1) {
        unsigned char sequenceIndex = created->slot;
        UwamonoEnemyUnit *enemy = &actor[actorIndex];
        unsigned char *linkState = enepc[enemy->actorIndex].linkState;

        enemy->update = MAP_updateUnitEnemy;
        linkState[8] = 0;
        linkState[11] = sequenceIndex;
        enemy->position = createdPosition->position;
        enemy->rotationX = 0.0f;
        enemy->heading = 0.0f;
        enemy->rotationZ = 0.0f;
        ACT_updateEnemy(enemy);
        enemy->flags |= UWAMONO_ACTOR_HIDDEN;
    }

    mapUnitIndex = state->signal;
    if (mapUnitIndex != -1) {
        UwamonoMapSlot *unit = &MapUnit[mapUnitIndex];
        struct UwamonoMapCollisionState *mapState = &unit->state;
        UwamonoHideEffect *effect = (UwamonoHideEffect *)unit->effectCf[0];
        MapPartsResource *resource = GameLoopState.partsRoot->resource;

        unit->linkedUnitIndex = created->slot;
        unit->position = createdPosition->position;
        unit->update = MAP_updateUnitHide;
        unit->flags |= MAP_UNIT_HIDDEN | MAP_UNIT_NO_DRAW;
        unit->draw = 0;
        if (effect != 0) {
            effect->enabled = 0;
        }
        if (mapState->kindIndex == UWAMONO_TYPE_KOWARE) {
            if (unit->serial >= 0 && unit->serial < resource->partCount) {
                nmlModelSetPartsVisible(resource, unit->serial, 0);
                return;
            }
            printf(breakableCreateErrorFormat, unit->serial, effect, resource);
            unit->serial = -1;
            unit->update = 0;
        }
    }
}

/* Hides the model of every breakable part recorded in the shoot list. */
void GameStateRestoreKoware(void)
{
    UwamonoGameLoopState *state;
    int index;
    index = 0;
    while (index < MAP_UNIT_COUNT && GameLoopState.shootUnit[index] != -1) {
        state = &GameLoopState;
        nmlModelSetPartsVisible(state->partsRoot->resource, state->shootUnit[index], 0);
        ++index;
    }
}

/* Clears the shoot list, runs the update of each active map unit, then unlocks the units. */
void SetMapUwamono(void)
{
    UwamonoGameLoopState *state = &GameLoopState;
    UwamonoMapSlot *unit = MapUnit;
    short *shootUnit = state->shootUnit;
    int remaining = MAP_UNIT_COUNT - 1;

    do {
        *shootUnit++ = -1;
        if (unit->flags & MAP_UNIT_ACTIVE) {
            if (unit->update)
                unit->update(unit);
        }
        --remaining;
        ++unit;
    } while (remaining >= 0);
    UnlockMapUnit();
}

/* Fills the shoot list with -1 and unlocks the map units. */
void ResetShootSys(void)
{
    UwamonoGameLoopState *state = &GameLoopState;
    short emptyShootUnitId = -1;
    int index = MAP_UNIT_COUNT - 1;
    short *shootUnit = &state->shootUnit[MAP_UNIT_COUNT - 1];

    do {
        index--;
        *shootUnit = emptyShootUnitId;
        shootUnit--;
    } while (index >= 0);

    UnlockMapUnit();
}

/* Creates a map unit at a position and Y rotation with a cleared uwamono state, then sets up its trap. */
MapUnitRecord *CreateUwamono(float x, float y, float z, float rotationY, int resourceId)
{
    MapUnitRecord *unit =
        MAP_createUnit(-1, resourceId);
    UwamonoCreatedState *state;
    int inactiveValue;

    if (unit == 0) {
        printf(createUwamonoUnitOverflowFormat);
        return 0;
    }
    MAP_loadUnitResource(unit, resourceId);
    state = &unit->uwamono;
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
    unit->rotation.y = rotationY;
    unit->rotation.x = 0.0f;
    unit->rotation.z = 0.0f;
    InitMapTrap(unit);
    return unit;
}

/* Creates a breakable map part at its parts-table position, taking its shape, dimensions and target from the kind table entry. */
UwamonoKowareUnit *CreateKowaremono(short resourceId, short kindIndex)
{
    UwamonoKowareUnit *unit = (UwamonoKowareUnit *)MAP_createUnit(-1, resourceId);
    UwamonoMapUnitState *state;
    if (unit == 0) {
        printf(createKowaremonoUnitOverflowFormat);
        return 0;
    }
    GetPartsPos((MapUnitRecord *)unit);
    state = &unit->state;
    if (state->state == -1) {
        state->state = KowareKind_t[kindIndex].shape;
    }
    unit->dimensions = KowareKind_t[kindIndex].dimensions;
    if (state->target == -1) {
        state->target = KowareKind_t[kindIndex].target;
    }
    unit->flags = 0;
    CreateUwamonoCommon((MapUnitRecord *)unit);
    unit->updateFunction = MAP_updateUnitKoware;
    return unit;
}

void CreateUwamonoCommon(MapUnitRecord *unit)
{
    unit->actionNo = 0;
    unit->flags |= MAP_UNIT_ACTIVE;
    unit->sequenceNo = 0;
    unit->sequenceSub = 0;
    unit->actionSub = 0;
}

/*
 * Per-frame update of a breakable part. On its first hit it is recorded in the
 * shoot list and plays the break effects and sound. Its model is hidden after
 * UWAMONO_KOWARE_HIDE_COUNT frames, and the part is removed after its frame limit.
 */
void MAP_updateUnitKoware(void *argument)
{
    UwamonoKowareUnit *unit = argument;
    UwamonoMapUnitState *state = &unit->state;
    int slot;

    if (state->timer != 0) {
        DrawActiveCursol((MapUnitRecord *)unit);
    }
    UwamonoCommonFunc((UwamonoCommonUnit *)unit);

    switch (unit->actionSub) {
    case 0:
        if (state->signal == 0) {
            return;
        }
        unit->actionSub = 1;
        for (slot = 0; slot < MAP_UNIT_COUNT; slot++) {
            if (GameLoopState.shootUnit[slot] == -1) {
                GameLoopState.shootUnit[slot] = unit->serial;
                break;
            }
        }

        Vibration_Set_Strong(255, 32, 30);
        UwamonoBrokenSe((UwamonoBrokenSoundUnit *)unit);
        sefCreateEffectCf(state->effectResourceId, &unit->position, &unit->rotation);
        ClearUwamonoEffect((UwamonoEffectUnit *)unit);
        SetUwaWind((UwamonoWindUnit *)unit);
        if (state->initialPartId != -1) {
            GameLoopState.enemyUpdateState = 0;
            CallMethod_I(brokenMethodName, state->initialPartId);
        }
        return;
    case 1:
        break;
    default:
        return;
    }
    unit->breakCount++;
    if (unit->breakCount >= UWAMONO_KOWARE_HIDE_COUNT) {
        nmlModelSetPartsVisible(GameLoopState.partsRoot->resource, unit->serial, 0);
        unit->flags |= MAP_UNIT_BROKEN;
    }
    if (state->frameLimit < unit->breakCount) {
        if (printflg != 0) {
            printf(kowareDeleteFormat, unit->serial, unit->sequenceIndex);
        }
        unit->updateFunction = 0;
        unit->flags = 0;
        unit->serial = -1;
        EXM_ClearWindStruct();
    }
}

/* Hides the linked enemy actor while its map unit is inactive; otherwise moves it to the unit and hands it to ACT_updateEnemy. */
void MAP_updateUnitEnemy(UwamonoEnemyUnit *unit)
{
    unsigned char linkedUnitId = enepc[unit->actorIndex].unitId;
    UwamonoGameLoopState *state;

    if (!MapUnit[linkedUnitId].actionSub) {
        unit->flags |= UWAMONO_ACTOR_HIDDEN;
    } else {
        unit->position = MapUnit[linkedUnitId].position;
        unit->flags &= ~UWAMONO_ACTOR_HIDDEN;
        unit->update = ACT_updateEnemy;
        state = &GameLoopState;
        state->enemyUpdateState = 0;
        unit->actorHeading = (unit->heading = Get_Angle(&unit->position, &state->actor->position));
    }
}

/*
 * Per-frame update of a hidden map unit: follows its linked unit while that unit
 * is active, then sets the update and draw functions for its kind.
 */
void MAP_updateUnitHide(void *argument)
{
    UwamonoMapSlot *unit = argument;
    UwamonoHideEffect *effect;
    struct UwamonoMapCollisionState *state = &unit->state;
    MapPartsResource *resource;
    Matrix4 *matrix;

    if (MapUnit[unit->linkedUnitIndex].actionSub == 0) {
        unit->flags |= MAP_UNIT_HIDDEN;
        state->hideOverride = 0;
        effect = (UwamonoHideEffect *)unit->effectCf[0];
        if (effect != 0) {
            effect->enabled = 0;
        }
        return;
    }

    unit->position = MapUnit[unit->linkedUnitIndex].position;
    unit->flags &= ~MAP_UNIT_HIDDEN;
    state->hideOverride = 1;
    effect = (UwamonoHideEffect *)unit->effectCf[0];
    if (effect != 0) {
        effect->enabled = 1;
    }
    matrix = unit->matrixView;
    (*matrix)[3][0] = unit->position.x;
    (*matrix)[3][1] = unit->position.y;
    (*matrix)[3][2] = unit->position.z;

    switch (state->kindIndex) {
    case UWAMONO_TYPE_ITEM_BOX:
        unit->update = MAP_updateUnitItemBox;
        unit->draw = MAP_drawUnitItemBox;
        break;
    case UWAMONO_TYPE_ITEM_SYMBOL:
        unit->flags &= ~MAP_UNIT_NO_DRAW;
        unit->update = MAP_updateUnitItemSymbol;
        break;
    case UWAMONO_TYPE_KOWARE:
        unit->flags &= ~MAP_UNIT_NO_DRAW;
        unit->update = MAP_updateUnitKoware;
        resource = GameLoopState.partsRoot->resource;
        if (unit->serial < 0 || unit->serial >= resource->partCount) {
            printf(breakableCreateErrorFormat, unit->serial);
            unit->serial = -1;
            unit->update = 0;
        } else {
            nmlModelSetPartsVisible(resource, unit->serial, 1);
        }
        break;
    case UWAMONO_TYPE_TRAP:
        unit->flags &= ~MAP_UNIT_NO_DRAW;
        unit->update = MAP_updateUnitTrap;
        break;
    default:
        break;
    }
}

void MAP_updateUnitSaveSymbol(MapUnitRecord *unit)
{
    unit->rotation.y += SAVE_SYMBOL_PHASE_STEP;
    if (CheckDist3D(&GameLoopState.actor->position, &unit->position) < 0.0f)
        return;
}

void MAP_updateUnitShopSymbol(MapUnitRecord *unit)
{
    unit->rotation.y += SHOP_SYMBOL_PHASE_STEP;
    if (CheckDist3D(&GameLoopState.actor->position, &unit->position) < 0.0f)
        return;
}

void MAP_updateUnitSymbol(MapUnitRecord *unit)
{
    unit->rotation.y = unit->rotation.y + UWAMONO_SYMBOL_PHASE_STEP;
}

/* Sets the global wind from the unit's position and height and passes it to EXM_SetWindStruct. */
void SetUwaWind(UwamonoWindUnit *unit)
{
    UwamonoWind *wind = &uwaWind;
    float heightDelta;

    wind->mode = 2;
    __builtin_memcpy(&wind->position,
                     &unit->position, sizeof(Vector4));
    heightDelta = unit->height * 0.5f;
    wind->position.w = WIND_POSITION_W;
    wind->decay = WIND_DECAY;
    wind->strength = 1.0f;
    wind->timer = 0;
    wind->position.y += heightDelta;
    EXM_SetWindStruct(wind);
}

/* Plays the break sound: the unit's own sound id when set, otherwise the one for its material. */
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
        xglSoundEffectNormalID(UWAMONO_SE_BREAK_MATERIAL0, channel);
        break;
    case 1:
        xglSoundEffectNormalID(UWAMONO_SE_BREAK_MATERIAL1, channel);
        break;
    case 2:
        xglSoundEffectNormalID(UWAMONO_SE_BREAK_MATERIAL2, channel);
        break;
    default:
        xglSoundEffectNormalID(UWAMONO_SE_BREAK_MATERIAL0, channel);
        break;
    }
}

/*
 * Shows the cursor effect of a selectable unit: the aimed cursor on the aimed
 * unit, the plain cursor on the others, and none when the unit is signalled or
 * not selectable.
 */
void DrawActiveCursol(MapUnitRecord *argument)
{
    DrawActiveCursorUnit *unit = (DrawActiveCursorUnit *)argument;
    UwamonoEffectUnit *effects = (UwamonoEffectUnit *)argument;
    SchedulerState *effect;
    float scale;
    /* effectCf[1] is the plain cursor and effectCf[2] the aimed cursor. */
    if (unit->signal == 1 || !(unit->flags & MAP_UNIT_SELECTABLE)) {
        if (effects->effectCf[1] != 0) {
            sefDeleteEffectCf(effects->effectCf[1]);
            effects->effectCf[1] = 0;
        }
        if (effects->effectCf[2] != 0) {
            sefDeleteEffectCf(effects->effectCf[2]);
            effects->effectCf[2] = 0;
        }
        return;
    } else if (GameLoopState.aimMapUnitIndex == unit->aimIndex) {
        if (effects->effectCf[1] != 0) {
            sefDeleteEffectCf(effects->effectCf[1]);
            effects->effectCf[1] = 0;
        }
        if (effects->effectCf[2] == 0) {
            effect = sefCreateEffectCf(UWAMONO_EFFECT_CURSOR_AIMED, 0, 0);
            if (effect != 0) {
                scale = (unit->cursorScale[0] + unit->cursorScale[1] +
                                unit->cursorScale[2]) / 3.0f;
                scale = scale * activeCursolMinScale;

                effect->casterUnit = argument;
                effect->flags = (effect->flags & ~0x400u) | 4;
                if (scale < activeCursolMinScale)
                    scale = activeCursolMinScale;
                if (scale > 2.0f)
                    scale = 2.0f;
                effects->effectCf[2] = (int)effect;
            } else {
                return;
            }
        } else {
            return;
        }
    } else {
        if (effects->effectCf[2] != 0) {
            sefDeleteEffectCf(effects->effectCf[2]);
            effects->effectCf[2] = 0;
        }
        if (effects->effectCf[1] == 0) {
            effect = sefCreateEffectCf(UWAMONO_EFFECT_CURSOR, 0, 0);
            if (effect != 0) {
                scale = (unit->cursorScale[0] + unit->cursorScale[1] +
                                unit->cursorScale[2]) / 3.0f;
                scale = scale * activeCursolScaleFloor;

                effect->casterUnit = argument;
                effect->flags = (effect->flags & ~0x400u) | 4;
                if (scale < activeCursolScaleFloor)
                    scale = activeCursolScaleFloor;
                if (scale > 2.0f)
                    scale = 2.0f;
                effects->effectCf[1] = (int)effect;
            } else {
                return;
            }
        } else {
            return;
        }
    }
    effect->scale.z = scale;
    effect->scale.y = scale;
    effect->scale.x = scale;
}

/* Returns the index of the first active, visible map unit whose collision test hits the actor, or -1. */
int HitCheckMapUnit(void *actor)
{
    int index = 0;

    do {
        struct UwamonoMapCollisionState *state = &MapUnit[index].state;
        UwamonoMapSlot *unit = &MapUnit[index];
        if (unit->serial != -1) {
            unsigned int flags = unit->flags;
            if (!(flags & MAP_UNIT_HIDDEN) && state->crossType &&
                (flags & MAP_UNIT_ACTIVE) &&
                HitCheckMapUnitAt(actor, (UwamonoCollisionUnit *)unit)) {
                return index;
            }
        }
        ++index;
    } while (index < MAP_UNIT_COUNT);
    return -1;
}

/*
 * Collision test that pushes the actor out of the map units it hits. Returns 1
 * when the actor was pushed clear of every hit unit, otherwise 0.
 */
int HitCheckMapUnitWithNyuru(UwamonoCollisionActor *actor)
{
    int candidates[MAP_UNIT_COUNT];
    int count;
    int index;
    int candidate;
    AlignedVector4 position;
    AlignedVector4 displacement;

    for (index = MAP_UNIT_COUNT - 1; index >= 0; index--) {
        candidates[index] = -1;
    }

    count = 0;
    index = 0;
    for (; index < MAP_UNIT_COUNT; index++) {
        struct UwamonoMapCollisionState *state = &MapUnit[index].state;
        UwamonoMapSlot *unit = &MapUnit[index];
        if (unit->serial != -1 && !(unit->flags & MAP_UNIT_HIDDEN) &&
            state->crossType != 0 && (unit->flags & MAP_UNIT_ACTIVE) &&
            HitCheckMapUnitAt(actor,
                              (UwamonoCollisionUnit *)unit)) {
            candidates[count] = index;
            count++;
        }
    }

    if (count == 0) {
        return 0;
    }
    if (count == 1) {
        return HitCheckNyuru(actor, (UwamonoCollisionUnit *)&MapUnit[candidates[0]]);
    }

    /* Try each candidate in turn: keep the first push that clears every unit, otherwise restore the position and cancel the move. */
    if (candidates[0] != -1) {
        candidate = 0;
        while (candidates[candidate] != -1) {
            UwamonoMapSlot *unit = &MapUnit[candidates[candidate]];

            position = actor->position;
            displacement = actor->displacement;
            switch (unit->state.crossType) {
            case UWAMONO_SHAPE_CIRCLE:
                NyuruCircle(actor,
                            (UwamonoCollisionUnit *)unit);
                break;
            case UWAMONO_SHAPE_BOX:
                NyuruMatrix(actor, (UwamonoCollisionUnit *)unit, 1);
                break;
            }

            if (HitCheckMapUnit(actor) == -1) {
                return 1;
            }

            actor->position = position;
            actor->displacement = displacement;
            candidate++;
        }
    }

    actor->position.x -= actor->displacement.x;
    actor->position.z -= actor->displacement.z;
    actor->displacement.x = 0.0f;
    actor->displacement.z = 0.0f;
    return 0;
}

/* Like HitCheckMapUnit, but scans downward from MAP_UNIT_COUNT and stops before index 0. */
int HitCheckMapUnitReverse(void *actor)
{
    int index = MAP_UNIT_COUNT;

    do {
        struct UwamonoMapCollisionState *state = &MapUnit[index].state;
        UwamonoMapSlot *unit = &MapUnit[index];
        if (unit->serial != -1) {
            unsigned int flags = unit->flags;
            if (!(flags & MAP_UNIT_HIDDEN) && state->crossType &&
                (flags & MAP_UNIT_ACTIVE) &&
                HitCheckMapUnitAt(actor, (UwamonoCollisionUnit *)unit))
                return index;
        }
        --index;
    } while (index > 0);
    return -1;
}

/*
 * Tests a point against the map units: HitCheckMapUnitPosSize with a radius
 * of 0.0f.
 */
int HitCheckMapUnitPos(const Vector4 *position) {
    return HitCheckMapUnitPosSize(position, 0.0f);
}

/* Returns the index of the first active map unit that a point with the given radius hits, or -1. */
int HitCheckMapUnitPosSize(const Vector4 *position, float radius)
{
    UwamonoCollisionActor probe;
    int index = 0;

    probe.position = *position;
    probe.radius = radius;
    do {
        struct UwamonoMapCollisionState *state = &MapUnit[index].state;
        UwamonoMapSlot *unit = &MapUnit[index];
        if (unit->serial != -1 && state->crossType &&
            (unit->flags & MAP_UNIT_ACTIVE) &&
            HitCheckMapUnitPosAt(&probe, (UwamonoCollisionUnit *)unit)) {
            return index;
        }
        ++index;
    } while (index < MAP_UNIT_COUNT);
    return -1;
}

/* Like HitCheckMapUnitPosSize with a zero radius, but scans downward from MAP_UNIT_COUNT and stops before index 0. */
int HitCheckMapUnitPosReverse(Vector4 *position)
{
    UwamonoCollisionActor probe;
    int index = MAP_UNIT_COUNT;

    probe.position = *position;
    probe.radius = 0.0f;
    do {
        struct UwamonoMapCollisionState *state = &MapUnit[index].state;
        UwamonoMapSlot *unit = &MapUnit[index];
        if (unit->serial != -1 && state->crossType &&
            (unit->flags & MAP_UNIT_ACTIVE) &&
            HitCheckMapUnitAt(&probe, (UwamonoCollisionUnit *)unit))
            return index;
        --index;
    } while (index > 0);
    return -1;
}

/* Tests the actor against one map unit by height, then by its circle or box shape. Returns 1 on a hit. */
int HitCheckMapUnitAt(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit)
{
    UwamonoMapUnitState *state;

    if (unit->position.vector.y + unit->dimensions.y * mapUnitHitHeightScale < actor->position.y) {
        return 0;
    }

    state = &unit->state;
    if (actor->position.y < unit->position.vector.y - mapUnitHitBelowMargin) {
        return 0;
    }

    switch (state->state) {
    case UWAMONO_SHAPE_CIRCLE: {
        float distance = CheckDist2D(&actor->position, &unit->position.vector);
        float collisionRadius = actor->radius + unit->dimensions.x;
        if (!(distance < collisionRadius)) {
            return 0;
        }
        return 1;
    }
    case UWAMONO_SHAPE_BOX:
        return HitCheckBox(actor, (struct UwamonoBoxUnit *)unit);
    }
    return 0;
}

/* Tests a point against one map unit by height, then by its shape. Returns nonzero on a hit. */
unsigned int HitCheckMapUnitPosAt(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit)
{
    UwamonoMapUnitState *state = &unit->state;
    float unitY = unit->position.vector.y;
    float actorY = actor->position.y;
    signed char shape;

    if (unitY + unit->dimensions.y * mapUnitPosHitHeightScale < actorY) {
        return 0;
    }
    if (actorY < unitY - mapUnitPosHitBelowMargin) {
        return 0;
    }
    shape = state->state;
    switch (shape) {
    case UWAMONO_SHAPE_CIRCLE:
        if (CheckDist2D(&actor->position, &unit->position.vector) <
            actor->radius + unit->dimensions.x) {
            return 1;
        }
        return 0;
    case UWAMONO_SHAPE_BOX:
        return HitCheckBoxCorner(actor, unit);
    default:
        return 0;
    }
}

/* Tests a circle actor against a box unit in the box's own X and Z axes. Returns nonzero when they overlap. */
int HitCheckBox(UwamonoCollisionActor *actor, struct UwamonoBoxUnit *boxUnit)
{
    const float (*basis)[4];
    UwamonoCornerCollisionUnit *unit = (UwamonoCornerCollisionUnit *)boxUnit;
    AlignedVector4 dimensions = unit->halfSize;
    DoublewordVector4 position;
    float unitX;
    float actorX;
    float actorZ;
    float basisX;
    float basisZ;
    float halfWidth;
    float margin;
    float testDistance;
    float depth;
    float depthSquared;
    float differenceX;
    float differenceZ;
    float axisX;
    float deltaZ;

    position.words[1] = unit->position.words[1];
    position.words[0] = unit->position.words[0];
    /* Begin with unit-relative X before reconstructing projected world X. */
    unitX = position.vector.x;
    actorX = actor->position.x;
    actorZ = actor->position.z;
    deltaZ = actorZ - position.vector.z;
    basis = unit->basis;
    axisX = actorX - unitX;
    /* The saved transform's first row supplies the horizontal box axis. */
    testDistance = axisX * basis[0][0] + deltaZ * basis[0][2];
    halfWidth = dimensions.x;
    margin = actor->radius * UWAMONO_RADIUS_SCALE;
    if (halfWidth + margin < __builtin_fabsf(testDistance)) {
        return 0;
    }
    depth = dimensions.z + margin;
    depthSquared = depth * depth;
    basisZ = basis[0][2];
    basisX = basis[0][0];
    axisX = testDistance * basisX + unitX;
    differenceX = actorX - axisX;
    deltaZ = testDistance * basisZ + position.vector.z;
    differenceZ = actorZ - deltaZ;
    /* The final test measures squared residual distance from the box axis. */
    testDistance = differenceX * differenceX + differenceZ * differenceZ;
    return testDistance < depthSquared;
}

/* Tests an actor against a box unit, falling back to HitCheckCorner when the actor is beyond the box's width. */
int HitCheckBoxCorner(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit)
{
    UwamonoMapSlot *box = (UwamonoMapSlot *)unit;
    int outsideWidth = 0;
    AlignedVector4 dimensions = box->dimensions;
    DoublewordVector4 position;
    struct UwamonoBoxBasis *basis;
    struct UwamonoBoxBasis **basisSlot;
    float basisX;
    float basisZ;
    float actorX;
    float actorZ;
    float testDistance;
    float offsetX;
    float offsetZ;
    float differenceX;
    float differenceZ;
    float depth;

    position.words[1] = unit->position.words[1];
    position.words[0] = unit->position.words[0];
    actorX = actor->position.x;
    actorZ = actor->position.z;
    basisSlot = &box->basis;
    basis = *basisSlot;
    basisX = basis->x;
    basisZ = basis->z;
    offsetX = actorX - position.vector.x;
    offsetZ = actorZ - position.vector.z;
    testDistance = offsetX * basisX + offsetZ * basisZ;
    if (dimensions.x + actor->radius * UWAMONO_RADIUS_SCALE < __builtin_fabsf(testDistance)) {
        return 0;
    }
    if (dimensions.x < __builtin_fabsf(testDistance)) {
        outsideWidth = 1;
    }
    offsetX = testDistance * basisX + position.vector.x;
    differenceX = actorX - offsetX;
    offsetZ = testDistance * basisZ + position.vector.z;
    differenceZ = actorZ - offsetZ;
    depth = dimensions.z + actor->radius * UWAMONO_RADIUS_SCALE;
    testDistance = differenceX * differenceX + differenceZ * differenceZ;
    if (testDistance < depth * depth) {
        if (dimensions.z * dimensions.z < testDistance &&
            outsideWidth == 1) {
            return HitCheckCorner(actor, unit) != 0;
        }
        return 1;
    }
    return 0;
}

/* Tests the actor against the four corners of a box unit; returns 1 when any corner is within the actor's radius. */
int HitCheckCorner(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit)
{
    UwamonoCornerCollisionUnit *box = (UwamonoCornerCollisionUnit *)unit;
    const float (*basis)[4] = box->basis;
    Vector4 corners[4];
    DoublewordVector4 center = box->position;
    AlignedVector4 halfSize = box->halfSize;

    corners[0].x = center.vector.x + (halfSize.x * basis[0][0] + halfSize.z * basis[2][0]);
    corners[0].y = center.vector.y;
    corners[0].z = center.vector.z + (halfSize.x * basis[0][2] + halfSize.z * basis[2][2]);
    if (CheckDist2D(&actor->position, &corners[0]) < actor->radius * UWAMONO_RADIUS_SCALE) {
        return 1;
    }

    corners[1].x = center.vector.x - (halfSize.z * basis[0][2] + halfSize.x * basis[2][2]);
    corners[1].y = center.vector.y;
    corners[1].z = center.vector.z + (halfSize.z * basis[0][0] + halfSize.x * basis[2][0]);
    if (CheckDist2D(&actor->position, &corners[1]) < actor->radius * UWAMONO_RADIUS_SCALE) {
        return 1;
    }

    corners[2].x = center.vector.x - (halfSize.x * basis[0][0] + halfSize.z * basis[2][0]);
    corners[2].y = center.vector.y;
    corners[2].z = center.vector.z - (halfSize.x * basis[0][2] + halfSize.z * basis[2][2]);
    if (CheckDist2D(&actor->position, &corners[2]) < actor->radius * UWAMONO_RADIUS_SCALE) {
        return 1;
    }

    corners[3].x = center.vector.x + (halfSize.z * basis[0][2] + halfSize.x * basis[2][2]);
    corners[3].y = center.vector.y;
    corners[3].z = center.vector.z - (halfSize.z * basis[0][0] + halfSize.x * basis[2][0]);
    return CheckDist2D(&actor->position, &corners[3]) < actor->radius * UWAMONO_RADIUS_SCALE;
}

/*
 * Debug probe for a box unit: holding circle or cross turns the unit about Y, and
 * the box matrix and distances are printed to the debug font. Always returns 0.
 */
int HitCheckBoxTest(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit)
{
    UwamonoCrossBasis basisCopy;
    float (*basis)[4];
    AlignedVector4 dimensions;
    DoublewordVector4 center;
    float nearestX;
    float nearestY;
    float nearestZ;
    float dx;
    float dy;
    float dz;
    float radiusSquared;
    float distance;

    basis = unit->basis->rows;
    basisCopy = *unit->basis;
    dimensions = unit->dimensions;
    if (PadData.half_28 & PAD_CIRCLE) {
        unit->rotationY += 0.05f;
        xglMatrixStackUnit();
        xglMatrixStackTrans(&unit->position.vector.x);
        xglMatrixStackRotX(unit->rotationX);
        xglMatrixStackRotY(unit->rotationY);
        xglMatrixStackRotZ(unit->rotationZ);
        xglMatrixStackSave(unit->matrix);
    }
    if (PadData.half_28 & PAD_CROSS) {
        unit->rotationY -= 0.05f;
        xglMatrixStackUnit();
        xglMatrixStackTrans(&unit->position.vector.x);
        xglMatrixStackRotX(unit->rotationX);
        xglMatrixStackRotY(unit->rotationY);
        xglMatrixStackRotZ(unit->rotationZ);
        xglMatrixStackSave(unit->matrix);
    }

    /* Print the four matrix rows to the debug font. */
    xglFontDebugPrintf(8, 0x80, hitCheckMatrixFormat,
                       basis[0][0],
                       basis[0][1],
                       basis[0][2]);
    xglFontDebugPrintf(8, 0x88, hitCheckMatrixFormat,
                       basis[1][0],
                       basis[1][1],
                       basis[1][2]);
    xglFontDebugPrintf(8, 0x90, hitCheckMatrixFormat,
                       basis[2][0],
                       basis[2][1],
                       basis[2][2]);
    xglFontDebugPrintf(8, 0x98, hitCheckPosFormat,
                       basis[3][0],
                       basis[3][1],
                       basis[3][2]);

    center = unit->position;
    center.vector.x = unit->position.vector.x -
               ((dimensions.x * basis[0][0] +
                 dimensions.z * basis[2][0]) * 0.5f);
    center.vector.y = unit->position.vector.y;
    center.vector.z = unit->position.vector.z -
               ((dimensions.x * basis[0][2] +
                 dimensions.z * basis[2][2]) * 0.5f);
    basisCopy.anchor[0] = center.vector.x;
    basisCopy.anchor[1] = center.vector.y + 2.0f;
    basisCopy.anchor[2] = center.vector.z;
    basisCopy.anchor[0] = unit->position.vector.x + basis[0][0];
    basisCopy.anchor[1] = unit->position.vector.y + 2.0f;
    basisCopy.anchor[2] = unit->position.vector.z + basis[0][2];
    basisCopy.anchor[0] = unit->position.vector.x + basis[2][0];
    basisCopy.anchor[1] = unit->position.vector.y + 2.0f;
    basisCopy.anchor[2] = unit->position.vector.z + basis[2][2];
    nearestX = actor->position.x - center.vector.x;
    nearestY = actor->position.y - center.vector.y;
    nearestZ = actor->position.z - center.vector.z;
    distance = (nearestX * basis[0][0]) + (nearestY * basis[0][1]) + (nearestZ * basis[0][2]);
    xglFontDebugPrintf(8, 0x3C, hitCheckLenFormat, distance);

    if (distance < 0.0f) {
        return 0;
    }
    if (unit->dimensions.x < distance) {
        return 0;
    }
    nearestX = distance * basis[0][0] + center.vector.x;
    nearestY = distance * basis[0][1] + center.vector.y;
    nearestZ = distance * basis[0][2] + center.vector.z;
    xglFontDebugPrintf(8, 0x18, hitCheckXFormat, nearestX);
    xglFontDebugPrintf(8, 0x20, hitCheckYFormat, nearestY);
    xglFontDebugPrintf(8, 0x28, hitCheckZFormat, nearestZ);

    dx = actor->position.x - nearestX;
    dy = actor->position.y - nearestY;
    dz = actor->position.z - nearestZ;
    radiusSquared = unit->dimensions.z * unit->dimensions.z;
    distance = (dx * dx) + (dy * dy) + (dz * dz);
    xglFontDebugPrintf(8, 0x54, hitCheckUFormat, dx);
    xglFontDebugPrintf(8, 0x5C, hitCheckVFormat, dy);
    xglFontDebugPrintf(8, 0x64, hitCheckWFormat, dz);
    xglFontDebugPrintf(8, 0x6C, hitCheckLenRadiusFormat,
                       distance, radiusSquared);
    if (distance < radiusSquared) {
        xglFontDebugPrintf(8, 8, matrixHitLabel);
    }

    return 0;
}

/* Pushes the actor out of one unit (circle or box). Returns 1 when the push leaves it clear of every map unit, otherwise 0. */
int HitCheckNyuru(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit)
{
    AlignedVector4 position = actor->position;
    AlignedVector4 displacement = actor->displacement;
    int mapIndex = -1;

    switch (unit->state.state) {
    case UWAMONO_SHAPE_CIRCLE:
        if (!NyuruCircle(actor, unit)) {
            return 0;
        }
        mapIndex = HitCheckMapUnit(actor);
        break;
    case UWAMONO_SHAPE_BOX:
        if (!NyuruMatrix(actor, unit, 0)) {
            return 0;
        }
        mapIndex = HitCheckMapUnit(actor);
        break;
    }
    if (mapIndex != -1) {
        actor->position = position;
        actor->displacement = displacement;
        actor->position.x -= actor->displacement.x;
        actor->position.z -= actor->displacement.z;
        actor->displacement.x = 0.0f;
        actor->displacement.z = 0.0f;
        return 0;
    }
    return 1;
}

/*
 * Slides the actor around a circular unit along its displacement, then checks the
 * ground height there. Returns 0 and undoes the move when no ground height is found.
 */
static int NyuruCircle(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit)
{
    float gapZ;
    float gapX;
    float displacementLength;
    float displacementAngle;
    float movedAngle;
    float scale;
    float angleCorrection;
    float combinedRadius;
    float distance;
    Vector4 target;
    float height;
    Vector4 direction;

    displacementLength = __builtin_sqrtf(actor->displacement.x * actor->displacement.x + actor->displacement.z * actor->displacement.z);
    scale = displacementLength;
    direction.x = actor->displacement.x / scale;
    direction.z = actor->displacement.z / scale;

    displacementAngle = Get_Angle(&actor->position, &unit->position.vector);
    (void)atan2f(actor->displacement.x, actor->displacement.z);
    combinedRadius = actor->radius + unit->dimensions.x;
    actor->position.x -= actor->displacement.x;
    actor->position.z -= actor->displacement.z;
    movedAngle = Get_Angle(&actor->position, &unit->position.vector);

    target.x = unit->position.vector.x - combinedRadius * xglSin(displacementAngle);
    target.z = unit->position.vector.z - combinedRadius * xglCos(displacementAngle);
    gapX = actor->position.x - target.x;
    gapZ = actor->position.z - target.z;
    distance = __builtin_sqrtf(gapX * gapX + gapZ * gapZ);
    scale = (displacementLength - distance) * 0.25f;
    angleCorrection = scale / (unit->dimensions.x * 2.0f);
    if (0.0f < movedAngle - displacementAngle) {
        displacementAngle -= angleCorrection;
    } else {
        displacementAngle += angleCorrection;
    }

    combinedRadius = actor->radius + unit->dimensions.x + nyuruCircleRadiusMargin;
    target.x = unit->position.vector.x - combinedRadius * xglSin(displacementAngle);
    target.z = unit->position.vector.z - combinedRadius * xglCos(displacementAngle);
    actor->displacement.x = target.x - actor->position.x;
    actor->displacement.z = target.z - actor->position.z;
    height = UnduCheck(&actor->position, &actor->displacement,
                       &actor->undulation);
    if (height == UNDU_NO_HEIGHT) {
        actor->position.x -= actor->displacement.x;
        actor->position.z -= actor->displacement.z;
        return 0;
    }
    actor->position.y = height;
    return 1;
}

/*
 * Slides the actor along the box unit's X or Z axis, whichever side it is on.
 * Returns 0 when no ground height is found; mode 0 also halves stalled moves.
 */
static int NyuruMatrix(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit, int mode)
{
    UwamonoCrossBasis *basis;
    AlignedVector4 dims;
    float dx = actor->displacement.x;
    float dz = actor->displacement.z;
    float remainingX = actor->position.x - dx;
    float remainingZ = actor->position.z - dz;
    float distanceSquared = dx * dx + dz * dz;
    float distance;
    float relativeX;
    float relativeZ;
    float alongX;
    Vector4 direction;
    float projection;
    UwamonoCrossBasis basisCopy;

    dims = unit->dimensions;
    basis = unit->basis;
    basisCopy = *basis;
    actor->position.x = remainingX;
    actor->position.z = remainingZ;
    relativeX = remainingX - unit->position.vector.x;
    relativeZ = remainingZ - unit->position.vector.z;
    alongX = relativeX * basis->xAxisX + relativeZ * basis->xAxisZ;
    distance = __builtin_sqrtf(distanceSquared);
    if (distance == 0.0f) {
        return 0;
    }
    direction.x = actor->displacement.x / distance;
    direction.z = actor->displacement.z / distance;
    direction.y = 0.0f;

        /* Inside the box's X extent the X axis is the wall to slide along, otherwise the Z axis. */
        if (__builtin_fabsf(alongX) < dims.x + actor->radius * UWAMONO_RADIUS_SCALE) {
        projection = direction.x * basis->xAxisX + direction.z * basis->xAxisZ;
        actor->displacement.x = projection * basis->xAxisX * distance;
        actor->displacement.z = projection * basis->xAxisZ * distance;
        if (__builtin_fabsf(actor->displacement.x) < nyuruMatrixStillThreshold &&
            __builtin_fabsf(actor->displacement.z) < nyuruMatrixStillThreshold && mode == 0) {
            if (alongX > dims.x) {
                actor->displacement.x = basis->xAxisX * distance * 0.5f;
                actor->displacement.z = basis->xAxisZ * distance * 0.5f;
            } else if (alongX < -dims.x) {
                actor->displacement.x = -basis->xAxisX * distance * 0.5f;
                actor->displacement.z = -basis->xAxisZ * distance * 0.5f;
            }
        }
    } else {
        projection = direction.x * basis->zAxisX + direction.z * basis->zAxisZ;
        actor->displacement.x = projection * basis->zAxisX * distance;
        actor->displacement.z = projection * basis->zAxisZ * distance;
        if (__builtin_fabsf(actor->displacement.x) < nyuruMatrixStillEpsilon &&
            __builtin_fabsf(actor->displacement.z) < nyuruMatrixStillEpsilon && mode == 0) {
            alongX = relativeX * basis->zAxisX + relativeZ * basis->zAxisZ;
            if (alongX > dims.z) {
                actor->displacement.x = basis->zAxisX * distance * 0.5f;
                actor->displacement.z = basis->zAxisZ * distance * 0.5f;
            } else if (alongX < -dims.z) {
                actor->displacement.x = -basis->zAxisX * distance * 0.5f;
                actor->displacement.z = -basis->zAxisZ * distance * 0.5f;
            }
        }
    }

    actor->undulation.attributeMask |= 0x4000;
    {
        float height = UnduCheck(&actor->position, &actor->displacement,
                                 &actor->undulation);
        if (height == UNDU_NO_HEIGHT) {
            actor->position.x -= actor->displacement.x;
            actor->position.z -= actor->displacement.z;
            return 0;
        }
        actor->position.y = height;
    }
    return 1;
}

/*
 * Like NyuruMatrix for a box unit. For mode 0 it first tries NyuruCorner when the
 * actor is at a corner. Returns 0 when no ground height is found.
 */
int NyuruBox(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit, int mode)
{
    UwamonoCrossBasis *basis;
    AlignedVector4 dims;
    Vector4 normal;
    UwamonoCrossBasis basisCopy;
    float dx;
    float dz;
    float distance;
    float relativeX;
    float relativeZ;
    float alongX;
    float projection;

    basis = unit->basis;
    basisCopy = *basis;
    dims = unit->dimensions;

    if (mode == 0 && HitCheckCorner(actor, unit)) {
        NyuruCorner(actor, unit);
        return 1;
    }
    dx = actor->displacement.x;
    dz = actor->displacement.z;
    actor->position.x -= dx;
    actor->position.z -= dz;
    relativeX = actor->position.x - unit->position.vector.x;
    relativeZ = actor->position.z - unit->position.vector.z;
    alongX = relativeX * basis->xAxisX + relativeZ * basis->xAxisZ;
    distance = __builtin_sqrtf(actor->displacement.x * actor->displacement.x + actor->displacement.z * actor->displacement.z);
    if (distance == 0.0f) {
        return 0;
    }

    normal.x = actor->displacement.x / distance;
    normal.y = 0.0f;
    normal.z = actor->displacement.z / distance;
    if (__builtin_fabsf(alongX) < dims.x + actor->radius * UWAMONO_RADIUS_SCALE) {
        projection = normal.x * basis->xAxisX + normal.z * basis->xAxisZ;
        actor->displacement.x = projection * basis->xAxisX * distance;
        actor->displacement.z = projection * basis->xAxisZ * distance;
    } else {
        projection = normal.x * basis->zAxisX + normal.z * basis->zAxisZ;
        actor->displacement.x = projection * basis->zAxisX * distance;
        actor->displacement.z = projection * basis->zAxisZ * distance;
    }

    actor->undulation.attributeMask |= 0x4000;
    {
        float height = UnduCheck(&actor->position, &actor->displacement,
                                 &actor->undulation);
        if (height == UNDU_NO_HEIGHT) {
            actor->position.x -= actor->displacement.x;
            actor->position.z -= actor->displacement.z;
            return 0;
        }
        actor->position.y = height;
    }
    return 1;
}

/* Pushes the actor away from the nearest corner of a box unit, treating that corner as a small circle of nyuruCornerRadius. */
static void NyuruCorner(UwamonoCollisionActor *actor, UwamonoCollisionUnit *unit)
{
    UwamonoCrossBasis *basis;
    DoublewordVector4 nearest;
    DoublewordVector4 candidate;
    DoublewordVector4 center;
    AlignedVector4 dimensions;
    UwamonoCollisionUnit adjustedUnit;
    float nearestDistance;
    float distance;
    float actorRadius;
    float adjustedRadius;

    basis = unit->basis;
    center = unit->position;
    dimensions = unit->dimensions;
    /* Start from one corner and keep the nearest of the four. */
    nearest.vector.x = center.vector.x +
                      (dimensions.x * basis->xAxisX +
                       dimensions.z * basis->zAxisX);
    nearest.vector.y = center.vector.y;
    nearest.vector.z = center.vector.z +
                      (dimensions.x * basis->xAxisZ +
                       dimensions.z * basis->zAxisZ);
    candidate.vector.x = center.vector.x -
                        (dimensions.z * basis->xAxisZ +
                         dimensions.x * basis->zAxisZ);
    candidate.vector.y = center.vector.y;
    candidate.vector.z = center.vector.z +
                        (dimensions.z * basis->xAxisX +
                         dimensions.x * basis->zAxisX);
    nearestDistance = CheckDist2D(&actor->position, &nearest.vector);
    distance = CheckDist2D(&actor->position, &candidate.vector);
    if (nearestDistance > distance) {
        nearest = candidate;
    }

    candidate.vector.x = center.vector.x -
                        (dimensions.x * basis->xAxisX +
                         dimensions.z * basis->zAxisX);
    candidate.vector.y = center.vector.y;
    candidate.vector.z = center.vector.z -
                        (dimensions.x * basis->xAxisZ +
                         dimensions.z * basis->zAxisZ);
    nearestDistance = CheckDist2D(&actor->position, &nearest.vector);
    distance = CheckDist2D(&actor->position, &candidate.vector);
    if (nearestDistance > distance) {
        nearest = candidate;
    }

    candidate.vector.x = center.vector.x +
                        (dimensions.z * basis->xAxisZ +
                         dimensions.x * basis->zAxisZ);
    candidate.vector.y = center.vector.y;
    candidate.vector.z = center.vector.z -
                        (dimensions.z * basis->xAxisX +
                         dimensions.x * basis->zAxisX);
    nearestDistance = CheckDist2D(&actor->position, &nearest.vector);
    distance = CheckDist2D(&actor->position, &candidate.vector);
    if (nearestDistance > distance) {
        nearest = candidate;
    }

    adjustedUnit = *unit;
    adjustedUnit.position = nearest;
    actorRadius = actor->radius;
    adjustedRadius = actorRadius * UWAMONO_RADIUS_SCALE;
    adjustedUnit.dimensions.z = adjustedRadius;
    adjustedUnit.dimensions.x = adjustedRadius;
    actor->radius = nyuruCornerRadius;
    NyuruCircle(actor, &adjustedUnit);
    actor->radius = actorRadius;
}

/*
 * Per-frame shot control. While the aim is set and the square button is pressed it
 * starts the shoot motion. During the motion it signals the aimed unit and releases
 * the aim and shoot flags after frame 0.5.
 */
void ShootMapUnit(void)
{
    UwamonoGameLoopState *state = &GameLoopState;
    Actor *actor = state->actor;
    int unitIndex;
    struct UwamonoMapCollisionState *unitState;
    float threshold;

    if (actor->attribute_flags & 0x0F000000) {
        UnlockMapUnit();
        return;
    }
    if (dispflg == 1) {
        DispDebugUwamono();
    }
    if (collflg == 1) {
        DispCollUwamono();
    }

    if (actor->animation.slot.currentDataId != UWAMONO_SHOOT_MOTION) {
        if (!(state->systemFlags & UWAMONO_SYSTEM_SHOOT_BUSY)) {
            AimMapUnit();
            if ((PadData.half_2a & PAD_SQUARE) &&
                state->aimMapUnitIndex != -1) {
                const Vector4 *targetPosition;

                unitIndex = state->aimMapUnitIndex;
                targetPosition = &MapUnit[unitIndex].position;
                actor->target_angle = Get_Angle(&actor->position,
                                               targetPosition);
                {
                    UwamonoShotEffect *effect =
                        (UwamonoShotEffect *)(void *)sefCreateEffectCf(UWAMONO_EFFECT_SHOT, 0, 0);
                    if (effect != 0) {
                        effect->owner = actor;
                        effect->flags &= ~0x400u;
                    }
                }
                actor->animation.slot.speed = shootMotionSpeed;
                state->systemFlags |= UWAMONO_SYSTEM_SHOOT_MOTION | UWAMONO_SYSTEM_SHOOT_BUSY;
                ACT_setMotion(actor, UWAMONO_SHOOT_MOTION);
                actor->animation.slot.flags = actor->animation.slot.flags & ~8;
                xglSoundEffectNormalID(UWAMONO_SE_SHOOT, 0);
            }
        }
        if (actor->animation.slot.currentDataId != UWAMONO_SHOOT_MOTION) {
            return;
        }
    }

    unitIndex = GameLoopState.aimMapUnitIndex;
    unitState = &MapUnit[unitIndex].state;
    if (actor->animation.slot.frame > shootProgressThreshold &&
        actor->animation.slot.frame < 0.36f) {
            Vibration_Set_Weak(4);
    }
    /* Signal the aimed unit once the motion passes the threshold frame. */
    threshold = 0.36f;
    if (threshold < actor->animation.slot.frame) {
        if (unitState->signal == 0) {
            if (printflg != 0) {
                printf(shootMapUnitFormat, GameLoopState.aimMapUnitIndex);
            }
            Enemy_FindByEar(actor, &MapUnit[unitIndex].position);
            unitState->signal = 1;
        }
    }
    /* After frame 0.5 release the aim and shoot flags. */
    if (actor->animation.slot.frame > 0.5f) {
        GameLoopState.aimMapUnitIndex = -1;
        GameLoopState.systemFlags &= ~UWAMONO_SYSTEM_SHOOT_MOTION;
        GameLoopState.systemFlags &= ~UWAMONO_SYSTEM_SHOOT_BUSY;
        actor->animation.slot.flags |= 8;
    }
}

/*
 * Finds where a moving point (from previous to position, with the given radius)
 * first hits a map unit, and writes the stop point to result. Returns 1 on a hit;
 * otherwise returns 0 with result set to previous.
 */
int CrossPointUwamono(const Vector4 *position, const Vector4 *previous,
                      float radius, Vector4 *result)
{
    int candidates[MAP_UNIT_COUNT];
    Vector4 nearStart;
    Vector4 nearEnd;
    Vector4 farStart;
    Vector4 farEnd;
    Vector4 normal;
    Vector4 candidate;
    int count;
    float scaledRadius;
    float reach;
    int empty;
    int i;

    normal.w = 1.0f;
    normal.x = position->x - previous->x;
    normal.z = position->z - previous->z;
    normal.y = 0.0f;
    scaledRadius = radius * UWAMONO_RADIUS_SCALE;
    reach = normal.x * normal.x + normal.y * normal.y + normal.z * normal.z;
    reach = __builtin_sqrtf(reach) + scaledRadius;
    empty = -1;
    for (i = MAP_UNIT_COUNT - 1; i >= 0; i--) {
        candidates[i] = empty;
    }
    count = 0;
    for (i = 0; i < MAP_UNIT_COUNT; i++) {
        struct UwamonoMapCollisionState *state = &MapUnit[i].state;
        UwamonoMapSlot *unit = &MapUnit[i];
        if (unit->serial != -1 && !(unit->flags & MAP_UNIT_HIDDEN) &&
            !(UWAMONO_CROSS_HEIGHT_RANGE < __builtin_fabsf(position->y - unit->position.y)) &&
            state->crossType != 0 && (unit->flags & MAP_UNIT_ACTIVE) &&
            UwamonoAreaCheck(position, unit, reach)) {
            candidates[count] = i;
            count++;
        }
    }

    result->x = CROSS_POINT_FAR;
    result->z = CROSS_POINT_FAR;
    result->y = CROSS_POINT_NONE;
    if (count == 0) {
        *result = *previous;
        return 0;
    }

    xglVectorNormal(&normal, &normal);
    nearStart = *position;
    nearEnd = *previous;
    farStart = *position;
    __builtin_memcpy(&farEnd, previous, sizeof(farEnd));
    farEnd.w = farStart.w = nearEnd.w = nearStart.w = 1.0f;
    nearStart.x += scaledRadius * normal.z;
    nearStart.z -= scaledRadius * normal.x;
    nearEnd.x += scaledRadius * normal.z;
    nearEnd.z -= scaledRadius * normal.x;
    farStart.x -= scaledRadius * normal.z;
    farStart.z += scaledRadius * normal.x;
    farEnd.x -= scaledRadius * normal.z;
    farEnd.z += scaledRadius * normal.x;

    /* Intersect the near and far edges of the swept radius with each candidate, and keep the nearest point. */
    for (i = 0; i < MAP_UNIT_COUNT; i++) {
        UwamonoMapSlot *unit;
        if (candidates[i] == -1) {
            break;
        }
        unit = &MapUnit[candidates[i]];
        CrossPointUwamonoAt(&nearStart, &nearEnd,
                            (MapUnitRecord *)unit, &candidate);
        if (candidate.y != CROSS_POINT_NONE) {
            CheckNearPoint(position, &candidate, result, result);
        }
        CrossPointUwamonoAt(&farStart, &farEnd,
                            (MapUnitRecord *)unit, &candidate);
        if (candidate.y != CROSS_POINT_NONE) {
            CheckNearPoint(position, &candidate, result, result);
        }
        candidate.x = position->x - unit->position.x;
        candidate.z = position->z - unit->position.z;
        if ((normal.x * candidate.x + normal.z * candidate.z) > 0.0f &&
            CheckPointLine(&nearStart, &nearEnd, &unit->position) !=
                CheckPointLine(&farStart, &farEnd, &unit->position)) {
            CrossPointUwamonoAt(position, &unit->position,
                                (MapUnitRecord *)unit, &candidate);
            CheckNearPoint(position, &candidate, result, result);
        }
    }

    if (result->y == CROSS_POINT_NONE) {
        *result = *previous;
        return 0;
    }
    /* Pull the stop point back by the radius along the movement. */
    nearEnd.x = position->x - result->x;
    nearEnd.y = position->y;
    nearEnd.z = position->z - result->z;
    reach = normal.x * nearEnd.x + normal.z * nearEnd.z;
    reach -= scaledRadius;
    if (reach < 0.0f) {
        *result = *position;
        return 1;
    }
    result->x = position->x - reach * normal.x;
    result->y = position->y;
    result->z = position->z - reach * normal.z;
    return 1;
}

/* Intersects a segment with one map unit, using the circle or box test for its shape. */
void CrossPointUwamonoAt(const Vector4 *first, const Vector4 *second, MapUnitRecord *unit, void *result)
{
    switch (unit->trap.windState) {
    case UWAMONO_SHAPE_CIRCLE:
        CalcNearCrossPointCircle(first, second, (UwamonoCollisionUnit *)unit, result);
        return;
    case UWAMONO_SHAPE_BOX:
        CalcNearCrossPointBox(first, second, unit, result);
        break;
    }
}

/* Writes the point where a segment reaches a circular unit's edge; out->y is CROSS_POINT_NONE when the segment misses. */
void CalcNearCrossPointCircle(const Vector4 *from, const Vector4 *to,
                              UwamonoCollisionUnit *unit, Vector4 *out)
{
    Vector4 delta;
    Vector4 direction;
    float distance;
    if (CrossCheckMapUnitCircle(from, to, unit)) {
        delta = *from;
        delta.y = unit->position.vector.y;
        Vu0SubVectorXYZ(&delta, &unit->position, from);
        xglVectorLength(&distance, &delta);
        xglVectorNormal(&direction, &delta);
        distance -= unit->dimensions.x;
        if (distance < 0.0f) {
            *out = *from;
        } else {
            out->x = from->x + distance * direction.x;
            out->y = from->y;
            out->z = from->z + distance * direction.z;
        }
    } else {
        out->y = CROSS_POINT_NONE;
    }
}

/* Writes the nearest crossing of a segment with a box unit's four edges to result; result->y is CROSS_POINT_NONE when there is none. */
void CalcNearCrossPointBox(const Vector4 *first, const Vector4 *second,
                           MapUnitRecord *unit, AlignedVector4 *result)
{
    Vector4 corners[4];
    DoublewordVector4 centerCopy; /* read through the pointer cast below: only the union keeps the 8-byte alignment (ld/sd) there */
    UwamonoAlignedBoxDims dims;
    AlignedVector4 candidate;
    float (*basis)[4] = unit->basis;

    centerCopy = *(DoublewordVector4 *)&unit->position;
    dims = *(UwamonoAlignedBoxDims *)&unit->radius;
    result->y = CROSS_POINT_NONE;
    candidate.y = first->y;

    corners[0].x = centerCopy.vector.x + (dims.value.radius * basis[0][0] + dims.value.boxDepth * basis[2][0]);
    corners[0].y = centerCopy.vector.y;
    corners[0].z = centerCopy.vector.z + (dims.value.radius * basis[0][2] + dims.value.boxDepth * basis[2][2]);
    corners[1].x = centerCopy.vector.x - (dims.value.boxDepth * basis[0][2] + dims.value.radius * basis[2][2]);
    corners[1].y = centerCopy.vector.y;
    corners[1].z = centerCopy.vector.z + (dims.value.boxDepth * basis[0][0] + dims.value.radius * basis[2][0]);
    corners[2].x = centerCopy.vector.x - (dims.value.radius * basis[0][0] + dims.value.boxDepth * basis[2][0]);
    corners[2].y = centerCopy.vector.y;
    corners[2].z = centerCopy.vector.z - (dims.value.radius * basis[0][2] + dims.value.boxDepth * basis[2][2]);
    corners[3].x = centerCopy.vector.x + (dims.value.boxDepth * basis[0][2] + dims.value.radius * basis[2][2]);
    corners[3].y = centerCopy.vector.y;
    corners[3].z = centerCopy.vector.z - (dims.value.boxDepth * basis[0][0] + dims.value.radius * basis[2][0]);
    if (CheckCrossLine(first, second, &corners[0], &corners[1])) {
        CalcCrossPoint(first, second, &corners[0], &corners[1], &candidate);
        *result = candidate;
    }
    if (CheckCrossLine(first, second, &corners[1], &corners[2])) {
        CalcCrossPoint(first, second, &corners[1], &corners[2], &candidate);
        if (result->y != CROSS_POINT_NONE) {
            CheckNearPoint(first, &candidate, result, result);
        } else {
            *result = candidate;
        }
    }
    if (CheckCrossLine(first, second, &corners[2], &corners[3])) {
        CalcCrossPoint(first, second, &corners[2], &corners[3], &candidate);
        if (result->y != CROSS_POINT_NONE) {
            CheckNearPoint(first, &candidate, result, result);
        } else {
            *result = candidate;
        }
    }
    if (CheckCrossLine(first, second, &corners[3], &corners[0])) {
        CalcCrossPoint(first, second, &corners[3], &corners[0], &candidate);
        if (result->y != CROSS_POINT_NONE) {
            CheckNearPoint(first, &candidate, result, result);
        } else {
            *result = candidate;
        }
    }
}

/* Writes whichever of the two candidate points is nearer to origin into out. */
void CheckNearPoint(const Vector4 *origin, const Vector4 *candA, const Vector4 *candB, Vector4 *out)
{
    Vector4 delta;
    float lenA;
    float lenB;

    Vu0SubVectorXYZ(&delta, origin, candA);
    xglVectorLength(&lenA, &delta);

    Vu0SubVectorXYZ(&delta, origin, candB);
    xglVectorLength(&lenB, &delta);

    if (lenA < lenB)
        *out = *candA;
    else
        *out = *candB;
}

/* Returns 1 when a point with the given radius lies inside a unit's circle or box, within the unit's height range. */
int UwamonoAreaCheck(const Vector4 *position, UwamonoMapSlot *unit, float radius)
{
    const struct UwamonoMapCollisionState *state = &unit->state;
    float positionY = position->y;
    float unitY = unit->position.y;
    float lowerHeight;

    if (positionY > unitY + unit->dimensions.y * uwamonoAreaHeightScale)
        return 0;
    lowerHeight = unitY - uwamonoAreaLowerMargin;
    if (positionY < lowerHeight)
        return 0;
    switch (state->crossType) {
    case UWAMONO_SHAPE_CIRCLE: {
        float distance = CheckDist2D(position, &unit->position);
        float limit = radius + unit->dimensions.x;
        if (distance < limit)
            return 1;
        break;
    }
    case UWAMONO_SHAPE_BOX: {
        int boxResult = AreaCheckBox(position, unit, radius);
        if (boxResult != 0)
            return 1;
        break;
    }
    default:
        break;
    }
    return 0;
}

static inline float UwamonoProjectBoxAxisXZ(float x, float z,
                                           Matrix4 *matrix)
{
    return x * (*matrix)[0][0] + z * (*matrix)[0][2];
}

/* Returns 1 when a point with the given radius lies inside a box unit's footprint along its X and Z axes. */
int AreaCheckBox(const Vector4 *position, UwamonoMapSlot *unit, float radius)
{
    AlignedVector4 dimensionsSnapshot;
    float pointX = position->x;
    float pointZ = position->z;
    float unitZ;
    float unitX;
    float offsetX;
    float offsetZ;
    float differenceZ;
    Matrix4 *matrix;
    Matrix4 **matrixReference;
    float basisZ;
    float basisX;
    float projection;
    float depth;

    unitX = unit->position.x;
    unitZ = unit->position.z;
    /* The dimensions copy is not used below: the checks read unit->dimensions directly. */
    dimensionsSnapshot = unit->dimensions;
    offsetX = pointX - unitX;
    offsetZ = pointZ - unitZ;
    /* Keep an addressed reference to the real matrixView member. */
    matrixReference = &unit->matrixView;
    matrix = *matrixReference;
    projection = UwamonoProjectBoxAxisXZ(offsetX, offsetZ, matrix);
    if (unit->dimensions.x + radius < __builtin_fabsf(projection)) {
        return 0;
    }
    depth = unit->dimensions.z + radius;
    basisZ = (*matrix)[0][2];
    basisX = (*matrix)[0][0];
    offsetX = pointX - (projection * basisX + unitX);
    differenceZ = pointZ - (projection * basisZ + unitZ);
    offsetZ = differenceZ * differenceZ;
    if (offsetX * offsetX + offsetZ < depth * depth) {
        return 1;
    }
    return 0;
}

/* Returns the index of the first map unit, other than one on the aim's own channel, that the segment from `from` to the aim position crosses, or -1. */
int CrossCheckMapUnitAim(Vector4 *from, UwamonoScanUnit *aim)
{
    int index = 0;
    int unitOffset = 0;

    do {
        struct UwamonoMapCollisionState *state = &MapUnit[index].state;
        UwamonoScanUnit *scanUnit = &((UwamonoScanUnit *)MapUnit)[index];
        UwamonoScanFields *scan = &scanUnit->scan;

        if (scan->resourceId != -1 && scan->resourceId != 0x7000) {
            UwamonoMapSlot *unit =
                (UwamonoMapSlot *)&((unsigned char *)&MapUnit)[unitOffset];
            unsigned int flags = scanUnit->flags;
            if (!(flags & MAP_UNIT_HIDDEN) && scan->channel != aim->scan.channel &&
                state->crossType && (flags & MAP_UNIT_ACTIVE) &&
                CrossCheckMapUnitAt(from, &aim->position, (UwamonoCrossUnit *)unit)) {
                return index;
            }
        }
        ++index;
        unitOffset += sizeof(MapUnit[0]);
    } while (index < MAP_UNIT_COUNT);
    return -1;
}

/* Returns the index of the first active map unit that the segment from `from` to `to` crosses, or -1. */
int CrossCheckMapUnit(Vector4 *from, Vector4 *to)
{
    int index = 0;

    do {
        struct UwamonoMapCollisionState *state = &MapUnit[index].state;
        UwamonoMapSlot *unit = &MapUnit[index];
        if (unit->serial != -1) {
            unsigned int flags = unit->flags;
            if (!(flags & MAP_UNIT_HIDDEN) && state->crossType &&
                (flags & MAP_UNIT_ACTIVE) &&
                CrossCheckMapUnitAt(from, to, (UwamonoCrossUnit *)unit)) {
                return index;
            }
        }
        ++index;
    } while (index < MAP_UNIT_COUNT);
    return -1;
}

/* Returns the index of the first visible actor, other than the source, whose circle the segment from the source to `to` crosses, or -1. */
int CrossCheckActor(UwamonoEnemyUnit *source, const Vector4 *to)
{
    int index;
    for (index = 0; index < ACTOR_COUNT; ++index) {
        if (!(crossActorHeightLimit < __builtin_fabsf(source->position.y - actor[index].position.y)) &&
            actor[index].resourceId >= 0 && !(actor[index].flags & UWAMONO_ACTOR_HIDDEN) &&
            actor[index].collisionObject != 0 && index != source->actorIndex &&
            CheckCrossCircle(actor[index].radius, &source->position, to, &actor[index].position)) {
            return index;
        }
    }
    return -1;
}

/* Tests a segment against one map unit by height, then by its circle or box shape. */
int CrossCheckMapUnitAt(Vector4 *first, Vector4 *second, UwamonoCrossUnit *unit)
{
    UwamonoCrossState *state = &unit->state;

    if (CROSS_CHECK_MAP_UNIT_HEIGHT < __builtin_fabsf(first->y - unit->position.y))
        return 0;
    switch (state->crossType) {
    case UWAMONO_SHAPE_CIRCLE:
        return CrossCheckMapUnitCircle(first, second, (UwamonoCollisionUnit *)unit);
    case UWAMONO_SHAPE_BOX:
        return CrossCheckMapUnitBox(first, second, unit);
    default:
        return 0;
    }
}

/* Returns 1 when the segment from `from` to `to` passes within a circular unit. */
int CrossCheckMapUnitCircle(const Vector4 *from, const Vector4 *to,
                            UwamonoCollisionUnit *unit)
{
    Vector4 middle;
    Vector4 direction;
    float radius;
    float halfLength;
    float projection;
    float projectedX;
    float offsetX;
    float offsetZ;
    int hit;

    direction.x = to->x - from->x;
    direction.y = 0.0f;
    direction.z = to->z - from->z;
    radius = unit->dimensions.x;
    halfLength = __builtin_sqrtf(direction.x * direction.x +
                                 direction.z * direction.z) * 0.5f;
    /* The normalizer's vector ABI uses w as storage, not an xyz input. */
    xglVectorNormal(&direction, &direction);
    halfLength += radius;
    hit = 0;
    middle.x = from->x + (to->x - from->x) * 0.5f;
    middle.z = from->z + (to->z - from->z) * 0.5f;
    offsetX = unit->position.vector.x - middle.x;
    offsetZ = unit->position.vector.z - middle.z;
    projection = offsetX * direction.x + offsetZ * direction.z;
    if (!(halfLength < __builtin_fabsf(projection))) {
        projectedX = projection * direction.x;
        offsetX = unit->position.vector.x - (projectedX + middle.x);
        offsetZ = unit->position.vector.z - (projection * direction.z + middle.z);
        hit = 1;
        if (!(offsetX * offsetX + offsetZ * offsetZ < radius * radius)) {
            hit = 0;
        }
    }
    return hit;
}

/* Returns 1 when the segment from `first` to `second` crosses any edge of a box unit, widened by crossCheckMapUnitPadding. */
int CrossCheckMapUnitBox(Vector4 *first, Vector4 *second,
                         UwamonoCrossUnit *unit)
{
    Vector4 corners[4];
    AlignedVector4 center;
    DoublewordVector4 extents;
    UwamonoCrossBasis *basis;

    extents.words[1] = unit->dimensions.words[1];
    extents.words[0] = unit->dimensions.words[0];
    center = unit->position;
    basis = unit->basis;
    extents.vector.x += crossCheckMapUnitPadding;
    extents.vector.z += crossCheckMapUnitPadding;
    corners[0].x = center.x + (extents.vector.x * basis->xAxisX + extents.vector.z * basis->zAxisX);
    corners[0].y = center.y;
    corners[0].z = center.z + (extents.vector.x * basis->xAxisZ + extents.vector.z * basis->zAxisZ);

    corners[1].x = center.x - (extents.vector.z * basis->xAxisZ + extents.vector.x * basis->zAxisZ);
    corners[1].y = center.y;
    corners[1].z = center.z + (extents.vector.z * basis->xAxisX + extents.vector.x * basis->zAxisX);
    if (CheckCrossLine(first, second, &corners[0], &corners[1]) != 0) {
        return 1;
    }

    corners[2].x = center.x - (extents.vector.x * basis->xAxisX + extents.vector.z * basis->zAxisX);
    corners[2].y = center.y;
    corners[2].z = center.z - (extents.vector.x * basis->xAxisZ + extents.vector.z * basis->zAxisZ);
    if (CheckCrossLine(first, second, &corners[1], &corners[2]) != 0) {
        return 1;
    }

    corners[3].x = center.x + (extents.vector.z * basis->xAxisZ + extents.vector.x * basis->zAxisZ);
    corners[3].y = center.y;
    corners[3].z = center.z - (extents.vector.z * basis->xAxisX + extents.vector.x * basis->zAxisX);
    if (CheckCrossLine(first, second, &corners[2], &corners[3]) != 0) {
        return 1;
    }
    return CheckCrossLine(first, second, &corners[3], &corners[0]) != 0;
}

/* Clears the selectable flag on every map unit, deletes their cursor effects and clears the aim index. */
void UnlockMapUnit(void)
{
    int unitIndex = MAP_UNIT_COUNT - 1;
    int *effectCf = &MapUnit[0].effectCf[0];

    do {
        MapUnit[MAP_UNIT_COUNT - 1 - unitIndex].flags &= ~MAP_UNIT_SELECTABLE;
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
            effectCf = &MapUnit[MAP_UNIT_COUNT - 1 - unitIndex].effectCf[0];
    } while (unitIndex >= 0);

    GameLoopState.aimMapUnitIndex = -1;
}

/* Returns the index of the selectable map unit nearest the actor within the selection distance, or -1. */
int CheckNearMapUnit(void)
{
    UwamonoScanUnit *unit = (UwamonoScanUnit *)MapUnit;
    Actor *actor = GameLoopState.actor;
    float nearest = GameLoopState.selectionDistance + 1000.0f;
    int selected = -1;
    int index = 0;

    do {
        if (CheckBrokenMapUnit((BrokenMapUnit *)unit) & 1) {
            if (unit->flags & MAP_UNIT_SELECTABLE) {
                float distance = CheckCornerDist(&actor->position, (UwamonoCollisionUnit *)unit);
                if (distance < nearest) {
                    nearest = distance;
                    selected = index;
                }
            }
        }
        ++index;
        ++unit;
    } while (index < MAP_UNIT_COUNT);
    if (nearest < GameLoopState.selectionDistance) {
        return selected;
    }
    return -1;
}

/* Returns the unit's broken id, or 0 when the unit is inactive, already signalled or has no id. */
int CheckBrokenMapUnit(BrokenMapUnit *unit)
{
    BrokenMapUnitState *state = &unit->brokenState;
    int flags;
    int brokenId;

    if (unit->serial == -1)
        return 0;
    flags = unit->flags;
    if (!(flags & MAP_UNIT_ACTIVE))
        return 0;
    if (flags & 4)
        return 0;
    if (state->signal == 1)
        return 0;
    brokenId = state->brokenId;
    return brokenId == -1 ? 0 : brokenId;
}

/* Returns 1 when the aim height check is off, or when the unit is within UWAMONO_AIM_HEIGHT_RANGE of the actor's height. */
int AimHeightCheck(MapUnitRecord *unit)
{
    UwamonoGameLoopState *state = &GameLoopState;
    Actor *heightReference = state->actor;
    if (!(state->aimHeightFlags & UWAMONO_AIM_HEIGHT_CHECK))
        return 1;
    return __builtin_fabsf(heightReference->position.y - unit->position.y) < UWAMONO_AIM_HEIGHT_RANGE;
}

/*
 * Chooses the aim target among the units in range and steps it with R1 and L1.
 * Returns the selected unit index, or the first candidate's index when none is selected.
 */
int AimMapUnit(void)
{
    UwamonoAlignedScreenPoint actorScreen;
    UwamonoAimCandidate candidates[MAP_UNIT_COUNT];
    UwamonoAimCandidate savedCandidate;
    AlignedVector4 actorPosition;
    AlignedVector4 unitPosition;
    int candidateCount;
    int selectedFound;
    UwamonoAimCandidate *candidate;
    UwamonoAimCandidate *list;
    Actor *actor = GameLoopState.actor;
    int index;

    actorPosition = actor->position;
    actorPosition.y += 1.0f;
    xglRotTransPers((Vector4 *)&actorScreen, 0, &actorPosition, 0);
    candidateCount = 0;
    selectedFound = 0;
    candidate = candidates;
    list = candidates;

    for (index = 0; index < MAP_UNIT_COUNT; ++index) {
        unsigned int oldFlags = MapUnit[index].flags;
        float distance;

        MapUnit[index].flags &= ~MAP_UNIT_SELECTABLE;
        candidate->unitIndex = -1;
        if (!(CheckBrokenMapUnit((BrokenMapUnit *)&MapUnit[index]) & 1)) {
            continue;
        }
        if (!AimHeightCheck((MapUnitRecord *)&MapUnit[index])) {
            continue;
        }

        distance = CheckCornerDist(&actor->position,
                                   (UwamonoCollisionUnit *)&MapUnit[index]);
        if (GameLoopState.selectionDistance < distance) {
            continue;
        }
        if (!AimMapUnitLookCheck((UwamonoScanUnit *)&MapUnit[index])) {
            continue;
        }

        if ((oldFlags & MAP_UNIT_SELECTABLE) == 0) {
            xglSoundEffectNormalID(UWAMONO_SE_AIM_LOCK, 0);
        }
        candidate->distance = distance;
        unitPosition = MapUnit[index].position;
        unitPosition.y += 0.5f * MapUnit[index].dimensions.y;
        MapUnit[index].flags = MapUnit[index].flags | MAP_UNIT_SELECTABLE;
        if (GameLoopState.aimMapUnitIndex == index) {
            selectedFound = 1;
        }
        candidate->unitIndex = index;
        xglRotTransPers((Vector4 *)&candidate->screenPoint, 0,
                        &unitPosition, 0);
        ++candidateCount;
        ++candidate;
    }

    if (candidateCount < 2) {
        GameLoopState.aimMapUnitIndex = candidates[0].unitIndex;
        if (candidates[0].unitIndex != -1) {
            SortLine(&actorScreen, &candidates[0].screenPoint, 1);
        }
        return candidates[0].unitIndex;
    }

    /* Bubble-sort the candidates by screen x, restarting after each swap. */
    index = 1;
    if (candidates[1].unitIndex != -1) {
        for (;;) {
            if (candidates[index - 1].screenPoint.value.x > candidates[index].screenPoint.value.x) {
            savedCandidate = candidates[index];
            candidates[index] = candidates[index - 1];
            candidates[index - 1] = savedCandidate;
            index = 0;
            }
            ++index;
            if (!(index < MAP_UNIT_COUNT && candidates[index].unitIndex != -1)) {
                break;
            }
        }
    }

    /* Draw a sight line to each candidate, highlighting the selected one. */
    for (index = 0; index < MAP_UNIT_COUNT - 1 && candidates[index].unitIndex != -1; ++index) {
        if (GameLoopState.aimMapUnitIndex == candidates[index].unitIndex) {
            int offset = index * 48 + 16;

            SortLine(&actorScreen, (UwamonoAlignedScreenPoint *)((char *)candidates + offset), 1);
        } else {
            int offset = index * 48 + 16;

            SortLine(&actorScreen, (UwamonoAlignedScreenPoint *)((char *)candidates + offset), 0);
        }
    }

    if (!selectedFound) {
        GameLoopState.aimMapUnitIndex = CheckNearMapUnit();
    }

    /* R1 and L1 step the selection to the next and previous candidate. */
    if (PadData.half_2a & PAD_R1) {
        xglSoundEffectNormalID(UWAMONO_SE_CURSOR_MOVE, 0);
        for (index = 0; index <= candidateCount; ++index) {
            if (GameLoopState.aimMapUnitIndex == candidates[index].unitIndex) {
                if (candidates[index + 1].unitIndex == -1) {
                    GameLoopState.aimMapUnitIndex = candidates[0].unitIndex;
                } else {
                    GameLoopState.aimMapUnitIndex = candidates[index + 1].unitIndex;
                }
                return GameLoopState.aimMapUnitIndex;
            }
        }
    }

    if (PadData.half_2a & PAD_L1) {
        xglSoundEffectNormalID(UWAMONO_SE_CURSOR_MOVE, 0);
        for (index = candidateCount - 1; index >= 0; --index) {
            if (GameLoopState.aimMapUnitIndex == candidates[index].unitIndex) {
                if (index == 0) {
                    GameLoopState.aimMapUnitIndex =
                        candidates[candidateCount - 1].unitIndex;
                } else {
                    GameLoopState.aimMapUnitIndex = candidates[index - 1].unitIndex;
                }
                return GameLoopState.aimMapUnitIndex;
            }
        }
    }

    return candidates[0].unitIndex;
}

/*
 * Returns 1 when the aim target is in view from the actor: no other unit blocks the
 * line (when UWAMONO_AIM_LOOK_UNITS is set), and the ground check reaches the
 * target (when UWAMONO_AIM_LOOK_GROUND is set).
 */
int AimMapUnitLookCheck(UwamonoScanUnit *target)
{
    Actor *listener = GameLoopState.actor;
    UwamonoAimQuery query;
    AlignedVector4 position;
    Vector4 direction;
    float result;

    if ((GameLoopState.aimHeightFlags & UWAMONO_AIM_LOOK_UNITS) != 0 &&
        CrossCheckMapUnitAim(&listener->position, target) != -1) {
        return 0;
    }
    if ((GameLoopState.aimHeightFlags & UWAMONO_AIM_LOOK_GROUND) == 0) {
        return 1;
    }

    UnduParamInit(&query);
    query.header = UnduDataGetHeader(0x305, UNDU_HEADER_DIRECT);
    if (query.header == 0) {
        query.header = UnduDataGetHeader(0, UNDU_HEADER_DIRECT);
        if ((listener->status_flags & 0x20) != 0) {
            query.attributeMask |= UNDU_QUERY_HEIGHT_TABLE;
        }
    }
    position = listener->position;
    direction.x = target->position.x - listener->position.x;
    direction.y = target->position.y - listener->position.y;
    direction.z = target->position.z - listener->position.z;
    direction.w = 1.0f;
    query.attributeMask = 0x1000 | UNDU_QUERY_FLOOR_BELOW | UNDU_QUERY_SAME_LEVEL;
    result = UnduCheck(&position, &direction, &query);
    if (result == UNDU_NO_HEIGHT) {
        return 0;
    }
    if (!(__builtin_fabsf(position.x - target->position.x) < aimMapUnitLookEpsilon)) {
        return 0;
    }
    return __builtin_fabsf(position.z - target->position.z) < aimMapUnitLookEpsilon;
}

/* Returns the distance from a point to a unit's outline: to the circle's edge, or for a box to its nearest corner. */
static float CheckCornerDist(const Vector4 *position, UwamonoCollisionUnit *unit)
{
    AlignedVector4 corner;
    float distance;

    switch (unit->state.state) {
    case UWAMONO_SHAPE_CIRCLE:
        distance = CheckDist2D(position, &unit->position.vector) - unit->dimensions.x;
        break;
    case UWAMONO_SHAPE_BOX:
        corner = unit->dimensions;
        {
            float offsetX = __builtin_fabsf(position->x - unit->position.vector.x);
            float offsetZ;

            if (offsetX < corner.x) {
                corner.x = offsetX;
            }
            offsetZ = __builtin_fabsf(position->z - unit->position.vector.z);
            if (offsetZ < corner.z) {
                corner.z = offsetZ;
            }
        }
        distance = CheckDist2D(position, &unit->position.vector);
        distance -= __builtin_sqrtf(corner.x * corner.x +
                                    corner.z * corner.z);
        break;
    default:
        distance = CheckDist2D(position, &unit->position.vector);
        break;
    }
    return distance;
}

/*
 * Per-frame update shared by the uwamono units: runs the sequence update, applies
 * the height check, copies the position to the linked unit and plays the BGM when
 * its timer runs.
 */
void UwamonoCommonFunc(UwamonoCommonUnit *unit)
{
    UwamonoTimers *timers = &unit->timers;

    if (unit->serial < UWAMONO_SERIAL_RESOURCE_BASE) {
        MAP_updateUnitPartsSequence(unit);
    } else {
        MAP_updateUnitSequence(unit);
    }

    if (timers->heightCheckFlag != 0) {
        CheckUwamonoHeight(unit);
    }

    unit->linkedUnit->position.x = unit->position.x;
    unit->linkedUnit->position.y = unit->position.y;
    unit->linkedUnit->position.z = unit->position.z;

    if (timers->bgmTimer > 0) {
        UwamonoBgmFunc(unit);
    }
}

/* Plays the unit's BGM sound at its position, or, when projectSound is set, at the point of its direction line nearest the actor. */
void UwamonoBgmFunc(UwamonoCommonUnit *unit)
{
    UwamonoTimers *timers = &unit->timers;
    AlignedVector4 soundPosition;

    if (timers->projectSound == 0) {
        soundPosition = unit->position;
    } else {
        float x = unit->position.x;
        float y = unit->position.y;
        float z = unit->position.z;
        UwamonoLinkedUnit *linkedUnit = unit->linkedUnit;
        float projection =
            (GameLoopState.actor->position.x - x) * linkedUnit->direction[0] +
            (GameLoopState.actor->position.y - y) * linkedUnit->direction[1] +
            (GameLoopState.actor->position.z - z) * linkedUnit->direction[2];

        soundPosition.x = x + projection * linkedUnit->direction[0];
        soundPosition.y = y + projection * linkedUnit->direction[1];
        soundPosition.z = z + projection * linkedUnit->direction[2];
        soundPosition.w = 1.0f;
    }
    xglSoundEffectPosID(timers->bgmTimer, &soundPosition.x, 1, unit->seChannel + 1);
}

/* Fades out the BGM of every active unit that has one. */
void UwamonoBgmFadeOut(void)
{
    MapUnitRecord *unit;
    int i;
    int bgmTimer;
    int bank;
    unsigned short handle;

    unit = (MapUnitRecord *) MapUnit;
    for (i = 0; i < MAP_UNIT_COUNT; i++, unit++) {
        if (!(unit->flags & MAP_UNIT_ACTIVE)) {
            continue;
        }
        bgmTimer = unit->bgmTimer;
        if (bgmTimer < 0) {
            continue;
        }
        if (bgmTimer > SOUND_ID_MAX) {
            continue;
        }
        bank = bgmTimer >> 16;
        handle = SoundWork.effectBanks[bank].handle;
        if (handle == SOUND_BANK_EMPTY) {
            continue;
        }
        SsdFadeoutEffect((handle << 16) + (bgmTimer & SOUND_ID_ENTRY_MASK), UWAMONO_BGM_FADE_FRAMES, unit->slot + 1);
    }
}

/* Applies gravity to the unit, then bounces it off the ground or off a unit above it. */
void CheckUwamonoHeight(UwamonoCommonUnit *unit)
{
    UwamonoTimers *timers = &unit->timers;
    float floorHeight;
    UwamonoAimQuery query;

    UnduParamInit(&query);
    query.header = UnduDataGetHeader(0, UNDU_HEADER_DIRECT);
    if (timers->heightBounce != 0) {
        query.attributeMask |= UNDU_QUERY_HEIGHT_TABLE;
    } else {
        query.attributeMask = 0x1000 | UNDU_QUERY_SAME_LEVEL;
    }

    floorHeight = UnduCheck(&unit->position, 0, &query);
    if (unit->heightVelocity > uwamonoMaxFallVelocity) {
        unit->heightVelocity -= uwamonoGravity;
    }

    unit->position.y += unit->heightVelocity;
    if (floorHeight > unit->position.y) {
        unit->heightVelocity = -unit->heightVelocity * UWAMONO_BOUNCE_DAMPING;
        unit->position.y = floorHeight + uwamonoFloorOffset;
        if (unit->heightVelocity < uwamonoLandStopVelocity) {
            unit->flags |= MAP_UNIT_LANDED;
            unit->heightVelocity = 0.0f;
        }
    } else {
        int hitIndex = HitCheckUwamono((UwamonoScanUnit *)unit);
        float hitHeight;
        float reboundVelocity;
        if (hitIndex == -1) {
            return;
        }
        hitHeight = MapUnit[hitIndex].position.y +
                    MapUnit[hitIndex].dimensions.y + uwamonoHitHeightPadding;
        reboundVelocity = -unit->heightVelocity;
        unit->heightVelocity = UWAMONO_BOUNCE_DAMPING * reboundVelocity;
        unit->position.y = hitHeight;
        if (unit->heightVelocity < uwamonoBounceStopVelocity) {
            unit->heightVelocity = 0.0f;
        }
    }
}

/* Returns the index of the first unbroken, visible, active map unit on another channel that the actor's box hits, or -1. */
int HitCheckUwamono(UwamonoScanUnit *actor)
{
    int index = 0;
    unsigned int callByteOffset = 0;

    do {
        struct UwamonoMapCollisionState *state = &MapUnit[index].state;
        UwamonoScanUnit *scanUnit = &((UwamonoScanUnit *)MapUnit)[index];
        UwamonoScanFields *scan = &scanUnit->scan;

        if (scan->resourceId != -1) {
            UwamonoBoxUnit *target = (UwamonoBoxUnit *)((unsigned char *)MapUnit + callByteOffset);
            if (scan->channel != actor->scan.channel) {
                unsigned int flags = MapUnit[index].flags;
                if (!(flags & MAP_UNIT_BROKEN)) {
                    if (!(flags & MAP_UNIT_HIDDEN) && state->crossType &&
                        (flags & MAP_UNIT_ACTIVE) &&
                        HitCheckUwamonoAt((UwamonoBoxUnit *)actor, target)) {
                        return index;
                    }
                }
            }
        }
        ++index;
        callByteOffset += sizeof(MapUnit[0]);
    } while (index < MAP_UNIT_COUNT);
    return -1;
}

/* Tests a box actor against one unit by height, then by its circle or box shape. Returns 1 on a hit. */
int HitCheckUwamonoAt(UwamonoBoxUnit *actor, UwamonoBoxUnit *unit)
{
    UwamonoBoxCollisionState *state = &unit->state;
    float actorHeight = actor->position.y;
    float baseHeight = unit->position.y;

    if (!(actorHeight < baseHeight) && !(baseHeight + unit->dimensions.y < actorHeight)) {
        switch (state->crossType) {
        case UWAMONO_SHAPE_CIRCLE:
            if (CheckDist2D(&actor->position, &unit->position) < unit->dimensions.x + UWAMONO_HIT_RADIUS_PADDING)
                return 1;
            return 0;
        case UWAMONO_SHAPE_BOX:
            return HitCheckBoxUwamono(actor, (UwamonoMapSlot *)unit);
        default:
            return 0;
        }
    }
    return 0;
}

/* Tests the actor against a box unit's footprint along its own axes, widened by UWAMONO_BOX_HIT_MARGIN. Returns nonzero on a hit. */
int HitCheckBoxUwamono(UwamonoBoxUnit *actor, UwamonoMapSlot *unit)
{
    AlignedVector4 dimensionsSnapshot;
    AlignedVector4 position;
    Matrix4 *matrix;
    Matrix4 **matrixReference;
    float margin;
    float actorX;
    float actorZ;
    float offsetZ;
    float offsetX;
    float testDistance;
    float depth;
    float differenceX;
    float differenceZ;
    float depthSquared;

    actorX = actor->position.x;
    actorZ = actor->position.z;
    position = unit->position;
    /* The dimensions copy is not used below: the checks read unit->dimensions directly. */
    dimensionsSnapshot = unit->dimensions;
    offsetZ = actorZ - position.z;
    /* Keep an addressed reference to the real matrixView member. */
    matrixReference = &unit->matrixView;
    matrix = *matrixReference;
    offsetX = actorX - position.x;
    testDistance = UwamonoProjectBoxAxisXZ(offsetX, offsetZ, matrix);
    margin = UWAMONO_BOX_HIT_MARGIN;
    if (__builtin_fabsf(testDistance) > unit->dimensions.x + margin) {
        return 0;
    }
    offsetX = testDistance * (*matrix)[0][0] + position.x;
    differenceX = actorX - offsetX;
    offsetZ = testDistance * (*matrix)[0][2] + position.z;
    differenceZ = actorZ - offsetZ;
    testDistance = differenceX * differenceX + differenceZ * differenceZ;
    depth = unit->dimensions.z + margin;
    depthSquared = depth * depth;
    return testDistance < depthSquared;
}

/*
 * Draws the radar: a red dot at the centre for the player, and a yellow dot for each
 * visible enemy within RADAR_RANGE, offset by RADAR_SCALE.
 */
void DrawRadar(void)
{
    UwamonoScreenPoint color;
    UwamonoAlignedScreenPoint position;
    const Actor *listener = GameLoopState.actor;
    const UwamonoEnemyUnit *candidate;
    int index;
    float diffX;
    float diffZ;
    float listenerZ;
    const Vector4 *candidatePosition;
    int z;
    int x;
    int absX;
    int absZ;

    color.x = 0xFF;
    color.y = 0;
    color.z = 0;
    color.w = 0x80;
    position.value.x = RADAR_CENTER_X;
    position.value.y = RADAR_CENTER_Y;
    position.value.z = RADAR_DEPTH;
    position.value.w = 0;
    DrawSprite(&position, &color, RADAR_DOT_HALF_SIZE, RADAR_DOT_HALF_SIZE);

    color.x = 0xFF;
    color.y = 0xFF;
    color.z = 0;
    color.w = 0x80;
    for (index = 0; index < ACTOR_COUNT; index++) {
        candidate = actor + index;
        if (candidate->resourceId < 0 || (candidate->flags & UWAMONO_ACTOR_HIDDEN) != 0 ||
            !candidate->collisionObject ||
            index == listener->number) {
            continue;
        }

        {
            candidatePosition = &candidate->position;
            listenerZ = listener->position.z;
            diffZ = candidatePosition->z - listenerZ;
            diffX = candidatePosition->x - listener->position.x;
            (&position.value)->x = RADAR_CENTER_X;
            (&position.value)->y = RADAR_CENTER_Y;
            z = (int)(diffZ * RADAR_SCALE);
            x = (int)(diffX * RADAR_SCALE);
            absX = x < 0 ? -x : x;
            absZ = z < 0 ? -z : z;
            if (absX < RADAR_RANGE && absZ < RADAR_RANGE) {
                (&position.value)->x = x + RADAR_CENTER_X;
                (&position.value)->y = z + RADAR_CENTER_Y;
                DrawSprite(&position, &color, RADAR_DOT_HALF_SIZE, RADAR_DOT_HALF_SIZE);
            }
        }
    }
}

/* Adds one sprite to the draw list, centred on position, with the given colour and half sizes. */
void DrawSprite(const UwamonoAlignedScreenPoint *position,
                const UwamonoScreenPoint *color, int width, int height)
{
    UwamonoAlignedScreenPoint point = *position;
    UwamonoSpritePacket *packet = &sPoint_0;

    packet->color0.x = color->x;
    packet->color0.y = color->y;
    packet->color0.z = color->z;
    packet->color0.w = color->w;
    packet->color1.x = color->x;
    packet->color1.y = color->y;
    packet->color1.z = color->z;
    packet->color1.w = color->w;
    packet->position0.x = point.value.x - width;
    packet->position0.y = point.value.y - height;
    packet->position1.x = point.value.x + width;
    packet->position1.y = point.value.y + height;
    packet->position0.z = point.value.z;
    packet->position0.w = point.value.w;
    packet->position1.z = point.value.z;
    packet->position1.w = point.value.w;
    xglPrimAddGifTagDirect(0, packet, 7);
}

/*
 * Draws a sight line between two screen points: blue when mode is 1 (selected),
 * red otherwise. It is drawn only when lineflg is set and both points have a
 * non-negative depth.
 */
void SortLine(const UwamonoAlignedScreenPoint *from,
              const UwamonoAlignedScreenPoint *to, int mode)
{
    UwamonoAlignedScreenPoint first;
    UwamonoAlignedScreenPoint second;

    first.words[0] = from->words[0];
    first.words[1] = from->words[1];
    from = &first;
    second.words[0] = to->words[0];
    second.words[1] = to->words[1];
    to = &second;

    if (from->coordinates[2] >= 0 && to->coordinates[2] >= 0 && lineflg) {
        if (mode == 1) {
            sSortLine_1.color0.x = 0;
            sSortLine_1.color0.y = 0;
            sSortLine_1.color0.z = 255;
            sSortLine_1.color0.w = 0;
            sSortLine_1.color1.x = 0;
            sSortLine_1.color1.y = 0;
            sSortLine_1.color1.z = 255;
            sSortLine_1.color1.w = 0;
        } else {
            sSortLine_1.color0.x = 255;
            sSortLine_1.color0.y = 0;
            sSortLine_1.color0.z = 0;
            sSortLine_1.color0.w = 0;
            sSortLine_1.color1.x = 255;
            sSortLine_1.color1.y = 0;
            sSortLine_1.color1.z = 0;
            sSortLine_1.color1.w = 0;
        }
        sSortLine_1.position0.x = from->coordinates[0];
        sSortLine_1.position0.y = from->coordinates[1];
        sSortLine_1.position0.z = from->coordinates[2];
        sSortLine_1.position0.w = 128;
        sSortLine_1.position1.x = to->coordinates[0];
        sSortLine_1.position1.y = to->coordinates[1];
        sSortLine_1.position1.z = to->coordinates[2];
        sSortLine_1.position1.w = 128;
        nmlModelDirectSend(5, &sSortLine_1, 6);
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* JP: ■■■上物常駐物オーバーフロー %dK byte■■■
 * EN: === Field object resident overflow %dK bytes === */
const char fieldResidentOverflowFormat[48] = "\242\243\242\243\242\243\276\345\312\252\276\357\303\363\312\252\245\252\241\274\245\320\241\274\245\325\245\355\241\274 %dK byte\242\243\242\243\242\243\n";

const char uwamonoDatUsageFormat[40] = "use uwamono dat=%dK byte obj count=%d\n";

const char karakamaItemboxPath[32] = "data\\karakama\\itembox.dat";

const char karakamaEvtItemPath[32] = "data\\karakama\\evtitem.dat";

const char alreadyMapUnitFormat[24] = "ID=%d already MapUnit\n";

const char createUwamonoOverflowFormat[32] = "Create Uwamono over flow!\n";

const char setSizeFormat[16] = "SetSize\n";

const char xyzVectorFormat[16] = "x=%f y=%f z=%f\n";

const char illegalUwaNullFormat[40] = "Illegal UWA_NULL!  id=%d serial=%d\n";

const char illegalUwamonoTypeFormat[40] = "Illegal Uwamono Type %d\n";

const char illegalKowareKindFormat[32] = "Illegal Koware Kind = %d\n";

/* JP: 壊れ物生成エラー パーツナンバーが不正です %d
 * EN: Breakable creation error: invalid part number %d */
const char breakableCreateErrorFormat[48] = "\262\365\244\354\312\252\300\270\300\256\245\250\245\351\241\274 \245\321\241\274\245\304\245\312\245\363\245\320\241\274\244\254\311\324\300\265\244\307\244\271 %d\n";

const char createKowaremonoFormat[56] = "Create Kowaremono id=%d kind=%d pos x=%f y=%f z=%f\n";

const char createUwamonoUnitOverflowFormat[32] = "Create Uwamono unit over flow!\n";

const char createKowaremonoUnitOverflowFormat[40] = "Create Kowaremono unit over flow!\n";

const char kowareDeleteFormat[40] = "unit koware delete id=0x%x serial=%d\n";

const char hitCheckMatrixFormat[16] = "MAT %f %f %f";

const char hitCheckPosFormat[16] = "POS %f %f %f";

const char hitCheckLenRadiusFormat[16] = "len=%f r=%f";

const char matrixHitLabel[16] = "MATRIX_HIT";

const char shootMapUnitFormat[32] = "Act Shoot MapUnit=%d\n";

float activeCursolMinScale = 0.55000001192092896f;

float activeCursolScaleFloor = 0.55000001192092896f;

const float mapUnitHitHeightScale = 0.89999997615814209f;

const float mapUnitHitBelowMargin = 1.7999999523162842f;

const float mapUnitPosHitHeightScale = 0.89999997615814209f;

const float mapUnitPosHitBelowMargin = 1.7999999523162842f;

float nyuruCircleRadiusMargin = 0.0099999997764825821f;

float nyuruMatrixStillThreshold = 0.0010000000474974513f;

float nyuruMatrixStillEpsilon = 0.0010000000474974513f;

float nyuruCornerRadius = 0.0099999997764825821f;

float shootMotionSpeed = 0.033333335071802139f;

float shootProgressThreshold = 0.20000000298023224f;

const float uwamonoAreaHeightScale = 0.89999997615814209f;

const float uwamonoAreaLowerMargin = 1.7999999523162842f;

const float crossActorHeightLimit = 0.30000001192092896f;

float crossCheckMapUnitPadding = 0.10000000149011612f;

float aimMapUnitLookEpsilon = 0.0099999997764825821f;

float uwamonoMaxFallVelocity = -0.60000002384185791f;

float uwamonoGravity = 0.05000000074505806f;

float uwamonoFloorOffset = 0.0010000000474974513f;

float uwamonoLandStopVelocity = 0.05000000074505806f;

float uwamonoBounceStopVelocity = 0.10000000149011612f;

float uwamonoHitHeightPadding = 0.0010000000474974513f;

const char lexFileExtension[8] = ".lex";

const char xtxFileExtension[8] = ".xtx";

const char brokenMethodName[8] = "broken";

const char hitCheckLenFormat[8] = "len=%f";

const char hitCheckXFormat[8] = "x=%f";

const char hitCheckYFormat[8] = "y=%f";

const char hitCheckZFormat[8] = "z=%f";

const char hitCheckUFormat[8] = "u=%f";

const char hitCheckVFormat[8] = "v=%f";

const char hitCheckWFormat[8] = "w=%f";
