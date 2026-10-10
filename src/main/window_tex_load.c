#include "common.h"

#include "shared.h"

#include "window_tex_load.h"

/* MenuWorkEndGet clears and returns this entire 64 KiB work region. */

static unsigned char MenuWorkEndTop[0x10000];

typedef unsigned int Quadword __attribute__((mode(TI)));

typedef struct {
    unsigned char menu_state[4];
    unsigned char item_selection;
    signed char sub_selection;
    unsigned char unmodeled_06[10];
    Quadword reset_work[7];
} MenuWorkState;

MenuWorkState MenuWork = { 0 };

/* Twelve consecutive five-byte saved menu-selection records. */

unsigned char MenuKeepSelect[0x64] = { 0 };

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char unmodeled_03;
} MenuBgColor;

MenuBgColor MenuBgRgba = { 0 };

XglTaskScheduler *MenuBgTask = 0;

extern void MenuModelDirectSendZClear(void);

extern void endBackTexDraw(MenuBgColor *color);

typedef struct {
    unsigned char unmodeled_00[0x34];
    short hp;  /* +0x34 */
    short ep;  /* +0x36 */
} UnitOrgHpEp;

typedef struct {
    short maxHp;  /* +0x00 */
    short maxEp;  /* +0x02 */
} UnitMaxHpEp;

/*
 * The second argument here is not a real parameter of dataUnitOrgGet (which
 * takes only the character index): it is whatever value the loop-continue
 * flag below leaves in $a1, which the original compiler passed on for free
 * because the register already held it.
 */

extern UnitOrgHpEp *func_A191C0(int chrNo, int continueFlag);

extern UnitMaxHpEp *func_00A11108(int chrNo, int *attack, int *defense);

extern void UmnMain2(int menuId);

extern void UmnkosmosSpecialSet(void);

extern void endPrintDirectFrameCopy(XglPacket *packet, int width, int height);

int MenuModelOut[4] = { 0 };

int UmnSimulationNo = 0;

int MenuScenarioNo = 0;

int MenuModelWorkTop = 0;

void *UmnGunoDataBaseTop = 0;

short UmnKosmosSpecialBox[4] = { 0 };

XglTaskScheduler *MenuTask_XMX = 0;

unsigned char *MainMenuWorkEnd = 0;

int MenuLoadCount = 0;

unsigned char MenuBibrationCount = 0;

unsigned char MenuBibrationAct = 0;

unsigned char MenuBibrationSpeed = 0;

unsigned char MenuBibrationPad = 0;

extern void xglCdLoadOverlay(int overlayId);

/* The defining headers supply the flags and renderer callback layouts. */
#define GAME_LOOP_FLAG_UMN 0x04000000

/* MainMenu copies the whole light setup as aligned doublewords (ld/sd), so the
 * record carries eight-byte alignment. */
typedef union {
    struct {
        float ambient[4];
        float parallel[3][4];
        float direction[3][4];
    } light;
    unsigned long long words[14];
} MenuLightConfig;
extern const MenuLightConfig D_004C3240;
extern const char D_004C32B0[];

/* The external data witness covers these 0x9c bytes; the spans around the
 * two evidenced bit sets remain unmodelled (see UmnDataBase). */

static unsigned char Menu02Tex[0x40800];

int WindowTexLoad(unsigned char *buffer, unsigned int request)
{
    int texture_slot = request & 0xffff;
    unsigned int asynchronous = request >> 16;
    int result = 0;

    if (buffer) {
        MenuWinAddr = buffer;
        endPrintInit();
    }
    switch (texture_slot) {
    case 0:
        result = xglCdReadFile(file_name_0[0], MenuWinAddr, 0, 1);
        break;
    case 1:
        if (!asynchronous)
            result = xglCdReadFile(file_name_0[1], MenuWinAddr, 0, 1);
        else
            MenuLoadFile(file_name_0[1], MenuWinAddr);
        break;
    case 2:
        if (!asynchronous)
            xglCdReadFile(file_name_0[2], Menu02Tex, 0, 1);
        else
            MenuLoadFile(file_name_0[2], Menu02Tex);
        break;
    }
    return result;
}

