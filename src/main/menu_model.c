#include "common.h"
#include "main/xgl_studio.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "main/act_1.h"
#include "main/xgl_2.h"
#include "main/xgl_pad.h"

#define MENU_MODEL_ACTOR_DRAW_STATE_OFFSET 0xc0

#define MENU_MODEL_ACTOR_DRAW_STATE(actor) \
    ((unsigned char *)(actor) + MENU_MODEL_ACTOR_DRAW_STATE_OFFSET)

#define MENU_MODEL_DRAW_STATE_PROGRESS_OFFSET 0x60
#define MENU_MODEL_DRAW_STATE_FUNC_OFFSET     0x64

#define MENU_MODEL_DRAW_STATE_PROGRESS(state) \
    ((float *)((state) + MENU_MODEL_DRAW_STATE_PROGRESS_OFFSET))

#define MENU_MODEL_DRAW_STATE_FUNC(state) \
    ((void (**)(MenuModelActor *))((state) + MENU_MODEL_DRAW_STATE_FUNC_OFFSET))

typedef struct Actor Actor;

typedef struct MenuModelScaleLimits {
    float value[2];
} MenuModelScaleLimits;

/* The two controller records are 0x68 bytes; the menu controls read +0x30. */
typedef struct MenuModelPadRecord {
    unsigned char unmodeled_00[0x30];
    unsigned short buttons;
    unsigned char unmodeled_32[0x36];
} MenuModelPadRecord;

/*
 * The model actor MenuModelDrawTypeSet (0x00280dd0) configures: it takes the
 * actor in a0 and the draw type (0..4, a jump table) in a1 and writes the
 * actor's +0x60/+0x64 fields. Only pointers to it are used here.
 */
typedef struct MenuModelActor MenuModelActor;

typedef struct MenuModelDrawTypeState {
    unsigned char unmodeled_00[0x60];
    float progress; /* actor +0x120; MenuModelAlphaDraw reads this as transparency */
    void (*drawTypeFunction)(MenuModelActor *actor); /* actor +0x124 */
} MenuModelDrawTypeState;

/*
 * +0x1: MenuModelSubWindowBreak forces this to -1 immediately before tail-
 * calling MenuModelSubWindowMainSub (0x0027fc88), which loads the same
 * pointer from MenuModelUnit.subWindow and reads this byte to decide how to
 * tear the subwindow down.
 */
typedef struct MenuModelSubWindowState {
    unsigned char unmodeled_00[1];
    signed char closeFlag;
} MenuModelSubWindowState;

/* The external unit-resource record fields consulted while naming a weapon motion. */
typedef struct MenuModelUnitResourceData {
    unsigned char unmodeled_00[0x5A];
    signed char weaponCharacter[3];       /* +0x5A */
    unsigned char unmodeled_5D[1];
    short weaponResource[3];             /* +0x5E */
} MenuModelUnitResourceData;

/*
 * Partial layout of the menu model "unit" object (main/tu165), recovered from
 * this allocation's own field accesses: MenuModelUnitOpen (+0x20),
 * MenuModelExtFuncSet (+0x13, +0x40, +0x44) and MenuModelWeaponActorGet's
 * dereferenced weapon slot array (+0x34). Only these offsets are evidenced;
 * the unmodeled_XX spans are untouched by this allocation.
 */
/*
 * +0x24/+0x2C: a second (actor, resource) pair, recovered from
 * MenuModelUnitDispose, which passes &actor2/&resource2 to
 * ActorAndResourceDispose with mode 1, alongside the existing +0x20/+0x28
 * pair passed with mode 0. MenuModelUnitActorSet (main:0x002800b0) stores a
 * second ACT_create(-2) result at +0x24 (the first, ACT_create(-1), goes to
 * +0x20) and loads a pointer from +0x28. +0x11, +0x14, +0x1C/+0x1D, +0x30
 * and +0x48 are recovered only as bytes CreateInit clears; no other function
 * in this allocation reads or writes them.
 */
typedef struct MenuModelUnit {
    unsigned char unmodeled_00[0x10];
    signed char taskPhase;                /* +0x10: forced to -1 by MenuModelUnitBreak once its pending resource request is cancelled */
    unsigned char unusedByCreateInit0;    /* +0x11 */
    unsigned char unmodeled_12[1];
    unsigned char extFuncFlag;            /* +0x13: cleared whenever the ext functions below are (re)installed */
    struct MenuModelUnitResourceData *unusedByCreateInit1; /* +0x14: resource-data pointer read by MenuModelResourceLoad */
    unsigned short modelId;               /* +0x18 */
    unsigned short resourceId;            /* +0x1A */
    unsigned char unusedByCreateInit2[2]; /* +0x1C/+0x1D */
    unsigned char unmodeled_1E[2];
    MenuModelActor *actor;                /* +0x20: the unit's own model, passed to MenuModelDrawTypeSet */
    MenuModelActor *actor2;               /* +0x24: second model, disposed with mode 1 by MenuModelUnitDispose */
    void *resource;                       /* +0x28: actor's resource slot, disposed with mode 0 by MenuModelUnitDispose */
    void *resource2;                      /* +0x2C: actor2's resource slot, disposed with mode 1 by MenuModelUnitDispose */
    int skipWeaponRegionCleanup;              /* +0x30 */
    struct MenuModelWeapon *weapon[3];           /* +0x34: one slot per MenuModelUnitOpen weapon loop iteration */
    void *extFunc1;                       /* +0x40: MenuModelExtFuncSet's second argument */
    void *extFunc2;                       /* +0x44: MenuModelExtFuncSet's third argument */
    int *unusedByCreateInit4;             /* +0x48: address of the caller's output handle */
    struct MenuModelSubWindowState *subWindow;   /* +0x4C: tested/forced by MenuModelSubWindowMain/Break, processed by MenuModelSubWindowMainSub */
} MenuModelUnit;

typedef struct MenuModelWeapon {
    unsigned char unmodeled_00[0x10];
    signed char taskPhase;                /* +0x10: cancellation and failed creation */
    unsigned char active;                 /* +0x11: task has opened its actor */
    unsigned char unmodeled_12;
    unsigned char extFuncFlag;            /* +0x13: cleared on task startup */
    unsigned char unmodeled_14[4];
    unsigned short weaponId;             /* +0x18: MenuRWeaponCheck result */
    unsigned short resourceId;            /* +0x1A: menu resource selected for this weapon */
    unsigned char weaponIndex;            /* +0x1C */
    unsigned char resourceLoaded;         /* +0x1D */
    short characterId;                    /* +0x1E: sign-extended resource byte */
    MenuModelActor *actor; /* +0x20: returned by MenuModelWeaponActorGet; MenuModelWeaponOpen hands it to MenuModelDrawTypeSet */
    unsigned char unmodeled_24[4];
    /*
     * +0x28: MenuModelWeaponDispose passes the address of this field to
     * ActorAndResourceDispose as the second (resource) slot, alongside
     * &actor; that callee clears whatever pointer it finds there, but nothing
     * recovered so far dereferences it, so its pointee type stays unproven.
     */
    void *resource;
    unsigned char unmodeled_2C[4];
    struct MenuModelUnit *unit;           /* +0x30: owning menu unit */
    unsigned char unmodeled_34[0xC];
    void (*extFunc)(struct MenuModelWeapon *weapon, void *context); /* +0x40 */
    void *extContext;                     /* +0x44 */
    int *result;                         /* +0x48: caller output handle */
    unsigned char unmodeled_4C[4];        /* task allocation initialized by CreateInit */
} MenuModelWeapon;

typedef struct MenuModelWeaponLoadTask {
    unsigned char unmodeled_00[0x18];
    unsigned short resourceId;            /* +0x18 */
    unsigned char unmodeled_1A[2];         /* +0x1A */
    unsigned char weaponIndex;            /* +0x1C */
    unsigned char resourceLoaded;         /* +0x1D */
    unsigned char characterId;            /* +0x1E */
    unsigned char unmodeled_1F[0x11];
    struct MenuModelUnit *unit;                  /* +0x30 */
} MenuModelWeaponLoadTask;

/*
 * The object bound to a menu model draw task, recovered from
 * MenuModelDrawTypeMain's own access. Only +0x20 is evidenced.
 */
typedef struct MenuModelDrawContext {
    unsigned char unmodeled_00[0x20];
    struct MenuModelDrawable *drawable; /* +0x20 */
} MenuModelDrawContext;

/*
 * The object MenuModelDrawTypeMain calls through, recovered from that
 * function's own access. Only +0x124 is evidenced.
 */
typedef struct MenuModelDrawable {
    unsigned char unmodeled_00[0x124];
    void (*drawFunc)(struct MenuModelDrawable *self); /* +0x124 */
} MenuModelDrawable;

typedef union MenuModelTransformVector {
    struct {
        float x;
        float y;
        float z;
        float w;
    } value;
    unsigned long long words[2];
} MenuModelTransformVector;

