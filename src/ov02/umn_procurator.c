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
extern const char D_00A12190[];

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

extern void endPrintExtFunc(int kind, int id, void *data);
extern const char D_00A12080[];

/* Window worker: the title-window step machine, run once per window record. */
void tskUmnManzaiWin(UmnManzaiTask *task, UmnManzaiWinTask *window)
{
    unsigned char mode = UmnWork.mode;
    int i;
    if (mode != 1) {
        task->state = -1;
        return;
    }
    if (task->state == 0) {
        window->phase = 0;
        if (window->task_index == mode) {
            window->x = 0;
            window->y = 16;
        } else {
            window->x = 48;
            window->y = 352;
        }
        for (i = 0; i < 6; i++) {
            window->rows[i].word = 0x00fffff0;
            window->rows[i].c = 0;
            window->rows[i].b = 0;
            window->rows[i].a = 0;
        }
        window->rows[1].d = 32;
        window->rows[3].d = 0x80;
        window->rows[0].d = 32;
        window->rows[2].d = 0x80;
        window->rows[5].d = 0xff;
        window->rows[4].d = 0xff;
        window->unmodeled_58 = 0;
        eMessageSet(&window->message, 0);
        window->message.page_available = 2;
        window->ready = 0;
        window->text = UmnManzaiText;
        window->message.draw_parameter = 0x00fffff2;
        return;
    }

    switch (window->phase) {
    case 0:
        window->phase = 1;
        /* fall through */
    case 1:
        if ((UmnManzaiFlag & 0x30) == 0x30) {
            if (*window->text == window->task_index) {
                window->phase = 10;
            } else {
                window->phase = 2;
            }
        }
        /* fall through */
    case 2: {
        unsigned int flags = UmnManzaiFlag;

        UmnManzaiFlag = flags & ~0x200;
        if (flags & 0x8) {
            if (*UmnManzaiText == window->task_index) {
                window->text = UmnManzaiText;
                window->phase = 10;
            } else {
                window->phase = 20;
            }
            UmnManzaiFlag |= 0x200;
        }
        if (UmnManzaiFlag & 0x40) {
            window->phase = 20;
        }
        break;
    }
    case 10:
        if (window->ready == 0) {
            for (i = 0; i < 6; i++) {
                if (window->task_index == 1) {
                    window->rows[i].x = 0;
                } else {
                    window->rows[i].x = 512;
                }
                if (i & 1) {
                    window->rows[i].y = window->y + 80;
                } else {
                    window->rows[i].y = window->y;
                }
            }
        }
        window->phase = 11;
        window->ready = 1;
        /* fall through */
    case 11: {
        short old_x = window->rows[4].x;
        short *slide = (short *)&window->rows[4].x;
        short targets[2] = { 464, 48 };

        MoveSlide(slide, &targets[window->task_index - 1], 3.0f);
        window->rows[5].x = window->rows[4].x;
        window->rows[2].x += window->rows[4].x - old_x;
        window->rows[3].x = window->rows[2].x;
        if (*slide == targets[window->task_index - 1]) {
            window->phase = 2;
            eMessageTextChange(&window->message, ((UmnManzaiTextEntry *)window->text)->text);
            eMessageModeChange(&window->message, 34);
        }
        break;
    }
    case 20:
        window->phase = 21;
        eMessageModeChange(&window->message, 112);
        /* fall through */
    case 21: {
        short old_x = window->rows[4].x;
        short *slide = (short *)&window->rows[4].x;
        short targets[2] = { 0, 512 };

        MoveSlide(slide, &targets[window->task_index - 1], 3.0f);
        window->rows[5].x = window->rows[4].x;
        window->rows[2].x += window->rows[4].x - old_x;
        window->rows[3].x = window->rows[2].x;
        if (*slide == targets[window->task_index - 1]) {
            if (UmnManzaiFlag & 0x40) {
                task->state = -1;
                return;
            }
            window->ready = 0;
            window->phase = 2;
        }
        break;
    }
    default:
        break;
    }

    if (window->ready != 0) {
        endPrintExtFunc(0, 3, window->rows);
        if (window->task_index == 1) {
            window->message.x = window->x + 22;
        } else {
            window->message.x = window->x + 2;
        }
        window->message.y = window->y + 4;
        eMessageMain(&window->message);
    }
    xglFontDebugPrintf(0x80, (window->task_index << 3) + 32, D_00A12080, window->phase);
}

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

extern DoublewordAlignedVector4 D_00A12160;
extern DoublewordAlignedVector4 D_00A12170;
extern const DoublewordAlignedVector4 D_00A12180;

