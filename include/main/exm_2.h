#ifndef INCLUDE_MAIN_EXM_2_H
#define INCLUDE_MAIN_EXM_2_H

#include "shared.h"

typedef struct UnitTrack {
    void *source;                                     /* +0x00 */
    short frame;                                      /* +0x04 */
    u16 last_frame;                                   /* +0x06 */
    u16 flags;                                        /* +0x08 */
} UnitTrack;

typedef struct UnitTransSequence {
    UnitTrack track;                                  /* +0x00 */
    unsigned char unmodeled_0c[0x10 - 0x0c];
    float x;                                          /* +0x10 */
    float y;                                          /* +0x14 */
    float z;                                          /* +0x18 */
    unsigned char unmodeled_1c[0x40 - 0x1c];
    int param;                                        /* +0x40 */
} UnitTransSequence;

typedef struct UnitRotChannel {
    UnitTrack track;                                  /* +0x00 */
    unsigned char unmodeled_0c[0x10 - 0x0c];
    int frames[3];                                    /* +0x10 */
    u32 flags;                                        /* +0x1c */
    unsigned char unmodeled_20[0x30 - 0x20];
    float target[3];                                  /* +0x30 */
    unsigned char unmodeled_3c[0x40 - 0x3c];
    float step[3];                                    /* +0x40 */
} UnitRotChannel;

typedef struct UnitScaleChannel {
    UnitTrack track;                                  /* +0x00 */
    unsigned char unmodeled_0c[0x10 - 0x0c];
    int frames[3];                                    /* +0x10 */
    unsigned char unmodeled_1c[0x30 - 0x1c];
    float target[3];                                  /* +0x30 */
    unsigned char unmodeled_3c[0x40 - 0x3c];
    float step[3];                                    /* +0x40 */
} UnitScaleChannel;

typedef struct UnitMotionChannel {
    unsigned char unmodeled_00[0x08];
    int motion;                                       /* +0x08 */
    unsigned char unmodeled_0c[0x10 - 0x0c];
    short first_frame;                                /* +0x10 */
    short last_frame;                                 /* +0x12 */
    u8 option;                                        /* +0x14 */
    unsigned char unmodeled_15[0x18 - 0x15];
    int blend;                                        /* +0x18 */
    float speed;                                      /* +0x1c */
} UnitMotionChannel;

/*
 * The per-unit table getState__/getPivot__/setPivot__/setAxis__ index with
 * MapUnitRecord.slot (lui %hi(unitSequence)/addiu %lo(unitSequence) at
 * 0x0030452c, size 0x9800 in config/symbols/main.txt, exactly 64 entries of
 * 0x260 bytes).
 *
 *   +0x04 state   the word getState__ returns (lw at 0x003044b4).
 *   +0x240 pivot_x/y/z
 *                 the three floats getPivot__/setPivot__ read and write.
 *   +0x250 axis_x/y/z/w
 *                 the four floats setAxis__FFFF writes; the class field
 *                 name pool this TU's lookups read from names the first
 *                 three "rx"/"ry"/"rz".
 */
typedef struct UnitSequenceEntry {
    u32 flags;                                        /* +0x00 */
    int state;                                        /* +0x04 */
    unsigned char unmodeled_08[0x0c - 0x08];
    int mode;                                         /* +0x0c */
    unsigned char unmodeled_10[0x14 - 0x10];
    JavaMethod *method;                               /* +0x14 */
    unsigned char unmodeled_18[0x24 - 0x18];
    void (*transSequence)(void *unit);                /* +0x24 */
    void (*rotateSequence)(void *unit);               /* +0x28 */
    void (*motionSequence)(void *unit);               /* +0x2c */
    void (*scaleSequence)(void *unit);                /* +0x30 */
    unsigned char unmodeled_34[0x38 - 0x34];
    UnitTransSequence trans;                          /* +0x38 */
    unsigned char unmodeled_7c[0xb8 - 0x7c];
    UnitRotChannel rotation;                          /* +0xb8 */
    unsigned char unmodeled_104[0x138 - 0x104];
    UnitMotionChannel motion;                         /* +0x138 */
    unsigned char unmodeled_158[0x1b8 - 0x158];
    UnitScaleChannel scale;                           /* +0x1b8 */
    unsigned char unmodeled_204[0x240 - 0x204];
    float pivot_x;                                    /* +0x240 */
    float pivot_y;                                    /* +0x244 */
    float pivot_z;                                    /* +0x248 */
    unsigned char unmodeled_24c[0x250 - 0x24c];
    float axis_x;                                      /* +0x250 */
    float axis_y;                                      /* +0x254 */
    float axis_z;                                      /* +0x258 */
    float axis_w;                                      /* +0x25c */
} UnitSequenceEntry;


