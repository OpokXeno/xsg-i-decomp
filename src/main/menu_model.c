#include "common.h"
#include "main/menu_model.h"
#include "main/xgl_light.h"
#include "main/xgl_studio.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "main/act_1.h"
#include "main/xgl_2.h"
#include "main/xgl_pad.h"
#include "main/act_2.h"
#include "main/menu_1.h"
#include "main/nml_model_set.h"
#include "main/window_tex_load.h"
#include "main/xgl_camera.h"
#include "main/xgl_font.h"
#include "vu0_macro.h"

/* Resource regions: the memory pool is cut into MENU_MODEL_REGION_COUNT
 * regions and resource requests queue in MENU_MODEL_REGION_COUNT records. */
#define MENU_MODEL_REGION_COUNT 16
#define MENU_MODEL_SUBWINDOW_COUNT 3

/* MenuModelFlag */
#define MENU_MODEL_FLAG_BREAK 1        /* cancel everything (MenuModelAllBreak) */
#define MENU_MODEL_FLAG_MOTION_LOADED 2 /* menu motion pack is resident */

/* Resource ids with this bit select the shared menu motion pack. */
#define MENU_MODEL_MOTION_ID_FLAG 0x8000
#define MENU_MODEL_MEMORY_TYPE_MOTION 4

/* MenuModelMemorySet layouts, named for the screen that selects them. */
enum {
    MENU_MODEL_MEMORY_NONE = -1, /* set by MenuModelMemoryInit */
    MENU_MODEL_MEMORY_MAIN_MENU = 10,
    MENU_MODEL_MEMORY_AGWS = 11,
    MENU_MODEL_MEMORY_TRIPLE_UNIT = 12, /* three full unit regions */
    MENU_MODEL_MEMORY_SHOP = 20
};

/* Unit and weapon task phases (taskPhase). */
enum {
    MENU_MODEL_PHASE_INIT = 0,
    MENU_MODEL_PHASE_WAIT_RESOURCE = 1,
    MENU_MODEL_PHASE_SPAWN = 2,
    MENU_MODEL_PHASE_RUN = 10,
    MENU_MODEL_PHASE_END = 255
};

/* Draw types MenuModelUnitOpen / MenuModelWeaponOpen apply to an actor. */
enum {
    MENU_MODEL_DRAW_SHOW = 0,    /* fully opaque */
    MENU_MODEL_DRAW_HIDE = 1,    /* fully transparent */
    MENU_MODEL_DRAW_FADE_IN = 2,
    MENU_MODEL_DRAW_FADE_OUT = 3,
    MENU_MODEL_DRAW_FADE_OUT_PARTIAL = 4 /* stops at a minimum opacity */
};

/* Unit model ids: characters 1..11, AGWS units 17..28. */
#define MENU_MODEL_CHARACTER_COUNT 11
#define MENU_MODEL_AGWS_ID_BASE 17
#define MENU_MODEL_AGWS_COUNT 12

#define MENU_MODEL_PIXEL_ALPHA 0x75

typedef struct MenuModelScaleLimits {
    float value[2];
} MenuModelScaleLimits;

/* The two controller records are 0x68 bytes; the menu controls read +0x30. */
typedef struct MenuModelPadRecord {
    unsigned char unmodeled_00[0x30];
    unsigned short buttons;
    unsigned char unmodeled_32[0x36];
} MenuModelPadRecord;

/* The external unit-resource record fields consulted while naming a weapon motion. */
typedef struct MenuModelUnitResourceData {
    unsigned char unmodeled_00[0x5A];
    signed char weaponCharacter[3];       /* +0x5A */
    unsigned char unmodeled_5D[1];
    short weaponResource[3];             /* +0x5E */
} MenuModelUnitResourceData;

typedef struct MenuModelWeapon {
    unsigned char unmodeled_00[0x10];
    unsigned char taskPhase;              /* +0x10: cancellation and failed creation */
    unsigned char active;                 /* +0x11: task has opened its actor */
    unsigned char unmodeled_12;
    unsigned char extFuncFlag;            /* +0x13: cleared on task startup */
    unsigned char unmodeled_14[4];
    unsigned short weaponId;             /* +0x18: MenuRWeaponCheck result */
    unsigned short resourceId;            /* +0x1A: menu resource selected for this weapon */
    unsigned char weaponIndex;            /* +0x1C */
    unsigned char resourceLoaded;         /* +0x1D */
    short characterId;                    /* +0x1E: sign-extended resource byte */
    Actor *actor; /* +0x20: returned by MenuModelWeaponActorGet; MenuModelWeaponOpen hands it to MenuModelDrawTypeSet */
    unsigned char unmodeled_24[4];
    /* +0x28: the weapon's menu resource; MenuModelWeaponDispose passes its address to ActorAndResourceDispose as the resource slot. */
    struct MenuModelResourceAccess *resource;
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
    signed char characterId;              /* +0x1E: copied into the signed UnitResourceData.weaponCharacter table */
    unsigned char unmodeled_1F[0x11];
    struct MenuModelUnit *unit;                  /* +0x30 */
} MenuModelWeaponLoadTask;

/*
 * The object bound to a menu model draw task. Only the actor at +0x20 is known.
 */
typedef struct MenuModelDrawContext {
    unsigned char unmodeled_00[0x20];
    Actor *actor; /* +0x20 */
} MenuModelDrawContext;

/*
 * +0x1: MenuModelSubWindowBreak sets this to -1 (0xFF) before calling
 * MenuModelSubWindowMainSub, which compares the byte as unsigned.
 */
typedef struct MenuModelSubWindowAccess {
    unsigned char unmodeled_00[1];
    unsigned char closeFlag;   /* +0x1 */
    unsigned char studioId;    /* +0x2 */
    unsigned char mode;        /* +0x3 */
    short x;                   /* +0x4 */
    short y;                   /* +0x6 */
    short width;               /* +0x8 */
    short height;              /* +0xA */
    StudioCamera *camera;      /* +0xC */
} MenuModelSubWindowAccess;


