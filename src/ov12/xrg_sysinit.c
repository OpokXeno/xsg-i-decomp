/*
 * OV12 original TU 87: 0x00a4c840..0x00a4caf8 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "ov12/xrg_rand_int.h"
#include "xrg_sysinit.h"

/*
 * Partial view of the light-set block xglStudioGetLight2 (0x0022c698) hands
 * back: src/main/xgl_studio.h models the full 0xf0-byte block as StudioLight
 * with the span from +0x70 on left opaque, and src/main/xgl_light.h models
 * only its first 0x70 bytes as XglLightSet. xglLightCalcMatrix (0x0022ade0,
 * src/main/xgl_light.c) writes its own two 4x4 output matrices into that
 * opaque span, at +0x70 (the light directions, transposed into columns with
 * a trailing (0,0,0,1) row) and +0xB0 (the light colors); _SetDefaultLight
 * only reaches those two matrices, so nothing before +0x70 is modeled here.
 */
typedef struct {
    unsigned char unmodeled_00[0x70];
    RgMatrix direction;
    RgMatrix color;
} XrgLightMatrixSet;

extern XrgLightMatrixSet *xglStudioGetLight2(void);
extern void xglLightCalcMatrix(XrgLightMatrixSet *lightSet);
extern void XrgUnitMatrix(RgMatrix destination);
extern float XrgNormalizeVector(RgVector destination, RgVector source);
extern RgVector ainLight_0[3];
extern RgVector ainLightCol_1[3];

static void _SetDefaultLight(void) {
    XrgLightMatrixSet *lightSet;
    float *directionDst;
    float *colorPtr;
    float *colorDst;
    RgVector direction;
    int i;

    lightSet = xglStudioGetLight2();
    directionDst = lightSet->direction;
    colorPtr = lightSet->color;
    XrgUnitMatrix(directionDst);
    colorDst = colorPtr;
    XrgUnitMatrix(colorPtr);

    for (i = 0; i < 3; i++) {
        XrgCopyVector(direction, ainLight_0[i]);
        XrgNormalizeVector(direction, direction);
        directionDst[i] = direction[0];
        directionDst[4 + i] = direction[1];
        directionDst[8 + i] = direction[2];
        directionDst[12 + i] = 0.0f;

        colorDst[i * 4] = ainLightCol_1[i][0];
        colorDst[i * 4 + 1] = ainLightCol_1[i][1];
        colorDst[i * 4 + 2] = ainLightCol_1[i][2];
        colorDst[i * 4] = 1.0f;
    }

    xglLightCalcMatrix(lightSet);
}

void XrgSystemInit(void) {
    const char *source_file = xrg_system_init_source_file;

    XrgLogSys(xrg_system_init_log_1, source_file, 0x47);
    XrgLogSys(xrg_system_init_log_2, source_file, 0x4c);
    XrgLogSys(xrg_system_init_log_3, source_file, 0x5b);
    InitRgHeap(InstanceOfRgHeap(), (void *)0x01000000, 0x00100000);
    InitRgHeap(InstanceOfRgHeapData(), (void *)0x01100000, 0x00f00000);
    XrgLogSys(xrg_system_init_log_4, source_file, 0x65);
    xglRenderClearFrame();
    xglRenderClearColor(0x80000000u);
    XrgLogSys(xrg_system_init_log_5, source_file, 0x6a);
    RgSingletonIDClear();
    ClearRgHeap(InstanceOfRgHeap());
    XrgLogSys(xrg_system_init_log_6, source_file, 0x70);
    _SetDefaultLight();
    XrgLogSys(xrg_system_init_log_7, source_file, 0x75);
    XrgLogSys(xrg_system_init_log_8, source_file, 0x7a);
}

extern void ACT_init(void);
extern void RgHeapDump_sub(RgHeap *pHeap, const char *comment,
                           const char *source_file, int line);
extern void RgSingletonDispose(void);

/*
 * External file-backed witnesses, not candidate-emitted data.
 *
 * ov12:0x00a591c0 contains the tag "system dispose".
 * ov12:0x00a591d0 contains the tag "system dispose 2".
 */
extern const char D_00A591C0[];
extern const char D_00A591D0[];

void XrgSystemDispose(void) {
    RgSingletonDispose();
    RgHeapDump_sub(InstanceOfRgHeap(), D_00A591C0, xrg_system_init_source_file, 0x85);
    RgHeapDump_sub(InstanceOfRgHeapData(), D_00A591D0, xrg_system_init_source_file, 0x86);
    ClearRgHeap(InstanceOfRgHeap());
    ACT_init();
}

void XrgSleep(void) {
    xglSleep();
}