/*
 * Work areas of the per-kind units, all at MapUnitRecord +0x1a0 (see the union
 * there). Each struct's own offsets are relative to +0x1a0.
 */
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


typedef struct ItemUnitInfo {
    unsigned char unmodeled_00[2];
    unsigned short itemNo;
    signed char signal;
    signed char unitNo;
    unsigned char unmodeled_06[0x27];
    signed char rsrcIndex;
    unsigned char unmodeled_2e[2];
    short item_get_method;
    unsigned char unmodeled_32[2];
    int seId;
    unsigned char unmodeled_38[0xc];
    int clearedWord;
} ItemUnitInfo;


typedef struct UwamonoInitSymbolState {
    unsigned char unmodeled_00[0x2d];
    signed char state;                 /* +0x2d */
    unsigned char unmodeled_2e[0x16];
    int timer;                         /* +0x44 */
} UwamonoInitSymbolState;

typedef struct UwamonoCreatedState {
    short mapType;                    /* +0x00 */
    short initialUnitId;              /* +0x02 */
    unsigned char unmodeled_04;
    signed char signal;
    unsigned char unmodeled_06[0x1e];
    float height;
    unsigned char unmodeled_28[4];
    signed char state;
    signed char initialState;
    signed char heightCheckFlag;
    signed char unmodeled_2f;
    short initialPartId;              /* +0x30 */
    signed char target;
    unsigned char unmodeled_33;
    int initialValue34;
    int initialValue38;
    unsigned char unmodeled_3c[4];
    int initialValue40;
    int timer;
    int initialValue48;
    unsigned char unmodeled_4c[0x34];
} UwamonoCreatedState;

typedef struct UwamonoMapUnitState {
    unsigned char unmodeled_00[2];
    short kindIndex;
    signed char signal;                 /* +0x04 */
    signed char hideObjectIndex;
    unsigned char unmodeled_06[6];
    float baseSizeX;
    float baseSizeY;
    float baseSizeZ;
    float sizeOffsetX;
    float sizeOffsetY;
    float sizeOffsetZ;
    unsigned char unmodeled_24[8];
    signed char enemyIndex;
    signed char state;                 /* +0x2d */
    unsigned char unmodeled_2e[2];
    short initialPartId;               /* +0x30 */
    signed char target;                /* +0x32 */
    unsigned char unmodeled_33[9];
    int frameLimit;                    /* +0x3c */
    union { int effectResourceId; int kindParameter; };              /* +0x40 */
    int timer;                         /* +0x44 */
} UwamonoMapUnitState;

/*
 * The 0x48-byte trap work area at MapUnitRecord +0x1a0 (init_map_trap.c's
 * MapUnitRecordTrapView.trap). Field offsets are relative to that area.
 */
typedef struct TrapDamageProperties {
    unsigned char unmodeled_00[2];
    s16 kindIndex;
    signed char state;
    unsigned char unmodeled_05[7];
    float sizeX;
    float sizeY;
    float sizeZ;
    float offsetX;
    float offsetY;
    float offsetZ;
    float damageRadius;
    unsigned char unmodeled_28[5];
    signed char windState;
    unsigned char unmodeled_2e[2];
    s16 methodId;
    signed char target;
    unsigned char unmodeled_33[9];
    int activeDuration;
    int effectId;
    unsigned int activeFlags;
} TrapDamageProperties;

