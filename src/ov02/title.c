/*
 * OV02 original TU 2: 0x00a00858..0x00a017a0 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/xgl_render.h"
#include "ov02/ov02.h"
#include "ov02/title_game_init.h"
#include "ov02/title_hdd_install.h"
#include "title.h"
#include "main/xgl_cd.h"
#include "main/xgl_mc.h"
#include "main/xgl_jpeg.h"
#include "main/xgl_hdd.h"
#include "main/xgl_thread.h"
#include "main/xgl_sound.h"
#include "main/xgl_dma.h"
#include "main/control_entry.h"
#include "main/file.h"

#include "main/xgl_packet.h"

/* Options of the title menu (flag in Title). */
#define TITLE_CHOICE_NEW_GAME 0
#define TITLE_CHOICE_CONTINUE 1
#define TITLE_CHOICE_HDD_INSTALL 2

/* Title hard disk state (cursor in Title) when no disk is usable. */
#define TITLE_HDD_UNAVAILABLE (-1)

/* Frames without input before the title leaves, and the frames a press is ignored after startup or a move. */
#define TITLE_TIMEOUT_FRAMES 600
#define TITLE_PREROLL_FRAMES 4

/* The fade overlay alpha (TestEnv_12) starts at TITLE_FADE_MAX and drops TITLE_FADE_STEP per frame. */
#define TITLE_FADE_MAX 128
#define TITLE_FADE_STEP 4

/* Snow particles: 256 records at TITLE_PARTICLE_BASE. */
#define TITLE_PARTICLE_COUNT 256
#define TITLE_PARTICLE_BASE 0x01000000

/* Memory map of the loaded title data: the object texture archive, then the jpeg behind it. */
#define TITLE_OBJECT_BASE 0x01001000
#define TITLE_JPEG_BASE 0x010E1000

/* The object texture is sent in two halves of TITLE_TEXTURE_QWC quadwords. */
#define TITLE_TEXTURE_QWC 0x7000
#define TITLE_TEXTURE_HALF_SIZE (TITLE_TEXTURE_QWC * 16)

#define TITLE_PARTICLE_STRIDE 16
#define TITLE_PARTICLE_AT(storage, index) \
    ((TitleParticleView *)((u8 *)(storage) + ((index) * TITLE_PARTICLE_STRIDE)))

extern u8 gameEntrySelection[];

/* Queues a VIF1 packet that copies the frame through the GS transfer registers. */
static int copyframe(void)
{
    XglPacket *packet;
    u64 *scratch;

    packet = xglPacketGetCurrent();
    scratch = (u64 *)SCRATCHPAD_BASE;
    scratch[0] = 0;
    scratch[1] = 0x51000009;
    scratch[2] = ((u64)0x10000000 << 32) | 0x8008;
    scratch[3] = GIF_REG_AD;
    scratch[4] = (0x80000 | (sRender.width << 5)) |
                 ((u64)sRender.height << 37) | ((u64)0x8000 << 36);
    scratch[5] = SCE_GS_BITBLTBUF;
    scratch[6] = 0;
    scratch[7] = SCE_GS_TRXPOS;
    scratch[8] = ((u64)OV02_SCREEN_HEIGHT << 32) | OV02_SCREEN_WIDTH;
    scratch[9] = SCE_GS_TRXREG;
    scratch[10] = 2;
    scratch[11] = SCE_GS_TRXDIR;
    scratch[12] = 0;
    scratch[13] = SCE_GS_TEXFLUSH;
    scratch[14] = (0x80000 | (sRender.width << 5)) |
                  ((u64)sRender.flip_base << 37) | ((u64)0x8000 << 36);
    scratch[15] = SCE_GS_BITBLTBUF;
    scratch[16] = 2;
    scratch[17] = SCE_GS_TRXDIR;
    scratch[18] = 0;
    scratch[19] = SCE_GS_TEXFLUSH;
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkAddDirectDataN(packet, scratch, 10);
    sceVif1PkCloseDirectHLCode(packet);
}