typedef struct MenuModelTransformControl {
    MenuModelTransformVector offset;
    MenuModelTransformVector rotation;
    MenuModelTransformVector scale;
    int enabled;
    unsigned char unmodeled_34[0xC];
    MenuModelTransformVector position;
    unsigned char unmodeled_50[0x10];
    float transparency; /* +0x60: menu draw progress */
    void (*drawTypeFunction)(MenuModelActor *actor); /* +0x64 */
    int initialMotionId; /* +0x68: selected before opening the unit actor */
    unsigned char unmodeled_6C[4];
} MenuModelTransformControl;

typedef unsigned int XglLightQuadword __attribute__((mode(TI)));

typedef struct {
    XglLightQuadword intensity;
    XglLightQuadword direction;
} XglPackedParallelLight;

typedef struct {
    XglLightQuadword ambientIntensity;
    XglPackedParallelLight parallel[3];
} XglPackedLightSet;

typedef struct MenuModelActorLight {
    XglPackedLightSet source;
    Matrix4 directionMatrix;
    Matrix4 intensityMatrix;
} MenuModelActorLight;

/*
 * Body for the actor tag this TU's own prelude only forward-declares above.
 * MenuModelAlphaDraw reads the float at +0x120 (mtc1 0x3f800000 compared with
 * c.lt.s against it at 0x00280018) before adjusting the model's transparency;
 * no other offset of this type is evidenced by this allocation.
 */
/*
 * +0x86: nonzero gates ACT_dispose in ActorAndResourceDispose, which clears
 * it afterward. MenuModelUnitActorSet (main:0x002800e0) also reads/writes
 * this halfword and extracts its top nibble as a type selector, so it packs
 * more than a plain boolean; only the zero/nonzero state is evidenced here.
 */
struct MenuModelActor {
    unsigned int actorFlags; /* +0x00: update/draw state flags */
    void (*update)(MenuModelActor *actor); /* +0x04 */
    void (*draw)(MenuModelActor *actor);   /* +0x08 */
    unsigned char unmodeled_0C[4];
    MenuModelTransformVector position;
    unsigned char unmodeled_20[0x30];
    MenuModelTransformVector rotation;
    MenuModelTransformVector scale;
    MenuModelTransformVector navel; /* +0x70: captured after motion and draw */
    unsigned char unmodeled_80[6];
    short flags; /* +0x86 */
    unsigned char unmodeled_88[0x38];
    MenuModelTransformControl menu;
    unsigned char unmodeled_130[0x3e0];
    MenuModelActorLight light; /* +0x510 */
    unsigned char unmodeled_600[0xf0];
    unsigned int motionFlags; /* +0x6F0: weapon 19 enables bit 3 */
    unsigned char unmodeled_6F4[0x1DC];
    void *modelResource;       /* +0x8D0 */
    void *matrixResource;      /* +0x8D4 */
    void *move;                /* +0x8D8 */
    void *animationPackages[8]; /* +0x8DC */
    unsigned char unmodeled_8FC[0xA4];
    unsigned int drawEnvironmentFlags; /* +0x9A0: ACT_setDrawEnv flags */
    unsigned int renderLevel;  /* +0x9A4: nmlModelSetRenderLevel */
    int pixelAlpha;            /* +0x9A8: nmlModelSetPixelAlpha */
    int partsPixelAlphaCount;  /* +0x9AC */
    unsigned char unmodeled_9B0[0x20];
    float distanceClipThreshold; /* +0x9D0: ACT_modelDrawSub camera-distance test */
};

/* A partial actor view used only to form the recovered draw-state address. */
typedef struct MenuModelActorDrawTypeView {
    unsigned char unmodeled_00[0xC0];
    MenuModelDrawTypeState drawTypeState;
} MenuModelActorDrawTypeView;

typedef struct MenuModelSubWindowAccess {
    unsigned char unmodeled_00[1];
    signed char closeFlag;
    unsigned char studioId;    /* +0x2 */
    unsigned char mode;        /* +0x3 */
    short x;                   /* +0x4 */
    short y;                   /* +0x6 */
    short width;               /* +0x8 */
    short height;              /* +0xA */
    StudioCamera *camera;      /* +0xC */
} MenuModelSubWindowAccess;

typedef struct MenuModelActorSubWindowAccess {
    unsigned char unmodeled_00[0x676];
    short subWindowId;
} MenuModelActorSubWindowAccess;

typedef MenuModelActor MenuModelMotionActor;

typedef struct MenuModelMotionTarget {
    unsigned char unmodeled_00[0x20];
    MenuModelMotionActor *actor;
} MenuModelMotionTarget;

/* One 0x1C-byte memory region returned by MenuModelMemoryGet. */
typedef struct MenuModelResourceAccess {
    unsigned char status;                 /* +0x0 */
    unsigned char memoryType;             /* +0x1 */
    unsigned short resourceId;            /* +0x2 */
    void *file[6];                        /* +0x4 */
} MenuModelResourceAccess;

/*
 * Partial layout of the menu-model "memory" state buffer (main/tu165).
 * MenuModelMenuMotionCheck reads state; MenuModelMemoryInit sets base and
 * size; MenuModelMemoryGet and MenuModelResourceCancel3 establish the sixteen
 * 0x1C-byte resource regions at +0xC.
 */
typedef struct MenuModelMemoryStateBuffer {
    int state; /* +0x0: memory-state code; 10-12 or 20 support menu motion */
    void *base; /* +0x4 */
    int size; /* +0x8 */
    MenuModelResourceAccess regions[16]; /* +0xC */
} MenuModelMemoryStateBuffer;

/* Record zero is the active request; the remaining fifteen records are queued.
 * MenuModelResourceMain moves a queued record into zero and forwards loadParam.
 */
typedef struct MenuModelResourceRequestRecord {
    unsigned short resourceId;
    unsigned char *result; /* Output object representation for the resource pointer. */
    void *loadParam;
} MenuModelResourceRequestRecord;

typedef struct MenuModelResourceStateBuffer {
    unsigned int resetWord;
    struct MenuModelResourceAccess *resource;
    MenuModelResourceRequestRecord requests[16];
    int motionHandle;
} MenuModelResourceStateBuffer;

typedef struct MenuModelResource {
    unsigned char status; /* +0x0 */
    unsigned char unmodeled_01[1];
    short handle; /* +0x2 */
} MenuModelResource;

typedef struct MenuModelTaskWork {
    unsigned char unmodeled_00[0x10];
    signed char taskPhase;
    unsigned char unmodeled_11[3];
    struct MenuModelUnitResourceData *unitData;
    unsigned short objectId;
    unsigned short resourceId;
    unsigned char weaponIndex;
    unsigned char resourceLoaded;
    short characterId;
    MenuModelActor *actor;
    MenuModelActor *actor2;
    void *resource;
    void *resource2;
    struct MenuModelTaskWork *owner;
    struct MenuModelTaskWork *weapon[3];
    void *extFunc;
    void *extContext;
    int *result;
    struct MenuModelSubWindowState *subWindow;
} MenuModelTaskWork;

typedef struct MenuModelZClearVertex {
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int fog;
} MenuModelZClearVertex;

typedef union {
    XglLightQuadword quadword;
    Vector4 value;
} XglLightVector;

typedef struct {
    Vector4 intensity;
    Vector4 direction;
} XglParallelLight;

typedef struct {
    Vector4 ambientIntensity;
    XglParallelLight parallel[3];
} XglLightSet;

extern MenuModelPadRecord PadData[2];
extern const MenuModelScaleLimits minimumScaleTable[];
extern const MenuModelScaleLimits maximumScaleTable[];
extern const char navelRotationXFormat[16];
extern const char navelRotationYFormat[16];
extern const char navelRotationZFormat[16];
extern const char navelScaleXFormat[16];

void *func_A1C910(void *packed_file);
void ACT_modelDraw(Actor *actor);
void ACT_updateDefault(MenuModelActor *actor);
void MenuModelResourceRequest(void *result, unsigned short resourceId, void *loadParam);
void MenuModelUnitActorSet(MenuModelUnit *unit);
void nmlModelUseSubWindow(unsigned int studioId, int enabled);
void nmlModelSetWindow(unsigned int studioId);
void xglCameraInit(StudioCamera *camera);
void xglCameraSetWindow(StudioCamera *camera, int x, int y, int width, int height);
void xglStudioChange(int studioId);
void ACT_setMotion(MenuModelMotionActor *actor, unsigned int dataId);
extern char *MenuFileNameGet(short resourceId, int fileType);
extern void MenuLoadFile(const char *name, void *buffer);

/* With OV01 loaded, this import resolves to dataMotNameGetMenu. */
extern int func_A1ADC8(char *name, int modelId, int weaponIndex);

static void MenuModelDrawTypeSet(MenuModelActor *actor, int drawType);
void MenuModelWeaponOpen(MenuModelUnit *unit, int drawType, int weaponIndex);
void MenuModelResourceMain(void);
void MenuModelResourceInit(void);
void MenuModelSubWindowinit(void);
void MenuModelResourceCancel(void);
void MenuModelResourceCancel3(MenuModelUnit *unit);
int MenuLoadSync(void);
void MenuLoadCancel(void);
static void MenuModelSubWindowMainSub(MenuModelUnit *unit);
static void ActorAndResourceDispose(MenuModelActor **actorSlot, void **resourceSlot, int mode);

