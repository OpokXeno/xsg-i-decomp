/* UMN code embedded in MAIN, original OV02 translation unit 8. */
/*
 * OV02 original TU 8: 0x00a04170..0x00a05738 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/e_message.h"
#include "ov02/tsk_umn_object_task_main.h"
extern UmnWorkData UmnWork;

/* The two original UmnManzaiWin records have a 0xa4-byte stride. */
typedef struct UmnManzaiWinTask {
    u8 unmodeled_00;
    u8 task_index;
    u8 unmodeled_02[0x5e];
    EMessageParam message;
    u8 unmodeled_90[0x14];
} UmnManzaiWinTask;

/* Owner proposal: src/main/menu_model.c, main/tu165. */
typedef struct MenuModelActor MenuModelActor;

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
    u8 unmodeled_34[0x0c];
    MenuModelTransformVector position;
    u8 unmodeled_50[0x10];
    float transparency;
    void (*drawTypeFunction)(MenuModelActor *actor);
    int initialMotionId;
    u8 unmodeled_6c[4];
} MenuModelTransformControl;

typedef unsigned int XglLightQuadword __attribute__((mode(TI)));

typedef struct XglPackedParallelLight {
    XglLightQuadword intensity;
    XglLightQuadword direction;
} XglPackedParallelLight;

typedef struct XglPackedLightSet {
    XglLightQuadword ambientIntensity;
    XglPackedParallelLight parallel[3];
} XglPackedLightSet;

typedef struct MenuModelActorLight {
    XglPackedLightSet source;
    Matrix4 directionMatrix;
    Matrix4 intensityMatrix;
} MenuModelActorLight;

struct MenuModelActor {
    u32 actorFlags;
    u8 unmodeled_04[0xbc];
    MenuModelTransformControl menu;
    u8 unmodeled_130[0x3e0];
    MenuModelActorLight light;
};

/* The menu unit is a scheduler task, distinct from the actor at +0x20. */
typedef struct MenuModelUnit {
    u8 unmodeled_00[0x10];
    signed char taskPhase;
    u8 active;
    u8 unmodeled_12;
    u8 extFuncFlag;
    u8 unmodeled_14[0x0c];
    MenuModelActor *actor;
} MenuModelUnit;

extern UmnManzaiWinTask UmnManzaiWin[2];
/*
 * TU-local declarations of ov02/tu008 (src/ov02/umn_procurator.c).
 */

#ifndef SRC_OV02_UMN_PROCURATOR_H
#define SRC_OV02_UMN_PROCURATOR_H

typedef void (*UmnTaskFunction)(void *task);

extern unsigned int UmnManzaiFlag;
extern short *UmnManzaiText;
extern void *UmnModelSon;
extern void *UmnModelUkn;

int UmnEventTextMake(void);
short *UmnEventTextNextGet(int index);
void UmnObjectTaskCreate(UmnTaskFunction function, void *argument);
void MenuModelCreate(void **model, int model_id);
void MenuModelExtFuncSet(void *model, UmnTaskFunction function, void *argument);
void MenuModelUnitOpen(void *model, int draw_type);

void UmnProcuratorManzai(void *task);
void UmnProcuratorManzai2(void *task);
void UmnProcuratorMail(void *task);
void UmnProcuratorTop(void *task);
void UmnProcuratorGoodBy(void *task);
void tskUmnManzaiWin(void *task);

/*
 * The xglMatrixStack* wrappers Rotation calls come from their definer,
 * main/tu100 xgl_2.c, through include/main/xgl_2.h.
 */

#endif /* SRC_OV02_UMN_PROCURATOR_H */

extern PadPrefix PadData;
extern int eMessageNextPage(void *page, int arg);
extern void xglSoundEffectNormalID(int id, int arg);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern const char D_00A12060[];