/* Queues a VIF1 packet that draws the title's textured quad and sets its mipmap state from the render buffer bases. */
static void build_mipmap(XglPacket *packet)
{
    TitleMipmapPacket *gif;
    u32 copy_u = OV02_SCREEN_WIDTH << 4; /* right edge of the full-size copy, in 12.4 */

    gif = (TitleMipmapPacket *)SCRATCHPAD_BASE;
    gif->vif_direct.lo = 0;
    gif->vif_direct.hi = 0x51000023;
    gif->frame_tag.lo = 0x1000000000000004ULL;
    gif->frame_tag.hi = GIF_REG_AD;
    gif->frame_1.data = (u64)sRender.texture_buffer_base | ((u64)8 << 16);
    gif->frame_1.reg = SCE_GS_FRAME_1;
    gif->tex0_1.data = 0x24020000 | (sRender.display_buffer_base << 5) | (0xC800ULL << 19);
    gif->tex0_1.reg = SCE_GS_TEX0_1;
    gif->tex1_1.data = 0x60;
    gif->tex1_1.reg = SCE_GS_TEX1_1;
    gif->test_1.data = 0x30000;
    gif->test_1.reg = SCE_GS_TEST_1;
    gif->vertex_tag.lo = 0x508B400000000001ULL;
    gif->vertex_tag.hi = 0x53531;
    gif->rgbaq_0.r = 0x80;
    gif->rgbaq_0.g = 0x80;
    gif->rgbaq_0.b = 0x80;
    gif->rgbaq_0.a = 0x80;
    gif->uv_0.u = 0;
    gif->uv_0.v = 0;
    gif->xyz_0.x = 0x6FF8;
    gif->xyz_0.y = 0x71F7;
    gif->xyz_0.z = 0;
    gif->xyz_0.adc = 0;
    gif->uv_1.u = copy_u;
    gif->uv_1.v = 0x1C00;
    gif->xyz_1.x = 0x7FF8;
    gif->xyz_1.y = 0x7FF7;
    gif->xyz_1.z = 0;
    gif->xyz_1.adc = 0;
    gif->texture_tag.lo = 0x008B400000000001ULL;
    gif->texture_tag.hi = 0x535315353153531EULL;
    gif->tex0_texture.data = 0x24020000 | (sRender.texture_buffer_base << 5) | (0xC800ULL << 19);
    gif->tex0_texture.reg = SCE_GS_TEX0_1;
    gif->rgbaq_1.r = 0x80;
    gif->rgbaq_1.g = 0x80;
    gif->rgbaq_1.b = 0x80;
    gif->rgbaq_1.a = 0x80;
    gif->uv_2.u = 0;
    gif->uv_2.v = 0;
    gif->xyz_2.x = 0x7FF8;
    gif->xyz_2.y = 0x71F7;
    gif->xyz_2.z = 0;
    gif->xyz_2.adc = 0;
    gif->uv_3.u = 0x1000;
    gif->uv_3.v = 0xE00;
    gif->xyz_3.x = 0x87F8;
    gif->xyz_3.y = 0x78F7;
    gif->xyz_3.z = 0;
    gif->xyz_3.adc = 0;
    gif->rgbaq_2.r = 0x80;
    gif->rgbaq_2.g = 0x80;
    gif->rgbaq_2.b = 0x80;
    gif->rgbaq_2.a = 0x80;
    gif->uv_4.u = 0x1000;
    gif->uv_4.v = 0;
    gif->xyz_4.x = 0x87F8;
    gif->xyz_4.y = 0x71F7;
    gif->xyz_4.z = 0;
    gif->xyz_4.adc = 0;
    gif->uv_5.u = 0x1800;
    gif->uv_5.v = 0x700;
    gif->xyz_5.x = 0x8BF8;
    gif->xyz_5.y = 0x7577;
    gif->xyz_5.z = 0;
    gif->xyz_5.adc = 0;
    gif->rgbaq_3.r = 0x80;
    gif->rgbaq_3.g = 0x80;
    gif->rgbaq_3.b = 0x80;
    gif->rgbaq_3.a = 0x80;
    gif->uv_6.u = 0x1800;
    gif->uv_6.v = 0;
    gif->xyz_6.x = 0x8BF8;
    gif->xyz_6.y = 0x71F7;
    gif->xyz_6.z = 0;
    gif->xyz_6.adc = 0;
    gif->uv_7.u = 0x1C00;
    gif->uv_7.v = 0x380;
    gif->xyz_7.x = 0x8DF8;
    gif->xyz_7.y = 0x73B7;
    gif->xyz_7.z = 0;
    gif->xyz_7.adc = 0;
    gif->mip_tag.lo = 0x1000000000008006ULL;
    gif->mip_tag.hi = GIF_REG_AD;
    gif->frame_2.data = (u64)sRender.display_buffer_base | ((u64)8 << 16);
    gif->frame_2.reg = SCE_GS_FRAME_1;
    gif->miptbp1.data = (u64)(sRender.texture_buffer_base << 5) | ((u64)8 << 14)
                        | ((u64)((sRender.texture_buffer_base << 5) + 128) << 20) | ((u64)8 << 34)
                        | ((u64)((sRender.texture_buffer_base << 5) + 192) << 40) | ((u64)8 << 54);
    gif->miptbp1.reg = SCE_GS_MIPTBP1_1;
    gif->miptbp2.data = (u64)((sRender.texture_buffer_base << 5) + 224) | ((u64)8 << 14);
    gif->miptbp2.reg = SCE_GS_MIPTBP2_1;
    gif->tex1_2.data = 0x170;
    gif->tex1_2.reg = SCE_GS_TEX1_1;
    gif->tex0_2.data = 0x24020000 | (sRender.display_buffer_base << 5) | (0xC800ULL << 19);
    gif->tex0_2.reg = SCE_GS_TEX0_1;
    gif->texture_flush.data = 0;
    gif->texture_flush.reg = SCE_GS_TEXFLUSH;
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkAddDirectDataN(packet, gif, 36);
    sceVif1PkCloseDirectHLCode(packet);
}

