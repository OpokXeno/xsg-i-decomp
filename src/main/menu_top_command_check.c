#include "common.h"

static unsigned short MenuTopCommandLock;

#include "menu_top_command_check.h"
#include "main/e_battle_win_open.h"

static inline int MenuSelectLayoutShift(const MenuTopMenuPanel *panel)
{
    return panel->rows == 3 ? -1 : 0;
}

static inline int MenuTopStateGet(void)
{
    return MenuWork.state;
}

static inline MenuStatusValuePair MenuStatusEncodeValue(short current, short maximum)
{
    MenuStatusValuePair value;

    value.halfwords[0] = current;
    value.halfwords[1] = maximum;
    return value;
}

extern const MenuFormationPositions menuFormationPositionTable;

extern const MenuLightingSources pointerLightingSources;

extern unsigned char *MenuTairetuModelPointerXtxAddr;

extern unsigned char *MenuTairetuModelPointerLexAddr;

extern const float pointerScaleRate;

extern void xglMatrixStackLoad(float matrix[4][4]);

typedef struct MenuPointaTask {
    XglTaskPrefix link;
    unsigned char state;
    unsigned char enabled;
    unsigned char initialized;
    unsigned char index;
    short previousCommand;
    short selectedSlot;
    short formationIndex;
    signed char paletteIndex;
    unsigned char unmodeled_1b;
    struct MenuModelTransform *formation;
    struct MenuModelTransform *pointer;
} MenuPointaTask;

static inline unsigned short MenuPointaGuardWord(MenuPointaTask *task)
{
    MenuTopPartyData *party = (MenuTopPartyData *)PartyDataGet();
    return *((const void *)(party->slot + task->index) ==
        (const void *)&party->firstTechniqueWord ?
        &party->firstTechniqueWord : &party->slot[task->index].id);
}

char *MenuTagTextGet(int index);

int MenuMaryIdChange(int id);

static void tskMenuTaiPointa(MenuPointaTask *task);

extern MenuModelTransform *MenuVecMat;

extern const float menuBackModelFov;

extern XglTaskScheduler *MenuTask_XMX;

extern void xglCameraInit(StudioCamera *camera);

int MenuTopCommandCheck(int commandId)
{
    unsigned int commandBit = 1 << (commandId - 1);
    return (commandBit & ~MenuTopCommandLock) != 0;
}

void MenuStatusDisp(MenuStatusPanelWindow *window, MenuStatusPanel *work);

void TopStatusWinMain(void)
{
    MenuTopStatusWin *status = TopStatusWin;
    short slideTarget;
    int chrNo;
    int i;
    int y;

    switch (status->state) {
    case 0: {
        status->count = 0;
        status->color = 0xF000F0;
        y = 28;
        i = 0;
        do {
            MenuTopPartyData *party;
            unsigned short partyId;

            party = (MenuTopPartyData *)PartyDataGet();
            partyId = party->slot[i].id;
            chrNo = MenuMaryIdChange(partyId);
            if (chrNo != 0) {
                MenuTopWindow *const window = &status->window[i];
                MenuStatusPanel *const panel = &status->panel[i];

                WindowDXSet((WindowDX *)window);
                status->window[i].draw = (void (*)(WindowDX *, void *))MenuStatusDisp;
                window->x = 16;
                window->x = -272;
                status->window[i].color = status->color;
                status->window[i].width = 240;
                status->window[i].height = 99;
                window->panel = panel;
                status->window[i].y = y;
                status->window[i].title = MenuTagTextGet(1);
                window->enabled = 1;
                status->panel[i].enabled = 1;
                status->panel[i].offsetX = 0;
                status->panel[i].offsetY = 0;
                status->panel[i].chrNo = chrNo;
                window->state = 1;
                WindowDXMain((WindowDX *)window);
                status->window[i].state = 3;
                status->count++;
            }
            ++i;
            y = 28 + 120 * i;
        } while (i < 3);
        status->state = 2;
    }
    case 2:
        slideTarget = 16;
        switch (MenuWork.state) {
        case 32:
        case 33:
            break;
        default:
            slideTarget = -272;
            break;
        }
        for (i = 0; i < status->count; i++) {
            MoveSlide(&status->window[i].x, &slideTarget, 5.0f);
            WindowDXMain((WindowDX *)&status->window[i]);
        }
        break;
    }
}

void TopInfoMain(void)
{
    MenuTopInfo *info = TopInfo;
    short slideTarget;

    switch (info->state) {
    case 0:
        WindowDXSet((WindowDX *)&info->window);
        info->window.x = -0x10;
        info->window.y = 0x1E0;
        info->window.width = 0x220;
        info->window.height = 0x36;
        info->window.color = 0xF000F0;
        info->window.draw = (void (*)(WindowDX *, void *))MenuInfoWindow;
        info->window.panel = &info->panel;
        info->panel.x = 0;
        info->panel.y = 0;
        info->window.state = 1;
        WindowDXMain((WindowDX *)&info->window);
        info->window.state = 3;
        info->slide = -1;
        info->enabled = 1;
        info->state = 2;
        info->panel.flag = 0;
    case 2:
        slideTarget = 0x182;
        switch (MenuWork.state) {
        case 0x20:
            info->slide = -1;
        case 0x21:
            info->panel.flag = 0;
            info->panel.y = 0;
            break;
        default:
            slideTarget = 0x1E0;
            break;
        }
        if (MenuWork.topCursor < 9) {
            info->panel.text = text[MenuWork.topCursor];
        } else {
            info->panel.text = topMenuEmptyText;
        }
        MoveSlide(&info->window.y, &slideTarget, 5.0f);
        WindowDXMain((WindowDX *)&info->window);
        break;
    }
}

void TopMenuWinMain(void)
{
    MenuTopMenuWin *menu = TopMenuWin;
    int menuState;
    short slideTarget;
    int i;

    switch (menu->state) {
    case 0:
        menu->color = 0xF000F0;
        for (i = 0; i < 9; i++) {
            if (MenuTopCommandCheck(MenuTopCommand[i]) == 0) {
                command_0[i].locked = 1;
            } else {
                command_0[i].locked = 0;
            }
        }
        WindowDXSet((WindowDX *)&menu->window);
        menu->window.x = 0x220;
        menu->window.y = 0x1C;
        menu->window.color = menu->color;
        menu->window.width = 0xC2;
        menu->window.height = 0xDE;
        menu->window.draw = (void (*)(WindowDX *, void *))MenuSelectWindow;
        menu->window.panel = &menu->panel;
        menu->window.title = topMenuTitleText;
        menu->panel.commands = command_0;
        menu->panel.cursor = 0;
        menu->panel.rows = 2;
        menu->window.state = 1;
        WindowDXMain((WindowDX *)&menu->window);
        menu->window.state = 3;
        menu->enabled = 1;
        menu->state = 2;
    case 2:
        slideTarget = 0x13C;
        menuState = MenuWork.state;
        if (menuState < 0x22) {
            if (menuState >= 0x20) {
                menu->panel.cursor = MenuWork.topCursor;
            } else {
                slideTarget = 0x220;
            }
        } else {
            slideTarget = 0x220;
        }
        MoveSlide(&menu->window.x, &slideTarget, 5.0f);
        WindowDXMain((WindowDX *)&menu->window);
        break;
    }
}

