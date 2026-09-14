/*
 * OV12 original TU 87: 0x00a4c840..0x00a4caf8 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "xrg_sysinit.h"

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sysinit", _SetDefaultLight);

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

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sysinit", XrgSystemDispose);

void XrgSleep(void) {
    xglSleep();
}
