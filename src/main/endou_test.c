#include "common.h"
#include "endou_test.h"

extern void xglSleep(void);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", EndouTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", TextTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", ShopTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", SeisanTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", ModelTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", BattleWindowTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", eNumberTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", eModelTestMain);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", eModelTest);

void ePrintTest(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/endou_test", eMessageTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", BgTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", TexturTest);

void UmnPrintTest(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/endou_test", UmnMailDispTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", e_test);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", ReLoadTest);

static void endCallback(int result, int amount)
{
    switch (result) {
    case 0: {
        const char *source = (const char *)amount;
        char *destination = callback_file_name;
        do {
            *destination = *source++;
        } while (((unsigned int)*destination++ << 24) != 0);
        break;
    }
    case 1:
        transfer_progress = I2F(amount) * progress_percent;
        break;
    case 2:
    case 3:
        if (result != 3)
            xglSleep();
        break;
    case -1: {
        for (;;) {
            xglFontDebugPrintf(8, 216, not_found_format, callback_file_name);
            xglSleep();
        }
    }
    case -2: {
        for (;;) {
            xglFontDebugPrintf(8, 216, read_error_format, callback_file_name);
            xglSleep();
        }
    }
    case 4:
        break;
    default:
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/endou_test", FileLoadTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", JpegTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", UmlTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", PauseMenuPagePartyDebugTakeAgws);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", PauseMenuPagePartyDebugLockParty);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", PauseMenuPagePartyDebugOutFriend);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", PauseMenuPagePartyDebugFriend);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", PauseMenuPagePartyDebugAttacker);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", PauseMenuPagePartyDebug);