void TopFaceExWinMain(void)
{
    MenuTopFaceEx *face = TopFaceExWin;
    short slideTarget[3];
    int count;
    int i;
    int j;

    switch (face->state) {
    case 0:
        face->first = 0;
        face->color = 0xFFFFF0;
        count = 0;
        for (j = 1; j < 8; j++) {
            if (PartyFriendLockCheck(j, 1) != 0 && PartyAttackerCheck(j) == 0) {
                face->ids[count] = j;
                count++;
            }
        }
        face->count = count;
        if (count == 0) {
            face->first = 1;
        }
        for (j = face->first; j < 3; j++) {
            eRibbonSet(&face->ribbon[j].ribbon, 3);
            face->ribbon[j].ribbon.x = 0x280;
            face->ribbon[j].ribbon.y = ((j - face->first) << 5) + 0x100;
            face->ribbon[j].ribbon.color = face->color;
            face->ribbon[j].ribbon.width = 0xF8;
            face->ribbon[j].ribbon.height = 0x18;
        }
        for (j = 0; j < face->count; j++) {
            eSpriteSet(&face->sprite[j], MenuFaceEpidGet(face->ids[j], 1));
            face->sprite[j].color = face->color + 2;
        }
        for (i = 0; i < 3; i++) {
            eTagFontSet(&face->tag[i], msg_1_0036C360[i]);
            face->tag[i].color = face->color + 2;
        }
        for (i = 0; i < 2; i++) {
            eNumberSet(&face->number[i], 0);
            face->number[i].color = face->color + 2;
        }
        face->enabled = 1;
        face->state = 2;
    case 2:
        for (i = 0; i < 3; i++) {
            slideTarget[i] = 0x138;
        }
        if (MenuTopStateGet() >= 0x22 || MenuTopStateGet() < 0x20) {
            for (i = 0; i < 3; i++) {
                slideTarget[i] = 0x280;
            }
        }
        for (i = face->first; i < 3; i++) {
            MoveSlide(&face->ribbon[i].ribbon.x, &slideTarget[i], 5.0f);
            eRibbonMain(&face->ribbon[i].ribbon);
        }
        for (i = face->first; i < 3; i++) {
            face->tag[i].x = face->ribbon[i].ribbon.x + 0x42;
            face->tag[i].y = face->ribbon[i].ribbon.y + 4;
            eTagFontMain(&face->tag[i]);
        }
        for (j = 0; j < face->count; j++) {
            face->sprite[j].x = face->ribbon[0].ribbon.x + j * 20 + 0x4A;
            face->sprite[j].y = face->ribbon[0].ribbon.y;
            eSpriteMain(&face->sprite[j]);
        }
        face->time = *PartyTimeUpDate();
        PartyTimeDispChange(&face->time);
        face->number[0].x = face->ribbon[1].ribbon.x + 0x4A;
        face->number[0].digits = 2;
        face->number[0].y = face->ribbon[1].ribbon.y + 4;
        face->number[0].value = face->time.packed;
        eNumberMain(&face->number[0]);
        face->number[1].x = face->ribbon[2].ribbon.x + 0x4A;
        face->number[1].y = face->ribbon[2].ribbon.y + 4;
        face->number[1].width = 9;
        face->number[1].digits = 0xE;
        face->number[1].value = dataMoneyBoxChk();
        eNumberMain(&face->number[1]);
        break;
    }
}

void TopMenu(void)
{
    int modelIndex;
    int soundEffect;
    MenuTopWork *initialWork = &MenuWork;

    switch (initialWork->state) {
    case 0:
    {
        unsigned char *workEnd = MainMenuWorkEnd;
        unsigned char *window;

        TopStatusWin = (void *)workEnd;
        window = workEnd;
        workEnd += 0x1d84;
        window[0] = 0;
        TopInfo = (void *)workEnd;
        window = workEnd;
        workEnd += 0x350;
        window[0] = 0;
        TopMenuWin = (void *)workEnd;
        window = workEnd;
        workEnd += 0x794;
        window[4] = 0;
        TopFaceExWin = (void *)workEnd;
        workEnd[0] = 0;
        if ((MenuScenarioNo - 0x90u) < 9u) {
            MenuTopCommandLock = 0x332;
        } else {
            MenuTopCommandLock = 0;
        }
        if (((MenuTopPartyData *)PartyDataGet())->takeAgwsMask == 0) {
            MenuTopCommandLock |= 8;
        }
        if (PartyFriendLockCheck(3, 1) == 0) {
            MenuTopCommandLock |= 0x20;
        }
        if (MenuScenarioNo == 1) {
            MenuTopCommandLock |= 0x20;
        }
        initialWork->state = 0x20;
        break;
    }
    case 0x20:
        MenuWork.state = 0x21;
        MenuWork.countdown = 0x10;
        if (MenuCursorKeepCheck() == 0) {
            MenuWork.topCursor = 0;
        }
        /* fall through */
    case 0x21:
        if (MenuWork.countdown == 0) {
            MenuWork.topCursor = MenuSelectMove(MenuWork.topCursor, 9, 0);
            if ((PadData.half_2a & 0x20) != 0) {
                unsigned int commandBit =
                    1u << ((unsigned int)MenuTopCommand[MenuWork.topCursor] - 1u);
                if ((commandBit & ~(unsigned int)MenuTopCommandLock) != 0) {
                    soundEffect = 1;
                    MenuWork.state = 0xf0;
                } else {
                    soundEffect = 5;
                }
                xglSoundEffectNormalID(soundEffect, 0);
            } else if ((PadData.half_2a & 0x40) != 0) {
                xglSoundEffectNormalID(2, 0);
                MenuWork.state = 0xf0;
                MenuWork.topCursor = 9;
            }
        } else {
            MenuWork.countdown--;
        }
        break;
    case 0xf0:
        MenuWork.state = 0xf1;
        if (MenuWork.topCursor == 6) {
            MenuModelMemorySet(12);
            MenuModelMenuMotionLoad();
            for (modelIndex = 0; modelIndex < 3; modelIndex++) {
                if (((MenuTopPartyData *)PartyDataGet())
                        ->slot[modelIndex].id < 0x11) {
                    MenuModelCreate(&MenuModelOut[modelIndex],
                        ((MenuTopPartyData *)PartyDataGet())
                            ->slot[modelIndex].id);
                }
            }
        }
        MenuWork.countdown = 12;
        /* fall through */
    case 0xf1:
        if (MenuWork.countdown == 0) {
            MenuWork.state = 0xff;
        } else {
            MenuWork.countdown--;
        }
        break;
    case 0xff:
        ChangeTopLevel(MenuTopCommand[MenuWork.topCursor]);
        break;
    default:
        break;
    }

    TopStatusWinMain();
    TopInfoMain();
    TopMenuWinMain();
    TopFaceExWinMain();
    if (debug_flag != 0) {
        xglFontDebugPrintf(0, 0x38, topMenuDebugLabel);
        xglFontDebugHex(0, 0x40, MenuWork.topCursor, 2);
    }
}

#include "main/xgl_2.h"
#include "main/xgl_studio.h"
#include "main/xgl_task.h"

void xglLightIntensityAmbient(void *light, void *intensity);
void xglLightIntensityParallel(void *light, int index, void *intensity);
void xglLightDirection(void *light, int index, void *direction);
void xglLightCalcMatrix(void *light);
void nmlModelSetLight(Matrix4 intensity, Matrix4 direction);
void nmlModelSetTexture(const char *texture);
void nmlModelSetPlace(float matrix[4][4]);
void nmlModelEntry(int model);
void nmlModelFlush(void);
static const float pointerTargetRotationVector[4] = {1.57079637f, 0.0f, 0.0f, 1.0f};
static const MenuLightingSources menuLightingDefaults = {
    .value = {
        .ambient = {0.5f, 0.5f, 0.5f, 1.0f},
        .intensity = {
            {0.8f, 0.8f, 0.8f, 1.0f},
            {0.8f, 0.8f, 0.8f, 1.0f},
            {0.8f, 0.8f, 0.8f, 1.0f}
        },
        .direction = {
            {1.0f, 1.0f, 0.5f, 1.0f},
            {-1.0f, 1.0f, 0.0f, 1.0f},
            {0.0f, 1.0f, -1.0f, 1.0f}
        }
    }
};

void xglRotTransPersN(void *destination, Matrix4 *matrix,
                     const Vector4 *points, int count, int cameraId);
void endPrintDirectLine(MenuModelLinePoint *line);
extern const char *MenuTairetuModelXtxAddr;
extern int MenuTairetuModelLexAddr;

