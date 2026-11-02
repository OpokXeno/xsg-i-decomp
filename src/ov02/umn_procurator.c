/*
 * OV02 original TU 8: 0x00a04170..0x00a05738 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/e_message.h"
#include "main/end_print.h"
#include "main/menu_model.h"
#include "main/xgl_2.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"
#include "main/xgl_dma.h"
#include "ov02/ov02.h"
#include "ov02/task_umn_top_win.h"
#include "ov02/title_hdd_install.h"
#include "ov02/tsk_umn_object_task_main.h"
#include "ov02/umn_event_text_symbol_check.h"
#include "ov02/umn_procurator.h"

/* Model id of the procurator that MenuModelCreate builds into UmnModelUkn. */
#define UMN_PROCURATOR_MODEL_ID 0x1D

/* UmnManzaiWinTask.phase: the steps of tskUmnManzaiWin. */
#define UMN_MANZAI_WIN_START 0
#define UMN_MANZAI_WIN_WAIT_SHOWN 1
#define UMN_MANZAI_WIN_IDLE 2
#define UMN_MANZAI_WIN_SLIDE_IN_START 10
#define UMN_MANZAI_WIN_SLIDE_IN 11
#define UMN_MANZAI_WIN_SLIDE_OUT_START 20
#define UMN_MANZAI_WIN_SLIDE_OUT 21

/* MenuModelUnit.extFuncFlag of the two Manzai model workers. */
#define UMN_MANZAI_MODEL_INIT 0
#define UMN_MANZAI_MODEL_ENTER 1
#define UMN_MANZAI_MODEL_SUBWINDOW 2
#define UMN_MANZAI_MODEL_SPEAKING 3       /* the sub window grows: this model has the text */
#define UMN_MANZAI_MODEL_WAIT_TEXT 4
#define UMN_MANZAI_MODEL_LISTENING 5      /* the sub window shrinks: the other model has the text */
#define UMN_MANZAI_MODEL_CLOSE 10
#define UMN_MANZAI_MODEL_DONE 11

/*
 * The subwindow record MenuModelUnit.subWindow points at. UmnProcuratorManzai2
 * slides the +4/+6 pair toward the +8/+0xA pair (read unsigned) and
 * UmnProcuratorManzai slides the +8/+0xA pair toward the +4/+6 pair, so each
 * function reads the record through its own view of the same layout.
 */
typedef struct UmnSubWindow {
    unsigned char unmodeled_00[4];
    short x;                /* +0x4 */
    short y;                /* +0x6 */
    unsigned short width;   /* +0x8 */
    unsigned short height;  /* +0xA */
    StudioCamera *camera;   /* +0xC */
} UmnSubWindow;

typedef struct UmnSubWindowView {
    unsigned char unmodeled_00[4];
    unsigned short x;       /* +0x4 */
    unsigned short y;       /* +0x6 */
    short width;            /* +0x8 */
    short height;           /* +0xA */
    StudioCamera *camera;   /* +0xC */
} UmnSubWindowView;

/* The original table is copied as fourteen doublewords. */
typedef union UmnLightSet {
    Vector4 v[7];
    XglLightQuadword quadword[7];
    u64 words[14];
} UmnLightSet;

/* Defined in main/tu165 over a view whose only used member is the actor at +0x20, the same as MenuModelUnit. */
extern void MenuModelMotionSet(MenuModelUnit *unit, unsigned int data_id);
extern const char D_00A12060[];
extern const char D_00A12080[];
extern const char D_00A12190[];
extern const UmnLightSet D_00A120F0;
extern DoublewordAlignedVector4 D_00A12160;
extern DoublewordAlignedVector4 D_00A12170;
extern const DoublewordAlignedVector4 D_00A12180;
extern DoublewordAlignedVector4 D_00A121D0;
extern DoublewordAlignedVector4 D_00A121E0;
extern DoublewordAlignedVector4 D_00A121F0;
extern DoublewordAlignedVector4 D_00A12230;