/* Builds a task list of `capacity` 0x80-byte tasks at `pool` and returns the
 * first byte after them (0 when capacity is 0), see 0x0021c528. */
void *xglTaskInitial(void *pool, int capacity, int flags);

/* nml_model_set.c (main); no header is published for it yet. */
extern void nmlModelSetToumei(int enabled);

extern void nmlModelSetTransparency(float transparency);
extern void nmlModelSetZwrite(int enabled);
void ACT_dispose(MenuModelActor *actor);
extern int MenuRWeaponCheck(int weaponId, long equipType);
extern int func_A1A330(int weaponId);
extern void nmlModelDirectSend(int mode, unsigned char *data, int count);
void ACT_setModelWrapper(MenuModelActor *actor, int mode);
void ACT_initMotion(MenuModelActor *actor);
int ACT_setFace(MenuModelActor *face, MenuModelActor *parent, int mode);
void ACT_setHand(MenuModelActor *actor, int mask);
void xglLightIntensityAmbient(XglPackedLightSet *lightSet, const XglLightQuadword *intensity);
void xglLightIntensityParallel(XglPackedLightSet *lightSet, unsigned int index, const XglLightQuadword *intensity);
void xglLightDirection(XglLightSet *lightSet, unsigned int index, const Vector4 *source);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
static void CreateInit(MenuModelUnit *unit);
void MenuModelWeaponActorSet(MenuModelUnit *unit, MenuModelWeapon *weapon);
void ACT_updateMotion(MenuModelActor *actor);
int MenuScenarioNoGet(void);
int func_A1A2E8(int modelId);
MenuModelUnitResourceData *func_A191C0(int modelId);

static unsigned short guno_eid_tbl[29];

XglTaskScheduler *MenuModelTask = 0;

int MenuModelFlag = 0;

/* First .data object of the original TU (0x0036D2F0): MenuModelCreate reads
   it through a folded base 34 entries before its start. */
static unsigned short guno_eid_tbl[29] = {
    0x4001, 0x4002, 0x4003, 0x4004, 0x4005, 0x4006, 0x4007, 0x4008,
    0x4009, 0x400A, 0x400B, 0x400C, 0x400D, 0x400E, 0x400F, 0x4010,
    0x4011, 0x4012, 0x4013, 0x4014, 0x4015, 0x4016, 0x4017, 0x4018,
    0x4019, 0x4001, 0x4002, 0x4003, 0x4037
};

MenuModelMemoryStateBuffer MenuModelMemoryState = {0};

MenuModelResourceStateBuffer MenuModelResourceState = {0};

MenuModelSubWindowAccess MenuModelSubWindow[3] = {{0}};

/* Shared fade-step values used by the draw-type routines below. */
float modelDrawOpen01Step
    = 0.1f;

float modelDrawClose01Step
    = 0.1f;

float modelDrawClose02Step
    = 0.1f;

float modelDrawClose02MinProgress
    = 0.2f;

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char wpMeqpMotionPath[0x18] = "data\\motion\\wp_meqp.fpk";

void MenuModelMemorySet(int state)
{
    MenuModelMemoryStateBuffer *memoryState;
    MenuModelResourceAccess *region;
    unsigned char *memory;
    int i;

    MenuModelFlag = 0;
    MenuModelMemoryState.state = state;
    for (i = 0; i < 16; i++) {
        MenuModelMemoryState.regions[i].status = 0;
        MenuModelMemoryState.regions[i].memoryType = 0;
        MenuModelMemoryState.regions[i].resourceId = 0;
    }
    memoryState = &MenuModelMemoryState;
    memset(memoryState->base, 0, memoryState->size);
    memory = memoryState->base;
    region = memoryState->regions;
    switch (state) {
    case 0:
        region->file[0] = memory;
        memory += 0xA9000;
        region->file[1] = memory;
        memory += 0x3C000;
        region->file[2] = memory;
        memory += 0x3000;
        region->file[3] = memory;
        region->memoryType = 2;
        region++;
        memory += 0x19800;
        region->file[0] = memory;
        memory += 0x6C000;
        region->file[1] = memory;
        memory += 0x3C000;
        region->file[2] = memory;
        memory += 0x3000;
        region->file[3] = memory;
        memory += 0x18000;
        region->file[4] = memory;
        memory += 0x32000;
        region->file[5] = memory;
        region->memoryType = 1;
        break;
    case 1:
        for (i = 0; i < 2; i++, region++) {
            region->file[0] = memory;
            memory += 0x60000;
            region->file[1] = memory;
            memory += 0x22000;
            region->file[2] = memory;
            memory += 0x3000;
            region->file[3] = memory;
            memory += 0x40000;
            region->memoryType = 2;
        }
        break;
    case 10: {
        region->file[0] = memory;
        memory += 0xA4800;
        region->file[1] = memory;
        memory += 0x40800;
        region->file[2] = memory;
        memory += 0x3000;
        region->file[4] = memory;
        memory += 0x40000;
        region->memoryType = 1;
        region->file[5] = memory;
        memory += 0x2000;
        region++;
        for (i = 0; i < 5; i++) {
            region->file[0] = memory;
            memory += 0x20000;
            region->file[1] = memory;
            memory += 0x14000;
            region->file[2] = memory;
            memory += 0x8000;
            region->file[3] = memory;
            memory += 0x10000;
            region->memoryType = 3;
            region++;
        }
        region->file[3] = memory;
        region->memoryType = 4;
        break;
    }
    case 11:
        for (i = 0; i < 10; i++) {
            unsigned char *nextMemory;
            region->file[0] = memory;
            memory += 0x20000;
            region->file[1] = memory;
            memory += 0x14000;
            region->file[2] = memory;
            memory += 0x8000;
            region->file[3] = memory;
            nextMemory = memory + 0x10000;
            region->memoryType = 3;
            region++;
            memory = nextMemory;
        }
        region->file[0] = memory;
        memory += 0xA0000;
        region->file[1] = memory;
        memory += 0x41000;
        region->file[2] = memory;
        region->memoryType = 2;
        region++;
        memory += 0x3000;
        region->file[3] = memory;
        region->memoryType = 4;
        break;
    case 12:
        for (i = 0; i < 3; i++) {
            region->file[0] = memory;
            memory += 0xA4800;
            region->file[1] = memory;
            memory += 0x40800;
            region->file[2] = memory;
            memory += 0x3000;
            region->file[4] = memory;
            memory += 0x40000;
            region->file[5] = memory;
            region->memoryType = 1;
            region++;
            memory += 0x2000;
        }
        region->file[3] = memory;
        region->memoryType = 4;
        break;
    case 20:
        region->file[0] = memory;
        memory += 0xA0000;
        region->file[1] = memory;
        memory += 0x41000;
        region->file[2] = memory;
        region->memoryType = 2;
        region++;
        memory += 0x3000;
        region->file[3] = memory;
        region->memoryType = 4;
        break;
    }
}

MenuModelResourceAccess *MenuModelMemoryGet(unsigned short resourceId, int memoryType)
{
    int i;

    (void)resourceId;
    for (i = 0; i < 16; i++) {
        if ((MenuModelMemoryState.regions[i].status == 0) &&
            (MenuModelMemoryState.regions[i].memoryType == memoryType)) {
            return &MenuModelMemoryState.regions[i];
        }
    }
    return 0;
}

void MenuModelMemoryInit(void *base, int size) {
    memset(&MenuModelMemoryState, 0, sizeof(MenuModelMemoryState));
    MenuModelMemoryState.base = base;
    MenuModelMemoryState.size = size;
    MenuModelMemoryState.state = -1;
}

void MenuModelResourceInit(void)
{
    int i;
    MenuModelResourceStateBuffer *state;

    state = &MenuModelResourceState;
    for (i = 0; i < 16; i++) {
        state->requests[i].result = 0;
        state->requests[i].resourceId = 0;
        state->requests[i].loadParam = 0;
    }
    state->resource = 0;
    state->motionHandle = 0;
    state->resetWord = 0;
}

int MenuModelMenuMotionCheck(void) {
    if ((MenuModelMemoryState.state >= 10) && ((MenuModelMemoryState.state < 13) || (MenuModelMemoryState.state == 20))) {
        return 1;
    }
    return 0;
}

int MenuModelMenuMotionGet(void) {
    int handle;

    handle = 0;
    if ((MenuModelMenuMotionCheck() == 1) && (MenuModelFlag & 2)) {
        handle = MenuModelResourceState.motionHandle;
    }
    return handle;
}

void MenuModelResourceCancel(void)
{
    MenuModelResourceAccess *resource;
    void *result;

    if (MenuLoadSync()) {
        MenuLoadCancel();
        resource = MenuModelResourceState.resource;
        if (resource != 0) {
            resource->status = 0;
        }
        result = MenuModelResourceState.requests[0].result;
        MenuModelResourceState.resource = 0;
        if (result != 0) {
            int failure = -1;
            unsigned int *outputWord = result;
            __builtin_memcpy(outputWord, &failure, sizeof(failure));
        }
        MenuModelResourceState.requests[0].result = 0;
        MenuModelResourceState.requests[0].resourceId = 0;
        MenuModelResourceState.requests[0].loadParam = 0;
    }
}