void UmnProcuratorManzai2(void *argument)
{
    MenuModelUnit *task = argument;
    Actor *actor;
    Actor *uknActor;
    ActorMenuControl *menu;
    ActorMenuControl *uknMenu;
    AlignedVector4 *uknRotation;
    AlignedVector4 *rotation;
    AlignedVector4 *scale;
    UmnSubWindow *sub;
    short target_x;
    short target_y;

    if (task->active == 0)
        return;

    actor = task->actor;
    menu = &actor->menu;
    rotation = &actor->menu.rotation;
    scale = &actor->menu.scale;
    uknActor = ((MenuModelUnit *)UmnModelUkn)->actor;
    uknMenu = &uknActor->menu;
    uknRotation = &uknActor->menu.rotation;
    if (((MenuModelUnit *)UmnModelUkn)->extFuncFlag < 2) {
        menu->transparency = 0.0f;
        return;
    }

    if (task->extFuncFlag < 11 && (UmnManzaiFlag & 0x40))
        task->extFuncFlag = 10;

    switch (task->extFuncFlag) {
    case 0:
        if (((MenuModelUnit *)UmnModelUkn)->extFuncFlag < 2)
            break;
        {
            float matrix[16];
            DoublewordAlignedVector4 position;
            DoublewordAlignedVector4 scale_src;

            position = D_00A12160;
            scale_src = D_00A12170;
            *scale = scale_src;
            menu->offset.x = uknMenu->offset.x;
            menu->offset.y = uknMenu->offset.y;
            menu->offset.z = uknMenu->offset.z;
            rotation->x = uknRotation->x;
            rotation->y = 1.5707964f - uknRotation->y;
            rotation->z = uknRotation->z;
            rotation->w = uknRotation->w;

            xglMatrixStackUnit();
            xglMatrixStackRotZ(uknRotation->z);
            xglMatrixStackRotY(uknRotation->y);
            xglMatrixStackRotX(uknRotation->x);
            xglMatrixStackTrans(&position.x);
            xglMatrixStackSave((float (*)[4])matrix);

            menu->offset.x += matrix[12];
            menu->offset.y += matrix[13];
            menu->offset.z += matrix[14];
            LocalLightSet(actor, 1);
            menu->transparency = 0.0f;
            task->extFuncFlag = 1;
        }
        /* fall through */
    case 1:
        if (((MenuModelUnit *)UmnModelUkn)->extFuncFlag < 2)
            break;
        menu->transparency += 0.2f;
        if (menu->transparency >= 1.0f) {
            menu->transparency = 1.0f;
            task->extFuncFlag = 2;
            UmnManzaiFlag |= 0x100;
        }
        break;
    case 2:
        {
            DoublewordAlignedVector4 local = D_00A12180;
            float matrix[16];
            StudioCamera *camera;
            Vector4 *cameraRotation;
            Vector4 *cameraPosition;

            MenuModelSubWindowSet(task, 2, 2);
            sub = (UmnSubWindow *)task->subWindow;
            camera = sub->camera;
            cameraRotation = &camera->rotation.vector;
            cameraPosition = &camera->position.vector;
            cameraRotation->x = actor->rotation.x;
            cameraRotation->y = actor->rotation.y;
            cameraRotation->z = actor->rotation.z;
            cameraPosition->x = actor->position.x;
            cameraPosition->y = actor->position.y;
            cameraPosition->z = actor->position.z;

            xglMatrixStackUnit();
            xglMatrixStackRotZ(cameraRotation->z);
            xglMatrixStackRotY(cameraRotation->y);
            xglMatrixStackRotX(cameraRotation->x);
            xglMatrixStackTrans(&local.x);
            xglMatrixStackSave((float (*)[4])matrix);

            cameraPosition->x += matrix[12];
            cameraPosition->y += matrix[13];
            cameraPosition->z += matrix[14];
            cameraRotation->x -= 0.3f;
            sub->width = 432;
            sub->x = 432;
            sub->height = 368;
            sub->y = 368;
            if (*UmnManzaiText == 2) {
                task->extFuncFlag = 3;
            } else {
                task->extFuncFlag = 5;
            }
        }
        /* fall through */
    case 3:
    case 5:
    case 10:
        sub = (UmnSubWindow *)task->subWindow;
        switch (task->extFuncFlag) {
        case 3:
            {
                int width = sub->width;
                int height = sub->height;
                target_x = width - 224;
                target_y = height - 240;
            }
            break;
        case 5:
            {
                int width = sub->width;
                int height = sub->height;
                target_x = width - 128;
                target_y = height - 144;
            }
            break;
        case 10:
            target_x = (short)sub->width;
            target_y = (short)sub->height;
            break;
        }
        if (sub->x == target_x && sub->y == target_y) {
            if (task->extFuncFlag == 10) {
                UmnManzaiFlag &= ~0x20;
                task->extFuncFlag = 11;
            } else {
                UmnManzaiFlag |= 0x20;
                task->extFuncFlag = 4;
            }
            UmnManzaiFlag &= ~4;
        } else {
            MoveSlide(&sub->x, &target_x, 3.0f);
            MoveSlide(&sub->y, &target_y, 3.0f);
        }
        break;
    case 4:
        if (UmnManzaiFlag & 0x8) {
            if (*UmnManzaiText == 2) {
                MenuModelMotionSet(task, ((signed char *)UmnManzaiText)[3]);
                task->extFuncFlag = 3;
            } else {
                task->extFuncFlag = 5;
            }
            UmnManzaiFlag |= 0x4;
        }
        break;
    case 11:
        MenuModelSubWindowBreak(task);
        task->taskPhase = -1;
        break;
    }

    xglFontDebugPrintf(0x80, 0x20, D_00A12190, task->extFuncFlag);
}

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