unsigned char *WindowTexAddrGet(int texture_slot)
{
    switch (texture_slot) {
    case 0:
    case 1:
        return MenuWinAddr;
    case 2:
        return Menu02Tex;
    default:
        return 0;
    }
}

void *MenuWorkEndGet(void) {
    memset(MenuWorkEndTop, 0, 0x10000U);
    return MenuWorkEndTop;
}

void MenuWorkEndCheck(void)
{
}

void ChangeTopLevel(int top_level)
{
    int remaining = 6;
    int slot = 0;

    MenuWork.menu_state[1] = top_level;
    MenuWork.menu_state[3] = 0;
    MenuWork.menu_state[0] = 0;
    MenuWork.menu_state[2] = 0;
    do {
        remaining--;
        MenuWork.reset_work[slot] = 0;
        slot++;
    } while (remaining >= 0);
}

void MenuKeepSelectReset(void)
{
    int i;

    for (i = 0; i < 12; i++)
        memset(&MenuKeepSelect[i * 5], 0, 5U);
}

int MenuSelectMove(int selection, int count, int wrap)
{
    int movement = 0;

    if (count < 2)
        return selection;

    if (PadData[0].repeat == 0x1000) {
        if (selection != 0 || wrap ||
            PadData[0].trigger == PadData[0].repeat) {
            movement = -1;
        }
    }
    if (PadData[0].repeat == 0x4000) {
        if (selection != count - 1 || wrap ||
            PadData[0].trigger == PadData[0].repeat)
            movement = 1;
    }
    if (movement) {
        selection += movement;
        if (selection < 0)
            selection = count - 1;
        if (selection > count - 1)
            selection = 0;
        xglSoundEffectNormalID(3, 0);
    }
    return selection;
}

int MenuSelectMove2(int selection, int count)
{
    int movement = 0;

    if (count < 2)
        return selection;

    switch (PadData[0].repeat) {
    case 0x1000:
        movement = -2;
        break;
    case 0x8000:
        movement = -1;
        break;
    case 0x4000:
        movement = 2;
        break;
    case 0x2000:
        movement = 1;
        break;
    }
    if (movement) {
        selection += movement;
        if (selection < 0)
            selection = count - 1;
        if (selection > count - 1)
            selection = 0;
        xglSoundEffectNormalID(3, 0);
    }
    return selection;
}

void MoveSlide(short *current, short *target, float rate)
{
    short current_position = *current;
    short target_position = *target;
    int movement;

    if (current_position == target_position)
        return;

    if (target_position < current_position) {
        movement = (int)(((float)(current_position - target_position) / rate)
                         + 1.5);
        if (movement < 0)
            movement = 0;
    } else {
        movement = (int)(((float)(current_position - target_position) / rate)
                         - 1.5);
        if (movement > 0)
            movement = 0;
    }

    switch (movement) {
    case 0:
        current_position = target_position;
        break;
    default:
        current_position = current_position - movement;
        break;
    }

    *current = current_position;
    *target = target_position;
}

void MoveSlide2(short *current, short *target, short step)
{
    short current_position = *current;
    short target_position = *target;

    if (current_position == target_position)
        return;

    if (target_position < current_position) {
        current_position = current_position - step;
        if (current_position < target_position)
            current_position = target_position;
    } else {
        current_position = current_position + step;
        if (target_position < current_position)
            current_position = target_position;
    }

    *current = current_position;
    *target = target_position;
}

static void menuCallback(int event)
{
    switch (event) {
    case 1:
        ++MenuLoadCount;
        return;
    case 4:
        --MenuLoadCount;
        return;
    case -1:
        MenuLoadCount = -1;
        return;
    case -2:
        MenuLoadCount = -2;
        break;
    case 0:
    case 2:
    case 3:
        return;
    default:
        return;
    }
}

