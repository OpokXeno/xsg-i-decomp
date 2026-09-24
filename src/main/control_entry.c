#include "common.h"
#include "shared.h"

extern int main_param_argc;
extern int main_param_argv;

extern void BootDisplay(void);
extern void xglThreadInitial(void);
extern void xglThreadRotate(void);

/*
 * InitializeSystem is file-local (LOCAL in the original symbol table) and is
 * still asm, so it is declared static before its use in main below.
 */
static void InitializeSystem(void);

INCLUDE_ASM("asm/main/nonmatchings/control_entry", ControlEntry);

/*
 * SCE SDK entry points InitializeSystem calls to bring the IOP and its
 * CD/file-system modules up. Their own translation units are SCE SDK
 * (out of recovery scope) and stay assembly, so the prototypes are declared
 * here from this call site's argument and return usage.
 */
extern void sceSifInitRpc(int mode);
extern int sceCdInit(int mode);
extern int sceSifRebootIop(const char *img_file);
extern int sceSifSyncIop(void);
extern int sceSifInitIopHeap(void);
extern int sceSifLoadFileReset(void);
extern int sceCdMmode(int media);
extern int sceFsReset(void);
extern void *sceCdPOffCallback(void (*func)(void), void *old_func);

/*
 * xgl subsystem entry points InitializeSystem calls. Each is defined in a
 * different translation unit of this unit that either is still assembly or
 * does not yet publish a shared header for it, so the prototype is declared
 * here from this call site's evidence.
 */
extern int xglCdSifLoadModule(const char *path, int flags);
extern void xglCdPowerOffCB(void);
extern void xglSoundInitial(void);
extern void xglCdInitial(void);
extern void xglPadInitial(void);
extern void xglMcInitial(void);
extern void *xglTaskInitial(void *pool, int capacity, int flags);
extern void xglDmaInitial(void);
extern void xglGeometryInit(void);
extern void xglPacketInit(void);
extern void xglRenderInit(void);
extern void xglFontInitial(void);
extern int xglMovieInit(void);
extern void xglMenuInitial(void);

extern const char D_004BE290[];
extern const char D_004D8A08[];
extern const char D_004D8A10[];
extern const char D_004D8A18[];
extern const char D_004D8A20[];
extern const char D_004D8A28[];
extern const char D_004D8A30[];
extern const char D_004D8A38[];

static void InitializeSystem(void)
{
    sceSifInitRpc(0);
    sceCdInit(0);
    while (sceSifRebootIop(D_004BE290) == 0) {
    }
    while (sceSifSyncIop() == 0) {
    }
    sceSifInitRpc(0);
    sceSifInitIopHeap();
    sceSifLoadFileReset();
    sceCdInit(0);
    sceCdMmode(2);
    sceFsReset();
    xglCdSifLoadModule(D_004D8A08, 0);
    xglCdSifLoadModule(D_004D8A10, 0);
    xglCdSifLoadModule(D_004D8A18, 0);
    xglCdSifLoadModule(D_004D8A20, 0);
    xglCdSifLoadModule(D_004D8A28, 0);
    xglCdSifLoadModule(D_004D8A30, 0);
    xglCdSifLoadModule(D_004D8A38, 0);
    sceCdPOffCallback(xglCdPowerOffCB, 0);
    WorkEnd = (u8 *)0xA80000;
    xglSoundInitial();
    xglCdInitial();
    xglPadInitial();
    xglMcInitial();
    xglTaskInitial(0, 0, 0);
    xglDmaInitial();
    xglGeometryInit();
    xglPacketInit();
    xglRenderInit();
    xglFontInitial();
    xglMovieInit();
    xglMenuInitial();
}

/*
 * The compiler emits the call to __main (running static/global constructors)
 * at entry to any function literally named main; that call precedes the
 * statements below and needs no explicit source-level call.
 */
int main(int argc, int argv)
{
    main_param_argc = argc;
    main_param_argv = argv;
    BootDisplay();
    InitializeSystem();
    xglThreadInitial();
    xglThreadRotate();
    return 0;
}