/* Gives particle index a new start position, depth and speed. */
static void particle_reset(TitleParticleView *particles, int index)
{
    int random_value;
    int y;

    if (index & 1) {
        TITLE_PARTICLE_AT(particles, index)->x =
            (s16)((xglSRand() & 0x7ff) + 6656);
        TITLE_PARTICLE_AT(particles, index)->y =
            (s16)((xglSRand() & 0x7f) + 8);
    } else {
        u16 random_x = (u16)(xglSRand() & 0x7ff);
        TITLE_PARTICLE_AT(particles, index)->x =
            (u16)(random_x + (u16)0xfe00);
        TITLE_PARTICLE_AT(particles, index)->y =
            (s16)-((xglSRand() & 0x7f) + 8);
    }

    TITLE_PARTICLE_AT(particles, index)->z = (s16)(xglSRand() & 0x1fff);
    y = TITLE_PARTICLE_AT(particles, index)->y;
    random_value = TITLE_PARTICLE_AT(particles, index)->z - 3584;
    if (random_value < 0)
        random_value = TITLE_PARTICLE_AT(particles, index)->z - 3073;
    if (y < 0)
        y = -y;
    random_value >>= 9;
    random_value *= y;
    if (random_value < 0)
        random_value += 15;
    TITLE_PARTICLE_AT(particles, index)->depth = (s16)(random_value >> 4);
    TITLE_PARTICLE_AT(particles, index)->speed =
        (s16)((xglSRand() & 0x3f) + 16);
}

/*
 * Runs the title screen: reads the memory cards and the HDD, loads and decodes the
 * title data, then loops over the menu until an entry is chosen or the timeout ends.
 * Returns the selection value for the caller, or 0 when nothing starts.
 */
