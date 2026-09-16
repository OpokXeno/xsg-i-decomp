#include "common.h"
#include "shared.h"
#include "window_tex_load.h"

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", WindowTexLoad);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", WindowTexAddrGet);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuWorkEndGet);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuWorkEndCheck);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", ChangeTopLevel);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuKeepSelectReset);

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

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuLoadEnd);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuLoadCancel);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuBibrationSet);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuBibrationInit);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuBibrationMain);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuCfTaikiPush);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuCfTaikiPop);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuBgTaskInit);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuBgTaskMain);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuBgTaskBreak);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MenuTairetuLoad);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", MainMenu);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", CharactorAllRecovery);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", AgwsAllRecovery);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", UmnkosmosSpecialInit);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", UmnkosmosSpecialSet);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", UmnInterface);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", UmnMailMain);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", UmnMailBoxSet);

INCLUDE_ASM("asm/main/nonmatchings/window_tex_load", UmnMailDataGet);

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
