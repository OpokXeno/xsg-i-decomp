/*
 * TU-local declarations of main/tu211 (src/main/sef.c).
 */

#ifndef SRC_MAIN_SEF_H
#define SRC_MAIN_SEF_H

#define offsetof(type, member) ((__SIZE_TYPE__) &(((type *)0)->member))

typedef struct SchedulerState SchedulerState;

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
 * other accepted function of this TU reads or writes it yet.
 */
typedef struct ScriptBinding {
    short state;
    short script_id;
    short task_id;
    short link_id; /* +0x6 */
} ScriptBinding;

/*
 * sefFreeSchedulerCf (main:0x002e5118) walks 32 records of 0x30 bytes each,
 * starting at +0xb0, passing each one to sefDestroyEffectData; sefIsDeadSchduler
 * (this file) and the SchedulerRecord view of a different TU
 * (src/main/sv_get_image_item.c) already name the following int at +0x6b0
 * "inUse". sefFreeSchedulerCf reads a second int at +0x6e0, the id of the
 * scheduler's own currently playing sound effect passed to
 * xglSoundEffectStopID, cleared once stopped.
 */
struct SchedulerState {
    Matrix worldMatrix;              /* +0x00 */
    Matrix localMatrix;              /* +0x40 */
    Vector4 vectors[3]; /* +0x80 */
    unsigned char effects[32][0x30];
    int inUse;                       /* +0x6b0 */
    unsigned char *scriptAddress;    /* +0x6b4 */
    unsigned char *savedActive;      /* +0x6b8 */
    struct SefActor *targetActor;   /* +0x6bc */
    struct SefActor *sourceActor;   /* +0x6c0 */
    struct SefParentRecord *parentRecord; /* +0x6c4 */
    int savedParameter;              /* +0x6c8 */
    int soundParameters[4];          /* +0x6cc, cleared with soundMode and soundId */
    int soundMode;                   /* +0x6dc */
    int soundId;                    /* +0x6e0 */
    unsigned char cameraChoice[0x44]; /* +0x6e4, filled by sdvSelectCamera */
    unsigned char unmodeled_728[0x730 - 0x728];
    int effectValue;                 /* +0x730, start of the 0x320-byte effect block */
    int effectScript;                /* +0x734 */
    unsigned char unmodeled_738[0x740 - 0x738];
    int effectMode;                  /* +0x740 */
    unsigned char unmodeled_744[0x9e6 - 0x744];
    short scriptParameter;          /* +0x9e6 */
    unsigned char unmodeled_9e8[0xa50 - 0x9e8];
    int alterValue;                  /* +0xa50, start of the 0x14-byte alter block */
    unsigned char unmodeled_a54[0xa60 - 0xa54];
    int alterMode;                   /* +0xa60 */
    unsigned char request[0xa74 - 0xa64]; /* +0xa64, copy of the SefEffectRequest */
    unsigned short targetLightCount; /* +0xa74 */
    unsigned short sourceLightCount; /* +0xa76 */
    short effectNo;                  /* +0xa78 */
    short nusumeResult;              /* +0xa7a */
    unsigned char unmodeled_a7c[0xa82 - 0xa7c];
    unsigned short initFlags;        /* +0xa82 */
    int requestParam;                /* +0xa84, the request's param word */
    float effectSpeed;               /* +0xa88 */
    /*
     * SchedulerState's flags word, the only field of the 0xab0-byte record this
     * TU dereferences by name: sefRewindEffectCf (main:0x002e7948) sets bit 0x1
     * of it, and sefFreeScheduler (main:0x002e51a8) tests the same word with a
     * wider mask (0x202). Bytes before it are not yet evidenced by any accepted
     * function in this TU.
     */
    int flags;
    ScriptBinding scriptBinding;
    short category;                  /* +0xa98 */
    signed char extraByte;           /* +0xa9a */
    unsigned char signalByte;        /* +0xa9b */
    short delayFrames;               /* +0xa9c, frames before the first step */
    short schedulerIndex;            /* +0xa9e */
    short effectNumber;              /* +0xaa0 */
    unsigned char unmodeled_aa2[0xaa4 - 0xaa2];
    short mapperIndex;               /* +0xaa4 */
    unsigned char unmodeled_aa6[0xab0 - 0xaa6];
};