int Title(void)
{
    XglPacket *packet;
    int *scratch;
    int mcResult[3];
    TitleMcRequest mc;
    struct HddExistRequest hddRequest;
    short portStatus[2];
    TitleJpegRequest jpeg;
    u8 *objectEnd;
    int frameTimer;
    int preroll;
    int flag;
    int fade;
    int cursor;
    u8 *jpegDst;
    int bytesRead;
    int i;
    XglRenderState *render;
    int r;
    u64 recordWord;

    copyframe();
    mc.buffer = WorkEnd;
    mc.slot = -1;
    frameTimer = TITLE_TIMEOUT_FRAMES;
    xglMcRequest(0, MC_REQUEST_DIRECTORY, &mc, &mcResult[0]);
    xglMcRequest(1, MC_REQUEST_DIRECTORY, &mc, &mcResult[1]);
    hddRequest.found = WorkEnd;
    hddRequest.card = -1;
    mcResult[2] = xglHddMcExist(&hddRequest);
    xglFontLoad(1, 1);
    do {
        /* xglMcMain returns both ports' status halfwords as one word. */
        *(unsigned int *)portStatus = xglMcMain();
        xglSleep();
    } while (0 != xglCdSync() || MC_STATUS_BUSY == portStatus[0] || MC_STATUS_ERROR == portStatus[0] ||
             MC_STATUS_BUSY == portStatus[1] || MC_STATUS_ERROR == portStatus[1]);
    if (mcResult[0] > 0 || mcResult[1] > 0 || mcResult[2] > 0)
        flag = TITLE_CHOICE_CONTINUE;
    else
        flag = TITLE_CHOICE_NEW_GAME;
    switch (xglHddMount()) {
    case 0:
        cursor = 0;
        break;
    case -2:
        cursor = 2;
        break;
    case -3:
        cursor = 3;
        break;
    case -4:
        cursor = 4;
        break;
    case -5:
    case 1:
        cursor = 1;
        break;
    default:
        cursor = TITLE_HDD_UNAVAILABLE;
        break;
    }
    bytesRead = xglCdReadFile("data\\yajima\\title_obj.xtx", (void *)TITLE_OBJECT_BASE, 0, 1);
    jpegDst = (u8 *)TITLE_JPEG_BASE + bytesRead;
    objectEnd = (u8 *)TITLE_OBJECT_BASE + bytesRead;
    xglCdReadFile("data\\title.jpg", jpegDst, 0, 1);
    memset(&jpeg, 0, 32);
    jpeg.source = jpegDst;
    jpeg.output_end = objectEnd;
    xglJpegDecode(&jpeg);
    for (i = 0; TITLE_PARTICLE_COUNT > i; i++)
        particle_reset((TitleParticleView *)TITLE_PARTICLE_BASE, i);
    copyframe();
    scratch = (int *)SCRATCHPAD_BASE;
    fade = TITLE_FADE_MAX;
    preroll = TITLE_PREROLL_FRAMES;

    /* Per-frame loop: handles input, then builds the frame packet. */
    for (;;) {
        TitleRecordHeader *header;
        TitleRecord *record;
        int particleIndex;
        int recordIndex;
        int m;
        int rowOffset;
        int step;
        int maxFlag;
        u16 repeat;

        packet = xglPacketGetCurrent();
        frameTimer--;
        if (0 >= frameTimer)
            return (int)logoEntrySelection;
        if (0 < preroll) {
            preroll--;
        } else if (fade == 0 && (PadData.half_2a & (PAD_START | PAD_CIRCLE))) {
            if (flag == TITLE_CHOICE_HDD_INSTALL && cursor < 0) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            } else {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                copyframe();
                xglSleep();
                break;
            }
        }

        repeat = PadData.mappedRepeat;
        maxFlag = cursor < 0 ? 1 : 2;
        step = 0;
        if (repeat & PAD_UP)
            step = 0 < flag ? -1 : 0;
        if (repeat & PAD_DOWN)
            step = maxFlag > flag ? 1 : step;
        if (0 != step) {
            while (0 != step) {
                flag += step;
                if (flag != TITLE_CHOICE_HDD_INSTALL || cursor != TITLE_HDD_UNAVAILABLE)
                    break;
            }
            xglSoundEffectNormalID(SE_CURSOR, 0);
            if (preroll < TITLE_PREROLL_FRAMES)
                preroll = TITLE_PREROLL_FRAMES;
            frameTimer = TITLE_TIMEOUT_FRAMES;
        }
        if (0 < fade) {
            fade -= TITLE_FADE_STEP;
            fade = 0 > fade ? 0 : fade;
        }

        render = &sRender;
        TestEnv_0_00A0F780[4] = ((u64)sRender.display_buffer_base << 37) | (0x8000ULL << 36);
        sceVif1PkCnt(packet, 0);
        sceVif1PkAddDataN(packet, TestEnv_0_00A0F780, 24);
        sceVif1PkRef(packet, TransEnv_1_00A0F7E0, 2, 0, 0, 0);
        sceVif1PkRef(packet, objectEnd, TITLE_TEXTURE_QWC, 0, 0x51007000, 0);
        sceVif1PkRef(packet, TransEnv_1_00A0F7E0, 2, 0, 0, 0);
        sceVif1PkRef(packet, objectEnd + TITLE_TEXTURE_HALF_SIZE, TITLE_TEXTURE_QWC, 0, 0x51007000, 0);
        sceVif1PkRef(packet, FlushEnv_2_00A0F800, 3, 0, 0, 0);
        sceVif1PkRef(packet, TestEnv_3, 11, 0, 0, 0);
        sceVif1PkRef(packet, image_4, 18, 0, 0, 0);
        *(TitleFrameCopy *)scratch = TestEnvOrg_6;
        sceVif1PkRef(packet, FlushEnv_5, 3, 0, 0, 0);
        sceVif1PkCnt(packet, 0);

        /* Snow particles: move each one and reset it once it leaves the visible area. */
        for (particleIndex = 0; TITLE_PARTICLE_COUNT > particleIndex; particleIndex++) {
            TitleParticleView *particle = (TitleParticleView *)(TITLE_PARTICLE_BASE + particleIndex * TITLE_PARTICLE_STRIDE);

            scratch[11] = 1 + (xglSRand() & 0xf);
            scratch[16] = OV02_GS_VERTEX_X + (s16)particle->x;
            scratch[17] = OV02_GS_VERTEX_Y + (s16)particle->z;
            scratch[24] = scratch[16] + (u16)particle->speed;
            scratch[25] = scratch[17] + (u16)particle->speed;
            particle->x = (u16)particle->x + (u16)particle->y;
            particle->z = (s16)((u16)particle->z + (u16)particle->depth);
            if (!(8193 > (u16)particle->x && 0 <= particle->z && 7169 > particle->z))
                particle_reset((TitleParticleView *)TITLE_PARTICLE_BASE, particleIndex);
            sceVif1PkAddDataN(packet, scratch, 28);
        }

        header = (TitleRecordHeader *)TITLE_OBJECT_BASE;
        record = (TitleRecord *)(TITLE_OBJECT_BASE + header->first);
        for (recordIndex = 0; (int)header->count > recordIndex; record++, recordIndex++) {
            u32 width;

            width = record->w0 & 0xffff;
            TestEnv_10[4] = ((u64)((record->w8 + 0xE0000) >> 6) << 32) |
                            ((u64)(record->w0 >> 16) << 48);
            TestEnv_10[8] = width | ((u64)record->w4 << 32);
            sceVif1PkCnt(packet, 0);
            sceVif1PkAddDataN(packet, TestEnv_10, 24);
            sceVif1PkRef(packet, (u8 *)header + record->w16, record->w12, 0, 0, 0);
        }

        /* Menu entries: the selected entry is drawn highlighted; HDD install is skipped when no disk is usable. */
        for (m = 0; 4 > m; m++) {
            int sel;
            int w9;
            int w3;
            int w8;
            int w6;

            w9 = 166;
            w8 = 288 + m * 18;
            w3 = 180;
            w6 = 18;
            sel = m;
            if (m == TITLE_CHOICE_HDD_INSTALL) {
                if (cursor == TITLE_HDD_UNAVAILABLE)
                    continue;
                if (cursor == 1)
                    sel = 3;
            }
            if (flag != m) {
                w9 = 186;
                w3 = 140;
                w8 = 291 + m * 18;
                w6 = 12;
            }
            if (m <= TITLE_CHOICE_HDD_INSTALL) {
                int selRow = sel * 288;
                int row9 = w9 * 16 + OV02_GS_VERTEX_X;
                int row8 = w8 * 16 + 29175;

                TestEnv_11[45] = selRow + 288;
                TestEnv_11[48] = row9 + w3 * 16;
                TestEnv_11[37] = selRow;
                TestEnv_11[49] = row8 + w6 * 16;
                TestEnv_11[40] = row9;
                TestEnv_11[41] = row8;
                sceVif1PkCnt(packet, 0);
                sceVif1PkAddDataN(packet, TestEnv_11, 52);
            }
        }

        recordWord = (sRender.flip_base << 5) | 0x24120000ULL;
        recordWord |= 0xC800ULL << 19;
        TestEnv_12[8] = 100 | ((u64)fade << 32);
        TestEnv_12[6] = recordWord;
        FlushCache(0);
        sceVif1PkRef(packet, TestEnv_12, 10, 0, 0, 0);
        xglSleep();
    }
        /* Starts the chosen option. */
        switch (flag) {
        case TITLE_CHOICE_NEW_GAME:
            TitleGameInit();
            GameLoopState.word_18 = 0x100003E8;
            GameLoopState.half_0e = 0;
            return (int)gameEntrySelection;
        case TITLE_CHOICE_CONTINUE:
            TitleGameInit();
            r = MenuFileMain(1);
            if (r == flag) {
                GameLoopState.half_0e = r;
                return (int)gameEntrySelection;
            }
            return 0;
        case TITLE_CHOICE_HDD_INSTALL:
            if (0 <= cursor)
                TitleHddInstall(cursor);
            return 0;
        case 3:
            TitleGameInit();
            GameLoopState.word_18 = 0;
            GameLoopState.half_0e = 0;
            return (int)gameEntrySelection;
        default:
            return 0;
        }
}