/*
 * One entry of the game-wide MapUnit[] array (0x300 bytes apart;
 * MAP_initUnitSequance stores MAP_updateUnitDefault as each entry's +0x04
 * update callback). The EXM view (exm_2.c, which defines the array), the MAP
 * view (map_create_unit_peer.c) and the native peer record of the Java
 * xeno.Unit class (src/main/unit.h's UnitPeer, merged here) describe the same
 * bytes. The type is not named MapUnit because that is the array's own symbol.
 *
 * The members MAP_updateUnitDefault (0x00320a80) reads or writes are
 * named: `position` is loaded/stored a field at a time (+0x10/+0x14/+0x18)
 * and also handed whole to xglMatrixStackTrans; `rotation` supplies the
 * three xglMatrixStackRot{X,Y,Z} angles (+0x20/+0x24/+0x28); `scale` and
 * `matrix` are passed whole to xglMatrixStackScale/xglMatrixStackSave.
 *
 * slot (+0xa0) is this unit's own slot number: unitSequence[] is indexed by
 * it. serial (+0xa4) is the 16-bit unit id the game compares against (-1 marks
 * a free slot), and partIndex reads the same halfword as the parts-matrix
 * slot MAP_updateUnitPartsSequence saves into. elevator_task (+0x0c) is what
 * the native peer's initElevatorFunc__ installs, typeUpdate what
 * MAP_updateUnit calls; each TU reads the word through its own member.
 * modelState/modelBlock (+0x240) are the in-place model state MDL_create
 * writes and the block MDL_setVisible receives the address of.
 * args (+0x1a0) is the native peer's byte buffer; drillResetValue reads its
 * halfword at +0x1a2.
 */
/* Kind 6003's coordinates are screen pixels or world units, chosen by
 * use_screen_coordinates. */
typedef union Unit6003Coordinate {
    int screen;
    float world;
} Unit6003Coordinate;

/* Kind 6003 shows frame type 0 from the raw XTX resource and frame type 1
 * from the movie work (src/main/unit6003.c). */
typedef union Unit6003MovieSource {
    struct Unit6003ResourceHeader *resource;
    struct XglMovieWorkXtxView *movie;
} Unit6003MovieSource;

/* +0x1a0: the work area of kind 6003 (unit6003_update/unit6003_draw). */
typedef struct Unit6003WorkArea {
    Unit6003Coordinate left; /* +0x1a0 */
    Unit6003Coordinate top; /* +0x1a4 */
    Unit6003Coordinate width; /* +0x1a8 */
    Unit6003Coordinate height; /* +0x1ac */
    unsigned int resource_id; /* +0x1b0 */
    int use_screen_coordinates; /* +0x1b4 */
    unsigned int texture_width : 16; /* +0x1b8 */
    unsigned int frames_per_row : 16; /* +0x1ba */
    unsigned int texture_height : 16; /* +0x1bc */
    unsigned int frames_per_column : 16; /* +0x1be */
    unsigned char alpha; /* +0x1c0 */
    unsigned char unmodeled_1c1[3];
    unsigned int movie_advance_count; /* +0x1c4 */
    unsigned int frame_count; /* +0x1c8 */
    int playback_direction; /* +0x1cc */
    float top_depth; /* +0x1d0 */
    float bottom_depth; /* +0x1d4 */
    unsigned char frame_index; /* +0x1d8 */
    unsigned char frame_timer; /* +0x1d9 */
    unsigned char frame_delay; /* +0x1da */
    unsigned char frame_type; /* +0x1db */
    unsigned char unmodeled_1dc[4];
    Unit6003MovieSource movie_work; /* +0x1e0 */
    unsigned char unmodeled_1e4[0x220 - 0x1e4];
} Unit6003WorkArea;

