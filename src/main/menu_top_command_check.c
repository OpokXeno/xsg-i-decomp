#include "common.h"
#include "main/menu_top_command_check.h"
#include "menu_top_command_check.h"
#include "main/e_battle_win_open.h"
#include "main/xgl_2.h"
#include "main/xgl_studio.h"
#include "main/xgl_task.h"
#include "main/nml_model_set.h"
#include "main/open_close_main.h"
#include "main/xgl_camera.h"
#include "main/xgl_font.h"
#include "main/xgl_light.h"

/* TopMenu states (MenuWork.state). */
#define MENU_TOP_STATE_INIT 0
#define MENU_TOP_STATE_ENTER 0x20  /* reset the cursor, start the countdown */
#define MENU_TOP_STATE_SELECT 0x21 /* choose a command */
#define MENU_TOP_STATE_LEAVE 0xF0  /* start the leave countdown */
#define MENU_TOP_STATE_LEAVING 0xF1
#define MENU_TOP_STATE_DONE 0xFF   /* hand over to the chosen top level */

/* Top-level command rows. The first nine rows are commands; the tenth is the
 * cancel row (exit). */
#define MENU_TOP_COMMAND_COUNT 9
#define MENU_TOP_CANCEL_CURSOR 9
#define MENU_TOP_FORMATION_CURSOR 6 /* row that opens the formation screen */
#define MENU_TOP_ENTER_FRAMES 16
#define MENU_TOP_LEAVE_FRAMES 12

/* MenuTopCommandLock bits are 1 << (top level - 1). */
#define MENU_TOP_LOCK_AGWS (1 << (MENU_LEVEL_AGWS - 1))
#define MENU_TOP_LOCK_UMN (1 << (MENU_LEVEL_UMN - 1))
/* Locked during the scenario range MENU_TOP_LOCK_SCENARIO_FIRST + 0..8: ether,
 * formation, UMN, tec and skill. */
#define MENU_TOP_LOCK_SCENARIO_MASK 0x332
#define MENU_TOP_LOCK_SCENARIO_FIRST 0x90u
#define MENU_TOP_LOCK_SCENARIO_COUNT 9u
#define MENU_TOP_SCENARIO_PROLOGUE 1

/* Draw keys of the top-menu windows (see EW_ORDER_*). */
#define MENU_TOP_DRAW_KEY 0xFFFFF0
#define MENU_TOP_DRAW_KEY_STATUS 0xF000F0

/* Party member ids (PartyDataPrefix.attack_slots[].party_id); anything at or
 * above this has no model. */
#define MENU_PARTY_ID_LIMIT 17

/* State of the four window tasks (status, info, menu, face) and of the pointer
 * and model tasks: set up, then run. */
#define MENU_TOP_TASK_INIT 0
#define MENU_TOP_TASK_RUN 2

/* Command rows of the top menu window. */
#define MENU_TOP_ROW_COUNT 12

/* Slide targets (x, or y for the info bar) of the top-menu windows, shown and
 * hidden (see MoveSlide). */
#define MENU_TOP_STATUS_Y 28      /* first status window, then STRIDE apart */
#define MENU_TOP_STATUS_STRIDE 120
#define MENU_TOP_STATUS_X_SHOWN 16
#define MENU_TOP_STATUS_X_HIDDEN (-272)
#define MENU_TOP_INFO_Y_SHOWN 0x182
#define MENU_TOP_INFO_Y_HIDDEN 0x1E0
#define MENU_TOP_MENU_X_SHOWN 0x13C
#define MENU_TOP_MENU_X_HIDDEN 0x220
#define MENU_TOP_FACE_X_SHOWN 0x138
#define MENU_TOP_FACE_X_HIDDEN 0x280

/* Maximum character level. */
#define CHARACTER_LEVEL_MAX 99

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

extern const MenuFormationPositions menuFormationPositionTable;
extern const MenuLightingSources pointerLightingSources;
extern unsigned char *MenuTairetuModelPointerXtxAddr;
extern unsigned char *MenuTairetuModelPointerLexAddr;
extern const float pointerScaleRate;
extern MenuModelTransform *MenuVecMat;
extern const float menuBackModelFov;
extern XglTaskScheduler *MenuTask_XMX;
extern const char *MenuTairetuModelXtxAddr;
extern int MenuTairetuModelLexAddr;
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

/* Original objects of other TUs referenced by this TU's initialized DATA. */
extern const char TopMenuTechAttacksText[16];

extern const char TopMenuCharactersText[16];
extern const char TopMenuAgwsText[16];
extern const char TopMenuBattleFormationText[24];
extern const char TopMenuGameOptionsText[16];
extern const char TopMenuReserveText[24];

char *MenuTagTextGet(int index);
int MenuMaryIdChange(int id);
static void tskMenuTaiPointa(MenuPointaTask *task);
void xglLightIntensityAmbient(void *light, void *intensity);
void xglLightIntensityParallel(void *light, int index, void *intensity);
void xglLightDirection(void *light, int index, void *direction);
void nmlModelEntry(int model);
void xglRotTransPersN(void *destination, Matrix4 *matrix,
                     const Vector4 *points, int count, int cameraId);

void endPrintDirectLine(MenuModelLinePoint *line);
int WindowTexAddrGet(int index);
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

static const char * text[12] = { TopMenuItemsDescriptionText, TopMenuEtherDescriptionText, TopMenuTechDescriptionText, TopMenuSkillsDescriptionText, TopMenuCharactersDescriptionText, TopMenuAgwsDescriptionText, TopMenuFormationDescriptionText, TopMenuUmnDescriptionText, TopMenuGameOptionsDescriptionText, TopMenuSaveLoadDescriptionText, TopMenuCommandListText, TopMenuCommandListText };

static unsigned short MenuTopCommandLock;

static inline int MenuSelectLayoutShift(const MenuTopMenuPanel *panel)
{
    return panel->rows == 3 ? -1 : 0;
}

static inline int MenuTopStateGet(void)
{
    return MenuWork.state;
}

/* Packs a current and a maximum value into one status word. */
static inline MenuStatusValuePair MenuStatusEncodeValue(short current, short maximum)
{
    MenuStatusValuePair value;

    value.halfwords[0] = current;
    value.halfwords[1] = maximum;
    return value;
}

/* Index 3 reads past the three attack slots: the halfword at +0x3c, the first
 * halfword of the first technique record. */
