/*
 * OV01 original TU 0: 0x00a00000..0x00a00218 (2 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"

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

/*
 * grOpenF2/grPutF2/grCloseF2/grPacketSend's own record (src/ov01/gr_gp_init.c,
 * still asm there; that TU owns the layout and has not published it through a
 * header yet -- shared-header need). This call site only ever takes its
 * address, so the storage is left untyped here rather than restating the
 * owner's struct (the element count matches src/ov01/battle_init.c's
 * curTexTrans, this TU's own already-accepted call site for the same API).
 */
extern void grGpInit(void *packet);
extern void grOpenF2(void *packet);
extern void grPutF2(void *packet, void *vertex, int flags);
extern void grCloseF2(void);
extern void grPacketSend(void *packet);

/*
 * grPutF2's own line-vertex record (evidenced only by this call site): two
 * homogeneous points (screen x/y, plus a fixed z/w pair grOpenF2's setup
 * never varies) and a packed RGBA colour, all in one 36-byte block.
 */
typedef struct F2Point {
    float x;
    float y;
    float z;
    float w;
} F2Point;

typedef struct F2Line {
    F2Point p0;
    F2Point p1;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} F2Line;

void testScreen(void)
{
    int packet[2];
    F2Line line;
    int y;
    int x;

    grGpInit(packet);

    line.p0.w = 1.0f;
    line.p1.w = 1.0f;
    line.a = 0x80;
    line.p0.z = 16777213.0f;
    line.p1.z = 16777213.0f;
    grOpenF2(packet);

    line.p1.x = 512.0f;
    line.p0.x = 0.0f;
    for (y = 0; y <= 448; y += 7) {
        float yf;

        if (y == 0 || y == 448) {
            line.r = 0xFF;
            line.g = 0x80;
            line.b = 0x80;
        } else {
            line.r = 0xFF;
            line.g = 0xFF;
            line.b = 0xFF;
        }
        yf = (float)y;
        line.p1.y = yf;
        line.p0.y = yf;
        grPutF2(packet, &line, 0);
    }

    line.p0.y = 0.0f;
    line.p1.y = 448.0f;
    for (x = 0; x <= 512; x += 8) {
        float xf;

        if (x == 0 || x == 512) {
            line.r = 0xFF;
            line.g = 0x80;
            line.b = 0x80;
        } else {
            line.r = 0xFF;
            line.g = 0xFF;
            line.b = 0xFF;
        }
        xf = (float)x;
        line.p1.x = xf;
        line.p0.x = xf;
        grPutF2(packet, &line, 0);
    }

    grCloseF2();
    grPacketSend(packet);
}