static u8 TitleMenuSelect = 1;

u32 title_dummy_table[21] = {
    0x00000000, 0x63257EC0, 0x0AA003F8, 0x6325A1C0,
    0x0BA00070, 0x01A00000, 0x0226F4D8, 0x012BCE98,
    0x012CB1A8, 0x012D9100, 0x012DC3F0, 0x012DD8F8,
    0x01A31340, 0x0CA00000, 0x50A00000, 0x00000000,
    0x63A02090, 0x63A0E228, 0x63A0ABE8, 0x63A039A8,
    0x00000000,
};

static u64 TestEnv_0_00A0F780[12] = { 0ULL, 5836665138546999296ULL, 1152921504606846980ULL, 14ULL, 0ULL, 80ULL, 0ULL, 81ULL, 1924145349120ULL, 82ULL, 0ULL, 83ULL };

static u64 TransEnv_1_00A0F7E0[4] = { 0ULL, 5836665121367130112ULL, 576460752303452160ULL, 0ULL };

static u64 FlushEnv_2_00A0F800[6] = { 0ULL, 5836665125662097408ULL, 1152921504606879745ULL, 14ULL, 0ULL, 63ULL };

static u64 TestEnv_3[22] = { 0ULL, 5836665160021835776ULL, 1152921504606879753ULL, 14ULL, 72ULL, 66ULL, 196608ULL, 71ULL, 2305843029816260608ULL, 6ULL, 96ULL, 20ULL, 0ULL, 8ULL, 2313372464840704ULL, 80ULL, 0ULL, 81ULL, 34359738376ULL, 82ULL, 0ULL, 83ULL };