extern DoublewordAlignedVector4 D_00A121D0;
extern DoublewordAlignedVector4 D_00A121E0;
extern DoublewordAlignedVector4 D_00A121F0;

void UmnProcuratorManzai(void *argument)
{
    MenuModelUnit *task = argument;
    Actor *actor;
    AlignedVector4 *offset;
    AlignedVector4 *rotation;
    AlignedVector4 *scale;
    UmnSubWindowView *window;
    StudioCamera *camera;
    int done;
    int state;
    short x;
    short y;

    if (task->active == 0)
        return;

    actor = task->actor;
    offset = &actor->menu.offset;
    rotation = &actor->menu.rotation;
    scale = &actor->menu.scale;

    if (task->extFuncFlag != 11) {
        if (UmnManzaiFlag & 0x40)
            task->extFuncFlag = 10;
    }

    state = task->extFuncFlag;
    switch (state) {
    case 0:
        task->extFuncFlag = 1;
        LocalLightSet(actor, 0);
        {
            StudioCamera *case_camera = xglStudioGetCamera2(0);
            DoublewordAlignedVector4 rotation_copy = D_00A121D0;
            DoublewordAlignedVector4 position_copy = D_00A121E0;

            case_camera->position.vector = position_copy;
            case_camera->rotation.vector = rotation_copy;
        }
        /* fall through */
    case 1:
        done = 0;
        offset->x += 0.02f;
        if (offset->x >= 1.5f) {
            offset->x = 1.5f;
            done++;
        }
        offset->y += 0.01f;
        if (offset->y >= 0.0f) {
            offset->y = 0.0f;
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
        scale->x -= 0.01f;
        if (scale->x <= 0.5f) {
            scale->x = 0.5f;
            done++;
        }
        scale->z = scale->x;
        scale->y = scale->x;
        if (done == 5)
            task->extFuncFlag = 2;
        break;
    case 2:
        if (!(UmnManzaiFlag & 0x100))
            break;
        {
            Vector4 *camera_rotation;
            DoublewordAlignedVector4 sub_position = D_00A121F0;

            MenuModelSubWindowSet(task, 1, 2);
            window = (UmnSubWindowView *)task->subWindow;
            camera = window->camera;
            camera_rotation = &camera->rotation.vector;
            camera_rotation->x = rotation->x;
            camera_rotation->y = rotation->y;
            camera_rotation->z = rotation->z;
            Rotation(&camera->position.vector.x, &sub_position.x, &offset->x,
                     &camera_rotation->x);
            window->width = 80;
            window->x = 80;
            window->height = 64;
            window->y = 64;
            if (*UmnManzaiText == 1)
                task->extFuncFlag = 3;
            else
                task->extFuncFlag = 5;
        }
        /* fall through */
    case 3:
    case 5:
    case 10:
        window = (UmnSubWindowView *)task->subWindow;
        switch (task->extFuncFlag) {
        case 3:
            x = window->x + 0xE0;
            y = window->y + 0x110;
            break;
        case 5:
            x = window->x + 0x80;
            y = window->y + 0x90;
            break;
        case 10:
            x = window->x;
            y = window->y;
            break;
        }
        if (window->width == x && window->height == y) {
            if (task->extFuncFlag == 10) {
                UmnManzaiFlag &= ~0x10;
                task->extFuncFlag = 11;
            } else {
                UmnManzaiFlag |= 0x10;
                task->extFuncFlag = 4;
            }
            UmnManzaiFlag &= ~2;
        } else {
            MoveSlide(&window->width, &x, 3.0f);
            MoveSlide(&window->height, &y, 3.0f);
        }
        break;
    case 4:
        if (UmnManzaiFlag & 8) {
            if (*UmnManzaiText == 1)
                task->extFuncFlag = 3;
            else
                task->extFuncFlag = 5;
            UmnManzaiFlag |= 2;
        }
        break;
    case 11:
        MenuModelSubWindowBreak(task);
        MenuModelExtFuncSet(UmnModelUkn, UmnProcuratorMail, 0);
        break;
    default:
        break;
    }
    xglFontDebugPrintf(0x80, 0x18, D_00A12190, task->extFuncFlag);
}

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
