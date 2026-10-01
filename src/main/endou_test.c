#include "common.h"
#include "shared.h"
#include "endou_test.h"

extern void xglSleep(void);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", EndouTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", TextTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", ShopTest);

typedef struct PadDataButtonView {
    u8 unmodeled_00[0x28];
    u64 button_state;
} PadDataButtonView;

extern PadDataButtonView PadData;
extern void xglCdLoadOverlay(int overlayId);
extern void SeisanInit(void);
extern void SeisanMain(void);
extern void SeisanDisp(void);
extern void xglRenderClearFrame(void);

void SeisanTest(void)
{
    const int exit_chord = 0x08000100;

    xglCdLoadOverlay(1);
    SeisanInit();
    xglRenderClearFrame();

    while ((PadData.button_state & exit_chord) != exit_chord) {
        SeisanMain();
        SeisanDisp();
        xglSleep();
    }
}

INCLUDE_ASM("asm/main/nonmatchings/endou_test", ModelTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", BattleWindowTest);

INCLUDE_ASM("asm/main/nonmatchings/endou_test", eNumberTest);

typedef struct EModelTestState {
    unsigned char unmodeled_00[0x11];
    unsigned char isReady;
    unsigned char unmodeled_12[1];
    unsigned char extFuncFlag;
} EModelTestState;
extern void MenuModelUnitOpen(void *unit, int drawType);
extern void MenuModelControl(void *unit);

void eModelTestMain(EModelTestState *unit)
{
    if (unit->isReady == 0) {
        return;
    }

    if (unit->extFuncFlag == 0) {
        MenuModelUnitOpen(unit, 2);
        unit->extFuncFlag = 20;
    }

    MenuModelControl(unit);
}

extern void ACT_init(void);
extern void MenuLoadInit(void);
extern int MenuModelInit(int work_start);
extern void MenuModelMemoryInit(void *base, int size);
extern void MenuModelMemorySet(int count);
extern void MenuModelCreate(int *model, int character_id);
extern void MenuModelExtFuncSet(int model,
                                void (*callback)(EModelTestState *),
                                int argument);
extern void MenuModelMain(void);
extern const char D_004C2AE8[];

void eModelTest(void)
{
    int model_id;

    xglCdLoadOverlay(1);
    ACT_init();
    xglRenderClearFrame();
    xglSleep();
    MenuLoadInit();
    MenuModelInit(0x01000000);
    MenuModelMemoryInit((void *)0x01100000, 0x00500000);
    MenuModelMemorySet(0);
    MenuModelCreate(&model_id, 29);
    MenuModelExtFuncSet(model_id, eModelTestMain, 0);

    while ((PadData.button_state & 0x08000100U) != 0x08000100U) {
        xglFontDebugPrintf(0, 0, D_004C2AE8);
        MenuModelMain();
        xglSleep();
    }
}

void ePrintTest(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/endou_test", eMessageTest);

typedef struct BgTestRenderState {
    unsigned char unmodeled_00[0x14];
    unsigned short packetControl;
} BgTestRenderState;
typedef struct BgTestEnvironment {
    unsigned char unmodeled_00[0x30];
    u64 packetWord;
} BgTestEnvironment;
enum {
    BG_TEST_COMMAND_HIGH = 0xc800,
    BG_TEST_COMMAND_BASE = 0x2412,
    BG_TEST_COMMAND_BASE_SHIFT = 16,
    BG_TEST_CONTROL_SHIFT = 5,
    BG_TEST_COMMAND_HIGH_SHIFT = 19
};
extern BgTestRenderState sRender;
extern BgTestEnvironment TestEnv_47;

void BgTest(void)
{
    XglPacket *packet;
    int packet_command = BG_TEST_COMMAND_BASE << BG_TEST_COMMAND_BASE_SHIFT;

    packet_command |= sRender.packetControl << BG_TEST_CONTROL_SHIFT;
    TestEnv_47.packetWord = packet_command |
        ((u64)BG_TEST_COMMAND_HIGH << BG_TEST_COMMAND_HIGH_SHIFT);
    packet = xglPacketGetCurrent();
    sceVif1PkRef(packet, &TestEnv_47, 10, 0, 0, 0);
}

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
