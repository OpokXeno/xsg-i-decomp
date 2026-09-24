#include "common.h"
#include "shared.h"

/*
 * MapUnit[] entries are game-wide unit records; only the fields this TU
 * touches are named. The rest of the struct is unmodeled padding recovered
 * as its exact byte span so the struct size and member offsets stay exact.
 */
typedef struct DrillMapUnit {
    u32 flags;
    void *update;
    unsigned char unmodeled_08[0x9c];
    u16 serial;
    unsigned char unmodeled_a6[0x25a];
} DrillMapUnit;

extern DrillMapUnit MapUnit[64];

INCLUDE_ASM("asm/main/nonmatchings/init_drill", InitDrill);

int CreateContainer(int, int);
extern unsigned char D_004CA960[];
int printf(const char *, ...);
void DrillClearContainer(void);

/*
 * Clears every existing drill container, then creates `count` new ones at
 * random serials in 0x7014..0x7027. A creation that fails (a full container
 * slot table) retries once with a fixed serial (0x7014..0x7017) chosen by
 * xglSRand() & 3.
 */
void SetContainer(int count)
{
    printf((char *) D_004CA960);
    DrillClearContainer();
    if (count > 0) {
        do {
            if (CreateContainer(-1, (xglSRand() % 20U) + 0x7014) == 0) {
                switch (xglSRand() & 3) {
                case 0:
                    CreateContainer(-1, 0x7014);
                    break;
                case 1:
                    CreateContainer(-1, 0x7015);
                    break;
                case 2:
                    CreateContainer(-1, 0x7016);
                    break;
                case 3:
                    CreateContainer(-1, 0x7017);
                    break;
                }
            }
        } while (--count != 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_drill", CreateContainer);

/*
 * Drill container serials run 0x7014..0x7027 (0x14 consecutive values); any
 * serial in that range, or any unit with the 0x100000 drill-container flag
 * bit set, is a drill container and gets cleared.
 */
void DrillClearContainer(void)
{
    int i;

    for (i = 0; i < 64; i++) {
        DrillMapUnit *unit = &MapUnit[i];

        if ((u32)unit->serial - 0x7014 < 0x14) {
            unit->serial = (u16)-1;
        } else if ((unit->flags & 0x100000) != 0) {
            unit->serial = (u16)-1;
        } else {
            continue;
        }
        unit->update = 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/init_drill", SetContainerParam);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", SetContainerItem);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", CalcPosContainer);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", MAP_updateUnitDrill);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillPowerOffFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillStandbyFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillZMoveFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillZStopFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillXMoveFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillCrashFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillReturnFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillCameraControlFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillCalcCameraPos);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillResetCameraPos);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillResetFlag);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", DrillHitCheck);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", HitCheckContainerPosSize);

INCLUDE_ASM("asm/main/nonmatchings/init_drill", HitCheckEnemy);
