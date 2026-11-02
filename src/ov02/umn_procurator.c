/*
 * OV02 original TU 8: 0x00a04170..0x00a05738 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "umn_procurator.h"

#include "shared.h"
extern PadPrefix PadData;
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

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", tskUmnManzaiWin);

extern void xglLightIntensityParallel(void *light, int index,
                                      const Vector4 *intensity);
extern void xglLightIntensityAmbient(void *light,
                                     const Vector4 *intensity);
extern void xglLightDirection(void *light, unsigned int index,
                              const Vector4 *direction);

/* The original table is copied as fourteen doublewords. */
typedef union UmnLightSet {
    Vector4 v[7];
    u64 words[14];
} UmnLightSet;

extern const UmnLightSet D_00A120F0;

static void LocalLightSet(void *model, int unused)
{
    UmnLightSet lights = D_00A120F0;
    Actor *actor = model;
    StudioLight *light = &actor->light;

    xglLightIntensityAmbient(light, &lights.v[0]);
    xglLightIntensityParallel(light, 0, &lights.v[1]);
    xglLightDirection(light, 0, &lights.v[4]);
    xglLightIntensityParallel(light, 1, &lights.v[2]);
    xglLightDirection(light, 1, &lights.v[5]);
    xglLightIntensityParallel(light, 2, &lights.v[3]);
    xglLightDirection(light, 2, &lights.v[6]);
    actor->flags |= 0x8000;
}

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", UmnProcuratorManzai2);

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

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", UmnProcuratorManzai);

extern DoublewordAlignedVector4 D_00A12230;

void UmnProcuratorMail(void *argument)
{
    MenuModelUnit *task = argument;
    AlignedVector4 *offset;
    AlignedVector4 *rotation;
    AlignedVector4 *scale;
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
            DoublewordAlignedVector4 camera_rotation;
            DoublewordAlignedVector4 position;

            task->extFuncFlag = 1;
            LocalLightSet(task->actor, 0);
            camera = xglStudioGetCamera2(0);
            memset(&camera_rotation, 0, sizeof(camera_rotation));
            camera_rotation.w = 1.0f;
            position = D_00A12230;
            camera->position.vector = position;
            camera->rotation.vector = camera_rotation;
            MenuModelUnitOpen(task, 2);
        }
    }
    done = 0;
    scale->x += 0.01f;
    if (scale->x >= 1.0f) {
        scale->x = 1.0f;
        done++;
    }
    scale->y = scale->x;
    scale->z = scale->x;
    offset->x += 0.02f;
    if (offset->x >= 1.0f) {
        offset->x = 1.0f;
        done++;
    }
    offset->y -= 0.01f;
    if (offset->y <= -0.3f) {
        offset->y = -0.3f;
        done++;
    }
    rotation->x += 0.01f;
    if (rotation->x >= 0.0f) {
        rotation->x = 0.0f;
        done++;
    }
    rotation->y -= 0.01f;
    if (rotation->y <= -0.7853982f) {
        rotation->y = -0.7853982f;
        done++;
    }
    if (done == 5)
        task->extFuncFlag = 2;
}

void UmnProcuratorTop(void *argument)
{
    MenuModelUnit *task = argument;
    AlignedVector4 *offset;
    AlignedVector4 *rotation;
    int state;
    Actor *actor;

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
        offset->x = 1.0f;
        offset->y = -0.3f;
        rotation->x = 0.0f;
        rotation->y = -0.7853982f;
        LocalLightSet(actor, 0);
        task->extFuncFlag = 1;
    }
    MenuModelUnitOpen(task, 2);
    task->extFuncFlag = 2;
}

void UmnProcuratorGoodBy(void *argument)
{
    MenuModelUnit *task = argument;
    ActorMenuControl *menu;
    AlignedVector4 *rotation;

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
        menu->offset.x += 0.4f;
        rotation->y += 0.4f;
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
