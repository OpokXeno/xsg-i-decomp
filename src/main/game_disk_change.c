#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/main/nonmatchings/game_disk_change", drawbg);

static void replace(u8 *str, int disk_number) {
    u8 *ch;

    ch = str;
    if (*ch != 0) {
        do {
            /* The placeholder is a fixed two-byte code read as one unit. */
            if ((*(u16 *)ch & 0xF0FF) == 0xB0A3) {
                ch[1] = (u8)(disk_number - 0x50);
            }
            ch += 2;
        } while (*ch != 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/game_disk_change", GameDiskChange);