static void tskMenuTai(MenuTaiTask *task)
{
    Vector4 targetRotation;
    MenuModelLighting light;
    MenuLightingSources sources;
    Vector4 grid[24];
    Vector4 *gridPoint;
    MenuModelProjectedPoint projected[2][4][3];
    MenuModelLinePoint lines[98];
    MenuModelLinePoint *point;
    MenuModelScalarTransform *model;
    float targetScale;
    short currentCommand;
    int i;
    int j;
    int k;
    int tint;
    int shade;

    if (MenuWork.command == 12) {
        xglTaskWaitRemove(&task->link);
        return;
    }
    switch (task->state) {
    case 0:
        model = task->model;
        model->position.y = 0.0f;
        model->position.z = 0.0f;
        model->position.w = 1.0f;
        model->rotation.w = 1.0f;
        model->rotation.x = 0.0f;
        model->rotation.y = 0.0f;
        model->rotation.z = 0.0f;
        model->position.x = 0.0f;
        task->previousCommand = -1;
        task->state = 2;
        task->enabled = 0;
        task->initialized = 0;
    case 2:
        currentCommand = MenuWork.command;
        if (task->previousCommand != currentCommand) {
            task->initialized = 0;
            task->previousCommand = MenuWork.command;
            currentCommand = MenuWork.command;
        }
        if (currentCommand == 0) {
            if (!task->initialized) {
                task->model->position.x = 0.28f;
                task->model->position.y = 1.08f * (1 - task->index) + 0.1f;
                task->model->position.z = 0.0f;
                task->model->position.w = 1.0f;
                task->model->rotation.x = -1.04719758f;
                task->model->rotation.y = -0.196349546f;
                task->model->rotation.z = 0.0f;
                task->model->rotation.w = 1.0f;
                task->model->scale.x = 0.0f;
                task->model->scale.y = 0.077f;
                task->model->scale.z = 0.077f;
                task->model->scale.w = 1.0f;
                task->initialized = task->enabled = 1;
            }
            model = task->model;
            __builtin_memcpy(&targetRotation, pointerTargetRotationVector, sizeof(targetRotation));
            targetScale = 0.0f;
            if (MenuWork.state == 33) {
                targetRotation.x = 0.785398185f;
                targetRotation.y = 0.314159274f;
                targetRotation.z = 0.196349546f;
                targetScale = 0.077f;
            }
            model->scale.x += (targetScale - model->scale.x) * 0.2f;
            model->rotation.x += (targetRotation.x - model->rotation.x) * 0.1f;
            model->rotation.y += (targetRotation.y - model->rotation.y) * 0.1f;
            model->rotation.z += (targetRotation.z - model->rotation.z) * 0.1f;
        } else {
            task->enabled = 0;
        }
        if (task->enabled == 0) {
            break;
        }
        xglStudioGetCamera2(0)->active = 0;
        xglStudioGetCamera2(1)->active = 0;
        xglStudioGetCamera2(2)->active = 1;
        xglMatrixStackUnit();
        xglMatrixStackScale(&task->model->scale.x);
        xglMatrixStackTrans(&task->model->position.x);
        xglMatrixStackRotZ(task->model->rotation.z);
        xglMatrixStackRotY(task->model->rotation.y);
        xglMatrixStackRotX(task->model->rotation.x);
        xglMatrixStackSave(task->model->place);
        __builtin_memcpy(sources.words, menuLightingDefaults.words, sizeof(sources.words));
        xglLightIntensityAmbient(&light, sources.value.ambient);
        xglLightIntensityParallel(&light, 0, sources.value.intensity[0]);
        xglLightDirection(&light, 0, sources.value.direction[0]);
        xglLightIntensityParallel(&light, 1, sources.value.intensity[1]);
        xglLightDirection(&light, 1, sources.value.direction[1]);
        xglLightIntensityParallel(&light, 2, sources.value.intensity[2]);
        xglLightDirection(&light, 2, sources.value.direction[2]);
        xglLightCalcMatrix(&light);
        nmlModelSetLight(light.matrix[1], light.matrix[0]);
        nmlModelSetTexture(MenuTairetuModelXtxAddr);
        nmlModelSetPlace(task->model->place);
        nmlModelEntry(MenuTairetuModelLexAddr);
        nmlModelFlush();

        for (i = 0; i < 2; i++) {
            for (j = 0; j < 4; j++) {
                for (k = 0; k < 3; k++) {
                    gridPoint = &grid[j * 3 + i * 12 + k];
                    gridPoint->x = k * 0.1875f + -0.1875f;
                    gridPoint->y = i * 0.375f;
                    gridPoint->z = j * 0.1875f + -0.253125f;
                    gridPoint->w = 1.0f;
                }
            }
        }
        xglRotTransPersN(projected, &task->model->place, grid, 24, 2);
        point = lines;
        for (i = 0, tint = 64; i < 2; i++) {
            shade = tint - 128;
            for (j = 0; j < 4; j++) {
                for (k = 0; k < 4; k++) {
                    if (k != 3) {
                        point->point = projected[i][j][k];
                        point->r = tint + 64;
                        point->b = shade;
                        point->g = shade;
                        point->a = 128;
                    } else {
                        point->point.depth = 0;
                        if (i == 0) {
                            if (j == 0 || j == k) {
                                endPrintDirectLine(point - 3);
                            }
                        } else {
                            endPrintDirectLine(point - 3);
                        }
                    }
                    point++;
                }
            }
            for (j = 0; j < 3; j++) {
                for (k = 0; k < 5; k++) {
                    if (k != 4) {
                        point->point = projected[i][k][j];
                        point->r = 64;
                        point->b = 128;
                        point->g = 128;
                        point->a = 128;
                    } else {
                        point->point.depth = 0;
                        if (i == 0) {
                            if (j != 1) {
                                endPrintDirectLine(point - 4);
                            }
                        } else {
                            endPrintDirectLine(point - 4);
                        }
                    }
                    point++;
                }
            }
            tint -= 64;
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 3; j++) {
                for (k = 0; k < 3; k++) {
                    if (k != 2) {
                        point->point = projected[k][i][j];
                        point->r = 64;
                        point->b = 128;
                        point->g = 128;
                        point->a = 128;
                    } else {
                        point->point.depth = 0;
                        endPrintDirectLine(point - 2);
                    }
                    point++;
                }
            }
        }
        break;
    }
}

static void tskMenuTaiPointa(MenuPointaTask *task)
{
    MenuFormationPositions positions;
    Vector4 targetScale;
    MenuModelLighting light;
    MenuLightingSources sources;

    __builtin_memcpy(positions.words, menuFormationPositionTable.words, sizeof(positions.words));

    if (MenuWork.command == 12 ||
        MenuPointaGuardWord(task) == 0 ||
        MenuPointaGuardWord(task) >= 17) {
        xglTaskWaitRemove(&task->link);
        return;
    }
    switch (task->state) {
    case 0: {
        MenuModelTransform *pointer;

        pointer = task->pointer;
        (&pointer->position.vector)->x = 0.0f;
        (&pointer->position.vector)->y = 0.0f;
        (&pointer->position.vector)->z = 0.0f;
        (&pointer->position.vector)->w = 1.0f;
        pointer->rotation.x = 0.0f;
        pointer->rotation.y = 0.0f;
        pointer->rotation.z = 0.0f;
        pointer->rotation.w = 1.0f;
        task->previousCommand = -1;
        task->state = 2;
        task->initialized = 0;
        task->paletteIndex = 0;
        task->enabled = 0;
        break;
    }
    case 2: {
        MenuModelTransform *pointer;
        MenuModelTransform *scalePointer;
        short currentCommand;
        short previousCommand;

        previousCommand = task->previousCommand;
        currentCommand = MenuWork.command;
        if (previousCommand != currentCommand) {
            task->initialized = 0;
            task->previousCommand = MenuWork.command;
            currentCommand = MenuWork.command;
        }
        if (currentCommand == 0) {
            if (task->initialized == 0) {
                if (task->index != 3) {
                    MenuTopPartyData *party = (MenuTopPartyData *)PartyDataGet();

                    pointer = task->pointer;
                    pointer->position.vector =
                        positions.vector[party->slot[task->index].attackPosition - 1];
                    pointer->target = pointer->position;
                    pointer->rotation.x = 0.0f;
                    pointer->rotation.y = 0.0f;
                    pointer->rotation.z = 0.0f;
                    pointer->rotation.w = 1.0f;
                    pointer->scale.x = 0.0f;
                    pointer->scale.y = 1.0f;
                    pointer->scale.z = 0.0f;
                    pointer->scale.w = 1.0f;
                    task->selectedSlot = task->index;
                    task->formationIndex = ((MenuTopPartyData *)PartyDataGet())->slot[task->index].attackPosition - 1;
                    task->enabled = 1;
                    task->paletteIndex = 0;
                } else {
                    task->enabled = 0;
                }
                task->initialized = 1;
            }
            targetScale.x = targetScale.y = targetScale.z = targetScale.w = 1.0f;
            scalePointer = task->pointer;
            scalePointer->scale.x += (targetScale.x - scalePointer->scale.x) * pointerScaleRate;
            scalePointer->scale.y += (targetScale.y - scalePointer->scale.y) * pointerScaleRate;
            scalePointer->scale.z += (targetScale.z - scalePointer->scale.z) * pointerScaleRate;
        } else {
            task->enabled = 0;
        }
        if (task->enabled != 0) {
            xglStudioGetCamera2(0)->active = 0;
            xglStudioGetCamera2(1)->active = 0;
            xglStudioGetCamera2(2)->active = 1;
            xglMatrixStackUnit();
            xglMatrixStackLoad(task->formation[task->selectedSlot].place);
            xglMatrixStackTrans(&task->pointer->position.vector.x);
            xglMatrixStackScale(&task->pointer->scale.x);
            xglMatrixStackSave(task->pointer->place);
            sources = pointerLightingSources;
            {
                MenuModelVector ambient[3] = {
                    {.vector = {0.40499997f, 0.27f, 0.179999992f, 1.0f}},
                    {.vector = {0.3f, 0.5f, 0.1f, 1.0f}},
                    {.vector = {1.0f, 0.3f, 0.3f, 1.0f}}
                };
                xglLightIntensityAmbient(&light, &ambient[task->paletteIndex].vector.x);
            }
            xglLightIntensityParallel(&light, 0, sources.value.intensity[0]);
            xglLightDirection(&light, 0, sources.value.direction[0]);
            xglLightIntensityParallel(&light, 1, sources.value.intensity[1]);
            xglLightDirection(&light, 1, sources.value.direction[1]);
            xglLightIntensityParallel(&light, 2, sources.value.intensity[2]);
            xglLightDirection(&light, 2, sources.value.direction[2]);
            xglLightCalcMatrix(&light);
            nmlModelSetLight(light.matrix[1], light.matrix[0]);
            nmlModelSetTexture(MenuTairetuModelPointerXtxAddr);
            nmlModelSetPlace(task->pointer->place);
            nmlModelEntry((int)MenuTairetuModelPointerLexAddr);
            nmlModelFlush();
        }
        break;
    }
    default:
        break;
    }
}