static inline unsigned short MenuPointaGuardWord(MenuPointaTask *task)
{
    PartyDataPrefix *party = PartyDataGet();
    return party->attack_slots[task->index].party_id;
}

/* Returns nonzero when the top-level command is not locked. */
int MenuTopCommandCheck(int commandId)
{
    unsigned int commandBit = 1 << (commandId - 1);
    return (commandBit & ~MenuTopCommandLock) != 0;
}

/*
 * Creates one status window per party member on the first call, then slides
 * the windows in or out for the current top-menu state.
 */
void TopStatusWinMain(void)
{
    MenuTopStatusWin *status = TopStatusWin;
    short slideTarget;
    int chrNo;
    int i;
    int y;

    switch (status->state) {
    case MENU_TOP_TASK_INIT: {
        status->count = 0;
        status->color = MENU_TOP_DRAW_KEY_STATUS;
        y = MENU_TOP_STATUS_Y;
        i = 0;
        do {
            PartyDataPrefix *party;
            unsigned short partyId;

            party = PartyDataGet();
            partyId = party->attack_slots[i].party_id;
            chrNo = MenuMaryIdChange(partyId);
            if (chrNo != 0) {
                WindowDX *const window = &status->window[i];
                MenuStatusPanel *const panel = &status->panel[i];

                WindowDXSet(window);
                status->window[i].close_callback = MenuStatusDisp;
                window->x = MENU_TOP_STATUS_X_SHOWN;
                window->x = MENU_TOP_STATUS_X_HIDDEN;
                status->window[i].style = status->color;
                status->window[i].width = 240;
                status->window[i].height = 99;
                window->close_callback_arg = panel;
                status->window[i].y = y;
                status->window[i].tag_id = MenuTagTextGet(1);
                window->ribbon_initialized = 1;
                status->panel[i].enabled = 1;
                status->panel[i].offsetX = 0;
                status->panel[i].offsetY = 0;
                status->panel[i].chrNo = chrNo;
                window->state = WINDOWDX_STATE_OPEN;
                WindowDXMain(window);
                status->window[i].state = WINDOWDX_STATE_READY;
                status->count++;
            }
            ++i;
            y = MENU_TOP_STATUS_Y + MENU_TOP_STATUS_STRIDE * i;
        } while (i < 3);
        status->state = MENU_TOP_TASK_RUN;
    }
    case MENU_TOP_TASK_RUN:
        slideTarget = MENU_TOP_STATUS_X_SHOWN;
        /* The windows stay shown only during enter and select. */
        switch (MenuWork.state) {
        case MENU_TOP_STATE_ENTER:
        case MENU_TOP_STATE_SELECT:
            break;
        default:
            slideTarget = MENU_TOP_STATUS_X_HIDDEN;
            break;
        }
        for (i = 0; i < status->count; i++) {
            MoveSlide(&status->window[i].x, &slideTarget, 5.0f);
            WindowDXMain(&status->window[i]);
        }
        break;
    }
}

/*
 * Sets up the info bar on the first call, then slides it in during enter and
 * select with the help text of the highlighted command.
 */
void TopInfoMain(void)
{
    MenuTopInfo *info = TopInfo;
    short slideTarget;

    switch (info->state) {
    case MENU_TOP_TASK_INIT:
        WindowDXSet(&info->window);
        info->window.x = -0x10;
        info->window.y = MENU_TOP_INFO_Y_HIDDEN;
        info->window.width = 0x220;
        info->window.height = 0x36;
        info->window.style = MENU_TOP_DRAW_KEY_STATUS;
        info->window.close_callback = MenuInfoWindow;
        info->window.close_callback_arg = &info->panel;
        info->panel.x = 0;
        info->panel.y = 0;
        info->window.state = WINDOWDX_STATE_OPEN;
        WindowDXMain(&info->window);
        info->window.state = WINDOWDX_STATE_READY;
        info->slide = -1;
        info->enabled = 1;
        info->state = MENU_TOP_TASK_RUN;
        info->panel.flag = 0;
    case MENU_TOP_TASK_RUN:
        slideTarget = MENU_TOP_INFO_Y_SHOWN;
        switch (MenuWork.state) {
        case MENU_TOP_STATE_ENTER:
            info->slide = -1;
        case MENU_TOP_STATE_SELECT:
            info->panel.flag = 0;
            info->panel.y = 0;
            break;
        default:
            slideTarget = MENU_TOP_INFO_Y_HIDDEN;
            break;
        }
        if (MenuWork.topCursor < MENU_TOP_COMMAND_COUNT) {
            info->panel.text = text[MenuWork.topCursor];
        } else {
            info->panel.text = topMenuEmptyText;
        }
        MoveSlide(&info->window.y, &slideTarget, 5.0f);
        WindowDXMain(&info->window);
        break;
    }
}

/*
 * Sets up the command window on the first call, then slides it in during
 * enter and select and keeps its cursor on the top-menu cursor.
 */
void TopMenuWinMain(void)
{
    MenuTopMenuWin *menu = TopMenuWin;
    int menuState;
    short slideTarget;
    int i;

    switch (menu->state) {
    case MENU_TOP_TASK_INIT:
        menu->color = MENU_TOP_DRAW_KEY_STATUS;
        for (i = 0; i < MENU_TOP_COMMAND_COUNT; i++) {
            if (MenuTopCommandCheck(MenuTopCommand[i]) == 0) {
                command_0[i].locked = 1;
            } else {
                command_0[i].locked = 0;
            }
        }
        WindowDXSet(&menu->window);
        menu->window.x = MENU_TOP_MENU_X_HIDDEN;
        menu->window.y = 0x1C;
        menu->window.style = menu->color;
        menu->window.width = 0xC2;
        menu->window.height = 0xDE;
        menu->window.close_callback = MenuSelectWindow;
        menu->window.close_callback_arg = &menu->panel;
        menu->window.tag_id = topMenuTitleText;
        menu->panel.commands = command_0;
        menu->panel.cursor = 0;
        menu->panel.rows = 2;
        menu->window.state = WINDOWDX_STATE_OPEN;
        WindowDXMain(&menu->window);
        menu->window.state = WINDOWDX_STATE_READY;
        menu->enabled = 1;
        menu->state = MENU_TOP_TASK_RUN;
    case MENU_TOP_TASK_RUN:
        slideTarget = MENU_TOP_MENU_X_SHOWN;
        menuState = MenuWork.state;
        if (menuState < MENU_TOP_STATE_SELECT + 1) {
            if (menuState >= MENU_TOP_STATE_ENTER) {
                menu->panel.cursor = MenuWork.topCursor;
            } else {
                slideTarget = MENU_TOP_MENU_X_HIDDEN;
            }
        } else {
            slideTarget = MENU_TOP_MENU_X_HIDDEN;
        }
        MoveSlide(&menu->window.x, &slideTarget, 5.0f);
        WindowDXMain(&menu->window);
        break;
    }
}

