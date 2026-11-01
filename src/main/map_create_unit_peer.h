/*
 * TU-local declarations of main/tu270 (src/main/map_create_unit_peer.c).
 */

#ifndef SRC_MAIN_MAP_CREATE_UNIT_PEER_H
#define SRC_MAIN_MAP_CREATE_UNIT_PEER_H

#include "shared.h"

/* flags bit: snap position.y to the ground undulation under x/z each update */
#define MAP_UNIT_FOLLOW_GROUND 0x100

/*
 * This TU's view of one entry of the game-wide MapUnit[] array (0x300 bytes
 * apart; MAP_initUnitSequance stores MAP_updateUnitDefault as each entry's
 * +0x04 update callback, and src/main/init_drill.c and
 * src/main/init_uwamono_sys.c keep their own views of the same record).
 * The type is not named MapUnit because that is the array's own symbol.
 * Only the members MAP_updateUnitDefault (0x00320a80) reads or writes are
 * named: `position` is loaded/stored a field at a time (+0x10/+0x14/+0x18)
 * and also handed whole to xglMatrixStackTrans; `rotation` supplies the
 * three xglMatrixStackRot{X,Y,Z} angles (+0x20/+0x24/+0x28); `scale` and
 * `matrix` are passed whole to xglMatrixStackScale/xglMatrixStackSave.
 */
typedef struct MapUnitRecord {
    unsigned int flags; /* +0x00, see MAP_UNIT_FOLLOW_GROUND */
    unsigned char unmodeled_04[0x0c];
    Vector4 position; /* +0x10 */
    Vector4 rotation; /* +0x20 */
    Vector4 scale;    /* +0x30 */
    Matrix4 matrix;   /* +0x40 */
    unsigned char unmodeled_80[0x20];
    unsigned char serial; /* +0xa0: selects this unit's sequence entry */
    unsigned char unmodeled_a1[3];
    short partIndex; /* +0xa4: matrix slot MAP_updateUnitPartsSequence saves into */
    unsigned char unmodeled_a6[0xd4 - 0xa6];
    void *resourceModel; /* +0xd4: parts-matrix update runs only while NULL */
} MapUnitRecord;

typedef struct MapUnitAnmState {
    unsigned int flags; /* +0x00: SEQ_motionUnit loads the motion track's flags */
    float motionTime; /* +0x04: MAP_updateUnitMPack mirrors PLAY_getCurrent's
                          own +0x44 float here every update. */
    float speed;     /* +0x08 */
    float loopStart; /* +0x0c */
    float loopEnd;   /* +0x10 */
    short motionId;   /* +0x14: MAP_setUnitMotion sets this from its own
                          argument before calling ANM_resetDefault. */
} MapUnitAnmState;

/*
 * A second, wider view of the same MapUnit[] entry MapUnitRecord models the
 * head of: MAP_setUnitMotion and MAP_updateUnitMPack, below, evidence its
 * embedded animation-player state at +0x100, which ANM_resetDefault (still
 * asm) otherwise owns. `position`/`rotation`/`scale`/`matrix` repeat
 * MapUnitRecord's own transform fields at the same offsets for
 * MAP_updateUnitMPack's matrix-stack sequence (MapUnitRecord itself cannot
 * grow to add the animation fields: MAP_updateUnitDefault, outside this
 * allocation, already uses it at its published size).
 */
typedef struct MapUnitMotionRecord {
    unsigned char unmodeled_00[0x10];
    Vector4 position; /* +0x10 */
    Vector4 rotation; /* +0x20 */
    Vector4 scale;    /* +0x30 */
    Matrix4 matrix;   /* +0x40 */
    unsigned char unmodeled_80[0xa0 - 0x80];
    unsigned char serial; /* +0xa0: selects this unit's sequence entry */
    unsigned char unmodeled_a1[0xDC - 0xa1];
    int animResetParam; /* +0xDC: MAP_setUnitMotion forwards this straight to
                            ANM_resetDefault's own second argument, whose role
                            that still-asm function does not evidence here. */
    unsigned char unmodeled_E0[0x100 - 0xE0];
    MapUnitAnmState anim; /* +0x100 */
} MapUnitMotionRecord;

/* A spline sampled once per frame: SEQ_moveUnitSPL, SEQ_rotateUnitSPL and
 * SEQ_scaleUnitSPL each keep one in their sequence channel. */
