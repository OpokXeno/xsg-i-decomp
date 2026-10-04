#include "common.h"
#include "shared.h"

extern char D_004D8A00[];
extern char D_004D8A08[];
extern char D_004D8A10[];
extern char D_004D8A18[];
extern char D_004D8A20[];
extern char D_004D8A28[];
extern char D_004D8A30[];
extern char D_004D8A38[];
extern u8 *WorkEnd;
extern int main_param_argc;
extern int main_param_argv;
extern const char D_004BE280[16];
extern const char D_004BE290[32];

extern void BootDisplay(void);
extern void xglThreadInitial(void);
extern void xglThreadRotate(void);

/*
 * InitializeSystem is file-local (LOCAL in the original symbol table) and is
 * still asm, so it is declared static before its use in main below.
 */
static void InitializeSystem(void);

/*
 * REGIST/REGIST2 (main:0x0021826c/0x0021827c): the sound-effect bank name
 * and SWD bank name ControlEntry loads before the boot logo. Still owned by
 * asm data.
 */

/*
 * xglCdLoadOverlay, the sound and SPU-DMA entry points and the ov02 boot
 * screens ControlEntry drives: each is defined in a different translation
 * unit that is still assembly or does not yet publish a shared header for
 * it, so the prototype is declared here from this call site's evidence.
 */
extern void xglCdLoadOverlay(int overlayId);
extern void xglSoundLoadEffect(const char *bankName, void *buffer, int mode);
extern void xglSoundLoadSwd(const char *swdName, void *buffer);
extern int SsdAddWaveData(void *data, int size, int wave);
extern int SsdSpuDmaCompleted(int wait);
extern void xglRenderDispOn(void);
extern void LogoFirst(void);
extern int Title(void);

/*
 * Loads the boot sound banks, waits for the SPU DMA transfer to finish,
 * shows the boot logo, then repeatedly runs the ov02 title screen: while it
 * returns 0 (no selection yet) or its own entry address (stay on title), it
 * reloads ov02 and calls it again; otherwise the low 24 bits are the chosen
 * screen's entry address and the top byte is the overlay to load before
 * jumping to it. Never returns.
 */
void ControlEntry(void)
{
    unsigned int selection;
    int (*entry)(void);

    xglSleep();
    xglSoundLoadEffect(D_004D8A00, WorkEnd, 0);
    xglSoundLoadSwd(D_004BE280, WorkEnd);
    SsdAddWaveData(WorkEnd, 0, 0);
    while (SsdSpuDmaCompleted(0) != 0) {
    }
    xglRenderDispOn();
    xglCdLoadOverlay(2);
    LogoFirst();

    for (;;) {
        xglCdLoadOverlay(2);
        selection = Title();
        if (selection == 0) {
            continue;
        }
        entry = (int (*)(void))(selection & 0x00ffffff);
        if (entry == Title) {
            continue;
        }
        xglCdLoadOverlay(selection >> 24);
        entry();
    }
}

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

char D_004D8A00[8] = "REGIST";
char D_004D8A08[8] = "sio2man";
char D_004D8A10[8] = "mcman";
char D_004D8A18[8] = "mcserv";
char D_004D8A20[8] = "padman";
char D_004D8A28[8] = "libsd";
char D_004D8A30[8] = "ssd";
char D_004D8A38[8] = "rssd";
u8 *WorkEnd = 0;
int main_param_argc = 0;
int main_param_argv = 0;
const char D_004BE280[16] = "sed\\REGIST2";
const char D_004BE290[32] = "cdrom0:\\IOP\\IOPRP24D.IMG;1";