void MenuFontLoad(int original_font, int asynchronous)
{
    void (*callback)(int) = 0;

    if (asynchronous)
        callback = menuCallback;
    if (original_font) {
        original_font_no_1 = xglFontLoad(1, callback);
    } else {
        xglFontLoad(original_font_no_1, callback);
        return;
    }
}

void MenuLoadFile(const char *name, void *buffer)
{
    xglCdReadFile(name, buffer, 1, (int)menuCallback);
}

int MenuLoadSync(void)
{
    return MenuLoadCount;
}

void MenuLoadInit(void)
{
    MenuLoadCount = 0;
}

void MenuLoadEnd(void)
{
}

void MenuLoadCancel(void)
{
    if (MenuLoadSync() != 0) {
        MenuLoadCount = 0;
        xglCdReadCancel();
    }
}

void MenuBibrationSet(unsigned char pad, unsigned char act, unsigned char speed,
                      unsigned char count)
{
    MenuBibrationCount = count;
    MenuBibrationAct = act;
    MenuBibrationSpeed = speed;
    MenuBibrationPad = pad;
}

void MenuBibrationInit(void)
{
    MenuBibrationSet(0, 0, 0, 0);
}

void MenuBibrationMain(void)
{
    if (MenuBibrationCount) {
        MenuBibrationCount = MenuBibrationCount + 255;
        PadData[MenuBibrationPad].actuator[MenuBibrationAct] = MenuBibrationSpeed;
    }
}

void MenuCfTaikiPush(void)
{
    int index;

    for (index = 0; index < 8; ++index) {
        XglRenderFadeCallback task = sRender.final_packet_callback[index];
        sRender.final_packet_callback[index] = 0;
        MenuCfTaiki[index] = task;
    }
}

void MenuCfTaikiPop(void)
{
    int index;

    for (index = 0; index < 8; ++index)
        sRender.final_packet_callback[index] = MenuCfTaiki[index];
}

typedef struct MenuBgCameraSnapshot {
    u64 words[0x5f0 / 8];
} MenuBgCameraSnapshot;

unsigned char *MenuBgTaskInit(unsigned char *work, int camera_id)
{
    float camera_rotation_x;
    float camera_rotation_z;
    float camera_position_z;
    u64 *saved_end;
    StudioCamera *camera;
    u64 *camera_words;
    u64 *saved_words;
    u32 align_mask;
    MenuBgTaskParameters *param_base;
    unsigned char *color;
    int color_value;
    int color_index;

    align_mask = ~15U;
    MenuBgTask = (XglTaskScheduler *)(((u32)(work + 15)) & align_mask);
    work = (unsigned char *)xglTaskInitial(MenuBgTask, 16, 0);
    param_base = (MenuBgTaskParameters *)(((u32)(work + 127)) & ~127U);
    MenuBgParam = param_base;
    camera_words = (u64 *)xglStudioGetCamera2(camera_id);
    MenuBgKeepCamera =
        (u64 *)(((u32)((unsigned char *)param_base + 0x84f)) & align_mask);
    saved_words = MenuBgKeepCamera;
    saved_end = saved_words + 0xbe;
    *(MenuBgCameraSnapshot *)saved_words = *(MenuBgCameraSnapshot *)camera_words;
    MenuBgCameraId = camera_id;
    camera = xglStudioGetCamera2(camera_id);
    xglCameraInit(camera);
    if (xglStudioGetActiveCamera() == 0)
        camera->active = 1;

    MenuBgParam->camera_id = camera_id;
    MenuBgParam->initialization_values[0] = 0x1b;
    MenuBgParam->initialization_values[1] = 0x1b;
    MenuBgParam->initialization_values[2] = 0x19;
    MenuBgParam->initialization_values[3] = 0x80;
    camera_rotation_x = D_004D7D58;
    camera_rotation_z = D_004D7D5C;
    camera_position_z = D_004D7D60;
    MenuBgParam->rotation_x = camera_rotation_x;
    MenuBgParam->rotation_z = camera_rotation_z;
    MenuBgParam->camera_position_z = camera_position_z;
    MenuBgParam->reset_value = 0;
    tyaMenuBgEntry(MenuBgTask, MenuBgParam);

    color_value = -128;
    color = (unsigned char *)&MenuBgRgba;
    for (color_index = 2; color_index >= 0; --color_index)
        color[color_index] = color_value;
    return (unsigned char *)saved_end;
}