/*
 * Sets up the party face panels and the play time and money readouts on the
 * first call, then slides and draws them.
 */
void TopFaceExWinMain(void)
{
    MenuTopFaceEx *face = TopFaceExWin;
    short slideTarget[3];
    int count;
    int i;
    int j;

    switch (face->state) {
    case MENU_TOP_TASK_INIT:
        face->first = 0;
        face->color = MENU_TOP_DRAW_KEY;
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
            eRibbonSet(&face->ribbon[j], 3);
            face->ribbon[j].x = MENU_TOP_FACE_X_HIDDEN;
            face->ribbon[j].y = ((j - face->first) << 5) + 0x100;
            face->ribbon[j].work = face->color;
            face->ribbon[j].width = 0xF8;
            face->ribbon[j].height = 0x18;
        }
        for (j = 0; j < face->count; j++) {
            eSpriteSet(&face->sprite[j], MenuFaceEpidGet(face->ids[j], 1));
            face->sprite[j].color = face->color + 2;
        }
        for (i = 0; i < 3; i++) {
            eTagFontSet(&face->tag[i], msg_1_0036C360[i]);
            face->tag[i].work = face->color + 2;
        }
        for (i = 0; i < 2; i++) {
            eNumberSet(&face->number[i], 0);
            face->number[i].work = face->color + 2;
        }
        face->enabled = 1;
        face->state = MENU_TOP_TASK_RUN;
    case MENU_TOP_TASK_RUN:
        for (i = 0; i < 3; i++) {
            slideTarget[i] = MENU_TOP_FACE_X_SHOWN;
        }
        if (MenuTopStateGet() >= MENU_TOP_STATE_SELECT + 1 || MenuTopStateGet() < MENU_TOP_STATE_ENTER) {
            for (i = 0; i < 3; i++) {
                slideTarget[i] = MENU_TOP_FACE_X_HIDDEN;
            }
        }
        for (i = face->first; i < 3; i++) {
            MoveSlide(&face->ribbon[i].x, &slideTarget[i], 5.0f);
            eRibbonMain(&face->ribbon[i]);
        }
        for (i = face->first; i < 3; i++) {
            face->tag[i].x = face->ribbon[i].x + 0x42;
            face->tag[i].y = face->ribbon[i].y + 4;
            eTagFontMain(&face->tag[i]);
        }
        for (j = 0; j < face->count; j++) {
            face->sprite[j].x = face->ribbon[0].x + j * 20 + 0x4A;
            face->sprite[j].y = face->ribbon[0].y;
            eSpriteMain(&face->sprite[j]);
        }
        /* Play time and money readouts. */
        face->time = *PartyTimeUpDate();
        PartyTimeDispChange(&face->time);
        face->number[0].x = face->ribbon[1].x + 0x4A;
        face->number[0].format = 2;
        face->number[0].y = face->ribbon[1].y + 4;
        face->number[0].value = face->time.packed;
        eNumberMain(&face->number[0]);
        face->number[1].x = face->ribbon[2].x + 0x4A;
        face->number[1].y = face->ribbon[2].y + 4;
        face->number[1].digits = 9;
        face->number[1].format = 0xE;
        face->number[1].value = dataMoneyBoxChk();
        eNumberMain(&face->number[1]);
        break;
    }
}

/*
 * Top menu state machine: sets up the windows and command locks on the first
 * call, runs command selection with its countdowns, then updates the windows.
 */
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
        /*
         * Build the command lock mask from the scenario, the AGWS roster and the UMN friend flag.
         */
        if ((MenuScenarioNo - MENU_TOP_LOCK_SCENARIO_FIRST) < MENU_TOP_LOCK_SCENARIO_COUNT) {
            MenuTopCommandLock = MENU_TOP_LOCK_SCENARIO_MASK;
        } else {
            MenuTopCommandLock = 0;
        }
        if ((PartyDataGet())->take_agws_mask == 0) {
            MenuTopCommandLock |= MENU_TOP_LOCK_AGWS;
        }
        if (PartyFriendLockCheck(3, 1) == 0) {
            MenuTopCommandLock |= MENU_TOP_LOCK_UMN;
        }
        if (MenuScenarioNo == MENU_TOP_SCENARIO_PROLOGUE) {
            MenuTopCommandLock |= MENU_TOP_LOCK_UMN;
        }
        initialWork->state = MENU_TOP_STATE_ENTER;
        break;
    }
    case MENU_TOP_STATE_ENTER:
        MenuWork.state = MENU_TOP_STATE_SELECT;
        MenuWork.countdown = MENU_TOP_ENTER_FRAMES;
        if (MenuCursorKeepCheck() == 0) {
            MenuWork.topCursor = 0;
        }
        /* fall through */
    case MENU_TOP_STATE_SELECT:
        if (MenuWork.countdown == 0) {
            MenuWork.topCursor = MenuSelectMove(MenuWork.topCursor, MENU_TOP_COMMAND_COUNT, 0);
            if ((PadData.half_2a & PAD_CIRCLE) != 0) {
                unsigned int commandBit =
                    1u << ((unsigned int)MenuTopCommand[MenuWork.topCursor] - 1u);
                if ((commandBit & ~(unsigned int)MenuTopCommandLock) != 0) {
                    soundEffect = SE_DECIDE;
                    MenuWork.state = MENU_TOP_STATE_LEAVE;
                } else {
                    soundEffect = SE_BUZZER;
                }
                xglSoundEffectNormalID(soundEffect, 0);
            } else if ((PadData.half_2a & PAD_CROSS) != 0) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuWork.state = MENU_TOP_STATE_LEAVE;
                MenuWork.topCursor = MENU_TOP_CANCEL_CURSOR;
            }
        } else {
            MenuWork.countdown--;
        }
        break;
    case MENU_TOP_STATE_LEAVE:
        MenuWork.state = MENU_TOP_STATE_LEAVING;
        /* The formation command builds the three party models before it leaves. */
        if (MenuWork.topCursor == MENU_TOP_FORMATION_CURSOR) {
            MenuModelMemorySet(12);
            MenuModelMenuMotionLoad();
            for (modelIndex = 0; modelIndex < 3; modelIndex++) {
                if ((PartyDataGet())
                        ->attack_slots[modelIndex].party_id < MENU_PARTY_ID_LIMIT) {
                    MenuModelCreate(&MenuModelOut[modelIndex],
                        (PartyDataGet())
                            ->attack_slots[modelIndex].party_id);
                }
            }
        }
        MenuWork.countdown = MENU_TOP_LEAVE_FRAMES;
        /* fall through */
    case MENU_TOP_STATE_LEAVING:
        if (MenuWork.countdown == 0) {
            MenuWork.state = MENU_TOP_STATE_DONE;
        } else {
            MenuWork.countdown--;
        }
        break;
    case MENU_TOP_STATE_DONE:
        ChangeTopLevel(MenuTopCommand[MenuWork.topCursor]);
        break;
    default:
        break;
    }

    /* Update the four windows for this frame. */
    TopStatusWinMain();
    TopInfoMain();
    TopMenuWinMain();
    TopFaceExWinMain();
    if (debug_flag != 0) {
        xglFontDebugPrintf(0, 0x38, topMenuDebugLabel);
        xglFontDebugHex(0, 0x40, MenuWork.topCursor, 2);
    }
}

