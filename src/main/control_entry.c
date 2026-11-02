#include "common.h"
#include "shared.h"
#include "main/control_entry.h"
#include "main/xgl_cd.h"
#include "main/xgl_mc.h"
#include "main/xgl_menu.h"
#include "main/xgl_render.h"
#include "main/xgl_thread.h"

/*
 * REGIST/REGIST2 (main:0x0021826c/0x0021827c): the sound-effect bank name
 * and SWD bank name ControlEntry loads before the boot logo. Still owned by
 * asm data.
 */
/* Overlay index of the boot logo / title screen (ov02). */
#define OVERLAY_TITLE 2

/* The title screen returns the chosen screen's entry address in the low 24
 * bits and the overlay index to load in the top byte. */
#define TITLE_ENTRY_MASK 0x00ffffff
#define TITLE_OVERLAY_SHIFT 24

/* First address of the work heap WorkEnd tracks. */
#define WORK_HEAP_START 0xA80000

/* sceCdMmode media: DVD. */
#define SCECdDVD 2

extern char soundEffectBankName[];
extern char sio2manModuleName[];
extern char mcmanModuleName[];
extern char mcservModuleName[];
extern char padmanModuleName[];
extern char libsdModuleName[];
extern char ssdModuleName[];
extern char rssdModuleName[];
extern const char soundRegist2Path[16];
extern const char iopRebootImagePath[32];

extern void BootDisplay(void);
/*
 * InitializeSystem is file-local (LOCAL in the original symbol table) and is
 * still asm, so it is declared static before its use in main below.
 */
static void InitializeSystem(void);

/* ControlEntry subsystem prototypes follow the argument and return
 * types evidenced by these cross-TU call sites. */
extern void xglCdLoadOverlay(int overlayId);

extern void xglSoundLoadEffect(const char *bankName, void *buffer, int mode);
extern void xglSoundLoadSwd(const char *swdName, void *buffer);
extern int SsdAddWaveData(void *data, int size, int wave);
extern int SsdSpuDmaCompleted(int wait);
extern void xglRenderDispOn(void);
extern void LogoFirst(void);
extern int Title(void);

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

/* InitializeSystem subsystem prototypes follow the argument and return
 * types evidenced by these cross-TU call sites. */
extern int xglCdSifLoadModule(const char *path, int flags);

extern void xglSoundInitial(void);
extern void xglPadInitial(void);
extern void *xglTaskInitial(void *pool, int capacity, int flags);
extern void xglDmaInitial(void);
extern void xglGeometryInit(void);
extern void xglPacketInit(void);
extern void xglFontInitial(void);
extern int xglMovieInit(void);
char soundEffectBankName[8] = "REGIST";

char sio2manModuleName[8] = "sio2man";

char mcmanModuleName[8] = "mcman";

char mcservModuleName[8] = "mcserv";

char padmanModuleName[8] = "padman";

char libsdModuleName[8] = "libsd";

char ssdModuleName[8] = "ssd";

char rssdModuleName[8] = "rssd";

u8 *WorkEnd = 0;

int main_param_argc = 0;

char **main_param_argv = 0;

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
    xglSoundLoadEffect(soundEffectBankName, WorkEnd, 0);
    xglSoundLoadSwd(soundRegist2Path, WorkEnd);
    SsdAddWaveData(WorkEnd, 0, 0);
    while (SsdSpuDmaCompleted(0) != 0) {
    }
    xglRenderDispOn();
    xglCdLoadOverlay(OVERLAY_TITLE);
    LogoFirst();

    for (;;) {
        xglCdLoadOverlay(OVERLAY_TITLE);
        selection = Title();
        if (selection == 0) {
            continue;
        }
        entry = (int (*)(void))(selection & TITLE_ENTRY_MASK);
        if (entry == Title) {
            continue;
        }
        xglCdLoadOverlay(selection >> TITLE_OVERLAY_SHIFT);
        entry();
    }
}

static void InitializeSystem(void)
{
    sceSifInitRpc(0);
    sceCdInit(0);
    while (sceSifRebootIop(iopRebootImagePath) == 0) {
    }
    while (sceSifSyncIop() == 0) {
    }
    sceSifInitRpc(0);
    sceSifInitIopHeap();
    sceSifLoadFileReset();
    sceCdInit(0);
    sceCdMmode(SCECdDVD);
    sceFsReset();
    xglCdSifLoadModule(sio2manModuleName, 0);
    xglCdSifLoadModule(mcmanModuleName, 0);
    xglCdSifLoadModule(mcservModuleName, 0);
    xglCdSifLoadModule(padmanModuleName, 0);
    xglCdSifLoadModule(libsdModuleName, 0);
    xglCdSifLoadModule(ssdModuleName, 0);
    xglCdSifLoadModule(rssdModuleName, 0);
    sceCdPOffCallback(xglCdPowerOffCB, 0);
    WorkEnd = (u8 *)WORK_HEAP_START;
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
int main(int argc, char **argv)
{
    main_param_argc = argc;
    main_param_argv = argv;
    BootDisplay();
    InitializeSystem();
    xglThreadInitial();
    xglThreadRotate();
    return 0;
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char soundRegist2Path[16] = "sed\\REGIST2";

const char iopRebootImagePath[32] = "cdrom0:\\IOP\\IOPRP24D.IMG;1";