void MenuBgTaskMain(void)
{
    int level = MenuBgRgba.r + 0xF8;

    if (MenuBgRgba.r != 0) {
        MenuBgRgba.r = level;
        MenuBgRgba.g = level;
        MenuBgRgba.b = level;
    }
    endBackTexDraw(&MenuBgRgba);
    xglTaskExecute(MenuBgTask);
    MenuModelDirectSendZClear();
}

void MenuBgTaskBreak(void)
{
    u64 *saved_words;
    u64 *camera_words;

    MenuBgParam->tasks_finished = -1;
    MenuBgTaskMain();
    camera_words = (u64 *)xglStudioGetCamera2(MenuBgCameraId);
    saved_words = MenuBgKeepCamera;
    *(MenuBgCameraSnapshot *)camera_words = *(MenuBgCameraSnapshot *)saved_words;
}

unsigned char *MenuTairetuLoad(unsigned char *buffer, int load_flags)
{
    MenuModelPath filename;
    unsigned char *destination;

    (void)load_flags;
    MenuTairetuModelXtxAddr =
        (unsigned char *)(((unsigned int)buffer + 0x7f) & ~0x7f);
    filename = D_004C3220;
    filename.path[20] = '0';
    filename.path[22] = 'x';
    filename.path[23] = 't';

    MenuLoadFile(filename.path, MenuTairetuModelXtxAddr);

    destination = MenuTairetuModelXtxAddr + 0x3000;
    filename.path[20] = '1';
    MenuTairetuModelPointerXtxAddr = destination;
    MenuLoadFile(filename.path, destination);

    destination = MenuTairetuModelPointerXtxAddr + 0x800;
    filename.path[22] = 'l';
    filename.path[23] = 'e';
    MenuTairetuModelPointerLexAddr = destination;
    MenuLoadFile(filename.path, destination);

    destination = MenuTairetuModelPointerLexAddr + 0x1000;
    filename.path[20] = '0';
    MenuTairetuModelLexAddr = destination;
    MenuLoadFile(filename.path, destination);

    return MenuTairetuModelLexAddr + 0x800;
}