int UmnManzaiControl(void)
{
    unsigned int flag = UmnManzaiFlag;
    short *next;
    unsigned int cleared;

    if (!(flag & 0x40)) {
        if ((flag & 0x30) == 0x30) {
            cleared = flag & ~1;
            UmnManzaiFlag = cleared & ~8;
            if ((flag & 0x206) == 0) {
                if (PadData.half_2a == 0x20) {
                    UmnManzaiFlag |= 1;
                    if (eMessageNextPage(&UmnManzaiWin[*UmnManzaiText - 1].message, 0) == 0) {
                        next = UmnEventTextNextGet(1);
                        xglSoundEffectNormalID(1, 0);
                        if (next == 0) {
                            UmnManzaiFlag |= 0x40;
                        } else {
                            UmnManzaiText = next;
                            UmnManzaiFlag |= 8;
                        }
                    }
                }
            }
        }
    } else if ((flag & 0x30) == 0) {
        return 0;
    }
    xglFontDebugPrintf(0x80, 0x40, D_00A12060, UmnManzaiFlag);
    return 1;
}

int UmnManzaiInit(void)
{
    UmnManzaiWinTask *window;
    int index;

    if (UmnEventTextMake() < 0) {
        return -1;
    }
    UmnManzaiText = UmnEventTextNextGet(0);
    if (UmnModelSon == 0) {
        MenuModelCreate(&UmnModelSon, 3);
    }
    MenuModelExtFuncSet(UmnModelUkn, UmnProcuratorManzai, 0);
    MenuModelExtFuncSet(UmnModelSon, UmnProcuratorManzai2, 0);
    index = 0;
    window = UmnManzaiWin;
    do {
        UmnObjectTaskCreate(tskUmnManzaiWin, window);

        {
            int next_index = index + 1;
            window->task_index = next_index;
            index = next_index;
        }
        /* Advance this loop to the next task record. */
        window++;
    } while (index < 2);
    UmnManzaiFlag = 6;
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/ov02/08_umn_procurator", tskUmnManzaiWin);

extern void xglLightIntensityParallel(void *light, int index,
                                      const Vector4 *intensity);
extern void xglLightIntensityAmbient(void *light,
                                     const Vector4 *intensity);
extern void xglLightDirection(void *light, unsigned int index,
                              const Vector4 *direction);

/* The light table is copied as fourteen original doublewords. */
typedef union UmnLightSet {
    Vector4 v[7];
    u64 words[14];
} UmnLightSet;

extern const UmnLightSet D_00A120F0;

static void LocalLightSet(void *model, int unused)
{
    UmnLightSet lights = D_00A120F0;
    MenuModelActor *actor = model;
    MenuModelActorLight *light = &actor->light;

    xglLightIntensityAmbient(light, &lights.v[0]);
    xglLightIntensityParallel(light, 0, &lights.v[1]);
    xglLightDirection(light, 0, &lights.v[4]);
    xglLightIntensityParallel(light, 1, &lights.v[2]);
    xglLightDirection(light, 1, &lights.v[5]);
    xglLightIntensityParallel(light, 2, &lights.v[3]);
    xglLightDirection(light, 2, &lights.v[6]);
    actor->actorFlags |= 0x8000;
}

INCLUDE_ASM("asm/main/nonmatchings/ov02/08_umn_procurator", UmnProcuratorManzai2);

static void Rotation(float *out, float *position, float *base, float *rotation)
{
    float matrix[16];

    xglMatrixStackUnit();
    xglMatrixStackRotZ(rotation[2]);
    xglMatrixStackRotY(rotation[1]);
    xglMatrixStackRotX(rotation[0]);
    xglMatrixStackTrans(position);
    xglMatrixStackSave((float (*)[4])matrix);

    out[0] = base[0] + matrix[12];
    out[1] = base[1] + matrix[13];
    out[2] = base[2] + matrix[14];
}

INCLUDE_ASM("asm/main/nonmatchings/ov02/08_umn_procurator", UmnProcuratorManzai);

/*
 * These are bit-preserving copies of the camera's 16-byte vectors.  The
 * original copies each vector with two aligned doublewords; the union keeps
 * that evidenced storage representation alongside its float components.
 */
typedef union UmnMailVec {
    Vector4 v;
    u64 words[2];
} UmnMailVec;

extern UmnMailVec D_00A12230;

void UmnProcuratorMail(void *argument)
{
    MenuModelUnit *task = argument;
    MenuModelTransformVector *offset;
    MenuModelTransformVector *rotation;
    MenuModelTransformVector *scale;
    int done;
    int state;

    if (task->active == 0)
        return;

    offset = &task->actor->menu.offset;
    rotation = &task->actor->menu.rotation;
    scale = &task->actor->menu.scale;

    state = task->extFuncFlag;
    if (state != 1) {
        if (state > 1)
            return;
        if (state != 0)
            return;
        {
            StudioCamera *camera;
            UmnMailVec camera_rotation;
            UmnMailVec position;

            task->extFuncFlag = 1;
            LocalLightSet(task->actor, 0);
            camera = xglStudioGetCamera2(0);
            memset(&camera_rotation, 0, sizeof(camera_rotation));
            camera_rotation.v.w = 1.0f;
            position = D_00A12230;
            *(UmnMailVec *)&camera->position = position;
            *(UmnMailVec *)&camera->rotation = camera_rotation;
            MenuModelUnitOpen(task, 2);
        }
    }
    done = 0;
    scale->value.x += 0.01f;
    if (scale->value.x >= 1.0f) {
        scale->value.x = 1.0f;
        done++;
    }
    scale->value.y = scale->value.x;
    scale->value.z = scale->value.x;
    offset->value.x += 0.02f;
    if (offset->value.x >= 1.0f) {
        offset->value.x = 1.0f;
        done++;
    }
    offset->value.y -= 0.01f;
    if (offset->value.y <= -0.3f) {
        offset->value.y = -0.3f;
        done++;
    }
    rotation->value.x += 0.01f;
    if (rotation->value.x >= 0.0f) {
        rotation->value.x = 0.0f;
        done++;
    }
    rotation->value.y -= 0.01f;
    if (rotation->value.y <= -0.7853982f) {
        rotation->value.y = -0.7853982f;
        done++;
    }
    if (done == 5)
        task->extFuncFlag = 2;
}

void UmnProcuratorTop(void *argument)
{
    MenuModelUnit *task = argument;
    MenuModelTransformVector *offset;
    MenuModelTransformVector *rotation;
    int state;
    MenuModelActor *actor;

    if (task->active == 0)
        return;

    actor = task->actor;
    offset = &actor->menu.offset;
    rotation = &actor->menu.rotation;

    state = task->extFuncFlag;
    if (state != 1) {
        if (state > 1)
            return;
        if (state != 0)
            return;
        offset->value.x = 1.0f;
        offset->value.y = -0.3f;
        rotation->value.x = 0.0f;
        rotation->value.y = -0.7853982f;
        LocalLightSet(actor, 0);
        task->extFuncFlag = 1;
    }
    MenuModelUnitOpen(task, 2);
    task->extFuncFlag = 2;
}

void UmnProcuratorGoodBy(void *argument)
{
    MenuModelUnit *task = argument;
    MenuModelTransformControl *menu;
    MenuModelTransformVector *rotation;

    if (task->active == 0)
        return;

    menu = &task->actor->menu;
    rotation = &task->actor->menu.rotation;

    switch (task->extFuncFlag) {
    case 0:
        task->extFuncFlag = 1;
        LocalLightSet(task->actor, 0);
        MenuModelUnitOpen(task, 3);
    case 1:
        menu->offset.value.x += 0.4f;
        rotation->value.y += 0.4f;
        if (menu->transparency == 0.0f)
            task->taskPhase = -1;
        break;
    }
}

void UmnProcuratorSet(void)
{
    switch (UmnWork.mode) {
    case 0:
        if (UmnModelUkn == 0)
            MenuModelCreate(&UmnModelUkn, 0x1D);
        MenuModelExtFuncSet(UmnModelUkn, UmnProcuratorTop, 0);
        break;
    case 1:
        if (UmnModelUkn == 0)
            MenuModelCreate(&UmnModelUkn, 0x1D);
        MenuModelExtFuncSet(UmnModelUkn, UmnProcuratorMail, 0);
        break;
    default:
        do { MenuModelExtFuncSet(UmnModelUkn, UmnProcuratorGoodBy, 0); } while (0);
        break;
    }
}