typedef struct MapUnitSplineTrack {
    void *spline;            /* +0x00 */
    short frame;             /* +0x04 */
    short lastFrame;         /* +0x06 */
    unsigned short flags;    /* +0x08 */
} MapUnitSplineTrack;

typedef struct MapUnitBoneOwner {
    unsigned char unmodeled_00[0x824];
    Matrix4 *matrices; /* +0x824 */
} MapUnitBoneOwner;

typedef struct MapUnitWalkTrack {
    struct MapUnitRecord *followed; /* +0x00 */
    int frames;                     /* +0x04 */
    unsigned char unmodeled_08[0x10 - 0x08];
    Vector4 start;       /* +0x10 */
    Vector4 destination; /* +0x20 */
} MapUnitWalkTrack;

typedef struct MapUnitBoneTrack {
    MapUnitBoneOwner *owner; /* +0x00 */
    unsigned char unmodeled_04[0x10 - 0x04];
    Vector4 offset; /* +0x10 */
} MapUnitBoneTrack;

/* Movement channel at +0x38 of a sequence entry: a straight walk to a target
 * (SEQ_moveUnitChr follows a unit, SEQ_moveUnitXZ a fixed point;
 * `destination.w` carries the step), a bone-attached offset
 * (SEQ_transCNSUnitChr) or a spline (`spline`, SEQ_moveUnitSPL). */
typedef union MapUnitMoveTrack {
    MapUnitSplineTrack spline;
    MapUnitWalkTrack walk;
    MapUnitBoneTrack bone;
} MapUnitMoveTrack;

typedef struct MapUnitTurnTrack {
    struct MapUnitRecord *target; /* +0x00 */
    unsigned char unmodeled_04[0x10 - 0x04];
    int duration[3];            /* +0x10 */
    unsigned int faceFlags;     /* +0x1c */
    float start[3];             /* +0x20 */
    unsigned char unmodeled_2c[0x04];
    float goal[3];              /* +0x30 */
    unsigned char unmodeled_3c[0x04];
    float step[3];              /* +0x40 */
} MapUnitTurnTrack;

typedef struct MapUnitBoneAimTrack {
    MapUnitBoneOwner *owner; /* +0x00 */
    unsigned char unmodeled_04[0x14 - 0x04];
    int boneIndex; /* +0x14 */
    unsigned char unmodeled_18[0x30 - 0x18];
    float goal[3]; /* +0x30 */
} MapUnitBoneAimTrack;

/* Rotation channel at +0xb8: SEQ_rotateUnitSPL samples a spline,
 * SEQ_rotateUnit turns each axis from `start` to `goal`, and
 * SEQ_rotYCNSUnitChr aims at a bone. */
typedef union MapUnitRotationTrack {
    MapUnitSplineTrack spline;
    MapUnitTurnTrack turn;
    MapUnitBoneAimTrack bone;
} MapUnitRotationTrack;

/* Motion channel at +0x138, played by SEQ_motionUnit. */
typedef struct MapUnitMotionTrack {
    unsigned char unmodeled_00[0x08];
    int motion; /* +0x08 */
    unsigned char unmodeled_0c[0x04];
    short loopStart; /* +0x10 */
    short loopEnd;   /* +0x12 */
    unsigned char unmodeled_14[0x04];
    unsigned int flags; /* +0x18 */
    float speed;        /* +0x1c */
} MapUnitMotionTrack;

/* Scale channel at +0x1b8: SEQ_scaleUnitSPL samples a spline,
 * SEQ_scaleUnit moves each axis from `start` to `goal`. */
typedef struct MapUnitScaleTrack {
    void *spline;    /* +0x00 */
    short frame;     /* +0x04 */
    short lastFrame; /* +0x06 */
    unsigned char unmodeled_08[0x10 - 0x08];
    int duration[3]; /* +0x10 */
    unsigned char unmodeled_1c[0x04];
    float start[3];  /* +0x20 */
    unsigned char unmodeled_2c[0x04];
    float goal[3];   /* +0x30 */
    unsigned char unmodeled_3c[0x04];
    float step[3];   /* +0x40 */
} MapUnitScaleTrack;

/*
 * MAP_callUnitGroup compares the group halfword in each 0x260-byte sequence
 * entry and calls its callback with the corresponding unit's flags word.
 */
