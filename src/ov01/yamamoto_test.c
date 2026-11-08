/*
 * OV01 original TU 0: 0x00a00000..0x00a00218 (2 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "ov01/gr_gp_init.h"

/* Only the packed-colour words at +0x20..+0x2c are accessed here. */
typedef struct ClearEnvColor {
    unsigned char unmodeled_00[0x20];
    unsigned int color_r;
    unsigned int color_g;
    unsigned int color_b;
    unsigned int color_a;
    unsigned char unmodeled_30[0x30];
} ClearEnvColor;

extern ClearEnvColor ClearEnv;
extern void xglRenderClearFrame(void);
extern void entryFirstInit(void);
extern void PartyDataInit2(void);
extern void xglFontLoad(int mode, int variant);
extern void WindowTexLoad(int window_id, int texture_set);
extern void debugBattleConfig(void);
extern void testScreen(void);
extern void battleProc(void);

/* Test entry point: sets the clear colour, initializes the party data and then runs the battle test loop. */
void YamamotoTest(void)
{
    unsigned int clearGreen = 0x40;
    unsigned int clearAlpha = 0x80;

    ClearEnv.color_a = clearAlpha;
    ClearEnv.color_g = clearGreen;
    ClearEnv.color_b = 0;
    ClearEnv.color_r = 0;
    xglRenderClearFrame();

    entryFirstInit();
    PartyDataInit2();

    for (;;) {
        xglFontLoad(1, 0);
        WindowTexLoad(0, 0);
        debugBattleConfig();
        testScreen();
        xglSleep();
        testScreen();
        battleProc();
    }
}

/* Draws a debug grid of horizontal and vertical lines, tinting the outer lines, and sends it as one packet. */
void testScreen(void)
{
    GrPacket packet;
    GrF2Vertices line;
    int y;
    int x;

    grGpInit(&packet);

    line.points[0].w = 1.0f;
    line.points[1].w = 1.0f;
    line.color[3] = 0x80;
    line.points[0].z = 16777213.0f;
    line.points[1].z = 16777213.0f;
    grOpenF2(&packet);

    line.points[1].x = 512.0f;
    line.points[0].x = 0.0f;
    for (y = 0; y <= 448; y += 7) {
        float yf;

        if (y == 0 || y == 448) {
            line.color[0] = 0xFF;
            line.color[1] = 0x80;
            line.color[2] = 0x80;
        } else {
            line.color[0] = 0xFF;
            line.color[1] = 0xFF;
            line.color[2] = 0xFF;
        }
        yf = (float)y;
        line.points[1].y = yf;
        line.points[0].y = yf;
        grPutF2(&packet, &line, 0);
    }

    line.points[0].y = 0.0f;
    line.points[1].y = 448.0f;
    for (x = 0; x <= 512; x += 8) {
        float xf;

        if (x == 0 || x == 512) {
            line.color[0] = 0xFF;
            line.color[1] = 0x80;
            line.color[2] = 0x80;
        } else {
            line.color[0] = 0xFF;
            line.color[1] = 0xFF;
            line.color[2] = 0xFF;
        }
        xf = (float)x;
        line.points[1].x = xf;
        line.points[0].x = xf;
        grPutF2(&packet, &line, 0);
    }

    grCloseF2();
    grPacketSend(&packet);
}