typedef struct MenuModelMotionTarget {
    unsigned char unmodeled_00[0x20];
    Actor *actor;
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
 * size; MenuModelMemorySet lays out the sixteen 0x1C-byte resource regions
 * at +0xC; MenuModelMemoryGet and MenuModelResourceCancel3 use them.
 */
typedef struct MenuModelMemoryStateBuffer {
    int state; /* +0x0: memory-state code; 10-12 or 20 support menu motion */
    void *base; /* +0x4 */
    int size; /* +0x8 */
    MenuModelResourceAccess regions[MENU_MODEL_REGION_COUNT]; /* +0xC */
} MenuModelMemoryStateBuffer;

/* Record zero is the active request; the remaining fifteen records are queued.
 * MenuModelResourceMain moves a queued record into zero and forwards loadParam.
 */
typedef struct MenuModelResourceRequestRecord {
    unsigned short resourceId;
    unsigned int *result; /* Caller output word: receives an int failure code or a resource pointer by memcpy. */
    void *loadParam;
} MenuModelResourceRequestRecord;

typedef struct MenuModelResourceStateBuffer {
    unsigned int resetWord;
    struct MenuModelResourceAccess *resource;
    MenuModelResourceRequestRecord requests[MENU_MODEL_REGION_COUNT];
    MenuModelResourceAccess *motionHandle;
} MenuModelResourceStateBuffer;

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
    Actor *actor;
    Actor *actor2;
    void *resource;
    void *resource2;
    struct MenuModelTaskWork *owner;
    struct MenuModelTaskWork *weapon[MENU_MODEL_WEAPON_COUNT];
    void *extFunc;
    void *extContext;
    int *result;
    struct MenuModelSubWindowAccess *subWindow;
} MenuModelTaskWork;

typedef struct MenuModelZClearVertex {
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int fog;
} MenuModelZClearVertex;

extern MenuModelPadRecord PadData[2];
extern const MenuModelScaleLimits minimumScaleTable[];
extern const MenuModelScaleLimits maximumScaleTable[];
extern const char navelRotationXFormat[16];
extern const char navelRotationYFormat[16];
extern const char navelRotationZFormat[16];
extern const char navelScaleXFormat[16];

void *func_A1C910(void *packed_file);
void ACT_updateDefault(Actor *actor);
void MenuModelResourceRequest(void *result, unsigned short resourceId, void *loadParam);
void MenuModelUnitActorSet(MenuModelUnit *unit);
void nmlModelUseSubWindow(unsigned int studioId, int enabled);
void xglCameraSetWindow(StudioCamera *camera, int x, int y, int width, int height);
extern char *MenuFileNameGet(short resourceId, int fileType);
extern void MenuLoadFile(const char *name, void *buffer);

/* With OV01 loaded, this import resolves to dataMotNameGetMenu. */
extern int func_A1ADC8(char *name, int modelId, int weaponIndex);

static void MenuModelDrawTypeSet(Actor *actor, int drawType);
void MenuModelWeaponOpen(MenuModelUnit *unit, int drawType, int weaponIndex);
void MenuModelResourceMain(void);
void MenuModelResourceInit(void);
void MenuModelSubWindowinit(void);
void MenuModelResourceCancel(void);
void MenuModelResourceCancel3(MenuModelUnit *unit);
static void MenuModelSubWindowMainSub(MenuModelUnit *unit);
static void ActorAndResourceDispose(Actor **actorSlot, MenuModelResourceAccess **resourceSlot, int mode);

/* Builds a task list of `capacity` 0x80-byte tasks at `pool` and returns the
 * first byte after them (0 when capacity is 0). */
void *xglTaskInitial(void *pool, int capacity, int flags);

void ACT_dispose(Actor *actor);
extern int MenuRWeaponCheck(int weaponId, long equipType);
extern int func_A1A330(int weaponId);
void ACT_setHand(Actor *actor, int mask);
void xglLightIntensityAmbient(StudioLight *lightSet, const XglLightQuadword *intensity);
void xglLightIntensityParallel(StudioLight *lightSet, unsigned int index, const XglLightQuadword *intensity);
void xglLightDirection(StudioLight *lightSet, unsigned int index, const Vector4 *source);
static void CreateInit(MenuModelUnit *unit);
void MenuModelWeaponActorSet(MenuModelUnit *unit, MenuModelWeapon *weapon);
int func_A1A2E8(int modelId);
MenuModelUnitResourceData *func_A191C0(int modelId);

static unsigned short guno_eid_tbl[29];

XglTaskScheduler *MenuModelTask = 0;

int MenuModelFlag = 0;

/* Resource ids for the unit model ids from 34 on: MenuModelCreate indexes
   this table with modelId - 34. */
static unsigned short guno_eid_tbl[29] = {
    0x4001, 0x4002, 0x4003, 0x4004, 0x4005, 0x4006, 0x4007, 0x4008,
    0x4009, 0x400A, 0x400B, 0x400C, 0x400D, 0x400E, 0x400F, 0x4010,
    0x4011, 0x4012, 0x4013, 0x4014, 0x4015, 0x4016, 0x4017, 0x4018,
    0x4019, 0x4001, 0x4002, 0x4003, 0x4037
};

MenuModelMemoryStateBuffer MenuModelMemoryState = {0};

MenuModelResourceStateBuffer MenuModelResourceState = {0};

MenuModelSubWindowAccess MenuModelSubWindow[MENU_MODEL_SUBWINDOW_COUNT] = {{0}};

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