void MenuModelResourceCancel3(MenuModelUnit *unit)
{
    int resourceIds[4];
    int resourceCount;
    int i;
    int j;

    if (unit == 0) {
        return;
    }
    resourceIds[0] = unit->resourceId;
    resourceCount = 1;
    for (i = 0; i < 3; i++) {
        MenuModelWeapon *weapon = unit->weapon[i];
        if (weapon != 0) {
            resourceIds[resourceCount++] = weapon->resourceId;
        }
    }
    for (j = 0; j < resourceCount; j++) {
        int resourceId = resourceIds[j];
        for (i = 1; i < 16; i++) {
            if (MenuModelResourceState.requests[i].resourceId == resourceId) {
                void *result = MenuModelResourceState.requests[i].result;
                MenuModelResourceState.requests[i].resourceId = 0;
                if (result != 0) {
                    int failure = -1;
                    unsigned int *outputWord = result;
                    __builtin_memcpy(outputWord, &failure, sizeof(failure));
                }
                MenuModelResourceState.requests[i].result = 0;
                MenuModelResourceState.requests[i].loadParam = 0;
            }
        }
    }
    if (unit->skipWeaponRegionCleanup == 0) {
        for (i = 0; i < 3; i++) {
            if (unit->weapon[i] != 0) {
                for (j = 0; j < 16; j++) {
                    if (MenuModelMemoryState.regions[j].resourceId ==
                        unit->weapon[i]->resourceId) {
                        MenuModelMemoryState.regions[j].status = 0;
                        MenuModelMemoryState.regions[j].resourceId = 0;
                        break;
                    }
                }
            }
        }
    }
}

void MenuModelResourceCancel2(MenuModelUnit *unit)
{
    MenuModelResourceCancel();
    MenuModelResourceCancel3(unit);
}

void MenuModelResourceLoad(int resourceId, MenuModelResourceAccess *buffers,
                           int resourceMode, MenuModelWeaponLoadTask *weaponTask)
{
    char fileName[0x80];
    unsigned int motionPackageOffset;

    if ((resourceId & 0x8000U) != 0) {
        MenuLoadFile(MenuFileNameGet((short)(resourceId & 0x7FFFU), 3), buffers->file[3]);
        return;
    }

    switch (resourceMode) {
    case 1:
        MenuLoadFile(MenuFileNameGet((short)resourceId, 4), buffers->file[4]);
        MenuLoadFile(MenuFileNameGet((short)resourceId, 5), buffers->file[5]);
        /* fall through */
    case 2:
        if (MenuModelMenuMotionCheck() == 0) {
            MenuLoadFile(MenuFileNameGet((short)resourceId, 3), buffers->file[3]);
        }
        break;
    case 3:
        break;
    default:
        return;
    }
    MenuLoadFile(MenuFileNameGet((short)resourceId, 0), buffers->file[0]);
    MenuLoadFile(MenuFileNameGet((short)resourceId, 1), buffers->file[1]);
    MenuLoadFile(MenuFileNameGet((short)resourceId, 2), buffers->file[2]);

    motionPackageOffset = (unsigned int)resourceId - 0x3007U;
    if (motionPackageOffset < 4U) {
        MenuLoadFile(wpMeqpMotionPath, buffers->file[3]);
    }

    {
        MenuModelUnit *unit;
        MenuModelUnitResourceData *unitData;
        unsigned int weaponIndex;
        signed char savedCharacter;
        short savedWeaponResource;
        unsigned short modelId;
        int hasMotionName;

        if (weaponTask == 0) {
            return;
        }
        weaponIndex = weaponTask->weaponIndex;
        if (weaponTask->resourceId <= 0x45) {
            return;
        }
        unit = weaponTask->unit;
        if (unit == 0) {
            return;
        }
        unitData = unit->unusedByCreateInit1;
        modelId = unit->modelId;
        if (unitData == 0) {
            return;
        }
        savedCharacter = unitData->weaponCharacter[weaponIndex];
        savedWeaponResource = unitData->weaponResource[weaponIndex];
        unitData->weaponResource[weaponIndex] = weaponTask->resourceId;
        unitData->weaponCharacter[weaponIndex] = (signed char)weaponTask->characterId;
        hasMotionName = func_A1ADC8(fileName, modelId, weaponIndex);
        unitData->weaponResource[weaponIndex] = savedWeaponResource;
        unitData->weaponCharacter[weaponIndex] = savedCharacter;
        if ((hasMotionName != 0) && (fileName[0] != '\0')) {
            MenuLoadFile(fileName, buffers->file[3]);
            weaponTask->resourceLoaded = 1;
            return;
        }
    }
}

void MenuModelResourceMain(void)
{
    int memoryTypes[5] = {1, 1, 2, 3, 2};
    MenuModelResourceStateBuffer *activeState;
    unsigned int resourceClass = 0;
    unsigned short resourceId;
    int i;

    if (MenuModelFlag & 1) {
        MenuModelResourceCancel();
        MenuModelResourceInit();
        return;
    }
    if (MenuLoadSync() != 0) {
        return;
    }
    if (MenuModelResourceState.resource != 0) {
        MenuModelResourceAccess *resource;
        unsigned int *result;
        unsigned short completedResourceId;
        MenuModelResourceState.resource->status = 1;
        resource = MenuModelResourceState.resource;
        completedResourceId = MenuModelResourceState.requests[0].resourceId;
        resource->resourceId = completedResourceId;
        if (completedResourceId & 0x8000) {
            MenuModelFlag |= 2;
        }
        result = (void *)MenuModelResourceState.requests[0].result;
        MenuModelResourceState.requests[0].result = 0;
        __builtin_memcpy(result, &resource, sizeof(resource));
        MenuModelResourceState.resource = 0;
    }
    if (MenuModelResourceState.requests[0].result == 0) {
        for (i = 1; i < 16; i++) {
            if (MenuModelResourceState.requests[i].result != 0) {
                MenuModelResourceState.requests[0].result = MenuModelResourceState.requests[i].result;
                MenuModelResourceState.requests[0].loadParam = MenuModelResourceState.requests[i].loadParam;
                MenuModelResourceState.requests[0].resourceId = MenuModelResourceState.requests[i].resourceId;
                MenuModelResourceState.requests[i].loadParam = 0;
                MenuModelResourceState.requests[i].result = 0;
                MenuModelResourceState.requests[i].resourceId = 0;
                break;
            }
        }
    }
    if (MenuModelResourceState.requests[0].result == 0) {
        return;
    }
    activeState = &MenuModelResourceState;
    resourceId = MenuModelResourceState.requests[0].resourceId;
    if (resourceId & 0x8000) {
        activeState->resource = MenuModelMemoryGet(resourceId, 4);
    } else {
        resourceClass = resourceId >> 12;
        activeState->resource = MenuModelMemoryGet(resourceId, memoryTypes[resourceClass]);
    }
    if (MenuModelResourceState.resource == 0) {
        int failure = -1;
        unsigned short *requestId = &MenuModelResourceState.requests[0].resourceId;
        unsigned int *result = (void *)MenuModelResourceState.requests[0].result;
        unsigned char **resultSlot = &MenuModelResourceState.requests[0].result;
        *requestId = 0;
        __builtin_memcpy(result, &failure, sizeof(failure));
        *resultSlot = 0;
        MenuModelResourceState.requests[0].loadParam = 0;
        return;
    }
    if (MenuModelResourceState.resource->status == 1 &&
        MenuModelResourceState.resource->resourceId == resourceId) {
        return;
    }
    MenuModelResourceState.resource->status = 0;
    MenuModelResourceLoad(resourceId, MenuModelResourceState.resource,
                          memoryTypes[resourceClass], MenuModelResourceState.requests[0].loadParam);
}

void MenuModelResourceRequest(void *result, unsigned short resourceId, void *loadParam)
{
    int i;

    for (i = 1; i < 16; i++) {
        if (MenuModelResourceState.requests[i].result == 0) {
            MenuModelResourceState.requests[i].result = result;
            MenuModelResourceState.requests[i].resourceId = resourceId;
            MenuModelResourceState.requests[i].loadParam = loadParam;
            return;
        }
    }
}

void MenuModelMenuMotionLoad(void)
{
    int *motionHandle;
    unsigned short motionId;

    motionHandle = &MenuModelResourceState.motionHandle;
    motionId = 0;
    if (MenuModelFlag & 2) {
        return;
    }
    switch (MenuModelMemoryState.state) {
    case 10:
        motionId = 0x8001;
        break;
    case 11:
        motionId = 0xA100;
        break;
    case 12:
        motionId = 0x8001;
        break;
    case 20:
        motionId = 0xA100;
        break;
    }
    if (motionId == 0) {
        return;
    }
    MenuModelResourceRequest(motionHandle, motionId, 0);
}

