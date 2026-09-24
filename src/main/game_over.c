#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "game_over.h"
#include "main/xgl_packet.h"

extern GameLoopStatePrefix GameLoopState;

/* GameLoopState.scene_id value that routes GameOver to the ending screen. */
#define GAME_END_SCENE_ID 9998

/* EE RAM staging buffer for the compressed image and the decoded image
 * that DrawImage presents. */
#define IMAGE_LOAD_BUFFER 0x010e0000
#define IMAGE_FRAME_BUFFER 0x01000000

/* PadData.half_2a press bits used by the ending prompts: 0x20 accepts
 * both questions, 0x40 declines the save question. */
#define PAD_DECIDE 0x20
#define PAD_CANCEL 0x40

/* GameEnd's prompt outcome. */
#define END_CHOICE_SAVE 1
#define END_CHOICE_QUIT 2

/* GS ALPHA_1 register: blend = ((A - B) * C >> 7) + D with the FIX
 * coefficient in bits 32..39 (SCE_GS_SET_ALPHA layout). */
#define GS_ALPHA(a, b, c, d, fix) \
    ((u64)(a) | ((u64)(b) << 2) | ((u64)(c) << 4) | ((u64)(d) << 6) | \
     ((u64)(fix) << 32))

/* One frame of the fade-out: present the image, program the ALPHA_1
 * register of a TestEnv packet with the new fade level, send the packet
 * and wait for the next frame. The original's schedule keeps this block
 * apart from the per-frame outcome check that follows it (no instruction
 * of the alpha update is hoisted above that check); a do/while(0)
 * statement macro reproduces that barrier. Receipt: attempt-68a6449851d1
 * forms 03 (plain statements, hoists slti/addiu above the outcome bne at
 * 0x00254078) vs 04 (wrapped in do{...}while(0), keeps the original
 * schedule) - the wrapper is what keeps the order, not a coincidence of
 * that one build. */
#define DRAW_FADE_FRAME(env, alpha)                                     \
    do {                                                                \
        DrawImage((void *)IMAGE_FRAME_BUFFER);                          \
        (env)[4] = GS_ALPHA(2, 0, 2, 1, (alpha));                       \
        FlushCache(0);                                                  \
        sceVif1PkRef(xglPacketGetCurrent(), (env), 6, 0, 0, 0);         \
        xglSleep();                                                     \
    } while (0)

/*
 * sRender's destination-buffer selector: folded (with a fixed high bit)
 * into the register DrawImage primes at TestEnv0[4] before the two-part
 * image transfer below. Only this halfword is evidenced here (see
 * src/main/map_1.c's MapRenderState for the same object's callback member
 * and src/main/window_tex_load.c's UmnRenderSize for the screen-size
 * halfwords).
 */
typedef struct {
    u8 unmodeled_00[0x20];
    u16 buffer_select;
} DrawImageRenderState;

extern DrawImageRenderState sRender;

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);
extern u64 TestEnv_0_00369CC0[12];
extern unsigned char TransEnv_1_00369D20[];
extern unsigned char FlushEnv_2_00369D40[];

