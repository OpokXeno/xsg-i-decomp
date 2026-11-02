#ifndef INCLUDE_MAIN_SEF_H
#define INCLUDE_MAIN_SEF_H

#include "shared.h"

/* The 0x24-byte effect request block sefCreateEffect hands to the script creator. */
typedef struct SefEffectRequest {
    struct Actor *source;            /* +0x00 */
    struct Actor *target;            /* +0x04 */
    struct MapUnitRecord *parent;    /* +0x08 */
    short size;                      /* +0x0c */
    short size_copy;                 /* +0x0e */
    short character_id;              /* +0x10 */
    unsigned short character_id_copy; /* +0x12 */
    short effect_no;                 /* +0x14 */
    short special;
    unsigned char reaction_mask;
    unsigned char reaction_mask_high;
    unsigned short flags;            /* +0x1a */
    unsigned short reactionGate;     /* +0x1c, bit 0 gates sefCreateReactionEffect's reaction effects */
    unsigned short initFlags;        /* +0x1e */
    int param;                       /* +0x20 */
} SefEffectRequest;

/*
 * sefDestroyScriptScheduler2 (main:0x002e5320) reads a script_id/task_id
 * pair right after flags, matched against its own two parameters to find
 * the slot to free. The compiler materializes their common base (the
 * record + 0xa90) once and reads both fields off it with small
 * displacements, so this sub-record is named on its own instead of two
 * flat members of SchedulerState.
 *
 * sefFreeSchedulerCf (main:0x002e5118) also stores a zero half-word to the
 * leading two bytes at the same time it resets script_id/task_id to -1; no
 * other accepted function of sef.c reads or writes it yet.
 */
typedef struct ScriptBinding {
    short state;
    short script_id;
    short task_id;
    short link_id; /* +0x6 */
} ScriptBinding;

/*
 * One of a scheduler's 32 effect-table records (sefInitEffectData): the
 * first 0x20 bytes are line-data handles, filled with 0xff by memset (a
 * negative handle is a free slot; svDrawSchedulerEffect draws the others);
 * owner, effect_no and category are set from sefInitEffectData's
 * parameters, flags and frame are reset to 0.
 */
typedef struct EffectData {
    signed char lineHandles[0x20];
    int owner;
    short category;
    short effect_no;
    short flags;
    short active;
    short frame;
} EffectData;

/*
 * The 0xab0-byte effect scheduler record (sef.c's _scheduler[128]); the
 * Java xeno.Effect peer is a pointer to one. The position, rotation and
 * scale vectors are the Effect's translate/rotate/scale (effect.c's
 * getTranslate__/getRotate__/setScale__FFF); caster/target (+0x6bc/+0x6c0)
 * are the actors of setCaster__Lxeno_Chr_/setTarget__Lxeno_Chr_ (the
 * request's source/target), casterUnit/targetUnit (+0x6c4/+0x6c8) the map
 * units of setCaster__Lxeno_Unit_/setTarget__Lxeno_Unit_.
 *
 * sefFreeSchedulerCf (main:0x002e5118) walks the 32 effects at +0xb0,
 * passing each one to sefDestroyEffectData, and reads soundId (+0x6e0), the
 * id of the scheduler's own currently playing sound effect passed to
 * xglSoundEffectStopID, cleared once stopped.
 */