int UmnManzaiControl(void)
{
    unsigned int flag = UmnManzaiFlag;
    UmnEventTextRecord *next;
    unsigned int cleared;

    if (!(flag & UMN_MANZAI_FINISHED)) {
        if ((flag & UMN_MANZAI_BOTH_SHOWN) == UMN_MANZAI_BOTH_SHOWN) {
            cleared = flag & ~UMN_MANZAI_CONFIRM;
            UmnManzaiFlag = cleared & ~UMN_MANZAI_NEW_TEXT;
            if ((flag & UMN_MANZAI_SLIDING) == 0) {
                if (PadData.half_2a == PAD_CIRCLE) {
                    UmnManzaiFlag |= UMN_MANZAI_CONFIRM;
                    if (eMessageNextPage(&UmnManzaiWin[UmnManzaiText->value - 1].message, 0) == 0) {
                        next = UmnEventTextNextGet(1);
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        if (next == 0) {
                            UmnManzaiFlag |= UMN_MANZAI_FINISHED;
                        } else {
                            UmnManzaiText = next;
                            UmnManzaiFlag |= UMN_MANZAI_NEW_TEXT;
                        }
                    }
                }
            }
        }
    } else if ((flag & UMN_MANZAI_BOTH_SHOWN) == 0) {
        return 0;
    }
    xglFontDebugPrintf(0x80, 0x40, D_00A12060, UmnManzaiFlag);
    return 1;
}

int UmnManzaiInit(int event_no)
{
    UmnManzaiWinTask *window;
    int index;

    if (UmnEventTextMake(event_no) < 0) {
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
    } while (index < UMN_MANZAI_WINDOW_COUNT);
    UmnManzaiFlag = UMN_MANZAI_UKN_MOVING | UMN_MANZAI_SON_MOVING;
    return 0;
}