typedef struct MapUnitRecord {
    unsigned int flags; /* +0x00, see MAP_UNIT_FOLLOW_GROUND */
    void (*update)(struct MapUnitRecord *unit); /* +0x04 */
    void (*draw)(struct MapUnitRecord *unit); /* +0x08 */
    union {
        void (*typeUpdate)(struct MapUnitRecord *unit); /* +0x0c */
        void (*elevator_task)(void *peer, int field_offset); /* +0x0c */
    };
    Vector4 position; /* +0x10 */
    Vector4 rotation; /* +0x20 */
    Vector4 scale; /* +0x30 */
    Matrix4 matrix;   /* +0x40 */
    /* +0x80: the unit's world matrix: its own matrix (map symbols) or its
     * entry of the map-parts matrix table (GetPartsPos). */
    float (*basis)[4];
    unsigned char unmodeled_84[0x0c];
    Vector4 globalPosition; /* +0x90: nmlModelGetGblPosition writes it */
    unsigned char slot; /* +0xa0: selects this unit's sequence entry */
    union {
        unsigned char signal; /* +0xa1 */
        signed char actionNo; /* +0xa1: Uwamono kinds' action number */
    };
    unsigned char actionSub; /* +0xa2: kind state; unit6003_update switches on it (lbu) */
    unsigned char unmodeled_a3;
    union {
        short serial; /* +0xa4: unit id, -1 for a free slot */
        short partIndex; /* +0xa4: matrix slot MAP_updateUnitPartsSequence saves into */
    };
    short monitor_priority; /* +0xa6 */
    short sequenceNo; /* +0xa8 */
    short sequenceSub; /* +0xaa */
    unsigned char unmodeled_ac[4];
    float radius; /* +0xb0 */
    unsigned char unmodeled_b4[0xc0 - 0xb4];
    Vector4 doorPosition; /* +0xc0: position of the door view */
    void *sequence; /* +0xd0: the unit's UnitSequenceEntry, when it has one */
    union {
        void *resourceModel; /* +0xd4: parts-matrix update runs only while NULL */
        unsigned int update_stamp; /* +0xd4: frame stamp MAP_updateUnitPartsSequence writes */
    };
    int cleared_d8[2]; /* +0xd8: zeroed by MAP_initUnit */
    union {
        struct {
            void *model; /* +0xe0 */
            const char *resourceStatus; /* +0xe4: texture address MAP_drawUnitAt passes to nmlModelSetTexture */
        };
        struct {
            int uwaresId; /* +0xe0: Uwamono model resource address (uwares_tbl entry) */
            int xtxresId; /* +0xe4: Uwamono texture resource address (xtxres_tbl entry) */
        };
        int modelEntry; /* +0xe0: nmlModelEntry index MAP_drawUnitAt reads from the same word */
        unsigned int modelResourceWords[2]; /* MAP_initUnit clears both words in one loop */
    };
    int effectCf[3]; /* +0xe8: sefCreateEffectCf handles, zeroed by MAP_createUnit */
    unsigned char unmodeled_f4[4];
    union {
        unsigned int parent_flags; /* +0xf8 */
        int parentMatrixIndex; /* +0xf8: parts-matrix slot read by MAP_drawUnitAt */
    };
    union {
        struct MapUnitParent *parent; /* +0xfc */
        void *parent_peer; /* +0xfc */
    };
    unsigned char unmodeled_100[0x118 - 0x100];
    short motion_mask; /* +0x118 */
    unsigned char unmodeled_11a[0x170 - 0x11a];
    Vector4 motion_root[3]; /* +0x170: root motion of the native peer */
    /* +0x1a0..+0x220: the work area of the unit's kind. Map symbols use the
     * generic args bytes; trap units (init_map_trap.c) use TrapDamageProperties,
     * kind 6003 (unit6003.c) uses Unit6003WorkArea. The elevator fields share the area's tail in both views. */
    union {
        struct {
            union {
                unsigned char args[0x18]; /* +0x1a0 */
                struct {
                    short symbol_kind; /* +0x1a0: map symbol kind */
                    short shop_id; /* +0x1a2: shop the symbol opens (DrillResetFlag tests the same halfword) */
                };
            };
            void *elevator_field; /* +0x1b8 */
            unsigned char unmodeled_1bc[4];
            int elevator_state; /* +0x1c0 */
            unsigned char unmodeled_1c4[0x1e8 - 0x1c4];
            int bgmTimer; /* +0x1e8: Uwamono BGM fade-out countdown */
            unsigned char unmodeled_1ec[0x220 - 0x1ec];
        };
        TrapDamageProperties trap; /* +0x1a0: trap kind's work area */
        Unit6003WorkArea unit6003; /* +0x1a0: kind 6003's work area */
        UwamonoCreatedState uwamono; /* +0x1a0: Uwamono kinds' work area (init_uwamono_sys.c) */
        UwamonoInitSymbolState symbol; /* +0x1a0: save/shop/event/return symbols' state */
    };
    float modelPosition[4]; /* +0x220: MAP_initUnit resets it to (0, 0, 0, 1) */
    unsigned char unmodeled_230[4];
    /* The same word is the shadow's X (shadow views) or the texture-map mode
     * (MAP_drawUnitAt's nmlModelSetTexMap switch). */
    union {
        int shadow_x; /* +0x234 */
        int texMapMode; /* +0x234 */
    };
    union {
        int shadow_y; /* +0x238 */
        int texMapSize; /* +0x238 */
    };
    union {
        float shadow_clip_scale; /* +0x23c */
        float texMapWeight; /* +0x23c */
    };
    union {
        void *modelState; /* +0x240 */
        unsigned char modelBlock[0x2d0 - 0x240]; /* +0x240 */
    };
    unsigned short shadow_map_ids[8]; /* +0x2d0 */
    int shadow_map_count; /* +0x2e0 */
    int render_command; /* +0x2e4 */
    int filter_mode; /* +0x2e8 */
    int sort_offset; /* +0x2ec */
    union {
        float filter_params[1]; /* +0x2f0 */
        float transparency; /* +0x2f0: nmlModelSetTransparency's argument */
    };
    float filterStart; /* +0x2f4: nmlModelSetFilter's start */
    float filterEnd; /* +0x2f8: nmlModelSetFilter's end */
    float reflTransparency; /* +0x2fc: nmlModelSetReflTransparency's argument */
} MapUnitRecord;