unsigned char *MenuBackModelSet(unsigned char *allocationStart)
{
    StudioCamera *camera = xglStudioGetCamera2(2);
    int pointaIndex;
    int modelIndex;

    xglCameraInit(camera);
    camera->position.x = 0.0f;
    camera->position.y = 0.0f;
    camera->position.z = 4.0f;
    camera->position.w = 1.0f;
    camera->rotation.x = 0.0f;
    camera->rotation.y = 0.0f;
    camera->rotation.z = 0.0f;
    camera->rotation.w = 1.0f;
    camera->fov = menuBackModelFov;
    camera->state = 1;
    camera->active = 1;

    {
        MenuModelTransform **modelBaseSlot = &MenuVecMat;

        *modelBaseSlot = (MenuModelTransform *)
            (((unsigned int)allocationStart + 127) & ~127);
        allocationStart = (unsigned char *)(*modelBaseSlot + 16);
    }

    pointaIndex = 0;
    do {
        int (*pointaCallback)(XglTaskPrefix *) =
            (int (*)(XglTaskPrefix *))tskMenuTaiPointa;
        MenuPointaTask *task = (MenuPointaTask *)xglTaskEntryNext(
            MenuTask_XMX, pointaCallback,
            MenuTask_XMX ? MenuTask_XMX->active_tail : 0);
        if (task != 0) {
            MenuModelTransform *base;

            task->state = 0;
            base = MenuVecMat;
            task->index = pointaIndex;
            task->formation = &base[1];
            task->pointer = &base[pointaIndex + 5];
        }
    } while (++pointaIndex < 4);

    modelIndex = 0;
    do {
        MenuTopPartyData *party = (MenuTopPartyData *)PartyDataGet();
        MenuTaiTask *task;

        if (party->slot[modelIndex].id != 0) {
            party = (MenuTopPartyData *)PartyDataGet();
            if (party->slot[modelIndex].id >= 17) {
                break;
            }
            task = (MenuTaiTask *)xglTaskEntryNext(
                MenuTask_XMX, (int (*)(XglTaskPrefix *))tskMenuTai,
                MenuTask_XMX ? MenuTask_XMX->active_tail : 0);
            if (task != 0) {
                MenuModelTransform *base;

                task->state = 0;
                base = MenuVecMat;
                task->index = modelIndex;
                task->model = (MenuModelScalarTransform *)&base[modelIndex + 1];
            }
        } else {
            return allocationStart;
        }
        modelIndex++;
    } while (modelIndex < 3);
    return allocationStart;
}