/* Clears the region table and lays out the resource regions for a memory state. */
void MenuModelMemorySet(int state)
{
    MenuModelMemoryStateBuffer *memoryState;
    MenuModelResourceAccess *region;
    unsigned char *memory;
    int i;

    MenuModelFlag = 0;
    MenuModelMemoryState.state = state;
    for (i = 0; i < MENU_MODEL_REGION_COUNT; i++) {
        MenuModelMemoryState.regions[i].status = 0;
        MenuModelMemoryState.regions[i].memoryType = 0;
        MenuModelMemoryState.regions[i].resourceId = 0;
    }
    memoryState = &MenuModelMemoryState;
    memset(memoryState->base, 0, memoryState->size);
    memory = memoryState->base;
    region = memoryState->regions;
    /* Each region takes consecutive slices of the buffer for its file[] slots. */
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
    case MENU_MODEL_MEMORY_MAIN_MENU: {
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
    case MENU_MODEL_MEMORY_AGWS:
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
    case MENU_MODEL_MEMORY_TRIPLE_UNIT:
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
    case MENU_MODEL_MEMORY_SHOP:
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

/* Returns the first free region of the given memory type, or 0. resourceId is unused. */
MenuModelResourceAccess *MenuModelMemoryGet(unsigned short resourceId, int memoryType)
{
    int i;

    (void)resourceId;
    for (i = 0; i < MENU_MODEL_REGION_COUNT; i++) {
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
    MenuModelMemoryState.state = MENU_MODEL_MEMORY_NONE;
}

/* Clears the request queue and the active resource handles. */
void MenuModelResourceInit(void)
{
    int i;
    MenuModelResourceStateBuffer *state;

    state = &MenuModelResourceState;
    for (i = 0; i < MENU_MODEL_REGION_COUNT; i++) {
        state->requests[i].result = 0;
        state->requests[i].resourceId = 0;
        state->requests[i].loadParam = 0;
    }
    state->resource = 0;
    state->motionHandle = 0;
    state->resetWord = 0;
}

/* Returns 1 when the memory state supports the menu motion pack, otherwise 0. */
int MenuModelMenuMotionCheck(void) {
    if ((MenuModelMemoryState.state >= MENU_MODEL_MEMORY_MAIN_MENU) &&
        ((MenuModelMemoryState.state < MENU_MODEL_MEMORY_TRIPLE_UNIT + 1) ||
         (MenuModelMemoryState.state == MENU_MODEL_MEMORY_SHOP))) {
        return 1;
    }
    return 0;
}

/* Returns the menu motion handle when the pack is loaded in a supporting state, else 0. */
MenuModelResourceAccess *MenuModelMenuMotionGet(void) {
    MenuModelResourceAccess *handle;

    handle = 0;
    if ((MenuModelMenuMotionCheck() == 1) && (MenuModelFlag & MENU_MODEL_FLAG_MOTION_LOADED)) {
        handle = MenuModelResourceState.motionHandle;
    }
    return handle;
}

/*
 * Cancels the load in progress, frees its region and reports failure (-1)
 * to the first queued request's result word.
 */
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

/*
 * Cancels the queued requests and weapon regions of a unit's resources.
 * The weapon region cleanup is skipped when skipWeaponRegionCleanup is set.
 */
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

/*
 * Loads the files of one resource into its region, chosen by resourceMode.
 * For a weapon task, the weapon motion is loaded when its name is found.
 */
void MenuModelResourceLoad(int resourceId, MenuModelResourceAccess *buffers,
                           int resourceMode, MenuModelWeaponLoadTask *weaponTask)
{
    char fileName[0x80];
    unsigned int motionPackageOffset;

    if ((resourceId & MENU_MODEL_MOTION_ID_FLAG) != 0) {
        MenuLoadFile(MenuFileNameGet(resourceId & 0x7FFFU, 3), buffers->file[3]);
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
        unitData->weaponCharacter[weaponIndex] = weaponTask->characterId;
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

/* Per-frame step of the resource queue: completes the active load, then starts the next one. */
void MenuModelResourceMain(void)
{
    int memoryTypes[5] = {1, 1, 2, 3, 2};
    MenuModelResourceStateBuffer *activeState;
    unsigned int resourceClass = 0;
    unsigned short resourceId;
    int i;

    if (MenuModelFlag & MENU_MODEL_FLAG_BREAK) {
        MenuModelResourceCancel();
        MenuModelResourceInit();
        return;
    }
    if (MenuLoadSync() != 0) {
        return;
    }
    /* Completes the load in progress and hands its handle to the requester. */
    if (MenuModelResourceState.resource != 0) {
        MenuModelResourceAccess *resource;
        unsigned int *result;
        unsigned short completedResourceId;
        MenuModelResourceState.resource->status = 1;
        resource = MenuModelResourceState.resource;
        completedResourceId = MenuModelResourceState.requests[0].resourceId;
        resource->resourceId = completedResourceId;
        if (completedResourceId & MENU_MODEL_MOTION_ID_FLAG) {
            MenuModelFlag |= MENU_MODEL_FLAG_MOTION_LOADED;
        }
        result = MenuModelResourceState.requests[0].result;
        MenuModelResourceState.requests[0].result = 0;
        __builtin_memcpy(result, &resource, sizeof(resource));
        MenuModelResourceState.resource = 0;
    }
    /* Moves the first queued request into slot 0 when the slot is empty. */
    if (MenuModelResourceState.requests[0].result == 0) {
        for (i = 1; i < MENU_MODEL_REGION_COUNT; i++) {
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
    if (resourceId & MENU_MODEL_MOTION_ID_FLAG) {
        activeState->resource = MenuModelMemoryGet(resourceId, MENU_MODEL_MEMORY_TYPE_MOTION);
    } else {
        resourceClass = resourceId >> 12;
        activeState->resource = MenuModelMemoryGet(resourceId, memoryTypes[resourceClass]);
    }
    if (MenuModelResourceState.resource == 0) {
        int failure = -1;
        unsigned short *requestId = &MenuModelResourceState.requests[0].resourceId;
        unsigned int *result = MenuModelResourceState.requests[0].result;
        unsigned int **resultSlot = &MenuModelResourceState.requests[0].result;
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

/* Queues a request in the first free slot after the active one; drops it when none is free. */
void MenuModelResourceRequest(void *result, unsigned short resourceId, void *loadParam)
{
    int i;

    for (i = 1; i < MENU_MODEL_REGION_COUNT; i++) {
        if (MenuModelResourceState.requests[i].result == 0) {
            MenuModelResourceState.requests[i].result = result;
            MenuModelResourceState.requests[i].resourceId = resourceId;
            MenuModelResourceState.requests[i].loadParam = loadParam;
            return;
        }
    }
}

/* Requests the menu motion pack for the current memory state, unless it is already loaded. */
void MenuModelMenuMotionLoad(void)
{
    MenuModelResourceAccess **motionHandle;
    unsigned short motionId;

    motionHandle = &MenuModelResourceState.motionHandle;
    motionId = 0;
    if (MenuModelFlag & MENU_MODEL_FLAG_MOTION_LOADED) {
        return;
    }
    switch (MenuModelMemoryState.state) {
    case MENU_MODEL_MEMORY_MAIN_MENU:
        motionId = 0x8001;
        break;
    case MENU_MODEL_MEMORY_AGWS:
        motionId = 0xA100;
        break;
    case MENU_MODEL_MEMORY_TRIPLE_UNIT:
        motionId = 0x8001;
        break;
    case MENU_MODEL_MEMORY_SHOP:
        motionId = 0xA100;
        break;
    }
    if (motionId == 0) {
        return;
    }
    MenuModelResourceRequest(motionHandle, motionId, 0);
}

/* Applies the unit's sub window: closes it when flagged, otherwise sets its camera window and mode. */
static void MenuModelSubWindowMainSub(MenuModelUnit *unit)
{
    MenuModelSubWindowAccess *subWindow;
    Actor *actor;

    subWindow = unit->subWindow;
    actor = unit->actor;
    if (MenuModelFlag & MENU_MODEL_FLAG_BREAK) {
        subWindow->closeFlag = -1;
    }
    if (subWindow->closeFlag != 0) {
        if (subWindow->closeFlag == 0xFFU) {
            nmlModelUseSubWindow(subWindow->studioId, 0);
            unit->subWindow = 0;
            return;
        }
    }
    xglCameraSetWindow(subWindow->camera, subWindow->x, subWindow->y, subWindow->width,
                       subWindow->height);
    nmlModelUseSubWindow(subWindow->studioId, subWindow->mode);
    if (subWindow->mode == 1) {
        actor->windowId = subWindow->studioId;
    } else {
        actor->windowId = 0;
    }
}

void MenuModelSubWindowMain(MenuModelUnit *unit)
{
    if (unit->subWindow != 0) {
        MenuModelSubWindowMainSub(unit);
    }
}

/* Marks the unit's sub window for closing and applies it at once. */
void MenuModelSubWindowBreak(MenuModelUnit *unit)
{
    MenuModelSubWindowAccess *subWindow;

    subWindow = unit->subWindow;
    if (subWindow != 0) {
        subWindow->closeFlag = -1;
        MenuModelSubWindowMainSub(unit);
    }
}

/* Attaches a sub window on studioId to the unit, with its own camera. Does nothing for studioId 0 or a unit without an actor. */
void MenuModelSubWindowSet(MenuModelUnit *unit, int studioId, int mode)
{
    MenuModelSubWindowAccess *subWindow;
    StudioCamera *camera;
    Actor *actor;

    actor = unit->actor;
    if (studioId == 0) {
        return;
    }
    if (actor == 0) {
        return;
    }
    subWindow = &MenuModelSubWindow[studioId - 1];
    unit->subWindow = subWindow;
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
    memset(MenuModelSubWindow, 0, MENU_MODEL_SUBWINDOW_COUNT * 0x10U);
}

/*
 * Disposes the actor in actorSlot when it is in use, then clears the resource
 * in resourceSlot unless mode is 1.
 */
static void ActorAndResourceDispose(Actor **actorSlot, MenuModelResourceAccess **resourceSlot, int mode)
{
    Actor *actor;
    Actor *disposedActor;
    MenuModelResourceAccess *resource;
    MenuModelResourceAccess *clearedResource;

    actor = *actorSlot;
    if (actor != 0) {
        if (actor->inUseId != 0) {
            ACT_dispose(actor);
            disposedActor = *actorSlot;
            *actorSlot = 0;
            disposedActor->inUseId = 0;
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
            clearedResource->resourceId = 0;
        }
    }
}

void MenuModelWeaponDispose(MenuModelWeapon *weapon)
{
    if (weapon != 0) {
        ActorAndResourceDispose(&weapon->actor, &weapon->resource, 2);
    }
}

/* Disposes a unit's weapons, then its body and face actors and their resources. */
void MenuModelUnitDispose(MenuModelUnit *unit)
{
    MenuModelUnit *self;
    MenuModelWeapon **weaponSlot;
    int weaponIndex;
    Actor **actorSlot;
    MenuModelResourceAccess **resourceSlot;

    self = unit;
    if (self != 0) {
        weaponSlot = &self->weapon[0];
        weaponIndex = MENU_MODEL_WEAPON_COUNT - 1;
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

void MenuModelAlphaDraw(Actor *actor)
{
    if (actor->menu.transparency < 1.0f) {
        nmlModelSetTransparency(actor->menu.transparency);
        nmlModelSetToumei(1);
        nmlModelSetZwrite(1);
    }
}

/*
 * Creates the unit's body actor and, for body ids below 0x2000, its face actor.
 * Sets the resources, draw settings, motion, hand and lighting.
 */
void MenuModelUnitActorSet(MenuModelUnit *unit)
{
    Actor *actor;
    Actor *face;
    MenuModelResourceAccess *resource;
    MenuModelResourceAccess *motionResource;
    int motion;
    int handMask;
    short actorKind;
    short motionActorKind;
    ActorMenuControl *control;

    unit->actor = ACT_create(-1, unit->resourceId);
    if (unit->actor == 0) return;
    actor = unit->actor;
    resource = unit->resource;
    control = &actor->menu;
    memset(control, 0, sizeof(*control));
    actor->drawEnv.flags = 0;
    actorKind = actor->inUseId;
    actor->modelResource = resource->file[0];
    actor->drawEnv.pixelAlpha = MENU_MODEL_PIXEL_ALPHA;
    actor->matrixResource = resource->file[1];
    actor->update = ACT_updateDefault;
    actor->draw = MenuModelAlphaDraw;
    actor->move = resource->file[2];
    actor->drawEnv.renderLevel = 0;
    actor->drawEnv.partAlphaCount = 0;
    actor->drawEnv.cameraCullDistance = 0.0f;
    actor->flags = 0;
    /* Selects the body's animation pack from the actor kind in bits 12-15. */
    switch ((actorKind >> 12) & 0xF) {
    case 0:
        motionResource = MenuModelMenuMotionGet();
        actor->animPackTables[0] = motionResource != 0 ?
            motionResource->file[3] : resource->file[3];
        break;
    case 2:
        if (actorKind == 0x2201) {
            actor->animPackTables[0] = resource->file[3];
        } else {
            motionResource = MenuModelMenuMotionGet();
            actor->animPackTables[0] = motionResource != 0 ?
                motionResource->file[3] : resource->file[3];
        }
        break;
    case 4:
        actor->animPackTables[0] = func_A1C910(resource->file[3]);
        break;
    }
    motion = 0;
    resource->status |= 2;
    ACT_allocMatrix(actor, -1);
    ACT_setModelWrapper(actor, 0);
    ACT_initMotion(actor);
    motionActorKind = actor->inUseId;
    /* Chooses the initial motion from the character or AGWS table. */
    switch ((motionActorKind >> 12) & 0xF) {
    case 0:
        if (MenuModelMenuMotionGet() != 0 &&
            (unsigned short)(unit->modelId - 1) < MENU_MODEL_CHARACTER_COUNT) {
            signed char characterMotion[MENU_MODEL_CHARACTER_COUNT] = {6, 2, 0, 0, 9, 7, 8, 0, 0, 10, 11};
            motion = characterMotion[unit->modelId - 1];
            if (unit->modelId == 2 && MenuModelMemoryState.state == MENU_MODEL_MEMORY_MAIN_MENU) {
                if (unit->unusedByCreateInit1->weaponResource[1] != 0) {
                    motion = unit->unusedByCreateInit1->weaponResource[1] - 17;
                }
            }
        }
        break;
    case 2:
        if (motionActorKind != 0x2201) {
            signed char agwsMotion[MENU_MODEL_AGWS_COUNT] = {0, 1, 2, 3, 0, 4, 0, 5, 0, 6, 7, 0};
            motion = agwsMotion[unit->modelId - MENU_MODEL_AGWS_ID_BASE];
        }
        break;
    case 4:
        break;
    }
    ACT_setMotion(actor, motion);
    control->initialMotionId = motion;
    /* Creates the face actor, sharing the body's matrix and animation pack. */
    if (actor->inUseId < 0x2000) {
        face = ACT_create(-2, actor->inUseId + 0x10000);
        unit->actor2 = face;
        if (face == 0) {
            MenuModelUnitDispose(unit);
            return;
        }
        face->drawEnv.pixelAlpha = MENU_MODEL_PIXEL_ALPHA;
        face->modelResource = resource->file[4];
        face->drawEnv.flags = 0;
        face->drawEnv.renderLevel = 0;
        face->move = resource->file[5];
        face->drawEnv.partAlphaCount = 0;
        face->drawEnv.cameraCullDistance = 0.0f;
        face->matrixResource = actor->matrixResource;
        face->animPackTables[0] = actor->animPackTables[0];
        ACT_allocMatrix(face, -1);
        ACT_setModelWrapper(face, 0);
        ACT_initMotion(face);
        face->update = ACT_updateDefault;
        face->flags &= ~8;
        ACT_setFace(face, actor, 0);
    }
    if ((actor->inUseId & 0xF000) == 0) {
        handMask = 0;
        switch (actor->inUseId) {
        case 1: handMask = 33; break;
        case 4: handMask = 1; break;
        }
        ACT_setHand(actor, handMask);
    }
    /* Ambient light plus three parallel lights, each with an intensity and a direction. */
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
        xglLightIntensityAmbient(&actor->light, &lighting[0].quadword);
        xglLightIntensityParallel(&actor->light, 0, &lighting[1].quadword);
        xglLightDirection(&actor->light, 0, &lighting[4].value);
        xglLightIntensityParallel(&actor->light, 1, &lighting[2].quadword);
        xglLightDirection(&actor->light, 1, &lighting[5].value);
        xglLightIntensityParallel(&actor->light, 2, &lighting[3].quadword);
        xglLightDirection(&actor->light, 2, &lighting[6].value);
    }
    {
        unsigned int actorFlags = actor->flags;
        actor->animation.slot.flags |= 0x100008;
        actor->flags = actorFlags | 0x8000;
        actor->animation.slot.flags |= 0x400000;
    }
}

/*
 * Creates the actor for a weapon, parents its arms to the unit's actor and
 * sets its motion for the weapon id.
 */
void MenuModelWeaponActorSet(MenuModelUnit *unit, MenuModelWeapon *weapon)
{
    Actor *actor;
    Actor *parent;
    MenuModelResourceAccess *resource;

    if (unit == 0) return;
    parent = unit->actor;
    if (parent == 0) return;
    actor = ACT_create(-1, weapon->resourceId);
    weapon->actor = actor;
    if (unit->modelId == 7 && weapon->weaponIndex == 1) {
        resource = weapon->unit->weapon[0]->resource;
    } else {
        resource = weapon->resource;
    }
    memset(&actor->menu, 0, 0x70);
    actor->drawEnv.flags = 0;
    actor->modelResource = resource->file[0];
    actor->matrixResource = resource->file[1];
    actor->drawEnv.pixelAlpha = MENU_MODEL_PIXEL_ALPHA;
    actor->flags = 8;
    actor->update = ACT_updateDefault;
    actor->draw = MenuModelAlphaDraw;
    actor->move = resource->file[2];
    actor->drawEnv.renderLevel = 0;
    actor->drawEnv.partAlphaCount = 0;
    actor->drawEnv.cameraCullDistance = 0.0f;
    if ((unsigned int)((unsigned short)actor->inUseId - 0x3007) < 4) {
        actor->animPackTables[0] = resource->file[3];
    } else if ((unsigned short)(unit->modelId - MENU_MODEL_AGWS_ID_BASE) < MENU_MODEL_AGWS_COUNT &&
               (unsigned short)(weapon->weaponId - 70) < 45) {
        actor->animPackTables[0] = func_A1C910(resource->file[3]);
    } else {
        actor->animPackTables[0] = 0;
    }
    resource->status |= 2;
    ACT_allocMatrix(actor, -1);
    ACT_setModelWrapper(actor, 0);
    ACT_initMotion(actor);
    if (unit->modelId == 2 && (unsigned short)(weapon->weaponId - 18) < 5) {
        signed char weaponMotion[5] = {0, 0, 1, 2, 3};
        unsigned short weaponId = weapon->weaponId;
        int parentMotion = weaponId - 17;
        ACT_setMotion(actor, weaponMotion[weaponId - 18]);
        ACT_setMotion(parent, parentMotion);
        parent->animation.slot.flags |= 8;
    }
    if ((unsigned short)(unit->modelId - MENU_MODEL_AGWS_ID_BASE) < MENU_MODEL_AGWS_COUNT &&
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
    actor->flags &= ~8;
}

/* Applies controller 1 input to the unit's rotation and uniform scale while its menu control is enabled. */
void MenuModelControl(MenuModelUnit *unit)
{
    Actor *actor = unit->actor;
    AlignedVector4 *rotation = &actor->menu.rotation;
    AlignedVector4 *scale = &actor->menu.scale;
    unsigned short buttons;

    if (actor->menu.enabled) {
        buttons = PadData[0].buttons;
        if (buttons & PAD_UP)
            rotation->x -= 0.1f;
        if (buttons & PAD_DOWN)
            rotation->x += 0.1f;
        if (buttons & PAD_LEFT)
            rotation->y -= 0.1f;
        if (buttons & PAD_RIGHT)
            rotation->y += 0.1f;
        if (buttons & PAD_L2) {
            scale->x -= 0.02f;
            if (actor->scale.x <= 0.25f)
                scale->x = 0.25f;
        }
        if (PadData[0].buttons & PAD_R2) {
            scale->x += 0.02f;
            if (actor->scale.x >= 0.9799995f)
                scale->x = 0.9799995f;
        }
        scale->y = scale->z = scale->x;
    }
}

/*
 * Applies both controllers' input: with R2 held it changes rotation, with L2
 * held it changes scale. preset selects the scale limit entry.
 */
void MenuModelControl2(MenuModelUnit *unit, int preset)
{
    Actor *actor = unit->actor;
    AlignedVector4 *rotation = &actor->menu.rotation;
    AlignedVector4 *scale = &actor->menu.scale;
    MenuModelScaleLimits minimum;
    MenuModelScaleLimits maximum;

    if (actor->menu.enabled) {
        if ((PadData[0].buttons & PAD_R2) || (PadData[1].buttons & PAD_R2)) {
            if ((PadData[0].buttons & PAD_UP) ||
                (PadData[1].buttons & PAD_UP)) {
                if (rotation->x >= -0.499999f)
                    rotation->x -= 0.1f;
            }
            if ((PadData[0].buttons & PAD_DOWN) ||
                (PadData[1].buttons & PAD_DOWN)) {
                if (rotation->x <= 1.0f)
                    rotation->x += 0.1f;
            }
            if ((PadData[0].buttons & PAD_LEFT) ||
                (PadData[1].buttons & PAD_LEFT))
                rotation->y -= 0.1f;
            if ((PadData[0].buttons & PAD_RIGHT) ||
                (PadData[1].buttons & PAD_RIGHT))
                rotation->y += 0.1f;
            if (rotation->x > 3.1415927f)
                rotation->x = rotation->x - 3.1415927f + -3.1415927f;
            else if (rotation->x < -3.1415927f)
                rotation->x = rotation->x + 3.1415927f + 3.1415927f;
            if (rotation->y > 3.1415927f)
                rotation->y = rotation->y - 3.1415927f + -3.1415927f;
            else if (rotation->y < -3.1415927f)
                rotation->y = rotation->y + 3.1415927f + 3.1415927f;
        } else if ((PadData[0].buttons & PAD_L2) || (PadData[1].buttons & PAD_L2)) {
            if ((PadData[0].buttons & PAD_UP) ||
                (PadData[1].buttons & PAD_UP)) {
                minimum = minimumScaleTable[0];
                scale->x -= 0.02f;
                if (actor->scale.x <= minimum.value[preset])
                    scale->x = minimum.value[preset];
            }
            if ((PadData[0].buttons & PAD_DOWN) ||
                (PadData[1].buttons & PAD_DOWN)) {
                maximum = maximumScaleTable[0];
                scale->x += 0.02f;
                if (actor->scale.x >= maximum.value[preset])
                    scale->x = maximum.value[preset];
            }
            scale->y = scale->z = scale->x;
        }
    }
}

/* Places the unit's actor from its menu control and prints the rotation and scale values. */
void MenuModelNavelMove(MenuModelUnit *unit)
{
    Matrix4 matrix;
    Actor *actor = unit->actor;
    ActorMenuControl *control = &actor->menu;

    actor->rotation = control->rotation;
    actor->scale = control->scale;
    xglMatrixStackUnit();
    xglMatrixStackRotX(actor->rotation.x);
    xglMatrixStackRotY(actor->rotation.y);
    xglMatrixStackScale(&actor->scale.x);
    xglMatrixStackTrans(&control->position.x);
    xglMatrixStackSave(matrix);
    actor->position.x = control->position.x - matrix[3][0];
    actor->position.y = control->position.y - matrix[3][1];
    actor->position.z = control->position.z - matrix[3][2];
    actor->position.w = 1.0f;
    /* VF2 retains position.w; VF2/VF3 and arithmetic flags are scratch here. */
    Vu0AddVectorXYZ(&actor->position, &actor->position, &control->offset);
    xglFontDebugPrintf(98, 32, navelRotationXFormat, (double)actor->rotation.x);
    xglFontDebugPrintf(98, 40, navelRotationYFormat, (double)actor->rotation.y);
    xglFontDebugPrintf(98, 48, navelRotationZFormat, (double)actor->rotation.z);
    xglFontDebugPrintf(98, 56, navelScaleXFormat, (double)actor->scale.x);
}

void MenuModelMotionSet(MenuModelMotionTarget *target, unsigned int dataId)
{
    Actor *actor;

    actor = target->actor;
    ACT_setMotion(actor, dataId);
    actor->animation.slot.flags |= 8;
}

/* Draw type function: fades the actor in by one step, ending at full opacity. */
void ModelDrawTypeOpen01(Actor *actor)
{
    ActorMenuControl *control;
    float progress;

    control = &actor->menu;
    progress = control->transparency + modelDrawOpen01Step;
    control->transparency = progress;
    if (progress >= 1.0f) {
        control->transparency = 1.0f;
        control->drawTypeFunction = 0;
    }
}

/* Draw type function: fades the actor out by one step, ending at fully transparent. */
void ModelDrawTypeClose01(Actor *actor)
{
    ActorMenuControl *control;
    float progress;

    control = &actor->menu;
    progress = control->transparency - modelDrawClose01Step;
    control->transparency = progress;
    if (progress <= 0.0f) {
        control->transparency = 0.0f;
        control->drawTypeFunction = 0;
    }
}

/* Draw type function: fades the actor out by one step, ending at the minimum opacity. */
void ModelDrawTypeClose02(Actor *actor)
{
    ActorMenuControl *control;
    float minProgress;
    float progress;

    control = &actor->menu;
    minProgress = modelDrawClose02MinProgress;
    progress = control->transparency - modelDrawClose02Step;
    control->transparency = progress;
    if (progress <= minProgress) {
        control->transparency = minProgress;
        control->drawTypeFunction = 0;
    }
}

/*
 * Applies a draw type: SHOW and HIDE set the transparency directly, the fade
 * types install a per-frame draw type function.
 */
static void MenuModelDrawTypeSet(Actor *actor, int drawType)
{
    ActorMenuControl *control;

    if (actor == 0) {
        return;
    }
    control = &actor->menu;
    switch (drawType) {
    case MENU_MODEL_DRAW_SHOW:
        control->transparency = 1.0f;
        break;
    case MENU_MODEL_DRAW_HIDE:
        control->transparency = 0.0f;
        break;
    case MENU_MODEL_DRAW_FADE_IN:
        control->drawTypeFunction = ModelDrawTypeOpen01;
        break;
    case MENU_MODEL_DRAW_FADE_OUT:
        control->drawTypeFunction = ModelDrawTypeClose01;
        break;
    case MENU_MODEL_DRAW_FADE_OUT_PARTIAL:
        control->drawTypeFunction = ModelDrawTypeClose02;
        break;
    }
}

/* Applies drawType to the weapon in weaponIndex, when it has an actor. */
void MenuModelWeaponOpen(MenuModelUnit *unit, int drawType, int weaponIndex) {
    Actor *actor;
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

/* Applies drawType to the unit's actor and to each of its weapons. */
void MenuModelUnitOpen(MenuModelUnit *unit, int drawType)
{
    int weaponIndex;

    MenuModelDrawTypeSet(unit->actor, drawType);
    for (weaponIndex = 0; weaponIndex < MENU_MODEL_WEAPON_COUNT; weaponIndex++) {
        MenuModelWeaponOpen(unit, drawType, weaponIndex);
    }
}

void MenuModelDrawTypeMain(MenuModelDrawContext *context)
{
    Actor *actor;

    actor = context->actor;
    if (actor != 0) {
        if (actor->menu.drawTypeFunction != 0) {
            actor->menu.drawTypeFunction(actor);
        }
    }
}

/* Per-frame task of a weapon: waits for its resource, spawns its actor, then runs it until it ends. */
static void tskMenuModelWeaponTaskMain(XglTaskPrefix *task)
{
    MenuModelWeapon *weapon;
    Actor *actor;
    MenuModelUnit *unit;
    void *resource;

    weapon = (MenuModelWeapon *)task;
    actor = weapon->actor;
    resource = weapon->resource;
    if (MenuModelFlag & MENU_MODEL_FLAG_BREAK) {
        weapon->taskPhase = MENU_MODEL_PHASE_END;
    }
    unit = weapon->unit;
    if (unit == 0) {
        weapon->taskPhase = MENU_MODEL_PHASE_END;
    } else if (unit->modelId == 7 && weapon->weaponIndex == 1) {
        if (unit->weapon[0] == 0) {
            weapon->taskPhase = MENU_MODEL_PHASE_END;
        } else {
            resource = unit->weapon[0]->resource;
        }
    }
    /* END releases the weapon and resets its slot with CreateInit. */
    switch (weapon->taskPhase) {
    case MENU_MODEL_PHASE_INIT:
        weapon->extFuncFlag = 0;
        weapon->taskPhase = MENU_MODEL_PHASE_WAIT_RESOURCE;
        /* fall through */
    case MENU_MODEL_PHASE_WAIT_RESOURCE:
        if (resource != 0) {
            if ((int)resource < 0) {
                weapon->taskPhase = MENU_MODEL_PHASE_END;
            } else if (unit->actor != 0 && unit->actor->menu.transparency == 1.0f) {
                weapon->taskPhase = MENU_MODEL_PHASE_SPAWN;
            }
        }
        break;
    case MENU_MODEL_PHASE_SPAWN:
        MenuModelWeaponActorSet(unit, weapon);
        MenuModelWeaponOpen(weapon->unit, MENU_MODEL_DRAW_FADE_IN, weapon->weaponIndex);
        weapon->taskPhase = MENU_MODEL_PHASE_RUN;
        weapon->active = 1;
        break;
    case MENU_MODEL_PHASE_RUN:
        break;
    case MENU_MODEL_PHASE_END:
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

/*
 * Per-frame task of a unit: waits for its resource, spawns its actor, then
 * updates and draws it. END tears down the unit and its weapons.
 */
static void tskMenuModelTaskMain(XglTaskPrefix *task)
{
    MenuModelUnit *unit;
    Actor *actor;
    ActorMenuControl *control;
    MenuModelWeapon **weaponSlot;
    DoublewordAlignedVector4 zero;
    int remainingWeapons;
    int terminatingPhase;
    void *resource;

    unit = (MenuModelUnit *)task;
    actor = unit->actor;
    resource = unit->resource;
    if (MenuModelFlag & MENU_MODEL_FLAG_BREAK) {
        unit->taskPhase = MENU_MODEL_PHASE_END;
    }
    switch (unit->taskPhase) {
    case MENU_MODEL_PHASE_INIT:
        unit->extFuncFlag = 0;
        unit->taskPhase = MENU_MODEL_PHASE_WAIT_RESOURCE;
        /* fall through */
    case MENU_MODEL_PHASE_WAIT_RESOURCE:
        if (resource != 0) {
            if ((int)resource < 0) {
                unit->taskPhase = MENU_MODEL_PHASE_END;
            } else {
                unit->taskPhase = MENU_MODEL_PHASE_SPAWN;
            }
        }
        break;
    case MENU_MODEL_PHASE_SPAWN:
        MenuModelUnitActorSet(unit);
        unit->taskPhase = MENU_MODEL_PHASE_RUN;
        break;
    case MENU_MODEL_PHASE_RUN:
        break;
    case MENU_MODEL_PHASE_END:
        MenuModelSubWindowBreak(unit);
        MenuModelResourceCancel3(unit);
        MenuModelUnitDispose(unit);
        if (unit->unusedByCreateInit4 != 0) {
            *unit->unusedByCreateInit4 = 0;
        }
        terminatingPhase = -1;
        weaponSlot = unit->weapon;
        for (remainingWeapons = MENU_MODEL_WEAPON_COUNT - 1; remainingWeapons >= 0; remainingWeapons--) {
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
        unit->extFunc1(unit, unit->extFunc2);
    }
    if (actor != 0) {
        control = &actor->menu;
        xglStudioChange(0);
        xglStudioGetCamera2(0)->active = 1;
        /* While the control is disabled, the transforms are reset to their default values. */
        if (control->enabled == 0) {
            memset(&zero, 0, sizeof(zero));
            zero.w = 1.0f;
            {
                DoublewordAlignedVector4 one = {1.0f, 1.0f, 1.0f, 1.0f};
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
        ACT_modelDraw(actor);
        if (control->enabled == 0) {
            control->enabled = 1;
            control->position = actor->globalPosition;
            unit->active = 1;
        }
    }
}

/* Cancels the unit's pending resource requests and moves it to its end phase. */
void MenuModelUnitBreak(MenuModelUnit *unit) {
    if (unit != 0) {
        MenuModelResourceCancel2(unit);
        unit->taskPhase = MENU_MODEL_PHASE_END;
    }
}

/* Resets a unit's task state, actor, resource and weapon slots to their initial values. */
static void CreateInit(MenuModelUnit *unit)
{
    int weaponIndex;

    unit->taskPhase = MENU_MODEL_PHASE_INIT;
    unit->active = 0;
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
    for (weaponIndex = MENU_MODEL_WEAPON_COUNT - 1; weaponIndex >= 0; weaponIndex--) {
        unit->weapon[weaponIndex] = 0;
    }
}

/*
 * Creates the weapon task for weaponIndex from the unit's weapon data, replacing
 * an existing weapon whose object id changed. Returns the new task, or 0.
 */
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
    if ((unsigned int)weaponIndex >= MENU_MODEL_WEAPON_COUNT) {
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
        unit->weapon[weaponIndex]->taskPhase = MENU_MODEL_PHASE_END;
        unit->weapon[weaponIndex] = 0;
        if (unit->unitData->weaponResource[weaponIndex] == 0) {
            return 0;
        }
    } else {
        if (unit->unitData->weaponResource[weaponIndex] == 0) {
            return 0;
        }
    }

    /* Resource 0x13 creates no weapon task; it only sets the unit's motion to 2. */
    if (unit->unitData->weaponResource[weaponIndex] == 0x13) {
        Actor *actor = unit->actor;

        ACT_setMotion(actor, 2);
        actor->animation.slot.flags |= 8;
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
        weapon->taskPhase = MENU_MODEL_PHASE_END;
        return 0;
    }

    unit->weapon[weaponIndex] = weapon;
    weapon->owner = unit;
    if (result != 0) {
        weapon->result = (int *)result;
        *result = (XglTaskPrefix *)weapon;
    }

    /* For unit 7, weapon 1 shares weapon 0's resource: weapon 0 creates it and this one returns. */
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

/*
 * Creates a unit task for modelId and requests its resource. The resource id
 * comes from the model id, the memory state or fixed tables. When result is
 * not null, *result receives the unit.
 */
void MenuModelCreate(void **result, int modelId)
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
        if (MenuModelMemoryState.state >= MENU_MODEL_MEMORY_MAIN_MENU) {
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
    if (MenuModelMemoryState.state >= MENU_MODEL_MEMORY_MAIN_MENU && modelId < 33)
        unit->unusedByCreateInit1 = func_A191C0(modelId);
    if (result != 0) {
        unit->unusedByCreateInit4 = result;
        *result = unit;
    }
    MenuModelResourceRequest(&unit->resource, unit->resourceId, 0);
}

void MenuModelExtFuncSet(MenuModelUnit *unit, void (*extFunc1)(MenuModelUnit *, void *), void *extFunc2)
{
    if (unit != 0) {
        unit->extFunc2 = extFunc2;
        unit->extFunc1 = extFunc1;
        unit->extFuncFlag = 0;
    }
}

Actor *MenuModelWeaponActorGet(MenuModelUnit *unit, int weaponIndex)
{
    Actor *actor;
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

/* Breaks every menu model: runs one step with the break flag set, then resets the resource queue and sub windows. */
void MenuModelAllBreak(void) {
    MenuModelFlag |= MENU_MODEL_FLAG_BREAK;
    MenuModelMain();
    MenuModelFlag &= ~MENU_MODEL_FLAG_BREAK;
    MenuModelResourceInit();
    MenuModelSubWindowinit();
}

/* Aligns pool to 16 bytes, builds the menu task list in it and returns the first byte after it. */
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
