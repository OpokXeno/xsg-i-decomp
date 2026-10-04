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
/* EUC-JP bytes for the model-test diagnostic with its leading control byte. */
const unsigned char D_004C2AE8[16] =
    "\x0B\xA5\xE2\xA5\xC7\xA5\xEB\xA5\xC6\xA5\xB9\xA5\xC8";
static const char not_found_format[16] = "%s/NotFound";
static const char read_error_format[24] = "%s/ReadError";

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

typedef struct GifPackedTag {
    unsigned int loop_count_and_eop;
    unsigned int primitive_flags_and_register_count;
    u64 register_descriptors;
} GifPackedTag;

typedef struct GifAdWrite {
    u64 value;
    u64 register_id;
} GifAdWrite;

typedef struct GifPackedRgbaq {
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    unsigned int alpha;
} GifPackedRgbaq;

typedef struct GifUv {
    unsigned int u;
    unsigned int v;
    unsigned int q;
    unsigned int reserved;
} GifUv;

typedef struct GifXyz2 {
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int fog;
} GifXyz2;

typedef struct BgTestRenderState {
    unsigned char unmodeled_00[0x14];
    unsigned short packetControl;
} BgTestRenderState;
typedef struct BgTestEnvironment {
    unsigned int vif_nop[3];
    unsigned int vif_directhl_nine_quadwords;
    GifPackedTag tag;
    GifAdWrite texture_setup;
    GifAdWrite packet_write;
    GifAdWrite primitive_setup;
    GifPackedRgbaq color;
    GifUv first_uv;
    GifXyz2 first_vertex;
    GifUv second_uv;
    GifXyz2 second_vertex;
} BgTestEnvironment;
enum {
    BG_TEST_COMMAND_HIGH = 0xc800,
    BG_TEST_COMMAND_BASE = 0x2412,
    BG_TEST_COMMAND_BASE_SHIFT = 16,
    BG_TEST_CONTROL_SHIFT = 5,
    BG_TEST_COMMAND_HIGH_SHIFT = 19
};
extern BgTestRenderState sRender;
static BgTestEnvironment TestEnv_47 = {
    { 0, 0, 0 }, 0x50000009,
    { 0x00008001, 0x808B4000, 0x0000000053531EEEULL },
    { 0x20, 0x14 },
    { 0, 0x6 },
    { 0x00050000, 0x47 },
    { 0x80, 0x80, 0x80, 0x80 },
    { 0, 0, 0, 0 },
    { 0x6FF8, 0x71F8, 0, 0 },
    { 0x2000, 0x1C00, 0, 0 },
    { 0x8FF8, 0x8DF8, 0, 0 },
};

void BgTest(void)
{
    XglPacket *packet;
    int packet_command = BG_TEST_COMMAND_BASE << BG_TEST_COMMAND_BASE_SHIFT;

    packet_command |= sRender.packetControl << BG_TEST_CONTROL_SHIFT;
    TestEnv_47.packet_write.value = packet_command |
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
void *test_data = 0;
static char fname_67[64];
static float size_68;
static unsigned int TestEnv_66[32] = {
    0x00008001, 0x208B4000, 0x000000EE, 0x00000000,
    0x00000000, 0x00000000, 0x0000003F, 0x00000000,
    0xDD343840, 0x20070005, 0x00000006, 0x00000000,
    0x00000080, 0x00000080, 0x00000080, 0x00000080,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00007C00, 0x00007BF8, 0x10000000, 0x00000000,
    0x00000800, 0x00000800, 0x00000000, 0x00000000,
    0x00008400, 0x000083F8, 0x10000000, 0x00000000
};

typedef struct WinTexEnvironment {
    unsigned int vif_nop_before_flush;
    unsigned int vif_nop_before_direct;
    unsigned int vif_flushe;
    unsigned int vif_directhl_six_quadwords;
    GifPackedTag gif_tag;
    GifAdWrite texture_flush;
    GifAdWrite bitblt_buffer;
    GifAdWrite transfer_position;
    GifAdWrite transfer_region;
    GifAdWrite transfer_direction;
} WinTexEnvironment;

/* VIF DIRECTHL carries a GIF PACKED tag and five GS A+D register writes. */
static WinTexEnvironment WinTexEnv_65 = {
    0, 0, 0x11000000, 0x51000006,
    { 0x00008005, 0x10000000, 0x000000000000000EULL },
    { 0x0000000000000000ULL, 0x000000000000003FULL },
    { 0x0008380000000000ULL, 0x0000000000000050ULL },
    { 0x0000000000000000ULL, 0x0000000000000051ULL },
    { 0x0000008000000200ULL, 0x0000000000000052ULL },
    { 0x0000000000000000ULL, 0x0000000000000053ULL },
};

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
    sceVif1PkRef(context->packet, (unsigned char *)&WinTexEnv_65,
                 7, 0, 0, 0);
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
        char *destination = fname_67;
        do {
            *destination = *source++;
        } while (((unsigned int)*destination++ << 24) != 0);
        break;
    }
    case 1:
        size_68 = I2F(amount) * progress_percent;
        break;
    case 2:
    case 3:
        if (result != 3)
            xglSleep();
        break;
    case -1: {
        for (;;) {
            xglFontDebugPrintf(8, 216, not_found_format, fname_67);
            xglSleep();
        }
    }
    case -2: {
        for (;;) {
            xglFontDebugPrintf(8, 216, read_error_format, fname_67);
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