void MenuStatusDisp(MenuStatusPanelWindow *window, MenuStatusPanel *work)
{
    int chrNo;
    MenuStatusExp *exp;
    int showStats;
    MenuStatusPara *para;
    int attack[4];
    int defense[4];
    int i;
    int left;
    int top;
    int nameY;
    int ribbonRow;
    int pass;

    showStats = 1;
    if (work->chrNo < 0) {
        chrNo = work->guestChrNo;
        if (work->chrNo == -2) {
            showStats = 0;
        }
    } else {
        chrNo = work->chrNo;
    }
    para = func_00A11108(chrNo, attack, defense);
    exp = func_A19210(chrNo);

    switch (window->state) {
    case 1:
        work->maxHp = para->maxHp;
        work->maxEp = para->maxEp;
        work->epMark = 0;
        work->hpMark = 0;
        eMessageSet(&work->name, MenuCharNameGet(chrNo));
        work->name.mode = 32;
        work->name.color = 0xFFFFFF;
        {
            MenuStatusTagInit init[8] = {
                { 4, 1, para->hp },
                { 4, 1, para->maxHp },
                { 3, 1, para->ep },
                { 3, 1, para->maxEp },
                { 2, 0, para->level },
                { 6, 1, exp->nextExp },
                { 1, 0, para->boost },
                { 6, 1, exp->exp },
            };
            int tag;

            for (tag = 0; tag < 8; tag++) {
                work->tag[tag].r = work->tag[tag].g = work->tag[tag].b = work->tag[tag].a = 128;
                work->tag[tag].value = init[tag].value;
                work->tag[tag].type = init[tag].type;
                work->tag[tag].mode = init[tag].mode;
                work->tag[tag].color = window->color + 2;
            }
        }
        work->shadeRgba[3][2] = 128;
        work->shadeRgba[2][2] = 128;
        work->shadeRgba[1][2] = 128;
        work->shadeRgba[0][2] = 128;
        work->shadeRgba[3][1] = 128;
        work->shadeRgba[2][1] = 128;
        work->shadeRgba[1][1] = 128;
        work->shadeRgba[0][1] = 128;
        work->shadeRgba[3][0] = 128;
        work->shadeRgba[2][0] = 128;
        work->shadeRgba[1][0] = 128;
        work->shadeRgba[0][0] = 128;
        work->shadeRgba[0][3] = 128;
        work->shadeRgba[1][3] = 128;
        work->shadeRgba[3][3] = 16;
        work->shadeRgba[2][3] = 0;
        work->shadeCorner[0].color = work->shadeCorner[1].color =
            work->shadeCorner[2].color = work->shadeCorner[3].color = window->color + 1;
        work->shadeTexture = work->chrNo + 0xC00;
        eRibbonSet(&work->ribbon, 0);
        for (i = 0; i < 6; i++) {
            switch (i) {
            case 0:
            case 1:
            case 2:
            case 3:
                work->ribbon.vertex[i].r = 32;
                work->ribbon.vertex[i].g = 52;
                work->ribbon.vertex[i].b = 52;
                break;
            case 4:
            case 5:
                work->ribbon.vertex[i].b = 0;
                work->ribbon.vertex[i].g = 0;
                work->ribbon.vertex[i].r = 0;
                break;
            }
            if (i >= 2 && i < 4) {
                work->ribbon.vertex[i].a = 32;
            } else {
                work->ribbon.vertex[i].a = 0;
            }
        }
        for (i = 0; i < 4; i++) {
            work->nameShade.vertex[i].b = 16;
            work->nameShade.vertex[i].g = 16;
            work->nameShade.vertex[i].r = 16;
            work->nameShade.vertex[i].color = window->color + 1;
        }
        work->nameShade.vertex[3].a = 0;
        work->nameShade.vertex[1].a = 128;
        work->nameShade.vertex[0].a = 128;
        work->nameShade.vertex[2].a = 0;
        work->nameShade.flags = 0;
        for (i = 0; i < 4; i++) {
            work->headerBand.vertex[i].b = 1;
            work->headerBand.vertex[i].g = 1;
            work->headerBand.vertex[i].r = 1;
            work->headerBand.vertex[i].color = window->color + 2;
        }
        work->headerBand.vertex[3].a = 0;
        work->headerBand.vertex[1].a = 128;
        work->headerBand.vertex[0].a = 128;
        work->headerBand.vertex[2].a = 0;
        work->headerBand.flags = 0;
        for (i = 0; i < 4; i++) {
            work->statsBand.vertex[i].b = 1;
            work->statsBand.vertex[i].g = 1;
            work->statsBand.vertex[i].r = 1;
            work->statsBand.vertex[i].color = window->color + 2;
        }
        work->statsBand.vertex[3].a = 0;
        work->statsBand.vertex[1].a = 128;
        work->statsBand.vertex[0].a = 128;
        work->statsBand.vertex[2].a = 0;
        work->statsBand.flags = 0;
        for (i = 0; i < 4; i++) {
            work->headerFill.vertex[i].b = 1;
            work->headerFill.vertex[i].g = 1;
            work->headerFill.vertex[i].r = 1;
            work->headerFill.vertex[i].a = 32;
            work->headerFill.vertex[i].color = window->color + 3;
        }
        work->headerFill.flags = 0;
        for (i = 0; i < 4; i++) {
            work->statsFill.vertex[i].b = 1;
            work->statsFill.vertex[i].g = 1;
            work->statsFill.vertex[i].r = 1;
            work->statsFill.vertex[i].a = 32;
            work->statsFill.vertex[i].color = window->color + 3;
        }
        work->statsFill.flags = 0;
        for (i = 0; i < 2; i++) {
            work->headerLine.vertex[i].b = 224;
            work->headerLine.vertex[i].g = 224;
            work->headerLine.vertex[i].r = 224;
            work->headerLine.vertex[i].a = 128;
            work->headerLine.vertex[i].color = window->color + 5;
        }
        work->headerLine.flags = 0;
        for (i = 0; i < 2; i++) {
            work->statsLine.vertex[i].b = 240;
            work->statsLine.vertex[i].g = 240;
            work->statsLine.vertex[i].r = 240;
            work->statsLine.vertex[i].a = 128;
            work->statsLine.vertex[i].color = window->color + 5;
        }
        work->statsLine.flags = 0;
        for (i = 0; i < 2; i++) {
            work->divider.vertex[i].b = 240;
            work->divider.vertex[i].g = 240;
            work->divider.vertex[i].r = 240;
            work->divider.vertex[i].a = 128;
            work->divider.vertex[i].color = window->color + 5;
        }
        work->divider.flags = 0;
        break;

    case 3:
    {
        short shown[2];

        left = window->x + work->offsetX;
        top = window->y + work->offsetY;
        work->box.x = window->x + 3;
        work->box.y = window->y + 3;
        work->box.color = window->color;
        work->box.width = window->width - 6;
        work->box.height = window->height - 6;
        endPrintExtFunc(work->box.color, 5, &work->box);

        ribbonRow = 1;
        if (work->hpMark != 2) {
            ribbonRow = (work->epMark == 2) ? 2 : 0;
        }
        if (ribbonRow != 0) {
            work->ribbon.x = left - (5 - ribbonRow) * 10 + 171;
            work->ribbon.y = top + ribbonRow * 16 + 15;
            work->ribbon.width = (5 - ribbonRow) * 20 + 23;
            work->ribbon.height = 13;
            work->ribbon.color = window->color;
            for (pass = 0; pass < 6; pass++) {
                eRibbonMain(&work->ribbon);
            }
        }

        work->name.x = left + 82;
        work->name.y = nameY = top + 3;
        work->name.color = window->color + 15;
        work->name.text = MenuCharNameGet(chrNo);
        eMessageDraw(&work->name);

        if (showStats) {
            if (work->chrNo >= 0) {
                work->label[0].x = left + 138;
            } else {
                work->label[0].x = left + 114;
            }
            work->label[0].color = window->color + 15;
            work->label[0].y = top + 29;
            /* Text: \x01Hp */
            work->label[0].text = "\001Hp";
            work->label[0].r = 112;
            work->label[0].g = 128;
            work->label[0].b = 128;
            work->label[0].a = 128;
            endPrintExtFunc(work->label[0].color, 7, &work->label[0]);
            eNumberSet(&work->number[0], 0);
            if (work->hpMark == 0) {
                shown[0] = para->hp;
                shown[1] = para->maxHp;
                work->number[0].format = 13;
            } else {
                shown[0] = para->maxHp;
                shown[1] = work->maxHp;
                work->number[0].format = 6;
            }
            if (work->chrNo >= 0) {
                work->number[0].x = left + 186;
            } else {
                work->number[0].x = left + 161;
            }
            work->number[0].y = top + 32;
            work->number[0].color = window->color + 15;
            work->number[0].value = *(MenuStatusValuePair *)shown;
            work->number[0].digits = 4;
            work->number[0].flag[0] = 0;
            if (work->hpMark != 0) {
                if (para->maxHp < work->maxHp) {
                    work->number[0].flag[1] = 2;
                } else if (work->maxHp < para->maxHp) {
                    work->number[0].flag[1] = 1;
                } else {
                    work->number[0].flag[1] = 0;
                }
            } else {
                work->number[0].flag[1] = 0;
            }
            work->number[0].flag[2] = 1;
            eNumberMain(&work->number[0]);
        }
        if (showStats) {
            if (work->chrNo >= 0) {
                work->label[1].x = left + 138;
            } else {
                work->label[1].x = left + 114;
            }
            work->label[1].color = window->color + 15;
            work->label[1].y = top + 45;
            /* Text: \x01Ep */
            work->label[1].text = "\001Ep";
            work->label[1].r = 112;
            work->label[1].g = 128;
            work->label[1].b = 128;
            work->label[1].a = 128;
            endPrintExtFunc(work->label[1].color, 7, &work->label[1]);
            eNumberSet(&work->number[1], 0);
            if (work->epMark == 0) {
                shown[0] = para->ep;
                shown[1] = para->maxEp;
                work->number[1].format = 4;
            } else {
                shown[0] = para->maxEp;
                shown[1] = work->maxEp;
                work->number[1].format = 6;
            }
            if (work->chrNo >= 0) {
                work->number[1].x = left + 186;
            } else {
                work->number[1].x = left + 161;
            }
            work->number[1].y = top + 48;
            work->number[1].color = window->color + 15;
            work->number[1].value = *(MenuStatusValuePair *)shown;
            work->number[1].digits = 2;
            work->number[1].flag[0] = 0;
            if (work->epMark != 0) {
                if (para->maxEp < work->maxEp) {
                    work->number[1].flag[1] = 2;
                } else if (work->maxEp < para->maxEp) {
                    work->number[1].flag[1] = 1;
                } else {
                    work->number[1].flag[1] = 0;
                }
            } else {
                work->number[1].flag[1] = 0;
            }
            work->number[1].flag[2] = 1;
            eNumberMain(&work->number[1]);
        }

        work->label[2].color = window->color + 15;
        work->label[2].x = left + 205;
        work->label[2].y = top + 7;
        /* Text: \x01Lv */
        work->label[2].text = "\001Lv";
        work->label[2].r = work->label[2].g = work->label[2].b = work->label[2].a = 128;
        endPrintExtFunc(work->label[2].color, 7, &work->label[2]);
        eNumberSet(&work->number[2], 0);
        work->number[2].x = work->label[2].x + 8;
        work->number[2].color = window->color + 15;
        work->number[2].y = top + 7;
        work->number[2].value.scalar = para->level;
        work->number[2].digits = 2;
        work->number[2].format = 0;
        eNumberMain(&work->number[2]);

        if (showStats) {
            /* Guest panels keep the party column here; the original still
             * tests chrNo (the lb survives) with both arms equal. */
            if (work->chrNo < 0) {
                work->label[3].x = left + 138;
            } else {
                work->label[3].x = left + 138;
            }
            work->label[3].color = window->color + 15;
            work->label[3].y = top + 63;
            /* Text: \x01Nextlv */
            work->label[3].text = "\001Nextlv";
            work->label[3].r = 112;
            work->label[3].g = 128;
            work->label[3].b = 128;
            work->label[3].a = 128;
            endPrintExtFunc(work->label[3].color, 7, &work->label[3]);
            eNumberSet(&work->number[3], 0);
            if (work->chrNo < 0) {
                work->number[3].x = left + 146;
            } else {
                work->number[3].x = left + 146;
            }
            work->number[3].y = top + 66;
            work->number[3].color = window->color + 15;
            if (para->level == 99) {
                work->number[3].value.scalar = -1;
            } else {
                work->number[3].value.scalar = exp->nextExp;
            }
            work->number[3].digits = 6;
            work->number[3].format = 1;
            eNumberMain(&work->number[3]);
        }
        if (showStats) {
            if (work->chrNo >= 0 && PartyLeaderCheck(chrNo)) {
                work->leader.x = left + 83;
                work->leader.y = top + 35;
                work->leader.color = window->color + 15;
                work->leader.spriteId = 811;
                endSpriteSet(&work->leader, 255);
                endPrintExtFunc(work->leader.color, 2, &work->leader);
            }
        }
        if (showStats) {
            work->label[5].y = top + 79;
            /* Text: \x01Exp */
            work->label[5].text = "\001Exp";
            work->label[5].color = window->color + 15;
            work->label[5].r = 112;
            work->label[5].g = 128;
            work->label[5].b = 128;
            work->label[5].a = 128;
            work->label[5].x = left + 138;
            endPrintExtFunc(work->label[5].color, 7, &work->label[5]);
            eNumberSet(&work->number[5], 0);
            work->number[5].x = left + 146;
            work->number[5].y = top + 82;
            work->number[5].color = window->color + 15;
            work->number[5].value.scalar = exp->exp;
            work->number[5].digits = 7;
            work->number[5].format = 1;
            eNumberMain(&work->number[5]);
        }

        work->nameShade.vertex[0].x = work->nameShade.vertex[1].x = left + 3;
        work->nameShade.vertex[0].y = work->nameShade.vertex[2].y = nameY;
        work->nameShade.vertex[2].x = work->nameShade.vertex[3].x = (float)left + 148.5f - 3.0f;
        work->nameShade.vertex[0].color = work->nameShade.vertex[1].color =
            work->nameShade.vertex[2].color = work->nameShade.vertex[3].color = window->color + 1;
        work->nameShade.vertex[1].y = work->nameShade.vertex[3].y = window->height + top - 3;
        endPrintExtFunc(work->nameShade.vertex[0].color, 3, &work->nameShade);

        work->headerBand.vertex[0].x = work->headerBand.vertex[1].x = left + 80;
        work->headerBand.vertex[2].x = work->headerBand.vertex[3].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo == -2) {
            work->headerBand.vertex[2].x = work->headerBand.vertex[3].x = left + 238;
        }
        work->headerBand.vertex[0].y = work->headerBand.vertex[2].y = window->y + 3;
        work->headerBand.vertex[0].color = work->headerBand.vertex[1].color =
            work->headerBand.vertex[2].color = work->headerBand.vertex[3].color = window->color + 2;
        work->headerBand.vertex[1].y = work->headerBand.vertex[3].y = work->headerBand.vertex[0].y + 23;
        endPrintExtFunc(work->headerBand.vertex[0].color, 3, &work->headerBand);

        work->headerFill.vertex[0].x = work->headerFill.vertex[1].x = left + 3;
        work->headerFill.vertex[2].x = work->headerFill.vertex[3].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo == -2) {
            work->headerFill.vertex[2].x = work->headerFill.vertex[3].x = left + 238;
        }
        work->headerFill.vertex[0].y = work->headerFill.vertex[2].y = window->y + 3;
        work->headerFill.vertex[0].color = work->headerFill.vertex[1].color =
            work->headerFill.vertex[2].color = work->headerFill.vertex[3].color = window->color + 4;
        work->headerFill.vertex[1].y = work->headerFill.vertex[3].y = work->headerFill.vertex[0].y + 23;
        endPrintExtFunc(work->headerFill.vertex[0].color, 3, &work->headerFill);

        work->statsBand.vertex[0].x = work->statsBand.vertex[1].x = left + 80;
        work->statsBand.vertex[2].x = work->statsBand.vertex[3].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo >= 0) {
            work->statsBand.vertex[0].y = work->statsBand.vertex[2].y = window->y + 62;
        } else {
            work->statsBand.vertex[2].x = work->statsBand.vertex[3].x = left + 256;
            work->statsBand.vertex[0].y = work->statsBand.vertex[2].y = window->y + 63;
        }
        work->statsBand.vertex[1].y = work->statsBand.vertex[3].y = window->y + 96;
        work->statsBand.vertex[0].color = work->statsBand.vertex[1].color =
            work->statsBand.vertex[2].color = work->statsBand.vertex[3].color = window->color + 2;
        endPrintExtFunc(work->statsBand.vertex[0].color, 3, &work->statsBand);

        work->headerLine.vertex[0].x = left + 3;
        work->headerLine.vertex[1].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo == -2) {
            work->headerLine.vertex[1].x = left + 238;
        }
        work->headerLine.vertex[1].y = work->headerLine.vertex[0].y = window->y + 26;
        work->headerLine.vertex[0].color = work->headerLine.vertex[1].color = window->color + 6;
        endPrintExtFunc(work->headerLine.vertex[0].color, 0, &work->headerLine);

        work->statsLine.vertex[0].x = left + 3;
        work->statsLine.vertex[1].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo >= 0) {
            work->statsLine.vertex[1].y = work->statsLine.vertex[0].y = window->y + 61;
        } else {
            if (work->chrNo == -1) {
                work->statsLine.vertex[1].x = left + 304;
            } else {
                work->statsLine.vertex[1].x = left + 238;
            }
            work->statsLine.vertex[1].y = work->statsLine.vertex[0].y = window->y + 61;
        }
        work->statsLine.vertex[0].color = work->statsLine.vertex[1].color = window->color + 6;
        endPrintExtFunc(work->statsLine.vertex[0].color, 0, &work->statsLine);

        work->divider.vertex[1].x = work->divider.vertex[0].x = left + 80;
        work->divider.vertex[0].color = work->divider.vertex[1].color = window->color + 6;
        work->divider.vertex[0].y = window->y + 3;
        work->divider.vertex[1].y = window->y + 96;
        endPrintExtFunc(work->divider.vertex[0].color, 0, &work->divider);

        if (work->chrNo >= 0 || showStats != 1) {
            work->statsFill.vertex[0].x = work->statsFill.vertex[1].x = left + 3;
            work->statsFill.vertex[2].x = work->statsFill.vertex[3].x = left + (window->width - work->offsetX) - 3;
            if (work->chrNo >= 0) {
                work->statsFill.vertex[0].y = work->statsFill.vertex[2].y = window->y + 61;
            } else if (work->chrNo == -2) {
                work->statsFill.vertex[2].x = work->statsFill.vertex[3].x = left + 238;
            } else {
                work->statsFill.vertex[0].y = work->statsFill.vertex[2].y = window->y + 63;
            }
            work->statsFill.vertex[1].y = work->statsFill.vertex[3].y = window->y + 96;
            work->statsFill.vertex[0].color = work->statsFill.vertex[1].color =
                work->statsFill.vertex[2].color = work->statsFill.vertex[3].color = window->color + 7;
            endPrintExtFunc(work->statsFill.vertex[0].color, 3, &work->statsFill);
        }

        work->face.x = left + 16;
        work->face.color = window->color + 15;
        work->face.y = nameY;
        work->face.spriteId = MenuFaceEpidGet(chrNo, 0);
        endSpriteSet(&work->face, 255);
        endPrintExtFunc(work->face.color, 2, &work->face);
        endPrintExtFunc(work->box.color, 6, 0);
        break;
    }
    }
}

