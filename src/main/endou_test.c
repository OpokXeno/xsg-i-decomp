#include "common.h"
#include "shared.h"
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

extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);
extern void *test_data;
extern unsigned char TestEnv_66[];
extern unsigned char WinTexEnv_65[];

/*
 * The context e_test's argument points to: only the VIF packet pointer at
 * +0x00 is evidenced here (main VA 0x002728f8, lw $4,0x0($16) before each
 * sceVif1Pk* call); the rest stays an unmodeled span (docs/naming.md).
 */
typedef struct ETestContext {
    XglPacket *packet;
} ETestContext;

extern void xglFontReloadTexture(ETestContext *context, int mode);

static void e_test(ETestContext *context)
{
    sceVif1PkCloseDirectHLCode(context->packet);
    sceVif1PkRef(context->packet, WinTexEnv_65, 7, 0, 0, 0);
    sceVif1PkRef(context->packet, (unsigned char *) test_data + 0x30, 0x4002, 0, 0, 0);
    sceVif1PkCnt(context->packet, 0);
    sceVif1PkOpenDirectHLCode(context->packet, 0);
    sceVif1PkAddDirectDataN(context->packet, TestEnv_66, 8);
    xglFontReloadTexture(context, 0);
}

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
