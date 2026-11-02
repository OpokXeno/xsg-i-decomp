/*
 * TU-local declarations of main/tu251 (src/main/stage_2.c).
 */

#ifndef SRC_MAIN_STAGE_2_H
#define SRC_MAIN_STAGE_2_H

#include "shared.h"
#include "main/jni.h"
#include "main/nml_model_set.h"
#include "main/init_vm.h"
#include "main/jthread.h"
#include "main/string_utf_get_hash.h"
#include "main/xgl_render.h"

/*
 * The "runnable" bit src/main/chr.h's JThread.flags documents at this same
 * offset: tested by JTHREAD_cntl (0x003058e0) and cleared by
 * JTHREAD_default unless 0x8 is set (0x00305610).
 */
#define STAGE_THREAD_RUNNING 0x10u

/* The VM thread argument every Java_xeno_* native receives is the JThread of
 * include/main/jni.h. */

/*
 * Call blocks: the natives below take no Java object receiver, only their
 * packed argument words starting at +0x00.
 */
typedef struct StageObjectCall {
    SceneObject object;
} StageObjectCall;

typedef struct StageIntCall {
    int value;
} StageIntCall;

typedef struct StageColorCall {
    float r;
    float g;
    float b;
} StageColorCall;

typedef struct StageFrameRenderCall {
    int model_id;
    int count;
} StageFrameRenderCall;

typedef struct StageJavaString StageJavaString;

typedef struct StageStringStorage StageStringStorage;

typedef struct StagePlayCall {
    SceneObject receiver;
    StageJavaString *string;
} StagePlayCall;

struct StageJavaString {
    u32 unmodeled_00;
    StageStringStorage *storage; /* +0x04 */
};

struct StageStringStorage {
    u32 unmodeled_00;
    int length;               /* +0x04 */
    const char *text;            /* +0x08 */
};

typedef struct StagePeer {
    u8 unmodeled_00[0x0c];
    u32 stage_state; /* +0x0c; cleared when a stage method is started */
} StagePeer;

typedef struct StageVisibleCall {
    int part_index;
    unsigned char visible;
} StageVisibleCall;

typedef struct StageCFBGCall {
    int draw_type;
    int unmodeled_04;
    StageBackground *background; /* +0x08 */
} StageCFBGCall;

typedef struct StageBgColorCall {
    float red;
    float green;
    float blue;
} StageBgColorCall;

typedef struct StageFadeCall {
    int mode;
    int duration_frames;
    float red;
    float green;
    float blue;
} StageFadeCall;

typedef union StageFadeValues {
    struct {
        float color[3];
        float duration;
    } components;
    unsigned long long aligned_words[2];
} StageFadeValues;

typedef union StageNativeSlot {
    int integer;
    float floating;
    SceneObject object;
    StageJavaString *method_name;
} StageNativeSlot;

/*
 * The stage's current model record (main 0x00338680+0x54, unrecovered
 * beyond this one field). setPartsLast__I forwards `id` unchanged as
 * nmlModelSetMapLastEntry's first argument.
 */
typedef struct StageModel {
    u8 unmodeled_00[4];
    int id;                  /* +0x04 */
} StageModel;

/*
 * GameLoopState is the global game-loop record (main 0x00338680, size
 * 0x2a030; TU-local by canon, config/header-canon.json). Only the fields
 * this TU's setters touch are modeled: the current stage model pointer at
 * +0x54 (setPartsLast__I), the three color components at
 * +0x80/+0x84/+0x88 (setColor__FFF) and the render-command/background-clip
 * words at +0xb0/+0xb4 (renderCommand__I, setBgClip__I; render_command is
 * named after the identical field src/main/unit.c's own
 * Java_xeno_Unit_renderCommand__I writes). Other accepted TUs name
 * different offsets of the same object under a different shape (for
 * example src/main/game_over.c's scene_id/frame_status pair), so this TU
 * does not repeat or contradict those spans.
 */
typedef struct StageGameLoopState {
    u8 unmodeled_00[0x2c];
    int background_mode;     /* +0x2c */
    int background_parameter;/* +0x30 */
    u8 unmodeled_34[0x54 - 0x34];
    StageModel *model;       /* +0x54 */
    int fade_mode;           /* +0x58 */
    int previous_fade_mode;  /* +0x5c */
    u8 unmodeled_60[0x80 - 0x60];
    float color_r;           /* +0x80 */
    float color_g;           /* +0x84 */
    float color_b;           /* +0x88 */
    u8 unmodeled_8c[0x90 - 0x8c];
    StageFadeValues fade;          /* +0x90 */
    StageFadeValues previous_fade; /* +0xa0 */
    int render_command;      /* +0xb0 */
    int bg_clip;              /* +0xb4 */
} StageGameLoopState;

/*
 * The runtime class handle for the Java xeno.Stage class (main 0x004dc7f0,
 * .sbss, gp-relative-loaded by every native below that calls
 * JNI_isInstanceOf/lookupClassField). No TU in this scope defines it.
 */
/*
 * The "peer" field name lookupClassField(classJava_xeno_Stage, ...) looks
 * up (main 0x004dc208, the string this data reference names).
 */
extern const char stagePeerFieldName[];

extern StageGameLoopState GameLoopState;
extern JavaField *lookupClassField(void *class_object, void *name, int flags);
extern JavaMethod *findMethod(JavaClass *clazz, SceneString *name,
                               void *signature_or_type);

extern void EnemySound_StopAll(int clear);
extern void nmlModelSetFadeInInterrupt(int frames, float red, float green, float blue);
extern void SCRIPT_fade(int frames);
extern void SCRIPT_load_DBG(const char *text);
extern void SCRIPT_exec(void);
extern void GameBgDrawType1Entry(int parameter, StageBackground *background);
extern void GameBgDrawType2Entry(int parameter, StageBackground *background);
/* Partial view of NmlModel (include/main/nml_model_set.h): the parts helpers
 * below read only part_count. */
typedef struct NmlModelPartListView {
    u8 unmodeled_00[0x40];
    int part_count;              /* +0x40 */
} NmlModelPartListView;

extern void nmlModelSetPartsVisible(NmlModelPartListView *parts, int part_index, int visible);
extern void nmlModelInitPartsVisible(NmlModelPartListView *parts, int visible);

#endif /* SRC_MAIN_STAGE_2_H */