static void MenuModelSubWindowMainSub(MenuModelUnit *unit)
{
    MenuModelSubWindowAccess *subWindow;
    MenuModelActorSubWindowAccess *actor;

    subWindow = (MenuModelSubWindowAccess *) unit->subWindow;
    actor = (MenuModelActorSubWindowAccess *) unit->actor;
    if (MenuModelFlag & 1) {
        subWindow->closeFlag = -1;
    }
    if ((unsigned char) subWindow->closeFlag != 0) {
        if ((unsigned char) subWindow->closeFlag == 0xFFU) {
            nmlModelUseSubWindow(subWindow->studioId, 0);
            unit->subWindow = 0;
            return;
        }
    }
    xglCameraSetWindow(subWindow->camera, subWindow->x, subWindow->y, subWindow->width,
                       subWindow->height);
    nmlModelUseSubWindow(subWindow->studioId, subWindow->mode);
    if (subWindow->mode == 1) {
        actor->subWindowId = subWindow->studioId;
    } else {
        actor->subWindowId = 0;
    }
}

void MenuModelSubWindowMain(MenuModelUnit *unit)
{
    if (unit->subWindow != 0) {
        MenuModelSubWindowMainSub(unit);
    }
}

void MenuModelSubWindowBreak(MenuModelUnit *unit)
{
    MenuModelSubWindowState *subWindow;

    subWindow = unit->subWindow;
    if (subWindow != 0) {
        subWindow->closeFlag = -1;
        MenuModelSubWindowMainSub(unit);
    }
}

void MenuModelSubWindowSet(MenuModelUnit *unit, int studioId, int mode)
{
    MenuModelSubWindowAccess *subWindow;
    StudioCamera *camera;
    MenuModelActor *actor;

    actor = unit->actor;
    if (studioId == 0) {
        return;
    }
    if (actor == 0) {
        return;
    }
    subWindow = &((MenuModelSubWindowAccess *) MenuModelSubWindow)[studioId - 1];
    unit->subWindow = (MenuModelSubWindowState *) subWindow;
    subWindow->closeFlag = 0;
    subWindow->studioId = studioId;
    subWindow->mode = mode;
    xglStudioChange(studioId);
    camera = xglStudioGetCamera2(0);
    subWindow->camera = camera;
    xglCameraInit(camera);
    xglCameraSetWindow(camera, 0, 0, 0, 0);
    camera->state = camera->active = 1;
    nmlModelUseSubWindow(studioId, mode);
    if (mode == 1) {
        nmlModelSetWindow(studioId);
    }
    xglStudioChange(0);
}

void MenuModelSubWindowinit(void) {
    memset(MenuModelSubWindow, 0, 0x30U);
}

static void ActorAndResourceDispose(MenuModelActor **actorSlot, void **resourceSlot, int mode)
{
    MenuModelActor *actor;
    MenuModelActor *disposedActor;
    MenuModelResource *resource;
    MenuModelResource *clearedResource;

    actor = *actorSlot;
    if (actor != 0) {
        if (actor->flags != 0) {
            ACT_dispose(actor);
            disposedActor = *actorSlot;
            *actorSlot = 0;
            disposedActor->flags = 0;
        } else {
            return;
        }
    }
    if (mode != 1) {
        resource = *resourceSlot;
        if ((resource != 0) && ((int) resource > 0)) {
            resource->status = 0;
            clearedResource = *resourceSlot;
            *resourceSlot = 0;
            clearedResource->handle = 0;
        }
    }
}

void MenuModelWeaponDispose(MenuModelWeapon *weapon)
{
    if (weapon != 0) {
        ActorAndResourceDispose(&weapon->actor, &weapon->resource, 2);
    }
}

void MenuModelUnitDispose(MenuModelUnit *unit)
{
    MenuModelUnit *self;
    MenuModelWeapon **weaponSlot;
    int weaponIndex;
    MenuModelActor **actorSlot;
    void **resourceSlot;

    self = unit;
    if (self != 0) {
        weaponSlot = &self->weapon[0];
        weaponIndex = 2;
        do {
            MenuModelWeaponDispose(*weaponSlot);
            weaponSlot++;
            weaponIndex--;
        } while (weaponIndex >= 0);
        actorSlot = &self->actor;
        resourceSlot = &self->resource;
        ActorAndResourceDispose(&self->actor2, &self->resource2, 1);
        ActorAndResourceDispose(actorSlot, resourceSlot, 0);
    }
}

void MenuModelAlphaDraw(MenuModelActor *actor)
{
    if (actor->menu.transparency < 1.0f) {
        nmlModelSetTransparency(actor->menu.transparency);
        nmlModelSetToumei(1);
        nmlModelSetZwrite(1);
    }
}

void MenuModelUnitActorSet(MenuModelUnit *unit)
{
    MenuModelActor *actor;
    MenuModelActor *face;
    MenuModelResourceAccess *resource;
    MenuModelResourceAccess *motionResource;
    int motion;
    int handMask;
    short actorKind;
    short motionActorKind;
    MenuModelTransformControl *control;

    unit->actor = (MenuModelActor *)ACT_create(-1, unit->resourceId);
    if (unit->actor == 0) return;
    actor = unit->actor;
    resource = (MenuModelResourceAccess *)unit->resource;
    control = &actor->menu;
    memset(control, 0, sizeof(*control));
    actor->drawEnvironmentFlags = 0;
    actorKind = actor->flags;
    actor->modelResource = resource->file[0];
    actor->pixelAlpha = 0x75;
    actor->matrixResource = resource->file[1];
    actor->update = ACT_updateDefault;
    actor->draw = MenuModelAlphaDraw;
    actor->move = resource->file[2];
    actor->renderLevel = 0;
    actor->partsPixelAlphaCount = 0;
    actor->distanceClipThreshold = 0.0f;
    actor->actorFlags = 0;
    switch ((actorKind >> 12) & 0xF) {
    case 0:
        motionResource = (MenuModelResourceAccess *)MenuModelMenuMotionGet();
        actor->animationPackages[0] = motionResource != 0 ?
            motionResource->file[3] : resource->file[3];
        break;
    case 2:
        if (actorKind == 0x2201) {
            actor->animationPackages[0] = resource->file[3];
        } else {
            motionResource = (MenuModelResourceAccess *)MenuModelMenuMotionGet();
            actor->animationPackages[0] = motionResource != 0 ?
                motionResource->file[3] : resource->file[3];
        }
        break;
    case 4:
        actor->animationPackages[0] = func_A1C910(resource->file[3]);
        break;
    }
    motion = 0;
    resource->status |= 2;
    ACT_allocMatrix((ActRecord *)actor, -1);
    ACT_setModelWrapper(actor, 0);
    ACT_initMotion(actor);
    motionActorKind = actor->flags;
    switch ((motionActorKind >> 12) & 0xF) {
    case 0:
        if (MenuModelMenuMotionGet() != 0 &&
            (unsigned short)(unit->modelId - 1) < 11) {
            signed char characterMotion[11] = {6, 2, 0, 0, 9, 7, 8, 0, 0, 10, 11};
            motion = characterMotion[unit->modelId - 1];
            if (unit->modelId == 2 && MenuModelMemoryState.state == 10) {
                if (unit->unusedByCreateInit1->weaponResource[1] != 0) {
                    motion = unit->unusedByCreateInit1->weaponResource[1] - 17;
                }
            }
        }
        break;
    case 2:
        if (motionActorKind != 0x2201) {
            signed char agwsMotion[12] = {0, 1, 2, 3, 0, 4, 0, 5, 0, 6, 7, 0};
            motion = agwsMotion[unit->modelId - 17];
        }
        break;
    case 4:
        break;
    }
    ACT_setMotion(actor, motion);
    control->initialMotionId = motion;
    if (actor->flags < 0x2000) {
        face = (MenuModelActor *)ACT_create(-2, actor->flags + 0x10000);
        unit->actor2 = face;
        if (face == 0) {
            MenuModelUnitDispose(unit);
            return;
        }
        face->pixelAlpha = 0x75;
        face->modelResource = resource->file[4];
        face->drawEnvironmentFlags = 0;
        face->renderLevel = 0;
        face->move = resource->file[5];
        face->partsPixelAlphaCount = 0;
        face->distanceClipThreshold = 0.0f;
        face->matrixResource = actor->matrixResource;
        face->animationPackages[0] = actor->animationPackages[0];
        ACT_allocMatrix((ActRecord *)face, -1);
        ACT_setModelWrapper(face, 0);
        ACT_initMotion(face);
        face->update = ACT_updateDefault;
        face->actorFlags &= ~8;
        ACT_setFace(face, actor, 0);
    }
    if ((actor->flags & 0xF000) == 0) {
        handMask = 0;
        switch (actor->flags) {
        case 1: handMask = 33; break;
        case 4: handMask = 1; break;
        }
        ACT_setHand(actor, handMask);
    }
    {
        XglLightVector lighting[7] = {
            { .value = {0.245f, 0.245f, 0.245f, 1.0f} },
            { .value = {0.8f, 0.74f, 0.65f, 1.0f} },
            { .value = {0.755f, 0.7f, 0.68f, 1.0f} },
            { .value = {0.6f, 0.58f, 0.72f, 1.0f} },
            { .value = {0.553f, 0.551f, -0.361f, 1.0f} },
            { .value = {-0.562f, 0.23f, -0.795f, 1.0f} },
            { .value = {0.725f, -0.001f, 0.688f, 1.0f} }
        };
        xglLightIntensityAmbient(&actor->light.source, &lighting[0].quadword);
        xglLightIntensityParallel(&actor->light.source, 0, &lighting[1].quadword);
        xglLightDirection((XglLightSet *)(void *)&actor->light.source, 0, &lighting[4].value);
        xglLightIntensityParallel(&actor->light.source, 1, &lighting[2].quadword);
        xglLightDirection((XglLightSet *)(void *)&actor->light.source, 1, &lighting[5].value);
        xglLightIntensityParallel(&actor->light.source, 2, &lighting[3].quadword);
        xglLightDirection((XglLightSet *)(void *)&actor->light.source, 2, &lighting[6].value);
    }
    {
        unsigned int actorFlags = actor->actorFlags;
        actor->motionFlags |= 0x100008;
        actor->actorFlags = actorFlags | 0x8000;
        actor->motionFlags |= 0x400000;
    }
}