/*
 * The 0x30-byte wind state of exm_2.c (EXM_SetWindPara/EXM_GetWindPara copy the
 * whole block; EXM_SetDirectionalWind/EXM_SetPointWind load direction).
 * src/main/script.c's debug wind view (the same 0x30 bytes, direction as
 * Vector4) is merged here.
 *
 * +0x00 is the mode EXM_StopWind and EXM_ResetWind clear to 0,
 * EXM_SetDirectionalWind sets to 1 and EXM_SetPointWind sets to 2; +0x10 is a
 * direction/point vector (four floats: EXM_ResetWind zeroes +0x10/+0x14/+0x18/
 * +0x1c individually). None of those bytes are written by a function claimed
 * here, so they stay modeled only for their evidenced size and 8-byte alignment
 * (`u64`): EXM_SetWindPara/EXM_GetWindPara move the whole 0x30-byte block in six
 * ld/sd pairs, which needs that alignment.
 */
typedef struct EXM_WindState {
    char mode;                /* +0x00, read signed (lb) by EXM_GetWindPower */
    unsigned char unmodeled_01[7];
    unsigned long long unmodeled_08;
    DoublewordAlignedVector4 direction; /* +0x10, four-component direction/position */
    float shake_power;        /* +0x20, EXM_SetShakePower */
    float shake_time;         /* +0x24, EXM_SetShakeTime */
    float shake_rad;          /* +0x28, EXM_SetShakeWind/EXM_GetShakeRad */
    unsigned char unmodeled_2c[4];
} EXM_WindState;

/* Functions and data used by other translation units. */
void EXM_StepShakeWind(void);
void EXM_InitWind(void);
void EXM_ResetWind(void);
void EXM_ClearWindStruct(void);
void EXM_SetShakePower(float power);
void EXM_SetShakeTime(float time);
void EXM_SetDirectionalWind(Vector4 *direction);
void EXM_SetPointWind(Vector4 *point);

#endif /* INCLUDE_MAIN_EXM_2_H */