int WindowTexAddrGet(int index);
void eMessageMain(void *message);

/* WindowDX discards this callback's return value. */
int MenuInfoWindow(MenuStatusPanelWindow *window, MenuTopInfoPanel *panel)
{
    const char *scan;
    int i;
    int color;
    int shade;

    switch (window->state) {
    case 0:
        break;
    case 1:
        eMessageSet(&panel->message, panel->text);
        panel->message.mode = 32;
        panel->blink = 0;
        panel->x = 0;
        panel->y = 0;
        panel->lineCount = 2;
        panel->message.lines = 2;
        panel->page = 0;
        panel->inputLock = 0;
        panel->text = 0;
        break;
    case 3:
        scan = panel->text;
        panel->lineCount = 1;
        if (scan == 0) {
            scan = topMenuEmptyText;
        }
        while (*scan != 0) {
            if (*scan == '\n') {
                panel->lineCount++;
            }
            scan++;
        }
        if (panel->inputLock == 0) {
            if (panel->lineCount >= 3) {
                if (PadData.half_2a & 2) {
                    panel->page ^= 1;
                }
            } else {
                panel->page = 0;
            }
        }

        panel->texture = WindowTexAddrGet(2);
        endPrintExtFunc(window->color, 14, &panel->texture);
        panel->header.image = 0xB01;
        panel->header.flag = 1;
        panel->header.mode = 0;
        for (i = 0; i < 4; i++) {
            panel->header.rgba[i].r = panel->header.rgba[i].g = panel->header.rgba[i].b = 128;
            if (i < 2) {
                panel->header.rgba[i].a = 128;
            } else {
                panel->header.rgba[i].a = 32;
            }
        }
        panel->header.vertex[0].x = panel->header.vertex[1].x = window->x + 24;
        panel->header.vertex[2].x = panel->header.vertex[3].x = window->x + 536;
        panel->header.vertex[0].y = panel->header.vertex[2].y = window->y - 5;
        panel->header.vertex[1].y = panel->header.vertex[3].y = window->y + 2;
        panel->header.vertex[0].color = panel->header.vertex[1].color =
            panel->header.vertex[2].color = panel->header.vertex[3].color = 0xFFFFFF;
        endPrintExtFunc(window->color, 8, &panel->header);

        panel->box.x = window->x + 3;
        color = window->color;
        panel->box.color = color;
        panel->box.y = window->y + 3;
        panel->box.width = window->width - 6;
        panel->box.height = window->height - 6;
        endPrintExtFunc(color, 5, &panel->box);

        panel->border.image = 0xB02;
        panel->border.mode = 0;
        panel->border.flag = 0;
        for (i = 0; i < 4; i++) {
            panel->border.rgba[i].r = panel->border.rgba[i].g = panel->border.rgba[i].b = 128;
            if ((i & 1) == 0) {
                panel->border.rgba[i].a = 128;
            } else {
                panel->border.rgba[i].a = 32;
            }
        }
        panel->border.vertex[0].color = 0xFFFFFF;
        panel->border.vertex[0].x = window->x + 24;
        panel->border.vertex[0].y = window->y + 3;
        endPrintExtFunc(window->color, 8, &panel->border);

        panel->footer.image = 0xB00;
        panel->footer.mode = 0;
        panel->footer.flag = 0;
        for (i = 0; i < 4; i++) {
            panel->footer.rgba[i].r = panel->footer.rgba[i].g = panel->footer.rgba[i].b = 128;
            panel->footer.rgba[i].a = 32;
        }
        panel->footer.vertex[0].x = 127;
        panel->footer.vertex[0].y = window->y + window->height - 51;
        panel->footer.vertex[0].color = window->color;
        endPrintExtFunc(window->color, 8, &panel->footer);
        endPrintExtFunc(panel->box.color, 6, 0);
        endPrintExtFunc(0, 15, 0);

        panel->clip.x = window->x + 3;
        color = window->color;
        panel->clip.color = color;
        panel->clip.y = window->y + 3;
        panel->clip.width = window->width - 6;
        panel->clip.height = window->height - 6;
        endPrintExtFunc(color, 101, &panel->clip);
        if (panel->inputLock == 0 && panel->lineCount >= 3) {
            panel->scroll.image = 0x10D;
            endSpriteSet(&panel->scroll, 255);
            shade = ((panel->blink * 4) & 24) + 72;
            panel->scroll.r = panel->scroll.g = panel->scroll.b = shade;
            color = panel->scroll.color = window->color;
            panel->scroll.x = window->x + window->width - 64;
            panel->scroll.y = window->y + window->height - 30;
            panel->blink++;
            endPrintExtFunc(color, 2, &panel->scroll);
        } else if (panel->inputLock == 0) {
            panel->blink = 0;
        }
        panel->message.x = window->x + panel->x + 35;
        panel->message.text = panel->text;
        panel->message.color = window->color + 2;
        panel->message.y = window->y + panel->y - panel->page * 48 + 3;
        eMessageMain(&panel->message);
        endPrintExtFunc(panel->clip.color, 102, 0);
        break;
    }
}