void MenuModelWeaponActorSet(MenuModelUnit *unit, MenuModelWeapon *weapon)
{
    MenuModelActor *actor;
    MenuModelActor *parent;
    MenuModelResourceAccess *resource;

    if (unit == 0) return;
    parent = unit->actor;
    if (parent == 0) return;
    actor = (MenuModelActor *)ACT_create(-1, weapon->resourceId);
    weapon->actor = actor;
    if (unit->modelId == 7 && weapon->weaponIndex == 1) {
        resource = (MenuModelResourceAccess *)weapon->unit->weapon[0]->resource;
    } else {
        resource = (MenuModelResourceAccess *)weapon->resource;
    }
    memset(&actor->menu, 0, 0x70);
    actor->drawEnvironmentFlags = 0;
    actor->modelResource = resource->file[0];
    actor->matrixResource = resource->file[1];
    actor->pixelAlpha = 0x75;
    actor->actorFlags = 8;
    actor->update = ACT_updateDefault;
    actor->draw = MenuModelAlphaDraw;
    actor->move = resource->file[2];
    actor->renderLevel = 0;
    actor->partsPixelAlphaCount = 0;
    actor->distanceClipThreshold = 0.0f;
    if ((unsigned int)((unsigned short)actor->flags - 0x3007) < 4) {
        actor->animationPackages[0] = resource->file[3];
    } else if ((unsigned short)(unit->modelId - 17) < 12 &&
               (unsigned short)(weapon->weaponId - 70) < 45) {
        actor->animationPackages[0] = func_A1C910(resource->file[3]);
    } else {
        actor->animationPackages[0] = 0;
    }
    resource->status |= 2;
    ACT_allocMatrix((ActRecord *)actor, -1);
    ACT_setModelWrapper(actor, 0);
    ACT_initMotion(actor);
    if (unit->modelId == 2 && (unsigned short)(weapon->weaponId - 18) < 5) {
        signed char weaponMotion[5] = {0, 0, 1, 2, 3};
        unsigned short weaponId = weapon->weaponId;
        int parentMotion = weaponId - 17;
        ACT_setMotion(actor, weaponMotion[weaponId - 18]);
        ACT_setMotion(parent, parentMotion);
        parent->motionFlags |= 8;
    }
    if ((unsigned short)(unit->modelId - 17) < 12 &&
        (unsigned short)(weapon->weaponId - 70) < 45) {
        if (ACT_animGetData(actor, 4) != 0) {
            ACT_setMotion(actor, 4);
        }
    }
    if (unit->modelId == 7 && weapon->weaponIndex == 1) {
        ACT_setArms(actor, parent, 32, 0);
    } else {
        ACT_setArms(actor, parent, (unsigned short)weapon->characterId, 0);
    }
    actor->actorFlags &= ~8;
}

void MenuModelControl(MenuModelUnit *unit)
{
    MenuModelActor *actor = unit->actor;
    MenuModelTransformVector *rotation = &actor->menu.rotation;
    MenuModelTransformVector *scale = &actor->menu.scale;
    unsigned short buttons;

    if (actor->menu.enabled) {
        buttons = PadData[0].buttons;
        if (buttons & 0x1000)
            rotation->value.x -= 0.1f;
        if (buttons & 0x4000)
            rotation->value.x += 0.1f;
        if (buttons & 0x8000)
            rotation->value.y -= 0.1f;
        if (buttons & 0x2000)
            rotation->value.y += 0.1f;
        if (buttons & 1) {
            scale->value.x -= 0.02f;
            if (actor->scale.value.x <= 0.25f)
                scale->value.x = 0.25f;
        }
        if (PadData[0].buttons & 2) {
            scale->value.x += 0.02f;
            if (actor->scale.value.x >= 0.9799995f)
                scale->value.x = 0.9799995f;
        }
        scale->value.y = scale->value.z = scale->value.x;
    }
}

void MenuModelControl2(MenuModelUnit *unit, int preset)
{
    MenuModelActor *actor = unit->actor;
    MenuModelTransformVector *rotation = &actor->menu.rotation;
    MenuModelTransformVector *scale = &actor->menu.scale;
    MenuModelScaleLimits minimum;
    MenuModelScaleLimits maximum;

    if (actor->menu.enabled) {
        if ((PadData[0].buttons & 2) || (PadData[1].buttons & 2)) {
            if ((PadData[0].buttons & 0x1000) ||
                (PadData[1].buttons & 0x1000)) {
                if (rotation->value.x >= -0.499999f)
                    rotation->value.x -= 0.1f;
            }
            if ((PadData[0].buttons & 0x4000) ||
                (PadData[1].buttons & 0x4000)) {
                if (rotation->value.x <= 1.0f)
                    rotation->value.x += 0.1f;
            }
            if ((PadData[0].buttons & 0x8000) ||
                (PadData[1].buttons & 0x8000))
                rotation->value.y -= 0.1f;
            if ((PadData[0].buttons & 0x2000) ||
                (PadData[1].buttons & 0x2000))
                rotation->value.y += 0.1f;
            if (rotation->value.x > 3.1415927f)
                rotation->value.x = rotation->value.x - 3.1415927f + -3.1415927f;
            else if (rotation->value.x < -3.1415927f)
                rotation->value.x = rotation->value.x + 3.1415927f + 3.1415927f;
            if (rotation->value.y > 3.1415927f)
                rotation->value.y = rotation->value.y - 3.1415927f + -3.1415927f;
            else if (rotation->value.y < -3.1415927f)
                rotation->value.y = rotation->value.y + 3.1415927f + 3.1415927f;
        } else if ((PadData[0].buttons & 1) || (PadData[1].buttons & 1)) {
            if ((PadData[0].buttons & 0x1000) ||
                (PadData[1].buttons & 0x1000)) {
                minimum = minimumScaleTable[0];
                scale->value.x -= 0.02f;
                if (actor->scale.value.x <= minimum.value[preset])
                    scale->value.x = minimum.value[preset];
            }
            if ((PadData[0].buttons & 0x4000) ||
                (PadData[1].buttons & 0x4000)) {
                maximum = maximumScaleTable[0];
                scale->value.x += 0.02f;
                if (actor->scale.value.x >= maximum.value[preset])
                    scale->value.x = maximum.value[preset];
            }
            scale->value.y = scale->value.z = scale->value.x;
        }
    }
}

void MenuModelNavelMove(MenuModelUnit *unit)
{
    Matrix4 matrix;
    MenuModelActor *actor = unit->actor;
    MenuModelTransformControl *control = &actor->menu;

    actor->rotation = control->rotation;
    actor->scale = control->scale;
    xglMatrixStackUnit();
    xglMatrixStackRotX(actor->rotation.value.x);
    xglMatrixStackRotY(actor->rotation.value.y);
    xglMatrixStackScale(&actor->scale.value.x);
    xglMatrixStackTrans(&control->position.value.x);
    xglMatrixStackSave(matrix);
    actor->position.value.x = control->position.value.x - matrix[3][0];
    actor->position.value.y = control->position.value.y - matrix[3][1];
    actor->position.value.z = control->position.value.z - matrix[3][2];
    actor->position.value.w = 1.0f;
    /* VF2 retains position.w; VF2/VF3 and arithmetic flags are scratch here. */
    __asm__ __volatile__(
        "lqc2 vf3,0(%0)\n\t"
        "lqc2 vf2,0(%1)\n\t"
        "vadd.xyz vf2,vf2,vf3\n\t"
        "sqc2 vf2,0(%1)\n\t"
        : : "r"(&control->offset), "r"(&actor->position) : "memory");
    xglFontDebugPrintf(98, 32, navelRotationXFormat, (double)actor->rotation.value.x);
    xglFontDebugPrintf(98, 40, navelRotationYFormat, (double)actor->rotation.value.y);
    xglFontDebugPrintf(98, 48, navelRotationZFormat, (double)actor->rotation.value.z);
    xglFontDebugPrintf(98, 56, navelScaleXFormat, (double)actor->scale.value.x);
}