/* A named .lit4 literal (config/symbols/main.txt), read by sefGetCirclePos;
 * referencing it by symbol keeps the compiler's own literal pool from
 * allocating a new, differently placed constant for the same original
 * GP-relative slot. */
extern const float circle_angle_scale_literal; /* 0x004d82e8, pi/180 */

/*
 * config/units/main-sef-lerp-vector-sc.json (sefLerpVectorSC @ 0x002e1908):
 * the effect scale factor is a scaffold-owned .lit4 word at 0x004d82dc,
 * reachable image-wide as the linker symbol `effect_scale_literal_0_1`
 * (config/symbols/main.txt). Reading it as a real extern keeps the original
 * gp-relative relocation instead of a hand-written $gp offset
 */
extern const float effect_scale_literal_0_1; /* 0x004d82dc, 0.1f */

/*
 * config/units/math-main-w3-002e7f40.json (sefDeg2RadVector @ 0x002e7f40):
 * the degrees-to-radians factor is a scaffold-owned .lit4 word at 0x004d8354,
 * reachable image-wide as the linker symbol `lit4_004d8354`
 * (config/symbols/main.txt). Reading it as a real extern keeps the original
 * gp-relative relocation instead of a hand-written $gp offset
 */
extern const float lit4_004d8354;

/*
 * sefCalcRotChange (main:0x002e2f00): a second, distinct .lit4 occurrence of
 * the same degrees-to-radians factor as lit4_004d8354 above, at its own
 * address 0x004d8318. Unlike lit4_004d8354, no accepted record has renamed
 * this word yet, so it still keeps the scaffold's own splat label
 * (build/main/asm/main/data/sef.lit4.s: `dlabel DEG_TO_RAD`), and this extern
 * names it verbatim rather than inventing a new symbol the scaffold does not
 * define
 */
extern const float DEG_TO_RAD;

/*
 * sefGetDmgNull (main:0x002e7e40): 0x0079411C is _battleData + SEF_BATTLE_PHASE
 * (0x00794110 + 0x00c), the same battle-phase field sefCalcLocalMatrix reads
 * as *(short *)(battle + SEF_BATTLE_PHASE) (this file, main:0x002e2d48).
 * Compiled in isolation (no other _battleData field is touched in
 * sefGetDmgNull), the original computes this address directly against its
 * own scaffold-owned .bss symbol instead of folding +0x00c onto
 * _battleData's own relocation, so this declaration keeps the scaffold's own
 * splat name rather than reusing _battleData (docs/naming.md,
 * "Scaffold-owned data keeps its splat name"); measured with
 * tools/asm_diff.py, `_battleData + SEF_BATTLE_PHASE` here relocates against
 * _battleData with the offset folded into the load's own immediate
 * (`lh v1,12(v0)`), while the original relocates against this narrower
 * symbol with a zero immediate (`lh v1,0(v0)`) -- byte-identical once linked,
 * but relocations_equal only holds for the latter.
 */
extern short sefDmgNullCount[];

int sefIsDeadSchduler(unsigned int scheduler_index);
SchedulerState *sefGetNowScheduler(void);

/* sefGetDirMatrix (main:0x002e2818) is defined later in this TU; declared
 * here so sefGetVecMatrix (main:0x002e2908) can call it. Caller evidence
 * (a0 unchanged destination matrix, a1 the temporary direction vector). */
static void sefGetDirMatrix(Matrix4 *dest, Vector4 *dir);

/* sefRandf (main:0x002e11b0) is defined later in this TU; declared here
 * so sefGetCirclePos (main:0x002e2280) can call it. Returns a unit random
 * scalar in f0. */
static float sefRandf(void);

/* Original TU-local current-scheduler pointer (main:0x004DC670). */
static SchedulerState *_nowScheduler;

/* sefGetLineAdr (main:0x002e7ea0): the line-effect table, scaffold-owned like
 * _scheduler/_ptAlloc/_battleData above; only its pointer accessor is C. */
/* 128 records of the evidenced 0x820-byte line-data stride. */
static unsigned char _lineData[0x41000];

#endif /* SRC_MAIN_SEF_H */