void eCursolSet(MenuSelectCursor *cursor, int mode);
void eCursolMain(MenuSelectCursor *cursor);
int xglFontGetStringWidth(const char *text);

/* WindowDX discards this callback's return value. */
int MenuSelectWindow(MenuStatusPanelWindow *window, MenuTopMenuPanel *panel)
{
    short rowShift;
    int i;
    MenuTopCommandEntry *entry;

    switch (window->state) {
    case 0:
        break;
    case 1: {
        MenuSelectCursor *cursor;

        cursor = &panel->marker;
        eCursolSet(cursor, 0);
        cursor->mode = 32;
        for (i = 0; i < 12; i++) {
            eMessageSet(&panel->message[i], 0);
            panel->message[i].mode = 32;
        }
        break;
    }
    case 3:
        switch (panel->rows) {
        case 0:
        case 2:
        {
            int bandLeft;
            int bandRight;
            int bandColor;
            int bandTop;
            int bandBottom;

            bandLeft = (unsigned short)window->x + 3;
            panel->cursorBand.vertex[0].x = panel->cursorBand.vertex[1].x = bandLeft;
            bandColor = window->color + 2;
            panel->cursorBand.vertex[0].color = panel->cursorBand.vertex[1].color =
                panel->cursorBand.vertex[2].color = panel->cursorBand.vertex[3].color = bandColor;
            bandRight = bandLeft + (unsigned short)window->width - 6;
            panel->cursorBand.vertex[2].x = panel->cursorBand.vertex[3].x = bandRight;
            panel->cursorBand.flags = 0;
            bandTop = (unsigned short)window->y + panel->cursor * 24 + 23;
            bandBottom = (unsigned short)window->y + panel->cursor * 24 + 27;
            panel->cursorBand.vertex[3].y = bandBottom;
            panel->cursorBand.vertex[0].y = panel->cursorBand.vertex[2].y = bandTop;
            panel->cursorBand.vertex[1].y = bandBottom;
            panel->cursorBand.vertex[0].b = panel->cursorBand.vertex[1].b =
                panel->cursorBand.vertex[2].b = panel->cursorBand.vertex[3].b = 16;
            panel->cursorBand.vertex[0].g = panel->cursorBand.vertex[1].g =
                panel->cursorBand.vertex[2].g = panel->cursorBand.vertex[3].g = 16;
            panel->cursorBand.vertex[1].r = panel->cursorBand.vertex[2].r = panel->cursorBand.vertex[3].r = 16;
            panel->cursorBand.vertex[0].r = 16;
            panel->cursorBand.vertex[0].a = panel->cursorBand.vertex[1].a = 64;
            panel->cursorBand.vertex[2].a = panel->cursorBand.vertex[3].a = 0;
            endPrintExtFunc(window->color, 3, &panel->cursorBand);
        }

        {
            int fillLeft;
            int fillRight;
            int fillTop;
            int fillBottom;

            fillLeft = (unsigned short)window->x + 3;
            panel->cursorFill.vertex[1].x = fillLeft;
            panel->cursorFill.vertex[0].x = fillLeft;
            panel->cursorFill.vertex[0].color = panel->cursorFill.vertex[1].color =
                panel->cursorFill.vertex[2].color = panel->cursorFill.vertex[3].color = window->color + 2;
            fillRight = fillLeft + (unsigned short)window->width - window->width / 4 - 6;
            panel->cursorFill.vertex[2].x = panel->cursorFill.vertex[3].x = fillRight;
            panel->cursorFill.flags = 0;
            fillTop = (unsigned short)window->y + panel->cursor * 24 + 3;
            fillBottom = (unsigned short)window->y + panel->cursor * 24 + 23;
            panel->cursorFill.vertex[0].y = panel->cursorFill.vertex[2].y = fillTop;
            panel->cursorFill.vertex[1].y = panel->cursorFill.vertex[3].y = fillBottom;
            panel->cursorFill.vertex[1].r = panel->cursorFill.vertex[2].r = panel->cursorFill.vertex[3].r = 32;
            panel->cursorFill.vertex[1].g = panel->cursorFill.vertex[2].g = panel->cursorFill.vertex[3].g = 52;
            panel->cursorFill.vertex[0].r = 32;
            panel->cursorFill.vertex[0].g = 52;
            panel->cursorFill.vertex[0].b = panel->cursorFill.vertex[1].b =
                panel->cursorFill.vertex[2].b = panel->cursorFill.vertex[3].b = 64;
            panel->cursorFill.vertex[0].a = panel->cursorFill.vertex[1].a = 64;
            panel->cursorFill.vertex[2].a = panel->cursorFill.vertex[3].a = 0;
            endPrintExtFunc(window->color, 3, &panel->cursorFill);
        }

        {
            int dividerTop;
            int dividerBottom;

            panel->divider.vertex[0].x = panel->divider.vertex[1].x = (unsigned short)window->x + 17;
            panel->divider.vertex[0].color = panel->divider.vertex[1].color = window->color + 2;
            panel->divider.flags = 0;
            dividerTop = (unsigned short)window->y + 3;
            panel->divider.vertex[0].y = dividerTop;
            dividerBottom = dividerTop + (unsigned short)window->height - 6;
            panel->divider.vertex[0].b = panel->divider.vertex[1].b = 160;
            panel->divider.vertex[0].g = panel->divider.vertex[1].g = 160;
            panel->divider.vertex[0].r = panel->divider.vertex[1].r = 160;
            panel->divider.vertex[0].a = panel->divider.vertex[1].a = 128;
            panel->divider.vertex[1].y = dividerBottom;
            endPrintExtFunc(window->color, 0, &panel->divider);
        }
            if (panel->rows == 0) {
                panel->message[0].x = (unsigned short)window->x + 19;
                panel->message[0].color = window->color + 6;
                panel->message[0].text = panel->text;
                panel->message[0].y = (unsigned short)window->y + 3;
                eMessageMain(&panel->message[0]);
            } else {
                entry = panel->commands;
                if (entry == 0) {
                    break;
                }
                i = 0;
                do {
                    if (entry->name == 0) {
                        break;
                    }
                    panel->message[i].x = (unsigned short)window->x + 19;
                    panel->message[i].color = window->color + 6;
                    panel->message[i].y = (unsigned short)window->y + i * 24 + 3;
                    if (entry->locked == 0) {
                        panel->message[i].b = 128;
                        panel->message[i].g = 128;
                        panel->message[i].r = 128;
                    } else {
                        panel->message[i].b = 64;
                        panel->message[i].g = 64;
                        panel->message[i].r = 64;
                    }
                    panel->message[i].text = entry->name;
                    entry++;
                    eMessageMain(&panel->message[i]);
                    i++;
                } while (i < 12);
            }
            panel->marker.x = (unsigned short)window->x + 3;
            panel->marker.y = (unsigned short)window->y + panel->cursor * 24 + 7;
            panel->marker.color = window->color + 3;
            eCursolMain(&panel->marker);
            break;
        case 1:
        case 3: {
            int width;

            width = xglFontGetStringWidth(panel->text);
            {
                int markerAvailable;
                int markerInset;

                rowShift = MenuSelectLayoutShift(panel);
                markerAvailable = window->width - width;
                markerInset = markerAvailable / 2;
                panel->marker.x = markerInset + (unsigned short)window->x - 16;
                panel->marker.y = (unsigned short)window->y + (panel->cursor * 24 + rowShift * 20) + 55;
                panel->marker.color = window->color + 2;
                eCursolMain(&panel->marker);
            }
            panel->message[0].x = (unsigned short)window->x + (window->width - width) / 2;
            panel->message[0].y = (unsigned short)window->y + rowShift * 20 + 51;
            panel->message[0].color = window->color + 2;
            panel->message[0].text = panel->text;
            eMessageMain(&panel->message[0]);
            width = xglFontGetStringWidth(panel->heading);
            panel->message[1].x = (unsigned short)window->x + (window->width - width) / 2;
            panel->message[1].y = (unsigned short)window->y + 3;
            panel->message[1].color = window->color + 2;
            panel->message[1].text = panel->heading;
            eMessageMain(&panel->message[1]);
            {
                int top;
                int color;
                int headingOriginX;
                int headingWidth;
                int headingRight;

                panel->headingLine.vertex[0].x = (unsigned short)window->x + 3;
                top = (unsigned short)window->y + rowShift * 9 + 39;
                color = window->color + 2;
                panel->headingLine.vertex[0].y = top;
                headingWidth = (unsigned short)window->width;
                headingOriginX = (unsigned short)window->x;
                headingRight = headingOriginX + headingWidth - 3;
                panel->headingLine.vertex[1].y = top;
                panel->headingLine.flags = 0;
                panel->headingLine.vertex[0].color = panel->headingLine.vertex[1].color = color;
                panel->headingLine.vertex[0].b = panel->headingLine.vertex[1].b = 160;
                panel->headingLine.vertex[0].g = panel->headingLine.vertex[1].g = 160;
                panel->headingLine.vertex[0].r = panel->headingLine.vertex[1].r = 160;
                panel->headingLine.vertex[0].a = panel->headingLine.vertex[1].a = 128;
                panel->headingLine.vertex[1].x = headingRight;
                endPrintExtFunc(color, 0, &panel->headingLine);
            }
            break;
        }
        }
        break;
    }
}