void MainMenu(void)
{
    MenuLightConfig lighting;
    void *light;
    int *out;
    int index;

    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.width << 5,
                           sRender.height << 5);
    GameLoopState.flags |= 0x04000000;
    xglSleep();
    mini_game_no = 0;
    UmnSimulationNo = 0;
    MenuDrillCall = 0;
    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.height << 5,
                           sRender.flip_base << 5);
    GameLoopState.flags |= 0x04000000;
    MenuGameDataPush();
    GameCFSoundMenuPurge(0);
    MenuLoadInit();
    MenuBibrationInit();
    xglStudioChange(0);
    light = xglStudioGetLight2();
    lighting = D_004C3240;
    xglLightIntensityAmbient(light, lighting.light.ambient);
    xglLightIntensityParallel(light, 0, lighting.light.parallel[0]);
    xglLightDirection(light, 0, lighting.light.direction[0]);
    xglLightIntensityParallel(light, 1, lighting.light.parallel[1]);
    xglLightDirection(light, 1, lighting.light.direction[1]);
    xglLightIntensityParallel(light, 2, lighting.light.parallel[2]);
    xglLightDirection(light, 2, lighting.light.direction[2]);
    MenuModelWorkTop = (int)GameResourceWorkAlloc(0x500000);
    MainMenuWorkEnd = (unsigned char *)MenuModelWorkTop + 0x480000;
    MainMenuWorkEnd = (unsigned char *)MenuModelInit((int)MainMenuWorkEnd);
    MenuModelMemoryInit((void *)MenuModelWorkTop, 0x480000);
    MenuModelMemorySet(10);
    out = MenuModelOut;
    index = 3;
    out += index;
    do {
        --index;
        *out = 0;
        --out;
    } while (index >= 0);
    MenuCfTaikiPush();
    MainMenuWorkEnd = MenuBgTaskInit(MainMenuWorkEnd, 3);
    MenuTask = (XglTaskScheduler *)MainMenuWorkEnd;
    MainMenuWorkEnd = xglTaskInitial(MainMenuWorkEnd, 0x20, 0);
    MenuTask_XMX = (XglTaskScheduler *)MainMenuWorkEnd;
    UmnGunoDataBaseTop = xglTaskInitial(MainMenuWorkEnd, 0x10, 0);
    MainMenuWorkEnd = (unsigned char *)UmnGunoDataBaseTop + 0xd00;
    MainMenuWorkEnd = MenuBackModelSet(MainMenuWorkEnd);
    memset(&MenuWork, 0, 0x80);
    endBackTexDraw((MenuBgColor *)-1);
    xglSleep();
    MainMenuWorkEnd = MenuMapExTextLoad(MainMenuWorkEnd, 1);
    MenuFontLoad(1, 1);
    WindowTexLoad(0, 0x10001);
    WindowTexLoad(0, 0x10002);
    MainMenuWorkEnd = MenuTairetuLoad(MainMenuWorkEnd, 1);
    xglSoundEffectNormalID(1, 0);
    while (MenuLoadSync()) {
        MenuBgTaskMain();
        xglSleep();
    }
    MenuScenarioNo = MenuScenarioNoGet();
    MenuEquipStealMaskCheck();
    while (MenuWork.menu_state[1] != 0xff) {
        GameSnapShotCheck();
        switch (MenuWork.menu_state[1]) {
        case 0: TopMenu(); break;
        case 1:
            mini_game_no = MenuItem();
            if (mini_game_no)
                ChangeTopLevel(0xff);
            break;
        case 2: MenuEther(); break;
        case 3: MenuCharactor(0); break;
        case 9: MenuTec(); break;
        case 10: MenuSkill(); break;
        case 4: MenuAgws(); break;
        case 5: MenuCharactor(1); break;
        case 6: UmnInterface(); ChangeTopLevel(0); break;
        case 7: MenuSystem(); break;
        case 12: ChangeTopLevel(0xff); break;
        default: ChangeTopLevel(0); break;
        }
        if (UmnSimulationNo) {
            ChangeTopLevel(0xff);
            break;
        }
        endPrintExtFunc(0, 100, 0);
        MenuBgTaskMain();
        nmlModelFlush();
        xglTaskExecute(MenuTask_XMX);
        xglTaskExecute(MenuTask);
        MenuModelMain();
        MenuBibrationMain();
        xglFontDebugPrintf(0, 0, D_004C32B0);
        xglFontDebugHex(0, 0x10, MenuWork.menu_state[1], 2);
        xglFontDebugHex(0, 0x18, MenuWork.item_selection, 2);
        xglFontDebugHex(0, 0x20, MenuWork.sub_selection, 2);
        xglFontDebugHex(0, 0x28, MenuWork.menu_state[3], 2);
        xglSleep();
    }
    MenuLoadCancel();
    MenuBgTaskBreak();
    xglTaskExecute(MenuTask_XMX);
    xglTaskExecute(MenuTask);
    MenuModelAllBreak();
    xglPadSetRepeat(0, 0xf00c, 8, 1);
    MenuCfTaikiPop();
    MenuFontLoad(0, 0);
    WindowTexLoad(0, 0);
    if (mini_game_no != 0 && mini_game_no != 15) {
        xglCullingIgnoreOff();
        nmlModelSendMenuEnd();
        GameStateRestoreCameraLight();
        Game_Data_Push();
        ACT_init();
        MenuCfTaikiPush();
        switch (mini_game_no) {
        case 12:
            xglCdLoadOverlay(12);
            func_00A01EA8();
            break;
        case 13:
            xglCdLoadOverlay(10);
            func_00A00338();
            break;
        case 14:
            xglCdLoadOverlay(11);
            func_00A00070();
            break;
        default:
            break;
        }
        MenuCfTaikiPop();
        xglRenderClearDepth();
        nmlModelInit();
        xglCdLoadOverlay(1);
        ACT_init();
        Game_Data_Pop();
    } else {
        if (mini_game_no == 15)
            MenuDrillCall = 1;
        if (!MenuDrillCall || !UmnSimulationNo) {
            GameLoopState.flags |= 0x40;
            GameResourceWorkReload();
            GameCfPlayerLoadResource(1);
            MenuGameDataPop();
            GameLoopState.flags &= ~0x40;
        }
    }
    GameLoopState.flags &= 0xfbffffff;
}

