/*
 * OV02 original TU 2: 0x00a00858..0x00a017a0 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
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
#include "title_game_init.h"

#include "main/xgl_packet.h"

#define TITLE_PARTICLE_STRIDE 16
#define TITLE_PARTICLE_AT(storage, index) \
    ((TitleParticleView *)((u8 *)(storage) + ((index) * TITLE_PARTICLE_STRIDE)))

extern u8 D_1245C58[];

static int copyframe(void)
{
    XglPacket *packet;
    u64 *scratch;

    packet = xglPacketGetCurrent();
    scratch = (u64 *)0x70000000;
    scratch[0] = 0;
    scratch[1] = 0x51000009;
    scratch[2] = ((u64)0x10000000 << 32) | 0x8008;
    scratch[3] = 14;
    scratch[4] = (0x80000 | (sRender.width << 5)) |
                 ((u64)sRender.height << 37) | ((u64)0x8000 << 36);
    scratch[5] = 80;
    scratch[6] = 0;
    scratch[7] = 81;
    scratch[8] = ((u64)448 << 32) | 0x200;
    scratch[9] = 82;
    scratch[10] = 2;
    scratch[11] = 83;
    scratch[12] = 0;
    scratch[13] = 63;
    scratch[14] = (0x80000 | (sRender.width << 5)) |
                  ((u64)sRender.framebuffer_page << 37) | ((u64)0x8000 << 36);
    scratch[15] = 80;
    scratch[16] = 2;
    scratch[17] = 83;
    scratch[18] = 0;
    scratch[19] = 63;
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkAddDirectDataN(packet, scratch, 10);
    sceVif1PkCloseDirectHLCode(packet);
}

static void build_mipmap(XglPacket *packet)
{
    TitleMipmapPacket *gif;
    u32 copy_u = 0x2000; /* right edge of the full-size copy, 512 texels in 12.4 */

    gif = (TitleMipmapPacket *)0x70000000;
    gif->vif_direct.lo = 0;
    gif->vif_direct.hi = 0x51000023;
    gif->frame_tag.lo = 0x1000000000000004ULL;
    gif->frame_tag.hi = 0xE;
    gif->frame_1.data = (u64)sRender.texture_buffer_base | ((u64)8 << 16);
    gif->frame_1.reg = 0x4C;
    gif->tex0_1.data = 0x24020000 | (sRender.display_buffer_base << 5) | (0xC800ULL << 19);
    gif->tex0_1.reg = 6;
    gif->tex1_1.data = 0x60;
    gif->tex1_1.reg = 0x14;
    gif->test_1.data = 0x30000;
    gif->test_1.reg = 0x47;
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
    gif->tex0_texture.reg = 6;
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
    gif->mip_tag.hi = 0xE;
    gif->frame_2.data = (u64)sRender.display_buffer_base | ((u64)8 << 16);
    gif->frame_2.reg = 0x4C;
    gif->miptbp1.data = (u64)(sRender.texture_buffer_base << 5) | ((u64)8 << 14)
                        | ((u64)((sRender.texture_buffer_base << 5) + 128) << 20) | ((u64)8 << 34)
                        | ((u64)((sRender.texture_buffer_base << 5) + 192) << 40) | ((u64)8 << 54);
    gif->miptbp1.reg = 0x34;
    gif->miptbp2.data = (u64)((sRender.texture_buffer_base << 5) + 224) | ((u64)8 << 14);
    gif->miptbp2.reg = 0x36;
    gif->tex1_2.data = 0x170;
    gif->tex1_2.reg = 0x14;
    gif->tex0_2.data = 0x24020000 | (sRender.display_buffer_base << 5) | (0xC800ULL << 19);
    gif->tex0_2.reg = 6;
    gif->texture_flush.data = 0;
    gif->texture_flush.reg = 0x3F;
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkAddDirectDataN(packet, gif, 36);
    sceVif1PkCloseDirectHLCode(packet);
}

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

