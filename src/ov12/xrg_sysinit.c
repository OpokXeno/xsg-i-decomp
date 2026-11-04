/*
 * OV12 original TU 87: 0x00a4c840..0x00a4caf8 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/xrg_rand_int.h"
#include "xrg_sysinit.h"
#include "main/act_1.h"
#include "main/xgl_light.h"
#include "main/xgl_studio.h"
#include "ov12/xrg_v_log.h"
#include "main/xgl_render.h"

extern StudioLight *xglStudioGetLight2(void);

/* The two heaps live back to back in the extended RAM the game reserves. */
#define XRG_CODE_HEAP_BASE 0x01000000
#define XRG_CODE_HEAP_SIZE 0x00100000
#define XRG_DATA_HEAP_BASE 0x01100000
#define XRG_DATA_HEAP_SIZE 0x00f00000

/* The number of lights the default set holds. */
#define XRG_DEFAULT_LIGHT_COUNT 3

static const char xrg_system_init_log_1[16] = "sys init 1\n";
const char xrg_system_init_source_file[24] = "../xrg_sysinit.euc.c";
static const char xrg_system_init_log_2[16] = "sys init 2\n";
static const char xrg_system_init_log_3[48] =
    "sys init 3\n\0\0\0\0\0over 32 M (tail %x %x,%x)\n";
static const char xrg_system_init_log_4[16] = "sys init 4\n";
static const char xrg_system_init_log_5[16] = "sys init 5\n";
static const char xrg_system_init_log_6[16] = "sys init 6\n";
static const char xrg_system_init_log_7[16] = "sys init 7\n";
static const char xrg_system_init_log_8[16] = "sys init 8\n";

static RgVector ainLight_0[XRG_DEFAULT_LIGHT_COUNT] = {
    {1.0f, -1.0f, -0.5f, 0.0f},
    {-1.0f, 1.0f, 0.5f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};
static RgVector ainLightCol_1[XRG_DEFAULT_LIGHT_COUNT] = {
    {0.5f, 0.5f, 0.5f, 1.0f},
    {0.3f, 0.38f, 0.3f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
};

static void _SetDefaultLight(void) {
    StudioLight *lightSet;
    float *directionDst;
    float *colorPtr;
    float *colorDst;
    RgVector direction;
    int i;

    lightSet = xglStudioGetLight2();
    directionDst = &lightSet->normal_matrix[0][0];
    colorPtr = &lightSet->color_modifier_matrix[0][0];
    XrgUnitMatrix(directionDst);
    colorDst = colorPtr;
    XrgUnitMatrix(colorPtr);

    for (i = 0; i < XRG_DEFAULT_LIGHT_COUNT; i++) {
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
    InitRgHeap(InstanceOfRgHeap(), (void *)XRG_CODE_HEAP_BASE, XRG_CODE_HEAP_SIZE);
    InitRgHeap(InstanceOfRgHeapData(), (void *)XRG_DATA_HEAP_BASE, XRG_DATA_HEAP_SIZE);
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

static const char sysInitDisposeHeapDumpLabel[16] = "system dispose";
static const char sysInitDisposeHeapDumpLabel2[24] = "system dispose 2";

void XrgSystemDispose(void) {
    RgSingletonDispose();
    RgHeapDump_sub(InstanceOfRgHeap(), sysInitDisposeHeapDumpLabel, xrg_system_init_source_file, 0x85);
    RgHeapDump_sub(InstanceOfRgHeapData(), sysInitDisposeHeapDumpLabel2, xrg_system_init_source_file, 0x86);
    ClearRgHeap(InstanceOfRgHeap());
    ACT_init();
}

void XrgSleep(void) {
    xglSleep();
}