static u64 image_4[36] = { 0ULL, 5836665190086606848ULL, 576460752303456272ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 9295429626597736448ULL, 2164260863ULL, 0ULL, 0ULL, 9295429628761997311ULL, 9295429628761997311ULL, 0ULL, 9295429626597736448ULL, 9295429628761997311ULL, 9295429628761997311ULL, 2164260863ULL, 9295429626597736448ULL, 9295429628761997311ULL, 9295429628761997311ULL, 2164260863ULL, 0ULL, 9295429628761997311ULL, 9295429628761997311ULL, 0ULL, 0ULL, 9295429626597736448ULL, 2164260863ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL };

static u64 FlushEnv_5[6] = { 0ULL, 5764607531624169472ULL, 1152921504606879745ULL, 14ULL, 0ULL, 63ULL };

static TitleFrameCopy TestEnvOrg_6 = { .bytes = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 80, 1, 128, 0, 0, 0, 64, 171, 80, 49, 53, 5, 0, 0, 0, 0, 0, 128, 0, 0, 0, 128, 0, 0, 0, 128, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 248, 115, 0, 0, 247, 117, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 0, 0, 0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 248, 119, 0, 0, 247, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

static int x_7 = 0x1000;

static int y_8 = 0x1400;

static u64 TestEnv_9[50] = {
    0, 0x5000001800000000,
    0x408EC00000000001, 0xE5E3,
    0, 0,
    0x3A83126F80808080, 0x1,
    0, 0,
    0x3F80000080808080, 0x1,
    0x2000000000008009, 0x53,
};

static u64 TestEnv_10[12] = { 0ULL, 5836665138546999296ULL, 4611686018427420673ULL, 61166ULL, 0ULL, 80ULL, 0ULL, 81ULL, 0ULL, 82ULL, 0ULL, 83ULL };

static u32 TestEnv_11[52] = { 0U, 0U, 0U, 1342177292U, 32769U, 2964013056U, 837742318U, 1333U, 0U, 0U, 63U, 0U, 5U, 0U, 8U, 0U, 96U, 0U, 20U, 0U, 196608U, 0U, 71U, 0U, 1677867008U, 6U, 6U, 0U, 100U, 128U, 66U, 0U, 128U, 128U, 128U, 128U, 0U, 0U, 0U, 0U, 28664U, 33271U, 0U, 0U, 1919U, 1152U, 0U, 0U, 30584U, 30327U, 0U, 0U };

static u64 TestEnv_12[20] = { 0ULL, 5764607561688940544ULL, 9271574626616508417ULL, 1397956206ULL, 196608ULL, 71ULL, 0ULL, 6ULL, 0ULL, 66ULL, 549755814016ULL, 549755814016ULL, 0ULL, 0ULL, 125305670889464ULL, 0ULL, 30786325585920ULL, 0ULL, 156091996475384ULL, 0ULL };