/*
 * Task that animates the formation model while command 0 is active and draws
 * its wireframe grid; it removes itself when command 12 is set.
 */
static void tskMenuTai(MenuTaiTask *task)
{
    Vector4 targetRotation;
    StudioLight light;
    MenuLightingSources sources;
    Vector4 grid[24];
    Vector4 *gridPoint;
    MenuModelProjectedPoint projected[2][4][3];
    MenuModelLinePoint lines[98];
    MenuModelLinePoint *point;
    MenuModelTransform *model;
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
    case MENU_TOP_TASK_INIT:
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
        task->state = MENU_TOP_TASK_RUN;
        task->enabled = 0;
        task->initialized = 0;
    case MENU_TOP_TASK_RUN:
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
        nmlModelSetLight(&light.color_modifier_matrix, &light.normal_matrix);
        nmlModelSetTexture(MenuTairetuModelXtxAddr);
        nmlModelSetPlace(&task->model->place);
        nmlModelEntry(MenuTairetuModelLexAddr);
        nmlModelFlush();

        /* Lay out the 24 grid points, 2 x 4 x 3, in model space. */
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
        /* Emit the wireframe segments between the projected points. */
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

/*
 * Task that places the formation pointer on its party slot and draws it while
 * command 0 is active. Slot 3 has no formation position, so its pointer stays off.
 */
static void tskMenuTaiPointa(MenuPointaTask *task)
{
    MenuFormationPositions positions;
    Vector4 targetScale;
    StudioLight light;
    MenuLightingSources sources;

    __builtin_memcpy(positions.words, menuFormationPositionTable.words, sizeof(positions.words));

    if (MenuWork.command == 12 ||
        MenuPointaGuardWord(task) == 0 ||
        MenuPointaGuardWord(task) >= 17) {
        xglTaskWaitRemove(&task->link);
        return;
    }
    switch (task->state) {
    case MENU_TOP_TASK_INIT: {
        MenuModelTransform *pointer;

        pointer = task->pointer;
        pointer->position.x = 0.0f;
        pointer->position.y = 0.0f;
        pointer->position.z = 0.0f;
        pointer->position.w = 1.0f;
        pointer->rotation.x = 0.0f;
        pointer->rotation.y = 0.0f;
        pointer->rotation.z = 0.0f;
        pointer->rotation.w = 1.0f;
        task->previousCommand = -1;
        task->state = MENU_TOP_TASK_RUN;
        task->initialized = 0;
        task->paletteIndex = 0;
        task->enabled = 0;
        break;
    }
    case MENU_TOP_TASK_RUN: {
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
                /* Place the pointer on its slot's formation position. */
                if (task->index != 3) {
                    PartyDataPrefix *party = PartyDataGet();

                    pointer = task->pointer;
                    pointer->position =
                        positions.vector[party->attack_slots[task->index].attack_position - 1];
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
                    task->formationIndex = (PartyDataGet())->attack_slots[task->index].attack_position - 1;
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
            xglMatrixStackTrans(&task->pointer->position.x);
            xglMatrixStackScale(&task->pointer->scale.x);
            xglMatrixStackSave(task->pointer->place);
            sources = pointerLightingSources;
            {
                /* The ambient colour table's .rodata initializer is only
                 * 8-aligned, so it is not an AlignedVector4. */
                DoublewordAlignedVector4 ambient[3] = {
                    {0.40499997f, 0.27f, 0.179999992f, 1.0f},
                    {0.3f, 0.5f, 0.1f, 1.0f},
                    {1.0f, 0.3f, 0.3f, 1.0f}
                };
                xglLightIntensityAmbient(&light, &ambient[task->paletteIndex].x);
            }
            xglLightIntensityParallel(&light, 0, sources.value.intensity[0]);
            xglLightDirection(&light, 0, sources.value.direction[0]);
            xglLightIntensityParallel(&light, 1, sources.value.intensity[1]);
            xglLightDirection(&light, 1, sources.value.direction[1]);
            xglLightIntensityParallel(&light, 2, sources.value.intensity[2]);
            xglLightDirection(&light, 2, sources.value.direction[2]);
            xglLightCalcMatrix(&light);
            nmlModelSetLight(&light.color_modifier_matrix, &light.normal_matrix);
            nmlModelSetTexture(MenuTairetuModelPointerXtxAddr);
            nmlModelSetPlace(&task->pointer->place);
            nmlModelEntry((int)MenuTairetuModelPointerLexAddr);
            nmlModelFlush();
        }
        break;
    }
    default:
        break;
    }
}

/*
 * Sets up the background camera, places the formation and pointer models in
 * the storage at allocationStart, creates their tasks, and returns the first
 * address after that storage.
 */
unsigned char *MenuBackModelSet(unsigned char *allocationStart)
{
    StudioCamera *camera = xglStudioGetCamera2(2);
    int pointaIndex;
    int modelIndex;

    xglCameraInit(camera);
    (&camera->position.vector)->x = 0.0f;
    (&camera->position.vector)->y = 0.0f;
    (&camera->position.vector)->z = 4.0f;
    (&camera->position.vector)->w = 1.0f;
    (&camera->rotation.vector)->x = 0.0f;
    (&camera->rotation.vector)->y = 0.0f;
    (&camera->rotation.vector)->z = 0.0f;
    (&camera->rotation.vector)->w = 1.0f;
    camera->fov = menuBackModelFov;
    camera->state = 1;
    camera->active = 1;

    {
        MenuModelTransform **modelBaseSlot = &MenuVecMat;

        *modelBaseSlot = (MenuModelTransform *)
            (((unsigned int)allocationStart + 127) & ~127);
        allocationStart = (unsigned char *)(*modelBaseSlot + 16);
    }

    /* One pointer task per slot 0 to 3. */
    pointaIndex = 0;
    do {
        int (*pointaCallback)(XglTaskPrefix *) =
            (int (*)(XglTaskPrefix *))tskMenuTaiPointa;
        MenuPointaTask *task = (MenuPointaTask *)xglTaskEntryNext(
            MenuTask_XMX, pointaCallback,
            MenuTask_XMX ? MenuTask_XMX->active_tail : 0);
        if (task != 0) {
            MenuModelTransform *base;

            task->state = MENU_TOP_TASK_INIT;
            base = MenuVecMat;
            task->index = pointaIndex;
            task->formation = &base[1];
            task->pointer = &base[pointaIndex + 5];
        }
    } while (++pointaIndex < 4);

    /* One formation task per party slot, stopping at the first empty slot. */
    modelIndex = 0;
    do {
        PartyDataPrefix *party = PartyDataGet();
        MenuTaiTask *task;

        if (party->attack_slots[modelIndex].party_id != 0) {
            party = PartyDataGet();
            if (party->attack_slots[modelIndex].party_id >= 17) {
                break;
            }
            task = (MenuTaiTask *)xglTaskEntryNext(
                MenuTask_XMX, (int (*)(XglTaskPrefix *))tskMenuTai,
                MenuTask_XMX ? MenuTask_XMX->active_tail : 0);
            if (task != 0) {
                MenuModelTransform *base;

                task->state = MENU_TOP_TASK_INIT;
                base = MenuVecMat;
                task->index = modelIndex;
                task->model = &base[modelIndex + 1];
            }
        } else {
            return allocationStart;
        }
        modelIndex++;
    } while (modelIndex < 3);
    return allocationStart;
}

/*
 * Status panel callback: on open it sets the labels, shades and bands; when
 * ready it draws the name, the stat numbers and the portrait for the panel's
 * character. A chrNo of -2 marks a guest panel without stats.
 */
void MenuStatusDisp(WindowDX *window, void *arg)
{
    MenuStatusPanel *work = arg;
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
    case WINDOWDX_STATE_OPEN:
        work->maxHp = para->maxHp;
        work->maxEp = para->maxEp;
        work->epMark = 0;
        work->hpMark = 0;
        eMessageSet(&work->name, MenuCharNameGet(chrNo));
        work->name.mode = E_MODE_SHOW_START;
        work->name.draw_parameter = 0xFFFFFF;
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
                work->tag[tag].color = window->style + 2;
            }
        }
        /* Background shade colours. */
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
            work->shadeCorner[2].color = work->shadeCorner[3].color = window->style + 1;
        work->shadeTexture = work->chrNo + 0xC00;
        eRibbonSet(&work->ribbon, 0);
        for (i = 0; i < 6; i++) {
            switch (i) {
            case 0:
            case 1:
            case 2:
            case 3:
                work->ribbon.points[i].red = 32;
                work->ribbon.points[i].green = 52;
                work->ribbon.points[i].blue = 52;
                break;
            case 4:
            case 5:
                work->ribbon.points[i].blue = 0;
                work->ribbon.points[i].green = 0;
                work->ribbon.points[i].red = 0;
                break;
            }
            if (i >= 2 && i < 4) {
                work->ribbon.points[i].alpha = 32;
            } else {
                work->ribbon.points[i].alpha = 0;
            }
        }
        for (i = 0; i < 4; i++) {
            work->nameShade.vertex[i].b = 16;
            work->nameShade.vertex[i].g = 16;
            work->nameShade.vertex[i].r = 16;
            work->nameShade.vertex[i].color = window->style + 1;
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
            work->headerBand.vertex[i].color = window->style + 2;
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
            work->statsBand.vertex[i].color = window->style + 2;
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
            work->headerFill.vertex[i].color = window->style + 3;
        }
        work->headerFill.flags = 0;
        for (i = 0; i < 4; i++) {
            work->statsFill.vertex[i].b = 1;
            work->statsFill.vertex[i].g = 1;
            work->statsFill.vertex[i].r = 1;
            work->statsFill.vertex[i].a = 32;
            work->statsFill.vertex[i].color = window->style + 3;
        }
        work->statsFill.flags = 0;
        for (i = 0; i < 2; i++) {
            work->headerLine.vertex[i].b = 224;
            work->headerLine.vertex[i].g = 224;
            work->headerLine.vertex[i].r = 224;
            work->headerLine.vertex[i].a = 128;
            work->headerLine.vertex[i].color = window->style + 5;
        }
        work->headerLine.flags = 0;
        for (i = 0; i < 2; i++) {
            work->statsLine.vertex[i].b = 240;
            work->statsLine.vertex[i].g = 240;
            work->statsLine.vertex[i].r = 240;
            work->statsLine.vertex[i].a = 128;
            work->statsLine.vertex[i].color = window->style + 5;
        }
        work->statsLine.flags = 0;
        for (i = 0; i < 2; i++) {
            work->divider.vertex[i].b = 240;
            work->divider.vertex[i].g = 240;
            work->divider.vertex[i].r = 240;
            work->divider.vertex[i].a = 128;
            work->divider.vertex[i].color = window->style + 5;
        }
        work->divider.flags = 0;
        break;

    case WINDOWDX_STATE_READY:
    {
        /* Formats 4, 6 and 13 print the current/maximum pair that the number
         * record keeps in its one value word: the two halfwords sit in memory
         * and are read back as that word. */
        short shown[2];

        left = window->x + work->offsetX;
        top = window->y + work->offsetY;
        work->box.x = window->x + 3;
        work->box.y = window->y + 3;
        work->box.color = window->style;
        work->box.width = window->width - 6;
        work->box.height = window->height - 6;
        endPrintExtFunc(work->box.color, END_PRINT_CMD_SCISSOR_SET,&work->box);

        ribbonRow = 1;
        if (work->hpMark != 2) {
            ribbonRow = (work->epMark == 2) ? 2 : 0;
        }
        if (ribbonRow != 0) {
            work->ribbon.x = left - (5 - ribbonRow) * 10 + 171;
            work->ribbon.y = top + ribbonRow * 16 + 15;
            work->ribbon.width = (5 - ribbonRow) * 20 + 23;
            work->ribbon.height = 13;
            work->ribbon.work = window->style;
            for (pass = 0; pass < 6; pass++) {
                eRibbonMain(&work->ribbon);
            }
        }

        work->name.x = left + 82;
        work->name.y = nameY = top + 3;
        work->name.draw_parameter = window->style + 15;
        work->name.text = MenuCharNameGet(chrNo);
        eMessageDraw(&work->name);

        /* Stat labels and numbers; the chrNo -2 panel has none. */
        if (showStats) {
            if (work->chrNo >= 0) {
                work->label[0].x = left + 138;
            } else {
                work->label[0].x = left + 114;
            }
            work->label[0].color = window->style + 15;
            work->label[0].y = top + 29;
            /* Text: \x01Hp */
            work->label[0].text = "\001Hp";
            work->label[0].r = 112;
            work->label[0].g = 128;
            work->label[0].b = 128;
            work->label[0].a = 128;
            endPrintExtFunc(work->label[0].color, END_PRINT_CMD_TAG_FONT, &work->label[0]);
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
            work->number[0].work = window->style + 15;
            work->number[0].value = *(int *)shown;
            work->number[0].digits = 4;
            work->number[0].colorModeFirst = 0;
            if (work->hpMark != 0) {
                if (para->maxHp < work->maxHp) {
                    work->number[0].colorModeSecond = 2;
                } else if (work->maxHp < para->maxHp) {
                    work->number[0].colorModeSecond = 1;
                } else {
                    work->number[0].colorModeSecond = 0;
                }
            } else {
                work->number[0].colorModeSecond = 0;
            }
            work->number[0].alignment = 1;
            eNumberMain(&work->number[0]);
        }
        if (showStats) {
            if (work->chrNo >= 0) {
                work->label[1].x = left + 138;
            } else {
                work->label[1].x = left + 114;
            }
            work->label[1].color = window->style + 15;
            work->label[1].y = top + 45;
            /* Text: \x01Ep */
            work->label[1].text = "\001Ep";
            work->label[1].r = 112;
            work->label[1].g = 128;
            work->label[1].b = 128;
            work->label[1].a = 128;
            endPrintExtFunc(work->label[1].color, END_PRINT_CMD_TAG_FONT, &work->label[1]);
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
            work->number[1].work = window->style + 15;
            work->number[1].value = *(int *)shown;
            work->number[1].digits = 2;
            work->number[1].colorModeFirst = 0;
            if (work->epMark != 0) {
                if (para->maxEp < work->maxEp) {
                    work->number[1].colorModeSecond = 2;
                } else if (work->maxEp < para->maxEp) {
                    work->number[1].colorModeSecond = 1;
                } else {
                    work->number[1].colorModeSecond = 0;
                }
            } else {
                work->number[1].colorModeSecond = 0;
            }
            work->number[1].alignment = 1;
            eNumberMain(&work->number[1]);
        }

        work->label[2].color = window->style + 15;
        work->label[2].x = left + 205;
        work->label[2].y = top + 7;
        /* Text: \x01Lv */
        work->label[2].text = "\001Lv";
        work->label[2].r = work->label[2].g = work->label[2].b = work->label[2].a = 128;
        endPrintExtFunc(work->label[2].color, END_PRINT_CMD_TAG_FONT, &work->label[2]);
        eNumberSet(&work->number[2], 0);
        work->number[2].x = work->label[2].x + 8;
        work->number[2].work = window->style + 15;
        work->number[2].y = top + 7;
        work->number[2].value = para->level;
        work->number[2].digits = 2;
        work->number[2].format = 0;
        eNumberMain(&work->number[2]);

        if (showStats) {
            /* Both arms give the same x. */
            if (work->chrNo < 0) {
                work->label[3].x = left + 138;
            } else {
                work->label[3].x = left + 138;
            }
            work->label[3].color = window->style + 15;
            work->label[3].y = top + 63;
            /* Text: \x01Nextlv */
            work->label[3].text = "\001Nextlv";
            work->label[3].r = 112;
            work->label[3].g = 128;
            work->label[3].b = 128;
            work->label[3].a = 128;
            endPrintExtFunc(work->label[3].color, END_PRINT_CMD_TAG_FONT, &work->label[3]);
            eNumberSet(&work->number[3], 0);
            if (work->chrNo < 0) {
                work->number[3].x = left + 146;
            } else {
                work->number[3].x = left + 146;
            }
            work->number[3].y = top + 66;
            work->number[3].work = window->style + 15;
            if (para->level == 99) {
                work->number[3].value = -1;
            } else {
                work->number[3].value = exp->nextExp;
            }
            work->number[3].digits = 6;
            work->number[3].format = 1;
            eNumberMain(&work->number[3]);
        }
        if (showStats) {
            if (work->chrNo >= 0 && PartyLeaderCheck(chrNo)) {
                work->leader.x = left + 83;
                work->leader.y = top + 35;
                work->leader.color = window->style + 15;
                work->leader.spriteId = 811;
                endSpriteSet(&work->leader, 255);
                endPrintExtFunc(work->leader.color, END_PRINT_CMD_SPRITE,&work->leader);
            }
        }
        if (showStats) {
            work->label[5].y = top + 79;
            /* Text: \x01Exp */
            work->label[5].text = "\001Exp";
            work->label[5].color = window->style + 15;
            work->label[5].r = 112;
            work->label[5].g = 128;
            work->label[5].b = 128;
            work->label[5].a = 128;
            work->label[5].x = left + 138;
            endPrintExtFunc(work->label[5].color, END_PRINT_CMD_TAG_FONT, &work->label[5]);
            eNumberSet(&work->number[5], 0);
            work->number[5].x = left + 146;
            work->number[5].y = top + 82;
            work->number[5].work = window->style + 15;
            work->number[5].value = exp->exp;
            work->number[5].digits = 7;
            work->number[5].format = 1;
            eNumberMain(&work->number[5]);
        }

        /* Gradient bands and fills for the name, header and stats. */
        work->nameShade.vertex[0].x = work->nameShade.vertex[1].x = left + 3;
        work->nameShade.vertex[0].y = work->nameShade.vertex[2].y = nameY;
        work->nameShade.vertex[2].x = work->nameShade.vertex[3].x = (float)left + 148.5f - 3.0f;
        work->nameShade.vertex[0].color = work->nameShade.vertex[1].color =
            work->nameShade.vertex[2].color = work->nameShade.vertex[3].color = window->style + 1;
        work->nameShade.vertex[1].y = work->nameShade.vertex[3].y = window->height + top - 3;
        endPrintExtFunc(work->nameShade.vertex[0].color, END_PRINT_CMD_RIBBON,&work->nameShade);

        work->headerBand.vertex[0].x = work->headerBand.vertex[1].x = left + 80;
        work->headerBand.vertex[2].x = work->headerBand.vertex[3].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo == -2) {
            work->headerBand.vertex[2].x = work->headerBand.vertex[3].x = left + 238;
        }
        work->headerBand.vertex[0].y = work->headerBand.vertex[2].y = window->y + 3;
        work->headerBand.vertex[0].color = work->headerBand.vertex[1].color =
            work->headerBand.vertex[2].color = work->headerBand.vertex[3].color = window->style + 2;
        work->headerBand.vertex[1].y = work->headerBand.vertex[3].y = work->headerBand.vertex[0].y + 23;
        endPrintExtFunc(work->headerBand.vertex[0].color, END_PRINT_CMD_RIBBON,&work->headerBand);

        work->headerFill.vertex[0].x = work->headerFill.vertex[1].x = left + 3;
        work->headerFill.vertex[2].x = work->headerFill.vertex[3].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo == -2) {
            work->headerFill.vertex[2].x = work->headerFill.vertex[3].x = left + 238;
        }
        work->headerFill.vertex[0].y = work->headerFill.vertex[2].y = window->y + 3;
        work->headerFill.vertex[0].color = work->headerFill.vertex[1].color =
            work->headerFill.vertex[2].color = work->headerFill.vertex[3].color = window->style + 4;
        work->headerFill.vertex[1].y = work->headerFill.vertex[3].y = work->headerFill.vertex[0].y + 23;
        endPrintExtFunc(work->headerFill.vertex[0].color, END_PRINT_CMD_RIBBON,&work->headerFill);

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
            work->statsBand.vertex[2].color = work->statsBand.vertex[3].color = window->style + 2;
        endPrintExtFunc(work->statsBand.vertex[0].color, END_PRINT_CMD_RIBBON,&work->statsBand);

        work->headerLine.vertex[0].x = left + 3;
        work->headerLine.vertex[1].x = left + (window->width - work->offsetX) - 3;
        if (work->chrNo == -2) {
            work->headerLine.vertex[1].x = left + 238;
        }
        work->headerLine.vertex[1].y = work->headerLine.vertex[0].y = window->y + 26;
        work->headerLine.vertex[0].color = work->headerLine.vertex[1].color = window->style + 6;
        endPrintExtFunc(work->headerLine.vertex[0].color, END_PRINT_CMD_LINE,&work->headerLine);

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
        work->statsLine.vertex[0].color = work->statsLine.vertex[1].color = window->style + 6;
        endPrintExtFunc(work->statsLine.vertex[0].color, END_PRINT_CMD_LINE,&work->statsLine);

        work->divider.vertex[1].x = work->divider.vertex[0].x = left + 80;
        work->divider.vertex[0].color = work->divider.vertex[1].color = window->style + 6;
        work->divider.vertex[0].y = window->y + 3;
        work->divider.vertex[1].y = window->y + 96;
        endPrintExtFunc(work->divider.vertex[0].color, END_PRINT_CMD_LINE,&work->divider);

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
                work->statsFill.vertex[2].color = work->statsFill.vertex[3].color = window->style + 7;
            endPrintExtFunc(work->statsFill.vertex[0].color, END_PRINT_CMD_RIBBON,&work->statsFill);
        }

        /* The portrait is drawn last. */
        work->face.x = left + 16;
        work->face.color = window->style + 15;
        work->face.y = nameY;
        work->face.spriteId = MenuFaceEpidGet(chrNo, 0);
        endSpriteSet(&work->face, 255);
        endPrintExtFunc(work->face.color, END_PRINT_CMD_SPRITE,&work->face);
        endPrintExtFunc(work->box.color, END_PRINT_CMD_SCISSOR_RESET, 0);
        break;
    }
    }
}