int Title(void)
{
    XglPacket *packet;
    int *scratch;
    int mcResult[3];
    TitleMcRequest mc;
    struct HddExistRequest hddRequest;
    short portStatus[2];
    TitleJpegRequest jpeg;
    int objectEnd;
    int frameTimer;
    int preroll;
    int flag;
    int fade;
    int cursor;
    int jpegDst;
    int bytesRead;
    int i;
    TitleRenderState *render;
    int r;
    u64 recordWord;

    copyframe();
    mc.buffer = WorkEnd;
    mc.slot = -1;
    frameTimer = 600;
    xglMcRequest(0, 2, &mc, &mcResult[0]);
    xglMcRequest(1, 2, &mc, &mcResult[1]);
    hddRequest.found = WorkEnd;
    hddRequest.card = -1;
    mcResult[2] = xglHddMcExist(&hddRequest);
    xglFontLoad(1, 1);
    do {
        /* xglMcMain returns both ports' status halfwords as one word. */
        *(unsigned int *)portStatus = xglMcMain();
        xglSleep();
    } while (0 != xglCdSync() || 255 == portStatus[0] || 128 == portStatus[0] ||
             255 == portStatus[1] || 128 == portStatus[1]);
    if (mcResult[0] > 0 || mcResult[1] > 0 || mcResult[2] > 0)
        flag = 1;
    else
        flag = 0;
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
        cursor = -1;
        break;
    }
    bytesRead = xglCdReadFile("data\\yajima\\title_obj.xtx", (void *)0x01001000, 0, 1);
    jpegDst = 0x010E1000 + bytesRead;
    objectEnd = 0x01001000 + bytesRead;
    xglCdReadFile("data\\title.jpg", (void *)jpegDst, 0, 1);
    memset(&jpeg, 0, 32);
    jpeg.source = (u8 *)jpegDst;
    jpeg.output_end = (u8 *)objectEnd;
    xglJpegDecode(&jpeg);
    for (i = 0; 256 > i; i++)
        particle_reset((TitleParticleView *)0x01000000, i);
    copyframe();
    scratch = (int *)0x70000000;
    fade = 128;
    preroll = 4;

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
            return (int)D_2A007F8;
        if (0 < preroll) {
            preroll--;
        } else if (fade == 0 && (PadData.half_2a & 0x820)) {
            if (flag == 2 && cursor < 0) {
                xglSoundEffectNormalID(5, 0);
            } else {
                xglSoundEffectNormalID(1, 0);
                copyframe();
                xglSleep();
                break;
            }
        }

        repeat = PadData.mappedRepeat;
        maxFlag = cursor < 0 ? 1 : 2;
        step = 0;
        if (repeat & 0x1000)
            step = 0 < flag ? -1 : 0;
        if (repeat & 0x4000)
            step = maxFlag > flag ? 1 : step;
        if (0 != step) {
            while (0 != step) {
                flag += step;
                if (flag != 2 || cursor != -1)
                    break;
            }
            xglSoundEffectNormalID(3, 0);
            if (preroll < 4)
                preroll = 4;
            frameTimer = 600;
        }
        if (0 < fade) {
            fade -= 4;
            fade = 0 > fade ? 0 : fade;
        }

        render = &sRender;
        TestEnv_0_00A0F780[4] = ((u64)sRender.display_buffer_base << 37) | (0x8000ULL << 36);
        sceVif1PkCnt(packet, 0);
        sceVif1PkAddDataN(packet, TestEnv_0_00A0F780, 24);
        sceVif1PkRef(packet, TransEnv_1_00A0F7E0, 2, 0, 0, 0);
        sceVif1PkRef(packet, (void *)objectEnd, 28672, 0, 0x51007000, 0);
        sceVif1PkRef(packet, TransEnv_1_00A0F7E0, 2, 0, 0, 0);
        sceVif1PkRef(packet, (void *)(objectEnd + 0x70000), 28672, 0, 0x51007000, 0);
        sceVif1PkRef(packet, FlushEnv_2_00A0F800, 3, 0, 0, 0);
        sceVif1PkRef(packet, TestEnv_3, 11, 0, 0, 0);
        sceVif1PkRef(packet, image_4, 18, 0, 0, 0);
        *(TitleFrameCopy *)scratch = TestEnvOrg_6;
        sceVif1PkRef(packet, FlushEnv_5, 3, 0, 0, 0);
        sceVif1PkCnt(packet, 0);

        for (particleIndex = 0; 256 > particleIndex; particleIndex++) {
            TitleParticleView *particle = (TitleParticleView *)(0x01000000 + particleIndex * TITLE_PARTICLE_STRIDE);

            scratch[11] = 1 + (xglSRand() & 0xf);
            scratch[16] = 28664 + (s16)particle->x;
            scratch[17] = 29176 + (s16)particle->z;
            scratch[24] = scratch[16] + (u16)particle->speed;
            scratch[25] = scratch[17] + (u16)particle->speed;
            particle->x = (u16)particle->x + (u16)particle->y;
            particle->z = (s16)((u16)particle->z + (u16)particle->depth);
            if (!(8193 > (u16)particle->x && 0 <= particle->z && 7169 > particle->z))
                particle_reset((TitleParticleView *)0x01000000, particleIndex);
            sceVif1PkAddDataN(packet, scratch, 28);
        }

        header = (TitleRecordHeader *)0x01001000;
        record = (TitleRecord *)(0x01001000 + header->first);
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
            if (m == 2) {
                if (cursor == -1)
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
            if (m <= 2) {
                int selRow = sel * 288;
                int row9 = w9 * 16 + 28664;
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

        recordWord = (sRender.framebuffer_page << 5) | 0x24120000ULL;
        recordWord |= 0xC800ULL << 19;
        TestEnv_12[8] = 100 | ((u64)fade << 32);
        TestEnv_12[6] = recordWord;
        FlushCache(0);
        sceVif1PkRef(packet, TestEnv_12, 10, 0, 0, 0);
        xglSleep();
    }
        switch (flag) {
        case 0:
            TitleGameInit();
            GameLoopState.word_18 = 0x100003E8;
            GameLoopState.half_0e = 0;
            return (int)D_1245C58;
        case 1:
            TitleGameInit();
            r = MenuFileMain(1);
            if (r == flag) {
                GameLoopState.half_0e = r;
                return (int)D_1245C58;
            }
            return 0;
        case 2:
            if (0 <= cursor)
                TitleHddInstall(cursor);
            return 0;
        case 3:
            TitleGameInit();
            GameLoopState.word_18 = 0;
            GameLoopState.half_0e = 0;
            return (int)D_1245C58;
        default:
            return 0;
        }
}