static void DrawImage(void *framebuffer)
{
    XglPacket *packet;

    packet = xglPacketGetCurrent();
    TestEnv_0_00369CC0[4] = ((u64)sRender.buffer_select << 0x25) | ((u64)0x8000 << 0x24);
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, TestEnv_0_00369CC0, 0x18);
    sceVif1PkRef(packet, TransEnv_1_00369D20, 2, 0, 0, 0);
    sceVif1PkRef(packet, framebuffer, 0x7000, 0, 0x51007000, 0);
    sceVif1PkRef(packet, TransEnv_1_00369D20, 2, 0, 0, 0);
    sceVif1PkRef(packet, (u8 *)framebuffer + 0x70000, 0x7000, 0, 0x51007000, 0);
    sceVif1PkRef(packet, FlushEnv_2_00369D40, 3, 0, 0, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/game_over", DrawBack);

INCLUDE_ASM("asm/main/nonmatchings/game_over", copyframe);

static void redraw_frame(void *framebuffer) {
    xglSleep();
    DrawImage(framebuffer);
    xglSleep();
    DrawImage(framebuffer);
    xglSleep();
}

/* The ending screen: fade the ending image in, ask whether to save, and
 * either run the save menu or (after a second confirmation) fade out. */
static void GameEnd(void)
{
    JpegDecodeRequest jpeg;
    u32 framebuffer = IMAGE_FRAME_BUFFER;
    u32 jpeg_source = IMAGE_LOAD_BUFFER;
    int alpha;
    /* One local serves as the fade-in hold counter and then as the prompt
     * outcome (both live in $16 = s0 in the original; a separate outcome
     * local is allocated after the fade-out invariants instead). Receipt:
     * attempt-68a6449851d1 forms 04 and 05 (the same source: the wrapped
     * DRAW_FADE_FRAME and a separate state/choice local; 05 changed only
     * the header pins) keep the outcome in s4 and first differ
     * at 0x00253fa4 (`addiu s4,zero,2` vs `addiu s0,zero,2`); merging the
     * fade-in counter and the outcome into one local is what puts the
     * outcome back in s0. */
    int count;
    int buttons;

    GameLoopState.frame_status = 10;
    copyframe();
    xglCdReadFile(GameEndImagePath, (void *)jpeg_source, 0, 1);
    memset(&jpeg, 0, sizeof(jpeg));
    jpeg.source = jpeg_source;
    jpeg.destination = framebuffer;
    xglJpegDecode(&jpeg);
    xglRenderDrawFlipPk(xglPacketGetCurrent());
    xglSleep();

    /* Fade the image in, then hold it for 121 more frames. */
    alpha = 128;
    count = 0;
    for (;;) {
        if (alpha > 0) {
            alpha -= 2;
        } else {
            count += 1;
            if (count >= 121) {
                break;
            }
        }
        DrawImage((void *)framebuffer);
        DrawBack(alpha);
        xglSleep();
    }

    for (;;) {
        DrawImage((void *)framebuffer);
        xglFontPrint(32, 32, -1, GameEndSaveQuestion);
        buttons = PadData.half_2a;
        if (buttons & PAD_DECIDE) {
            redraw_frame((void *)framebuffer);
            xglStudioMainCameraInit();
            MenuFileMain(0);
            count = END_CHOICE_SAVE;
            break;
        }
        if (buttons & PAD_CANCEL) {
            xglSoundEffectNormalDirect(5);
            do {
                xglSleep();
                DrawImage((void *)framebuffer);
                xglFontPrint(32, 32, -1, GameEndQuestion);
            } while (!(PadData.half_2a & (PAD_DECIDE | PAD_CANCEL)));
            if (PadData.half_2a & PAD_DECIDE) {
                xglSoundEffectNormalDirect(1);
                redraw_frame((void *)framebuffer);
                count = END_CHOICE_QUIT;
                break;
            }
            xglSoundEffectNormalDirect(5);
        }
        xglSleep();
    }

    if (count == END_CHOICE_QUIT) {
        /* Fade to black through the TestEnv.4 ALPHA_1 packet.  The original
         * re-checks the outcome every frame (bne s0,s3 after xglSleep). */
        alpha = 4;
        for (;;) {
            DRAW_FADE_FRAME(TestEnv4, alpha);
            if (count != END_CHOICE_QUIT) {
                break;
            }
            if (alpha < 255) {
                alpha += 4;
                if (alpha > 255) {
                    alpha = 255;
                }
            } else {
                break;
            }
        }
    }
    copyframe();
    xglSleep();
}

/* The game-over screen: show the game-over image with its stream, then
 * fade picture and stream volume out together. */
void GameOver(void)
{
    JpegDecodeRequest jpeg;
    u32 framebuffer;
    int alpha;
    int frame_count;

    if (GameLoopState.scene_id == GAME_END_SCENE_ID) {
        GameEnd();
        return;
    }

    copyframe();
    framebuffer = IMAGE_FRAME_BUFFER;
    xglCdReadFile(GameOverImagePath, (void *)IMAGE_LOAD_BUFFER, 0, 1);
    memset(&jpeg, 0, sizeof(jpeg));
    jpeg.source = IMAGE_LOAD_BUFFER;
    jpeg.destination = framebuffer;
    xglJpegDecode(&jpeg);

    if (GameLoopState.scene_id != GAME_END_SCENE_ID) {
        xglSoundStreamOpenVagStereoParam(0, GameOverStreamPath, 3763, 127);
    }

    alpha = 128;
    xglRenderDrawFlipPk(xglPacketGetCurrent());
    frame_count = 0;
    xglSleep();

    /* Fade in, then hold the picture: a non-negative xglSoundStreamMain()
     * status restarts the 121-frame hold, and a decide/0x800 press ends it. */
    for (;;) {
        if (alpha > 0) {
            alpha -= 2;
        } else {
            frame_count += 1;
            if (frame_count >= 121 || (PadData.half_2a & (PAD_DECIDE | 0x800))) {
                break;
            }
        }
        DrawImage((void *)framebuffer);
        DrawBack(alpha);
        if (xglSoundStreamMain() >= 0) {
            frame_count = 0;
        }
        xglSleep();
    }
    xglSleep();

    /* Fade the picture to black through the TestEnv.5 ALPHA_1 packet while
     * the stream volume falls with it. */
    alpha = 4;
    for (;;) {
        DrawImage((void *)framebuffer);
        TestEnv5[4] = GS_ALPHA(2, 0, 2, 1, alpha);
        FlushCache(0);
        sceVif1PkRef(xglPacketGetCurrent(), TestEnv5, 6, 0, 0, 0);
        SsdSetVagStreamVolume(-1, (255 - alpha) >> 1, 0);
        xglSoundStreamMain();
        xglSleep();
        if (alpha < 255) {
            alpha += 4;
            if (alpha > 255) {
                alpha = 255;
            }
        } else {
            break;
        }
    }
    xglSoundStreamStop(0);
    copyframe();
    xglSleep();
    xglSleep();
}

INCLUDE_ASM("asm/main/nonmatchings/game_over", Intermission);