/*
 * Info bar callback: on open it resets the text state; when ready it draws the
 * frame and the help text, with a scroll mark for multi-line text.
 * WindowDX discards this callback's return value.
 */
int MenuInfoWindow(WindowDX *window, void *arg)
{
    MenuTopInfoPanel *panel = arg;
    const char *scan;
    int i;
    int color;
    int shade;

    switch (window->state) {
    case WINDOWDX_STATE_CLOSED:
        break;
    case WINDOWDX_STATE_OPEN:
        eMessageSet(&panel->message, panel->text);
        panel->message.mode = E_MODE_SHOW_START;
        panel->blink = 0;
        panel->x = 0;
        panel->y = 0;
        panel->lineCount = 2;
        panel->message.page_available = 2;
        panel->page = 0;
        panel->inputLock = 0;
        panel->text = 0;
        break;
    case WINDOWDX_STATE_READY:
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
        endPrintExtFunc(window->style, END_PRINT_CMD_FONT_TEX_CHANGE,&panel->texture);
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
        endPrintExtFunc(window->style, END_PRINT_CMD_SPRITE_QUAD,&panel->header);

        panel->box.x = window->x + 3;
        color = window->style;
        panel->box.color = color;
        panel->box.y = window->y + 3;
        panel->box.width = window->width - 6;
        panel->box.height = window->height - 6;
        endPrintExtFunc(color, END_PRINT_CMD_SCISSOR_SET,&panel->box);

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
        endPrintExtFunc(window->style, END_PRINT_CMD_SPRITE_QUAD,&panel->border);

        panel->footer.image = 0xB00;
        panel->footer.mode = 0;
        panel->footer.flag = 0;
        for (i = 0; i < 4; i++) {
            panel->footer.rgba[i].r = panel->footer.rgba[i].g = panel->footer.rgba[i].b = 128;
            panel->footer.rgba[i].a = 32;
        }
        panel->footer.vertex[0].x = 127;
        panel->footer.vertex[0].y = window->y + window->height - 51;
        panel->footer.vertex[0].color = window->style;
        endPrintExtFunc(window->style, END_PRINT_CMD_SPRITE_QUAD,&panel->footer);
        endPrintExtFunc(panel->box.color, END_PRINT_CMD_SCISSOR_RESET, 0);
        endPrintExtFunc(0, END_PRINT_CMD_FONT_TEX_RELOAD, 0);

        panel->clip.x = window->x + 3;
        color = window->style;
        panel->clip.color = color;
        panel->clip.y = window->y + 3;
        panel->clip.width = window->width - 6;
        panel->clip.height = window->height - 6;
        endPrintExtFunc(color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET,&panel->clip);
        if (panel->inputLock == 0 && panel->lineCount >= 3) {
            panel->scroll.image = 0x10D;
            endSpriteSet(&panel->scroll, 255);
            shade = ((panel->blink * 4) & 24) + 72;
            panel->scroll.r = panel->scroll.g = panel->scroll.b = shade;
            color = panel->scroll.color = window->style;
            panel->scroll.x = window->x + window->width - 64;
            panel->scroll.y = window->y + window->height - 30;
            panel->blink++;
            endPrintExtFunc(color, END_PRINT_CMD_SPRITE,&panel->scroll);
        } else if (panel->inputLock == 0) {
            panel->blink = 0;
        }
        panel->message.x = window->x + panel->x + 35;
        panel->message.text = panel->text;
        panel->message.draw_parameter = window->style + 2;
        panel->message.y = window->y + panel->y - panel->page * 48 + 3;
        eMessageMain(&panel->message);
        endPrintExtFunc(panel->clip.color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET,0);
        break;
    }
}