void CharactorAllRecovery(int unused, int continueFlag) {
    int attack[4];
    int defense[4];
    int chrNo;
    UnitMaxHpEp *recalculated;
    unsigned short hp;
    UnitOrgHpEp *org;

    chrNo = 1;
    do {
        org = func_A191C0(chrNo, continueFlag);
        recalculated = func_00A11108(chrNo, &attack[0], &defense[0]);
        hp = recalculated->maxHp;
        chrNo += 1;
        continueFlag = chrNo < 0xC;
        org->hp = (short) hp;
        org->ep = recalculated->maxEp;
    } while (continueFlag != 0);
}

void AgwsAllRecovery(int unused, int continueFlag) {
    int attack[4];
    int defense[4];
    int chrNo;
    UnitMaxHpEp *recalculated;
    unsigned short hp;
    UnitOrgHpEp *org;

    chrNo = 0x11;
    do {
        org = func_A191C0(chrNo, continueFlag);
        recalculated = func_00A11108(chrNo, &attack[0], &defense[0]);
        hp = recalculated->maxHp;
        chrNo += 1;
        continueFlag = chrNo < 0x21;
        org->hp = (short) hp;
        org->ep = recalculated->maxEp;
    } while (continueFlag != 0);
}

void UmnkosmosSpecialInit(void)
{
    memset(UmnKosmosSpecialBox, 0, sizeof(UmnKosmosSpecialBox));
}

void UmnkosmosSpecialSet(void)
{
    int index = 0;
    short *special = UmnKosmosSpecialBox;

    for (;;) {
        short item;

        if (index >= 4)
            return;
        ++index;
        item = *special++;
        if (item == 0)
            return;
        func_A19750(2, item);
    }
}