typedef struct MapUnitGroupSequenceEntry {
    unsigned int flags; /* +0x00 */
    int state;          /* +0x04 */
    unsigned char unmodeled_08[0x04];
    int sequenceMode; /* +0x0c */
    short group; /* +0x10 */
    unsigned char unmodeled_12[0x02];
    unsigned int sequenceParameters[4]; /* +0x14..+0x20 */
    void (*update_callbacks[4])(MapUnitRecord *unit); /* +0x24 */
    unsigned int frameCounter; /* +0x34 */
    MapUnitMoveTrack move; /* +0x38 */
    unsigned char unmodeled_68[0x78 - 0x68];
    int boneIndex; /* +0x78 */
    unsigned char unmodeled_7c[0xb8 - 0x7c];
    MapUnitRotationTrack rotation; /* +0xb8 */
    unsigned char unmodeled_104[0x138 - 0x104];
    MapUnitMotionTrack motion; /* +0x138 */
    unsigned char unmodeled_158[0x1b8 - 0x158];
    MapUnitScaleTrack scale; /* +0x1b8 */
    unsigned char unmodeled_204[0x240 - 0x204];
    float pivot_x; /* +0x240 */
    float pivot_y; /* +0x244 */
    float pivot_z; /* +0x248 */
    float pivot_w; /* +0x24c */
    float axis_x; /* +0x250 */
    float axis_y; /* +0x254 */
    float axis_z; /* +0x258 */
    float axis_w; /* +0x25c */
} MapUnitGroupSequenceEntry;

typedef struct MapUnitGroupEntry {
    int flags; /* +0x00; callback argument used by Unit suspend/resume */
    void (*update)(MapUnitRecord *unit); /* +0x04 */
    unsigned char unmodeled_08[0x08];
    Vector4 position; /* +0x10 */
    Vector4 rotation; /* +0x20 */
    Vector4 scale; /* +0x30 */
    Matrix4 matrix; /* +0x40 */
    unsigned char unmodeled_80[0x20];
    unsigned char serial; /* +0xa0 */
    unsigned char serialFlags; /* +0xa1 */
    unsigned char unmodeled_a2[0x06];
    short animationIndex; /* +0xa8 */
    unsigned char unmodeled_aa[0x26];
    int peer; /* +0xd0: EE 32-bit sequence address carrier; this TU only clears it. */
    unsigned char unmodeled_d4[0x100 - 0xd4];
    MapUnitAnmState anim; /* +0x100 */
    short animationMask; /* +0x118: MAP_createUnitPeer stores 0x1ff */
    unsigned char unmodeled_11a[0x300 - 0x11a];
} MapUnitGroupEntry;

/* Parts-matrix store walked by MAP_updateUnitPartsSequence: the model's
 * matrices start `matrixOffset` bytes past the model, `partCount` of them. */
typedef struct MapPartsModel {
    unsigned char unmodeled_00[0x40];
    int partCount; /* +0x40 */
    unsigned char unmodeled_44[0x50 - 0x44];
    unsigned int matrixOffset; /* +0x50 */
} MapPartsModel;

typedef struct MapPartsDisplay {
    unsigned char unmodeled_00[4];
    MapPartsModel *model; /* +0x04 */
} MapPartsDisplay;

typedef struct MapPartsLoopState {
    unsigned char unmodeled_00[0x54];
    MapPartsDisplay *display; /* +0x54 */
} MapPartsLoopState;

extern MapPartsLoopState GameLoopState;
extern MapUnitGroupSequenceEntry unitSequence[64];
extern MapUnitGroupEntry MapUnit[64];

/* Undulate.c (main); no header is published for it yet (also declared this
 * way in src/main/near_dir.c and src/ov01/unit_cmd.h). */
extern float UnduGet(float x, float z);

/*
 * Defined in src/main/xgl_2.c (main/tu100) with this spelling, but not in
 * include/main/xgl_2.h: src/ov02/umn_procurator.h declares it as
 * `void xglMatrixStackTrans(float *)`, and header_harvest shares no name
 * with divergent spellings (reports/header-harvest-conflicts.md).
 */
extern void xglMatrixStackTrans(const float translation[4]);

extern void MAP_setUnitMotion(MapUnitMotionRecord *unit, int motionId);
extern void SPL_getValueXYZ(float *destination, void *spline, float frame);
extern float nearDir(float from, float to);
extern float xglAtan2(float x, float y);
extern float xglSin(float angle);
extern float xglCos(float angle);

#endif /* SRC_MAIN_MAP_CREATE_UNIT_PEER_H */