void MenuModelMotionSet(MenuModelMotionTarget *target, unsigned int dataId)
{
    MenuModelMotionActor *actor;

    actor = target->actor;
    ACT_setMotion(actor, dataId);
    actor->motionFlags |= 8;
}

void ModelDrawTypeOpen01(MenuModelActor *actor)
{
    unsigned char *state;
    float progress;

    state = MENU_MODEL_ACTOR_DRAW_STATE(actor);
    progress = *MENU_MODEL_DRAW_STATE_PROGRESS(state) + modelDrawOpen01Step;
    *MENU_MODEL_DRAW_STATE_PROGRESS(state) = progress;
    if (progress >= 1.0f) {
        *MENU_MODEL_DRAW_STATE_PROGRESS(state) = 1.0f;
        *MENU_MODEL_DRAW_STATE_FUNC(state) = 0;
    }
}

void ModelDrawTypeClose01(MenuModelActor *actor)
{
    unsigned char *state;
    float progress;

    state = MENU_MODEL_ACTOR_DRAW_STATE(actor);
    progress = *MENU_MODEL_DRAW_STATE_PROGRESS(state) - modelDrawClose01Step;
    *MENU_MODEL_DRAW_STATE_PROGRESS(state) = progress;
    if (progress <= 0.0f) {
        *MENU_MODEL_DRAW_STATE_PROGRESS(state) = 0.0f;
        *MENU_MODEL_DRAW_STATE_FUNC(state) = 0;
    }
}

void ModelDrawTypeClose02(MenuModelActor *actor)
{
    unsigned char *state;
    float minProgress;
    float progress;

    state = MENU_MODEL_ACTOR_DRAW_STATE(actor);
    minProgress = modelDrawClose02MinProgress;
    progress = *MENU_MODEL_DRAW_STATE_PROGRESS(state) - modelDrawClose02Step;
    *MENU_MODEL_DRAW_STATE_PROGRESS(state) = progress;
    if (progress <= minProgress) {
        *MENU_MODEL_DRAW_STATE_PROGRESS(state) = minProgress;
        *MENU_MODEL_DRAW_STATE_FUNC(state) = 0;
    }
}

static void MenuModelDrawTypeSet(MenuModelActor *actor, int drawType)
{
    MenuModelActorDrawTypeView *actorView;
    MenuModelDrawTypeState *state;

    if (actor == 0) {
        return;
    }
    actorView = (MenuModelActorDrawTypeView *) actor;
    state = &actorView->drawTypeState;
    switch (drawType) {
    case 0:
        state->progress = 1.0f;
        break;
    case 1:
        state->progress = 0.0f;
        break;
    case 2:
        state->drawTypeFunction = ModelDrawTypeOpen01;
        break;
    case 3:
        state->drawTypeFunction = ModelDrawTypeClose01;
        break;
    case 4:
        state->drawTypeFunction = ModelDrawTypeClose02;
        break;
    }
}

void MenuModelWeaponOpen(MenuModelUnit *unit, int drawType, int weaponIndex) {
    MenuModelActor *actor;
    MenuModelWeapon *weapon;

    if (unit != 0) {
        weapon = unit->weapon[weaponIndex];
        if (weapon != 0) {
            actor = weapon->actor;
            if (actor != 0) {
                MenuModelDrawTypeSet(actor, drawType);
            }
        }
    }
}

void MenuModelUnitOpen(MenuModelUnit *unit, int drawType)
{
    int weaponIndex;

    MenuModelDrawTypeSet(unit->actor, drawType);
    for (weaponIndex = 0; weaponIndex < 3; weaponIndex++) {
        MenuModelWeaponOpen(unit, drawType, weaponIndex);
    }
}

void MenuModelDrawTypeMain(MenuModelDrawContext *context)
{
    MenuModelDrawable *drawable;

    drawable = context->drawable;
    if (drawable != 0) {
        if (drawable->drawFunc != 0) {
            drawable->drawFunc(drawable);
        }
    }
}

static void tskMenuModelWeaponTaskMain(XglTaskPrefix *task)
{
    MenuModelWeapon *weapon;
    MenuModelActor *actor;
    MenuModelUnit *unit;
    void *resource;

    weapon = (MenuModelWeapon *)task;
    actor = weapon->actor;
    resource = weapon->resource;
    if (MenuModelFlag & 1) {
        weapon->taskPhase = -1;
    }
    unit = weapon->unit;
    if (unit == 0) {
        weapon->taskPhase = -1;
    } else if (unit->modelId == 7 && weapon->weaponIndex == 1) {
        if (unit->weapon[0] == 0) {
            weapon->taskPhase = -1;
        } else {
            resource = unit->weapon[0]->resource;
        }
    }
    switch ((unsigned char)weapon->taskPhase) {
    case 0:
        weapon->extFuncFlag = 0;
        weapon->taskPhase = 1;
        /* fall through */
    case 1:
        if (resource != 0) {
            if ((int)resource < 0) {
                weapon->taskPhase = -1;
            } else if (unit->actor != 0 && unit->actor->menu.transparency == 1.0f) {
                weapon->taskPhase = 2;
            }
        }
        break;
    case 2:
        MenuModelWeaponActorSet(unit, weapon);
        MenuModelWeaponOpen(weapon->unit, 2, weapon->weaponIndex);
        weapon->taskPhase = 10;
        weapon->active = 1;
        break;
    case 10:
        break;
    case 255:
        MenuModelResourceCancel3((MenuModelUnit *)weapon);
        MenuModelWeaponDispose(weapon);
        if (weapon->result != 0) {
            *weapon->result = 0;
        }
        xglTaskWaitRemove(task);
        CreateInit((MenuModelUnit *)weapon);
        return;
    }
    if (weapon->extFunc != 0) {
        weapon->extFunc(weapon, weapon->extContext);
    }
    if (actor != 0) {
        MenuModelDrawTypeMain((MenuModelDrawContext *)weapon);
        if (weapon->resourceLoaded != 0) {
            ACT_updateMotion(actor);
        }
    }
}

static void tskMenuModelTaskMain(XglTaskPrefix *task)
{
    MenuModelUnit *unit;
    MenuModelActor *actor;
    MenuModelTransformControl *control;
    MenuModelWeapon **weaponSlot;
    MenuModelTransformVector zero;
    int remainingWeapons;
    int terminatingPhase;
    void *resource;

    unit = (MenuModelUnit *)task;
    actor = unit->actor;
    resource = unit->resource;
    if (MenuModelFlag & 1) {
        unit->taskPhase = -1;
    }
    switch ((unsigned char)unit->taskPhase) {
    case 0:
        unit->extFuncFlag = 0;
        unit->taskPhase = 1;
        /* fall through */
    case 1:
        if (resource != 0) {
            if ((int)resource < 0) {
                unit->taskPhase = -1;
            } else {
                unit->taskPhase = 2;
            }
        }
        break;
    case 2:
        MenuModelUnitActorSet(unit);
        unit->taskPhase = 10;
        break;
    case 10:
        break;
    case 255:
        MenuModelSubWindowBreak(unit);
        MenuModelResourceCancel3(unit);
        MenuModelUnitDispose(unit);
        if (unit->unusedByCreateInit4 != 0) {
            *unit->unusedByCreateInit4 = 0;
        }
        terminatingPhase = -1;
        weaponSlot = unit->weapon;
        for (remainingWeapons = 2; remainingWeapons >= 0; remainingWeapons--) {
            if (*weaponSlot != 0) {
                (*weaponSlot)->taskPhase = terminatingPhase;
                tskMenuModelWeaponTaskMain((XglTaskPrefix *)*weaponSlot);
            }
            weaponSlot++;
        }
        xglTaskWaitRemove(task);
        CreateInit(unit);
        return;
    }
    if (unit->extFunc1 != 0) {
        ((void (*)(MenuModelUnit *, void *))unit->extFunc1)(unit, unit->extFunc2);
    }
    if (actor != 0) {
        control = &actor->menu;
        xglStudioChange(0);
        xglStudioGetCamera2(0)->active = 1;
        if (control->enabled == 0) {
            memset(&zero, 0, sizeof(zero));
            zero.value.w = 1.0f;
            {
                MenuModelTransformVector one = {{1.0f, 1.0f, 1.0f, 1.0f}};
                actor->position = zero;
                actor->rotation = zero;
                actor->scale = one;
                control->transparency = 0.0f;
                control->offset = zero;
                control->rotation = zero;
                control->scale = one;
            }
        }
        if (control->enabled != 0) {
            MenuModelSubWindowMain(unit);
            MenuModelNavelMove(unit);
            MenuModelDrawTypeMain((MenuModelDrawContext *)unit);
        }
        ACT_updateMotion(actor);
        ACT_modelDraw((Actor *)actor);
        if (control->enabled == 0) {
            control->enabled = 1;
            control->position = actor->navel;
            unit->unusedByCreateInit0 = 1;
        }
    }
}

void MenuModelUnitBreak(MenuModelUnit *unit) {
    if (unit != 0) {
        MenuModelResourceCancel2(unit);
        unit->taskPhase = -1;
    }
}