/*
 * Command window callback: on open it resets the marker and the row messages;
 * when ready it draws the command rows (dimmed when locked) or, for the
 * single-line layouts, the text under its heading.
 * WindowDX discards this callback's return value.
 */
int MenuSelectWindow(WindowDX *window, void *arg)
{
    MenuTopMenuPanel *panel = arg;
    short rowShift;
    int i;
    MenuTopCommandEntry *entry;

    switch (window->state) {
    case WINDOWDX_STATE_CLOSED:
        break;
    case WINDOWDX_STATE_OPEN: {
        ECursolMainData *cursor;

        cursor = &panel->marker;
        eCursolSet(cursor, 0);
        cursor->state = E_MODE_SHOW_START;
        for (i = 0; i < MENU_TOP_ROW_COUNT; i++) {
            eMessageSet(&panel->message[i], 0);
            panel->message[i].mode = E_MODE_SHOW_START;
        }
        break;
    }
    case WINDOWDX_STATE_READY:
        /* Rows 0 and 2 draw the command list; rows 1 and 3 draw one text line with a heading. */
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
            bandColor = window->style + 2;
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
            endPrintExtFunc(window->style, END_PRINT_CMD_RIBBON,&panel->cursorBand);
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
                panel->cursorFill.vertex[2].color = panel->cursorFill.vertex[3].color = window->style + 2;
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
            endPrintExtFunc(window->style, END_PRINT_CMD_RIBBON,&panel->cursorFill);
        }

        {
            int dividerTop;
            int dividerBottom;

            panel->divider.vertex[0].x = panel->divider.vertex[1].x = (unsigned short)window->x + 17;
            panel->divider.vertex[0].color = panel->divider.vertex[1].color = window->style + 2;
            panel->divider.flags = 0;
            dividerTop = (unsigned short)window->y + 3;
            panel->divider.vertex[0].y = dividerTop;
            dividerBottom = dividerTop + (unsigned short)window->height - 6;
            panel->divider.vertex[0].b = panel->divider.vertex[1].b = 160;
            panel->divider.vertex[0].g = panel->divider.vertex[1].g = 160;
            panel->divider.vertex[0].r = panel->divider.vertex[1].r = 160;
            panel->divider.vertex[0].a = panel->divider.vertex[1].a = 128;
            panel->divider.vertex[1].y = dividerBottom;
            endPrintExtFunc(window->style, END_PRINT_CMD_LINE,&panel->divider);
        }
            if (panel->rows == 0) {
                panel->message[0].x = (unsigned short)window->x + 19;
                panel->message[0].draw_parameter = window->style + 6;
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
                    panel->message[i].draw_parameter = window->style + 6;
                    panel->message[i].y = (unsigned short)window->y + i * 24 + 3;
                    if (entry->locked == 0) {
                        panel->message[i].color_b = 128;
                        panel->message[i].color_g = 128;
                        panel->message[i].color_r = 128;
                    } else {
                        panel->message[i].color_b = 64;
                        panel->message[i].color_g = 64;
                        panel->message[i].color_r = 64;
                    }
                    panel->message[i].text = entry->name;
                    entry++;
                    eMessageMain(&panel->message[i]);
                    i++;
                } while (i < 12);
            }
            panel->marker.x = (unsigned short)window->x + 3;
            panel->marker.y = (unsigned short)window->y + panel->cursor * 24 + 7;
            panel->marker.work = window->style + 3;
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
                panel->marker.work = window->style + 2;
                eCursolMain(&panel->marker);
            }
            panel->message[0].x = (unsigned short)window->x + (window->width - width) / 2;
            panel->message[0].y = (unsigned short)window->y + rowShift * 20 + 51;
            panel->message[0].draw_parameter = window->style + 2;
            panel->message[0].text = panel->text;
            eMessageMain(&panel->message[0]);
            width = xglFontGetStringWidth(panel->heading);
            panel->message[1].x = (unsigned short)window->x + (window->width - width) / 2;
            panel->message[1].y = (unsigned short)window->y + 3;
            panel->message[1].draw_parameter = window->style + 2;
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
                color = window->style + 2;
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
                endPrintExtFunc(color, END_PRINT_CMD_LINE,&panel->headingLine);
            }
            break;
        }
        }
        break;
    }
}

void MenuPasWindow(WindowDX *window, void *arg) {

}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
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

static const char * msg_1_0036C360[3] = { TopMenuReserveText, topMenuTimeText, topMenuMoneyText };

static MenuTopCommandEntry command_0[11] = { { .name = topMenuItemsText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = topMenuEtherText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = TopMenuTechAttacksText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = topMenuSkillsText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = TopMenuCharactersText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = TopMenuAgwsText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = TopMenuBattleFormationText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = topMenuUmnText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = TopMenuGameOptionsText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = topMenuEmptyText, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } }, { .name = 0, .unmodeled_04 = 0, .locked = 0, .unmodeled_09 = { 0, 0, 0 } } };

static unsigned char MenuTopCommand[10] = { 1, 2, 9, 10, 3, 4, 5, 6, 7, 12 };