/* Window worker: the title-window step machine, run once per window record. */
void tskUmnManzaiWin(UmnObjectTask *task, void *data)
{
    UmnManzaiWinTask *window = data;
    unsigned char mode = UmnWork.mode;
    int i;
    if (mode != UMN_MODE_MAIL) {
        task->state = -1;
        return;
    }
    if (task->state == 0) {
        window->phase = UMN_MANZAI_WIN_START;
        if (window->task_index == mode) {
            window->x = 0;
            window->y = 16;
        } else {
            window->x = 48;
            window->y = 352;
        }
        for (i = 0; i < UMN_MANZAI_ROW_COUNT; i++) {
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
    case UMN_MANZAI_WIN_START:
        window->phase = UMN_MANZAI_WIN_WAIT_SHOWN;
        /* fall through */
    case UMN_MANZAI_WIN_WAIT_SHOWN:
        if ((UmnManzaiFlag & UMN_MANZAI_BOTH_SHOWN) == UMN_MANZAI_BOTH_SHOWN) {
            if (window->text->value == window->task_index) {
                window->phase = UMN_MANZAI_WIN_SLIDE_IN_START;
            } else {
                window->phase = UMN_MANZAI_WIN_IDLE;
            }
        }
        /* fall through */
    case UMN_MANZAI_WIN_IDLE: {
        unsigned int flags = UmnManzaiFlag;

        UmnManzaiFlag = flags & ~UMN_MANZAI_TEXT_TAKEN;
        if (flags & UMN_MANZAI_NEW_TEXT) {
            if (UmnManzaiText->value == window->task_index) {
                window->text = UmnManzaiText;
                window->phase = UMN_MANZAI_WIN_SLIDE_IN_START;
            } else {
                window->phase = UMN_MANZAI_WIN_SLIDE_OUT_START;
            }
            UmnManzaiFlag |= UMN_MANZAI_TEXT_TAKEN;
        }
        if (UmnManzaiFlag & UMN_MANZAI_FINISHED) {
            window->phase = UMN_MANZAI_WIN_SLIDE_OUT_START;
        }
        break;
    }
    case UMN_MANZAI_WIN_SLIDE_IN_START:
        if (window->ready == 0) {
            for (i = 0; i < UMN_MANZAI_ROW_COUNT; i++) {
                if (window->task_index == 1) {
                    window->rows[i].x = 0;
                } else {
                    window->rows[i].x = OV02_SCREEN_WIDTH;
                }
                if (i & 1) {
                    window->rows[i].y = window->y + 80;
                } else {
                    window->rows[i].y = window->y;
                }
            }
        }
        window->phase = UMN_MANZAI_WIN_SLIDE_IN;
        window->ready = 1;
        /* fall through */
    case UMN_MANZAI_WIN_SLIDE_IN: {
        short old_x = window->rows[4].x;
        short *slide = &window->rows[4].x;
        short targets[UMN_MANZAI_WINDOW_COUNT] = { 464, 48 };

        MoveSlide(slide, &targets[window->task_index - 1], 3.0f);
        window->rows[5].x = window->rows[4].x;
        window->rows[2].x += window->rows[4].x - old_x;
        window->rows[3].x = window->rows[2].x;
        if (*slide == targets[window->task_index - 1]) {
            window->phase = UMN_MANZAI_WIN_IDLE;
            eMessageTextChange(&window->message, window->text->text);
            eMessageModeChange(&window->message, E_MODE_OPEN_START);
        }
        break;
    }
    case UMN_MANZAI_WIN_SLIDE_OUT_START:
        window->phase = UMN_MANZAI_WIN_SLIDE_OUT;
        eMessageModeChange(&window->message, E_MODE_HIDE_START);
        /* fall through */
    case UMN_MANZAI_WIN_SLIDE_OUT: {
        short old_x = window->rows[4].x;
        short *slide = &window->rows[4].x;
        short targets[UMN_MANZAI_WINDOW_COUNT] = { 0, OV02_SCREEN_WIDTH };

        MoveSlide(slide, &targets[window->task_index - 1], 3.0f);
        window->rows[5].x = window->rows[4].x;
        window->rows[2].x += window->rows[4].x - old_x;
        window->rows[3].x = window->rows[2].x;
        if (*slide == targets[window->task_index - 1]) {
            if (UmnManzaiFlag & UMN_MANZAI_FINISHED) {
                task->state = -1;
                return;
            }
            window->ready = 0;
            window->phase = UMN_MANZAI_WIN_IDLE;
        }
        break;
    }
    default:
        break;
    }

    if (window->ready != 0) {
        endPrintExtFunc(0, END_PRINT_CMD_RIBBON, window->rows);
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

static void LocalLightSet(void *model, int unused)
{
    UmnLightSet lights = D_00A120F0;
    Actor *actor = model;
    StudioLight *light = &actor->light;

    xglLightIntensityAmbient(light, &lights.quadword[0]);
    xglLightIntensityParallel(light, 0, &lights.quadword[1]);
    xglLightDirection(light, 0, &lights.v[4]);
    xglLightIntensityParallel(light, 1, &lights.quadword[2]);
    xglLightDirection(light, 1, &lights.v[5]);
    xglLightIntensityParallel(light, 2, &lights.quadword[3]);
    xglLightDirection(light, 2, &lights.v[6]);
    actor->flags |= 0x8000;
}

void UmnProcuratorManzai2(MenuModelUnit *task, void *argument)
{
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
    uknActor = UmnModelUkn->actor;
    uknMenu = &uknActor->menu;
    uknRotation = &uknActor->menu.rotation;
    if (UmnModelUkn->extFuncFlag < 2) {
        menu->transparency = 0.0f;
        return;
    }

    if (task->extFuncFlag < UMN_MANZAI_MODEL_DONE && (UmnManzaiFlag & UMN_MANZAI_FINISHED))
        task->extFuncFlag = UMN_MANZAI_MODEL_CLOSE;

    switch (task->extFuncFlag) {
    case UMN_MANZAI_MODEL_INIT:
        if (UmnModelUkn->extFuncFlag < 2)
            break;
        {
            float matrix[4][4];
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
            xglMatrixStackSave(matrix);

            menu->offset.x += matrix[3][0];
            menu->offset.y += matrix[3][1];
            menu->offset.z += matrix[3][2];
            LocalLightSet(actor, 1);
            menu->transparency = 0.0f;
            task->extFuncFlag = UMN_MANZAI_MODEL_ENTER;
        }
        /* fall through */
    case UMN_MANZAI_MODEL_ENTER:
        if (UmnModelUkn->extFuncFlag < 2)
            break;
        menu->transparency += 0.2f;
        if (menu->transparency >= 1.0f) {
            menu->transparency = 1.0f;
            task->extFuncFlag = UMN_MANZAI_MODEL_SUBWINDOW;
            UmnManzaiFlag |= UMN_MANZAI_SON_VISIBLE;
        }
        break;
    case UMN_MANZAI_MODEL_SUBWINDOW:
        {
            DoublewordAlignedVector4 local = D_00A12180;
            float matrix[4][4];
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
            xglMatrixStackSave(matrix);

            cameraPosition->x += matrix[3][0];
            cameraPosition->y += matrix[3][1];
            cameraPosition->z += matrix[3][2];
            cameraRotation->x -= 0.3f;
            sub->width = 432;
            sub->x = 432;
            sub->height = 368;
            sub->y = 368;
            if (UmnManzaiText->value == UMN_EVENT_SPEAKER_SHION) {
                task->extFuncFlag = UMN_MANZAI_MODEL_SPEAKING;
            } else {
                task->extFuncFlag = UMN_MANZAI_MODEL_LISTENING;
            }
        }
        /* fall through */
    case UMN_MANZAI_MODEL_SPEAKING:
    case UMN_MANZAI_MODEL_LISTENING:
    case UMN_MANZAI_MODEL_CLOSE:
        sub = (UmnSubWindow *)task->subWindow;
        switch (task->extFuncFlag) {
        case UMN_MANZAI_MODEL_SPEAKING:
            {
                int width = sub->width;
                int height = sub->height;
                target_x = width - 224;
                target_y = height - 240;
            }
            break;
        case UMN_MANZAI_MODEL_LISTENING:
            {
                int width = sub->width;
                int height = sub->height;
                target_x = width - 128;
                target_y = height - 144;
            }
            break;
        case UMN_MANZAI_MODEL_CLOSE:
            target_x = (short)sub->width;
            target_y = (short)sub->height;
            break;
        }
        if (sub->x == target_x && sub->y == target_y) {
            if (task->extFuncFlag == UMN_MANZAI_MODEL_CLOSE) {
                UmnManzaiFlag &= ~UMN_MANZAI_SON_SHOWN;
                task->extFuncFlag = UMN_MANZAI_MODEL_DONE;
            } else {
                UmnManzaiFlag |= UMN_MANZAI_SON_SHOWN;
                task->extFuncFlag = UMN_MANZAI_MODEL_WAIT_TEXT;
            }
            UmnManzaiFlag &= ~UMN_MANZAI_SON_MOVING;
        } else {
            MoveSlide(&sub->x, &target_x, 3.0f);
            MoveSlide(&sub->y, &target_y, 3.0f);
        }
        break;
    case UMN_MANZAI_MODEL_WAIT_TEXT:
        if (UmnManzaiFlag & UMN_MANZAI_NEW_TEXT) {
            if (UmnManzaiText->value == UMN_EVENT_SPEAKER_SHION) {
                MenuModelMotionSet(task, UmnManzaiText->number);
                task->extFuncFlag = UMN_MANZAI_MODEL_SPEAKING;
            } else {
                task->extFuncFlag = UMN_MANZAI_MODEL_LISTENING;
            }
            UmnManzaiFlag |= UMN_MANZAI_SON_MOVING;
        }
        break;
    case UMN_MANZAI_MODEL_DONE:
        MenuModelSubWindowBreak(task);
        task->taskPhase = -1;
        break;
    }

    xglFontDebugPrintf(0x80, 0x20, D_00A12190, task->extFuncFlag);
}

static void Rotation(float *out, float *position, float *base, float *rotation)
{
    float matrix[4][4];

    xglMatrixStackUnit();
    xglMatrixStackRotZ(rotation[2]);
    xglMatrixStackRotY(rotation[1]);
    xglMatrixStackRotX(rotation[0]);
    xglMatrixStackTrans(position);
    xglMatrixStackSave(matrix);

    out[0] = base[0] + matrix[3][0];
    out[1] = base[1] + matrix[3][1];
    out[2] = base[2] + matrix[3][2];
}

void UmnProcuratorManzai(MenuModelUnit *task, void *argument)
{
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

    if (task->extFuncFlag != UMN_MANZAI_MODEL_DONE) {
        if (UmnManzaiFlag & UMN_MANZAI_FINISHED)
            task->extFuncFlag = UMN_MANZAI_MODEL_CLOSE;
    }

    state = task->extFuncFlag;
    switch (state) {
    case UMN_MANZAI_MODEL_INIT:
        task->extFuncFlag = UMN_MANZAI_MODEL_ENTER;
        LocalLightSet(actor, 0);
        {
            StudioCamera *case_camera = xglStudioGetCamera2(0);
            DoublewordAlignedVector4 rotation_copy = D_00A121D0;
            DoublewordAlignedVector4 position_copy = D_00A121E0;

            case_camera->position.vector = position_copy;
            case_camera->rotation.vector = rotation_copy;
        }
        /* fall through */
    case UMN_MANZAI_MODEL_ENTER:
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
            task->extFuncFlag = UMN_MANZAI_MODEL_SUBWINDOW;
        break;
    case UMN_MANZAI_MODEL_SUBWINDOW:
        if (!(UmnManzaiFlag & UMN_MANZAI_SON_VISIBLE))
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
            if (UmnManzaiText->value == UMN_EVENT_SPEAKER_UKUN)
                task->extFuncFlag = UMN_MANZAI_MODEL_SPEAKING;
            else
                task->extFuncFlag = UMN_MANZAI_MODEL_LISTENING;
        }
        /* fall through */
    case UMN_MANZAI_MODEL_SPEAKING:
    case UMN_MANZAI_MODEL_LISTENING:
    case UMN_MANZAI_MODEL_CLOSE:
        window = (UmnSubWindowView *)task->subWindow;
        switch (task->extFuncFlag) {
        case UMN_MANZAI_MODEL_SPEAKING:
            x = window->x + 0xE0;
            y = window->y + 0x110;
            break;
        case UMN_MANZAI_MODEL_LISTENING:
            x = window->x + 0x80;
            y = window->y + 0x90;
            break;
        case UMN_MANZAI_MODEL_CLOSE:
            x = window->x;
            y = window->y;
            break;
        }
        if (window->width == x && window->height == y) {
            if (task->extFuncFlag == UMN_MANZAI_MODEL_CLOSE) {
                UmnManzaiFlag &= ~UMN_MANZAI_UKN_SHOWN;
                task->extFuncFlag = UMN_MANZAI_MODEL_DONE;
            } else {
                UmnManzaiFlag |= UMN_MANZAI_UKN_SHOWN;
                task->extFuncFlag = UMN_MANZAI_MODEL_WAIT_TEXT;
            }
            UmnManzaiFlag &= ~UMN_MANZAI_UKN_MOVING;
        } else {
            MoveSlide(&window->width, &x, 3.0f);
            MoveSlide(&window->height, &y, 3.0f);
        }
        break;
    case UMN_MANZAI_MODEL_WAIT_TEXT:
        if (UmnManzaiFlag & UMN_MANZAI_NEW_TEXT) {
            if (UmnManzaiText->value == UMN_EVENT_SPEAKER_UKUN)
                task->extFuncFlag = UMN_MANZAI_MODEL_SPEAKING;
            else
                task->extFuncFlag = UMN_MANZAI_MODEL_LISTENING;
            UmnManzaiFlag |= UMN_MANZAI_UKN_MOVING;
        }
        break;
    case UMN_MANZAI_MODEL_DONE:
        MenuModelSubWindowBreak(task);
        MenuModelExtFuncSet(UmnModelUkn, UmnProcuratorMail, 0);
        break;
    default:
        break;
    }
    xglFontDebugPrintf(0x80, 0x18, D_00A12190, task->extFuncFlag);
}

void UmnProcuratorMail(MenuModelUnit *task, void *argument)
{
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

void UmnProcuratorTop(MenuModelUnit *task, void *argument)
{
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

void UmnProcuratorGoodBy(MenuModelUnit *task, void *argument)
{
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
    case UMN_MODE_TOP_MENU:
        if (UmnModelUkn == 0)
            MenuModelCreate(&UmnModelUkn, UMN_PROCURATOR_MODEL_ID);
        MenuModelExtFuncSet(UmnModelUkn, UmnProcuratorTop, 0);
        break;
    case UMN_MODE_MAIL:
        if (UmnModelUkn == 0)
            MenuModelCreate(&UmnModelUkn, UMN_PROCURATOR_MODEL_ID);
        MenuModelExtFuncSet(UmnModelUkn, UmnProcuratorMail, 0);
        break;
    default:
        do { MenuModelExtFuncSet(UmnModelUkn, UmnProcuratorGoodBy, 0); } while (0);
        break;
    }
}
