/*
 * TU-local declarations of ov12/tu087 (src/ov12/xrg_sysinit.c).
 */

#ifndef SRC_OV12_XRG_SYSINIT_H
#define SRC_OV12_XRG_SYSINIT_H

#include "shared.h"

void XrgSystemInit(void);

extern const char xrg_system_init_log_1[];

extern const char xrg_system_init_source_file[];

extern const char xrg_system_init_log_2[];

extern const char xrg_system_init_log_3[];

extern const char xrg_system_init_log_4[];

extern const char xrg_system_init_log_5[];

extern const char xrg_system_init_log_6[];

extern const char xrg_system_init_log_7[];

extern const char xrg_system_init_log_8[];

extern void XrgLogSys(const char *, const char *, int, ...);

extern RgHeap *InstanceOfRgHeap(void);

extern RgHeap *InstanceOfRgHeapData(void);

extern void InitRgHeap(RgHeap *, void *, u32);

extern void RgSingletonIDClear(void);

extern void ClearRgHeap(RgHeap *);

extern void _SetDefaultLight(void);

extern void xglRenderClearFrame(void);

extern void xglRenderClearColor(u32);

void XrgSleep(void);

#endif /* SRC_OV12_XRG_SYSINIT_H */