void UmnInterface(void)
{
    UmnTextPageMap text_pages;
    UmnGunoRecord *record;
    unsigned char default_character;
    int unit_id;

    endPrintExtFunc(0, 100, 0);
    MenuBgTaskMain();
    nmlModelFlush();
    xglTaskExecute(MenuTask_XMX);
    xglTaskExecute(MenuTask);
    MenuModelMain();
    xglSleep();

    text_pages = D_004C32F8;
    default_character = D_004DA810[0];
    record = UmnGunoDataBaseTop;

    for (unit_id = 0x22; unit_id < 0x3f; ++unit_id, ++record) {
        UmnUnitInitRecord *unit_init;
        UmnUnitTextRecord *unit_text;
        UmnTextLookup *text;
        short page;
        int text_id;

        memset(record, 0, sizeof(*record));
        unit_init = func_A19270(unit_id);
        unit_text = func_A1A698(unit_id);

        strcpy(record->unit_name, *func_A2C828(unit_id));
        record->unit_type = unit_init->unit_type;
        record->duplicate_unit_type = unit_init->unit_type;
        record->text_type = unit_init->text_type;
        record->property_a = unit_text->property_a;
        record->property_b = unit_text->property_b;
        record->property_c = unit_text->property_c;
        record->value = unit_text->value;
        record->unit_id = unit_text->unit_id;

        if (unit_text->name_id != 0) {
            page = text_pages.menu_page[unit_text->name_page];
            text_id = unit_text->name_id + ((int)page << 16);
            text = MenuTextGet(text_id);
            strcpy(record->name, text->text);
        } else {
            record->name[0] = default_character;
        }

        if (unit_text->description_id != 0) {
            page = text_pages.menu_page[unit_text->description_page];
            text_id = unit_text->description_id + ((int)page << 16);
            text = MenuTextGet(text_id);
            strcpy(record->description, text->text);
        } else {
            record->description[0] = default_character;
        }
    }

    UmnkosmosSpecialInit();
    MenuModelAllBreak();
    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.height << 5,
                            sRender.flip_base << 5);
    endBackTexDraw((MenuBgColor *)-1);
    xglSleep();
    xglSleep();
    xglCdLoadOverlay(2);
    xglSleep();
    UmnMain();
    xglSleep();
    xglCdLoadOverlay(1);
    xglSleep();
    UmnkosmosSpecialSet();
    MenuModelMemoryInit((void *)MenuModelWorkTop, 0x480000);
}

void UmnMailMain(int menuId)
{
    UmnkosmosSpecialInit();
    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.width << 5,
                             sRender.height << 5);
    GameLoopState.flags |= GAME_LOOP_FLAG_UMN;
    xglSleep();
    xglCdLoadOverlay(2);
    xglSleep();
    UmnMain2(menuId);
    xglCdLoadOverlay(1);
    xglSleep();
    UmnkosmosSpecialSet();
}

void UmnMailBoxSet(int mail_id)
{
    int index;
    unsigned char *mail = &SaveData[SAVE_UMN_MAIL_BOXES];
    unsigned char *entry;

    index = 0;
    if ((signed char)mail[0] == -1) {
        mail[0] = mail_id;
        return;
    }
next_box:
    if (++index >= 0x80)
        return;
    entry = &mail[index];
    if ((signed char)*entry != -1)
        goto next_box;
    *entry = mail_id;
}

unsigned char *UmnMailDataGet(int box_id)
{
    return &SaveData[SAVE_UMN_MAIL_DATA + box_id * 2];
}

void UmnDataBaseMonsterSet(int monster_id)
{
    UmnDataBase *state = (UmnDataBase *)UmnDataBaseStateData;
    int bit_number = monster_id - 0x22;

    if ((unsigned int)bit_number < 0x1d) {
        state->monster_discovered[bit_number / 8] |=
            (unsigned char)(1 << (bit_number % 8));
    }
}

int UmnDataBaseMonsterCheck(int monster_id)
{
    UmnDataBase *state = (UmnDataBase *)UmnDataBaseStateData;
    int bit_number = monster_id - 0x22;
    int result = 0;

    if ((unsigned int)bit_number < 0x1d) {
        result = ((unsigned int)state->monster_discovered[bit_number / 8] >>
                  (bit_number % 8)) & 1;
    }

    return result;
}

void UmnDataBaseAnalisisSet(int monster_id)
{
    UmnDataBase *state = (UmnDataBase *)UmnDataBaseStateData;
    int bit_number = monster_id - 0x22;

    if ((unsigned int)bit_number < 0x1d) {
        state->monster_analysed[bit_number / 8] |=
            (unsigned char)(1 << (bit_number % 8));
    }
}

int UmnDataBaseAnalisisCheck(int monster_id)
{
    UmnDataBase *state = (UmnDataBase *)UmnDataBaseStateData;
    int bit_number = monster_id - 0x22;
    int result = 0;

    if ((unsigned int)bit_number < 0x1d) {
        result = ((unsigned int)state->monster_analysed[bit_number / 8] >>
                  (bit_number % 8)) & 1;
    }

    return result;
}

int hen(void)
{
    return MenuModelInit(0);
}