struct SchedulerState {
    Matrix4 worldMatrix;             /* +0x00 */
    Matrix4 localMatrix;             /* +0x40 */
    AlignedVector4 position;         /* +0x80 */
    AlignedVector4 rotation;         /* +0x90, radians */
    AlignedVector4 scale;            /* +0xa0 */
    EffectData effects[32];          /* +0xb0 */
    int inUse;                       /* +0x6b0 */
    unsigned char *scriptAddress;    /* +0x6b4 */
    SceneObject savedActive;         /* +0x6b8, also getPeer_Effect's Java Effect */
    struct Actor *caster;            /* +0x6bc */
    struct Actor *target;            /* +0x6c0 */
    struct MapUnitRecord *casterUnit; /* +0x6c4 */
    struct MapUnitRecord *targetUnit; /* +0x6c8 */
    int soundParameters[4];          /* +0x6cc, cleared with soundMode and soundId */
    int soundMode;                   /* +0x6dc */
    int soundId;                     /* +0x6e0 */
    unsigned char cameraChoice[0x44]; /* +0x6e4, filled by sdvSelectCamera */
    unsigned char unmodeled_728[0x730 - 0x728];
    int effectValue;                 /* +0x730, start of the 0x320-byte effect block */
    int effectScript;                /* +0x734 */
    unsigned char unmodeled_738[0x740 - 0x738];
    int effectMode;                  /* +0x740 */
    unsigned char unmodeled_744[0x9e6 - 0x744];
    short scriptParameter;           /* +0x9e6 */
    unsigned char unmodeled_9e8[0xa50 - 0x9e8];
    int alterValue;                  /* +0xa50, start of the 0x14-byte alter block */
    unsigned char unmodeled_a54[0xa60 - 0xa54];
    int alterMode;                   /* +0xa60 */
    union {
        SefEffectRequest request;    /* +0xa64, copy of the request block */
        struct {
            unsigned char unmodeled_a64[0xa74 - 0xa64];
            unsigned short targetLightCount; /* +0xa74 */
            unsigned short sourceLightCount; /* +0xa76 */
            short effectNo;                  /* +0xa78 */
            short nusumeResult;              /* +0xa7a */
            unsigned char unmodeled_a7c[0xa82 - 0xa7c];
            unsigned short initFlags;        /* +0xa82 */
            int requestParam;                /* +0xa84, the request's param word */
        };
    };
    float effectSpeed;               /* +0xa88 */
    /*
     * sefRewindEffectCf (main:0x002e7948) sets bit 0x1, sefFreeScheduler
     * (main:0x002e51a8) tests 0x202; effect.c's getForceLoop__/getClip__ read
     * bits 0x20/0x10. Signed: svDrawSchedulerBlk's (flags >> 2) is an sra.
     */
    int flags;                       /* +0xa8c */
    ScriptBinding scriptBinding;     /* +0xa90 */
    short category;                  /* +0xa98 */
    signed char display;             /* +0xa9a, Java_xeno_Effect_disp__Z; 1 when created */
    unsigned char signalByte;        /* +0xa9b */
    short delayFrames;               /* +0xa9c, frames before the first step */
    short schedulerIndex;            /* +0xa9e */
    short effectNumber;              /* +0xaa0 */
    unsigned char unmodeled_aa2[0xaa4 - 0xaa2];
    short mapperIndex;               /* +0xaa4 */
    unsigned char unmodeled_aa6[0xab0 - 0xaa6];
};

int sefIsDeadSchduler(unsigned int scheduler_index);
void sefKillEffect(int effect_no);
int sefSearchMapperIndex(int effect_id);
void sefInitEffectCf(void);
void sefExecEffect(void);
int sefLoadEffectCf(int cf_id, int effect_no);

/* The effect API reads complete 16-byte position/orientation vectors and
 * returns the allocated scheduler, or null when allocation fails. */
SchedulerState *sefCreateEffectCf(int effectNo, const Vector4 *position,
                                const Vector4 *orientation);

int sefIsFinishEffect2(int effect_no);
void sefSetLoadQue(int effect_no);
int *sefGetLoadQue(void);
void *sevGetPtAllocator2(int allocator_index);

/* Functions and data used by other translation units. */
extern int _nowEftNo;
extern Vector4 _defLight[15];
void sefRewindEffectCf(SchedulerState *scheduler);
void sefClearEffectCf(SchedulerState *scheduler);
extern int sefCreateScheduler2(int effect_no, void *parameters, void *position, int table, int flags, int index);
unsigned char *sefGetBattleData(void);
SchedulerState *sefGetNowScheduler(void);
SchedulerState *sefGetScheduler(void);
void sefDestroyEffect(void);
void sefDestroyEffectCf(void);
void sefSetupEffect(void);

#endif /* INCLUDE_MAIN_SEF_H */