void MenuPasWindow(void) {

}

extern const char TopMenuFormationDescriptionText[48];
extern const char TopMenuItemsDescriptionText[80];
extern const char TopMenuUmnDescriptionText[48];
extern const char TopMenuGameOptionsDescriptionText[32];
extern const char TopMenuCommandListText[88];
extern const char TopMenuEtherDescriptionText[64];
extern const char TopMenuSkillsDescriptionText[56];
extern const char TopMenuCharactersDescriptionText[72];
extern const char TopMenuTechDescriptionText[32];
extern const char TopMenuAgwsDescriptionText[56];
extern const char TopMenuSaveLoadDescriptionText[16];

static const char * text[12] = { TopMenuItemsDescriptionText, TopMenuEtherDescriptionText, TopMenuTechDescriptionText, TopMenuSkillsDescriptionText, TopMenuCharactersDescriptionText, TopMenuAgwsDescriptionText, TopMenuFormationDescriptionText, TopMenuUmnDescriptionText, TopMenuGameOptionsDescriptionText, TopMenuSaveLoadDescriptionText, TopMenuCommandListText, TopMenuCommandListText };

const MenuFormationPositions menuFormationPositionTable = { .vector = { { .x = -0.10000000149011612f, .y = 0.0f, .z = 0.21000000834465027f, .w = 1.0f }, { .x = -0.10000000149011612f, .y = 0.0f, .z = 0.019999999552965164f, .w = 1.0f }, { .x = -0.10000000149011612f, .y = 0.0f, .z = -0.17000000178813934f, .w = 1.0f }, { .x = 0.10000000149011612f, .y = 0.0f, .z = 0.21000000834465027f, .w = 1.0f }, { .x = 0.10000000149011612f, .y = 0.0f, .z = 0.019999999552965164f, .w = 1.0f }, { .x = 0.10000000149011612f, .y = 0.0f, .z = -0.17000000178813934f, .w = 1.0f } } };

const MenuLightingSources pointerLightingSources = { .value = { .ambient = { 1.0f, 1.0f, 1.0f, 1.0f }, .intensity = { { 0.80000001192092896f, 0.80000001192092896f, 0.80000001192092896f, 1.0f }, { 0.80000001192092896f, 0.80000001192092896f, 0.80000001192092896f, 1.0f }, { 0.80000001192092896f, 0.80000001192092896f, 0.80000001192092896f, 1.0f } }, .direction = { { 1.0f, 1.0f, 0.5f, 1.0f }, { -1.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, -1.0f, 1.0f } } } };

const float pointerScaleRate = 0.20000000298023224f;

const float menuBackModelFov = 0.078000001609325409f;

char topMenuEmptyText[8] = "";

const char topMenuUmnText[8] = "U.M.N.";

const char topMenuSkillsText[8] = "Skills";

const char topMenuEtherText[8] = "Ether";

const char topMenuItemsText[8] = "Items";

char topMenuTitleText[8] = "Menu";

/* Text: \x01Money */
const char topMenuMoneyText[8] = "\001Money";

/* Text: \x01Time */
const char topMenuTimeText[8] = "\001Time";

char topMenuDebugLabel[8] = "topmemu";

MenuModelTransform * MenuVecMat = 0;

MenuTopStatusWin * TopStatusWin = 0;

MenuTopInfo * TopInfo = 0;

MenuTopMenuWin * TopMenuWin = 0;

MenuTopFaceEx * TopFaceExWin = 0;

/* Original objects of other TUs referenced by this TU's initialized DATA. */
extern const char TopMenuTechAttacksText[16];
extern const char TopMenuCharactersText[16];
extern const char TopMenuAgwsText[16];
extern const char TopMenuBattleFormationText[24];
extern const char TopMenuGameOptionsText[16];
extern const char TopMenuReserveText[24];

static const char * msg_1_0036C360[3] = { TopMenuReserveText, topMenuTimeText, topMenuMoneyText };

static MenuTopCommandEntry command_0[11] = { { .name = topMenuItemsText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = topMenuEtherText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = TopMenuTechAttacksText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = topMenuSkillsText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = TopMenuCharactersText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = TopMenuAgwsText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = TopMenuBattleFormationText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = topMenuUmnText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = TopMenuGameOptionsText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = topMenuEmptyText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = 0, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } } };

static unsigned char MenuTopCommand[10] = { 1, 2, 9, 10, 3, 4, 5, 6, 7, 12 };