static void CreateInit(MenuModelUnit *unit)
{
    int weaponIndex;

    unit->taskPhase = 0;
    unit->unusedByCreateInit0 = 0;
    unit->extFunc1 = 0;
    unit->extFunc2 = 0;
    unit->unusedByCreateInit1 = 0;
    unit->subWindow = 0;
    unit->actor = 0;
    unit->resource = 0;
    unit->actor2 = 0;
    unit->resource2 = 0;
    unit->skipWeaponRegionCleanup = 0;
    unit->unusedByCreateInit2[0] = 0;
    unit->unusedByCreateInit2[1] = 0;
    unit->unusedByCreateInit4 = 0;
    for (weaponIndex = 2; weaponIndex >= 0; weaponIndex--) {
        unit->weapon[weaponIndex] = 0;
    }
}

XglTaskPrefix *MenuModelWeaponCreate(MenuModelTaskWork *unit,
                                     XglTaskPrefix **result,
                                     int weaponIndex)
{
    MenuModelTaskWork *weapon;

    if (unit == 0) {
        return 0;
    }
    if (unit->unitData == 0) {
        return 0;
    }
    if ((unsigned int)weaponIndex >= 3) {
        return 0;
    }
    if (unit->weapon[weaponIndex] != 0) {
        MenuModelTaskWork *oldWeapon = unit->weapon[weaponIndex];

        if (oldWeapon->objectId ==
            unit->unitData->weaponResource[weaponIndex]) {
            return 0;
        }
        MenuModelResourceCancel2((MenuModelUnit *)(void *)oldWeapon);
        MenuModelWeaponDispose((MenuModelWeapon *)unit->weapon[weaponIndex]);
        unit->weapon[weaponIndex]->taskPhase = -1;
        unit->weapon[weaponIndex] = 0;
        if (unit->unitData->weaponResource[weaponIndex] == 0) {
            return 0;
        }
    } else {
        if (unit->unitData->weaponResource[weaponIndex] == 0) {
            return 0;
        }
    }

    if (unit->unitData->weaponResource[weaponIndex] == 0x13) {
        MenuModelActor *actor = unit->actor;

        ACT_setMotion(actor, 2);
        actor->motionFlags |= 8;
        return 0;
    }

    weapon = (MenuModelTaskWork *)xglTaskEntryNext(
        MenuModelTask, (int (*)(XglTaskPrefix *))tskMenuModelWeaponTaskMain,
        MenuModelTask != 0 ? MenuModelTask->active_tail : 0);
    if (weapon == 0) {
        return 0;
    }

    CreateInit((MenuModelUnit *)weapon);
    weapon->weaponIndex = weaponIndex;
    weapon->characterId = unit->unitData->weaponCharacter[weaponIndex];
    weapon->objectId = unit->unitData->weaponResource[weaponIndex];
    weapon->objectId =
        MenuRWeaponCheck(weapon->objectId,
                         (unsigned short)weapon->characterId);
    weapon->resourceId = func_A1A330(weapon->objectId);
    if ((weapon->resourceId & 0xFF) == 0) {
        weapon->taskPhase = -1;
        return 0;
    }

    unit->weapon[weaponIndex] = weapon;
    weapon->owner = unit;
    if (result != 0) {
        weapon->result = (int *)result;
        *result = (XglTaskPrefix *)weapon;
    }

    if (unit->objectId == 7 && weapon->weaponIndex == 1) {
        return (XglTaskPrefix *)weapon;
    }
    MenuModelResourceRequest(&weapon->resource,
                             weapon->resourceId, weapon);
    if (unit->objectId == 7 && weapon->weaponIndex == 0) {
        short savedWeaponResource;
        savedWeaponResource = unit->unitData->weaponResource[1];
        unit->unitData->weaponResource[1] =
            unit->unitData->weaponResource[0];
        MenuModelWeaponCreate(unit, result, 1);
        unit->unitData->weaponResource[1] = savedWeaponResource;
    }
    return (XglTaskPrefix *)weapon;
}

void MenuModelCreate(int *result, int modelId)
{
    MenuModelUnit *unit;

    if (modelId == 0)
        return;
    unit = (MenuModelUnit *)xglTaskEntryNext(MenuModelTask, (int (*)(XglTaskPrefix *))tskMenuModelTaskMain,
                                         MenuModelTask ? MenuModelTask->active_tail : 0);
    if (unit == 0)
        return;
    CreateInit(unit);
    unit->modelId = modelId;
    switch (modelId) {
    case 11:
        unit->resourceId = 0x120;
        break;
    case 2:
        if (MenuScenarioNoGet() < 10)
            unit->resourceId = 14;
        else
            unit->resourceId = modelId;
        break;
    default:
        if (MenuModelMemoryState.state >= 10) {
            unit->resourceId = func_A1A2E8(modelId);
        } else if ((unsigned int)(modelId - 34) < 29) {
            unit->resourceId = guno_eid_tbl[modelId - 34];
        } else {
            switch (modelId) {
            case 3:
                unit->resourceId = 1;
                break;
            case 29:
                unit->resourceId = 0x2201;
                break;
            }
        }
        break;
    }
    if (MenuModelMemoryState.state >= 10 && modelId < 33)
        unit->unusedByCreateInit1 = func_A191C0(modelId);
    if (result != 0) {
        unit->unusedByCreateInit4 = result;
        *result = (int)unit;
    }
    MenuModelResourceRequest(&unit->resource, unit->resourceId, 0);
}

void MenuModelExtFuncSet(MenuModelUnit *unit, void *extFunc1, void *extFunc2)
{
    if (unit != 0) {
        unit->extFunc2 = extFunc2;
        unit->extFunc1 = extFunc1;
        unit->extFuncFlag = 0;
    }
}

MenuModelActor *MenuModelWeaponActorGet(MenuModelUnit *unit, int weaponIndex)
{
    MenuModelActor *actor;
    MenuModelWeapon *weapon;

    actor = 0;
    if (unit != 0) {
        weapon = unit->weapon[weaponIndex];
        if (weapon != 0) {
            actor = weapon->actor;
        }
    }
    return actor;
}

void MenuModelMain(void)
{
    MenuModelResourceMain();
    xglTaskExecute(MenuModelTask);
}

void MenuModelAllBreak(void) {
    MenuModelFlag |= 1;
    MenuModelMain();
    MenuModelFlag &= ~1;
    MenuModelResourceInit();
    MenuModelSubWindowinit();
}

void *MenuModelInit(void *pool)
{
    pool = (void *) (((int) pool + 0xF) & ~0xF);
    MenuModelTask = pool;
    MenuModelFlag = 0;
    pool = xglTaskInitial(pool, 32, 0);
    MenuModelResourceInit();
    MenuModelSubWindowinit();
    return pool;
}

/* VIF DIRECT header followed by a packed GIF TEST_2 and two XYZF vertices. */
void MenuModelDirectSendZClear(void)
{
    typedef union MenuModelZClearRecord {
        struct {
            unsigned long long dmaTag;
            unsigned long long vifCommands;
        } dmaVif;
        struct {
            unsigned long long tag;
            unsigned long long registers;
        } gif;
        struct {
            unsigned long long value;
            unsigned long long registerAddress;
        } registerWrite;
        MenuModelZClearVertex vertex;
    } MenuModelZClearRecord;
    volatile MenuModelZClearRecord *record = (volatile MenuModelZClearRecord *)0x70000000;
    unsigned long long vifCommands = 0x5100000411000000ULL;
    unsigned long long gifTag = 0x3023400000008001ULL;
    unsigned long long gifRegisters = 0x55E;
    unsigned long long testValue = 0x32001;
    unsigned long long *dmaTag;
    volatile MenuModelZClearVertex *vertices;

    record[2].registerWrite.registerAddress = 0x47;
    record[0].dmaVif.vifCommands = vifCommands;
    record[1].gif.tag = gifTag;
    record[1].gif.registers = gifRegisters;
    record[2].registerWrite.value = testValue;
    /* The tag is ordinary staging memory until the direct-send call. */
    dmaTag = (unsigned long long *)&record[0].dmaVif.dmaTag;
    *dmaTag = 0;
    record += 3;
    vertices = &record[0].vertex;
    vertices[0].x = 0x6FF8;
    vertices[0].y = 0x71F8;
    vertices[1].x = 0x8FF8;
    vertices[1].y = 0x8DF8;
    vertices[1].fog = 0;
    vertices[0].z = 0;
    vertices[0].fog = 0;
    vertices[1].z = 0;
    nmlModelDirectSend(1, (unsigned char *)0x70000000, 5);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char navelRotationXFormat[16] = "rot : x=%f";

const char navelRotationYFormat[16] = "    : y=%f";

const char navelRotationZFormat[16] = "    : z=%f";

const char navelScaleXFormat[16] = "scl :   %f";

const MenuModelScaleLimits minimumScaleTable[1] = { { .value = { 0.80000001192092896f, 0.72500002384185791f } } };

const MenuModelScaleLimits maximumScaleTable[1] = { { .value = { 2.0f, 1.0f } } };
