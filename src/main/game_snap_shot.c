#include "common.h"

/* Points at the slot digit inside the snapshot file name (snapname + 0x1F). */
extern char *snapnameno;

int GameSnapShotNumber(int number) {
    if (number >= 0) {
        *snapnameno = number + '0';
    }
    return *snapnameno - '0';
}

void GameSnapShotCheck(void) {
}

INCLUDE_ASM("asm/main/nonmatchings/game_snap_shot", GameSnapShotExecute);

INCLUDE_ASM("asm/main/nonmatchings/game_snap_shot", GameSnapShotSaveThumbnail);

INCLUDE_ASM("asm/main/nonmatchings/game_snap_shot", GameSnapShotSave);

INCLUDE_ASM("asm/main/nonmatchings/game_snap_shot", GameSnapShotSaveFile);
