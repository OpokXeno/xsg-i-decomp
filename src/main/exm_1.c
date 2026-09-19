#include "common.h"
#include "exm_1.h"

static signed char GetLastWord(signed char *text)
{
    int index;

    index = 0;
    if (text != 0) {
        if (text[0] != 0 && !(text[0] & 0x80)) {
            do {
                index++;
            } while (text[index] != 0 && !(text[index] & 0x80));
        }
        if (index != 0) {
            return text[index - 1];
        }
        return 0;
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/exm_1", EXM_PresetHair);

INCLUDE_ASM("asm/main/nonmatchings/exm_1", GetHairTypeSelection);

INCLUDE_ASM("asm/main/nonmatchings/exm_1", EXM_CheckExCollision);

INCLUDE_ASM("asm/main/nonmatchings/exm_1", CheckSkirtCollisionSub);

INCLUDE_ASM("asm/main/nonmatchings/exm_1", EXM_CheckSkirtCollision);

INCLUDE_ASM("asm/main/nonmatchings/exm_1", AdjustJrCoatWidth);

INCLUDE_ASM("asm/main/nonmatchings/exm_1", EXM_CalcExMotion);

void EXM_InitMovedHair(Actor *actor)
{
    if (actor != 0) {
        actor->moved_hair_mode = 2;
        actor->moved_hair_delay = 0.5f;
        actor->moved_hair_sway[0] = 0.0f;
        actor->moved_hair_sway[1] = 0.0f;
        actor->moved_hair_sway[2] = 0.0f;
        actor->moved_hair_sway[3] = 0.0f;
    }
}

void EXM_OnMovedHair(Actor *actor)
{
    if (actor != 0) {
        actor->moved_hair_mode = 2;
    }
}

void EXM_OffMovedHair(Actor *actor)
{
    if (actor != 0) {
        actor->moved_hair_mode = 0;
    }
}

void EXM_SetDelayMovedHair(Actor *actor, float delay)
{
    if (actor != 0) {
        actor->moved_hair_delay = delay;
    }
}
