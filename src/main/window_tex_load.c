#include "common.h"
#include "shared.h"
#include "window_tex_load.h"

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", WindowTexLoad);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", WindowTexAddrGet);

extern unsigned char MenuWorkEndTop[];

void *MenuWorkEndGet(void) {
    memset(MenuWorkEndTop, 0, 0x10000U);
    return MenuWorkEndTop;
}

void MenuWorkEndCheck(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", ChangeTopLevel);

/* Twelve consecutive five-byte saved menu-selection records. */
extern unsigned char MenuKeepSelect[12 * 5];

void MenuKeepSelectReset(void)
{
    int i;

    for (i = 0; i < 12; i++)
        memset(&MenuKeepSelect[i * 5], 0, 5U);
}

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuSelectMove);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuSelectMove2);

/*
 * Advance a signed 16-bit position toward its signed 16-bit target.  The
 * float-derived movement is rounded through the resident double helpers.
 */
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

/* Advance toward the target by a fixed signed step without overshooting. */
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

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", menuCallback);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuFontLoad);

/*
 * The fourth argument of xglCdReadFile is the completion-callback slot that
 * xglCdReadFilePart stores at +0x0c of the read request (main:0x0021DFAC).
 * It selects a default callback for the small values it tests first - 0 takes
 * the global default, 1 takes xglCdDummyCallback (0x0021D698) and 2 takes
 * xglCdDefaultCallback (0x0021D690), main:0x0021DEFC..0x0021DF50 - and uses
 * any other value as the callback address itself.  The slot is an int, so a
 * caller that supplies its own callback passes its address as one.
 */
int MenuLoadFile(const char *name, void *buffer)
{
    return xglCdReadFile(name, buffer, 1, (int)menuCallback);
}

/*
 * MenuLoadCount is the original GLOBAL .sdata word used by the menu-load
 * status API.  Its detailed negative-status meanings remain unresolved; this
 * partial TU recovers only the two GLOBAL accessors below and does not define
 * the external storage.
 */

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

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuBibrationMain);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuCfTaikiPush);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuCfTaikiPop);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuBgTaskInit);

/*
 * The background fade color: only the r/g/b bytes MenuBgTaskMain reads and
 * writes are evidenced (main:0x002754b0..0x002754c4); the alpha byte at +3
 * stays unmodeled.
 */
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char unmodeled_03;
} MenuBgColor;

extern MenuBgColor MenuBgRgba;
extern XglTaskScheduler *MenuBgTask;

extern void MenuModelDirectSendZClear(void);
extern void endBackTexDraw(MenuBgColor *color);

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

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuBgTaskBreak);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuTairetuLoad);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MainMenu);

/*
 * dataUnitOrgGet's own 0x180-byte record (ov01 VA 0x00a191c0; main links it
 * only under the scaffold label func_A191C0, since dataUnitOrgGet itself is
 * not in this image -- see src/ov01/data_unit_org_get.c). Only the current
 * hp/ep halfwords this pair writes back are modelled.
 */
typedef struct {
    unsigned char unmodeled_00[0x34];
    short hp;  /* +0x34 */
    short ep;  /* +0x36 */
} UnitOrgHpEp;

/*
 * calcTotalParaMenu's recalculated parameter record (ov01 VA 0x00a11108,
 * scaffold label func_00A11108); src/main/menu_para_pt_rate_get.c models
 * this same record as CharParaData with its other evidenced offsets. Only
 * the recalculated max hp/ep halfwords this pair reads are modelled here.
 */
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

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", UmnkosmosSpecialSet);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", UmnInterface);

extern void UmnMain2(int menuId);
extern void UmnkosmosSpecialSet(void);
extern void endPrintDirectFrameCopy(XglPacket *packet, int width, int height);
extern void xglCdLoadOverlay(int overlayId);

/*
 * GameLoopState's 32-bit flags word at +0x10 (lw/sw, main:0x00276134); other
 * TUs model the same word under other names (e.g. src/main/script.h's
 * GameLoopFlagsPrefix). This TU only sets one bit of it.
 */
typedef struct {
    unsigned char unmodeled_00[0x10];
    int flags;  /* +0x10 */
} GameLoopFlagsView;

extern GameLoopFlagsView GameLoopState;

/* The bit UmnMailMain sets in GameLoopState's flags word while UMN is up. */
#define GAME_LOOP_FLAG_UMN 0x04000000

/*
 * sRender's screen-size halfwords: only +0x10/+0x12 are evidenced here (see
 * src/main/map_1.c's MapRenderState for the same object's callback member).
 */
typedef struct {
    unsigned char unmodeled_00[0x10];
    unsigned short width;   /* +0x10 */
    unsigned short height;  /* +0x12 */
} UmnRenderSize;

extern UmnRenderSize sRender;

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

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", UmnMailBoxSet);

unsigned char *UmnMailDataGet(int box_id)
{
    return &SaveData[SAVE_UMN_MAIL_DATA + box_id * 2];
}

/* The external data witness covers these 0x9c bytes; the spans around the
 * two evidenced bit sets remain unmodelled (see UmnDataBase). */

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
