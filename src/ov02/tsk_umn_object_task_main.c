/*
 * OV02 original TU 5: 0x00a023a8..0x00a02d68 (7 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"
#include "main/menu_model.h"
#include "main/xgl_studio.h"

#include "tsk_umn_object_task_main.h"
#include "main/xgl_task.h"
extern int UmnSimulationNo;
extern int UmnMemory;
extern unsigned char *umn_task;
extern unsigned char *task_xmx;
extern int UmnWorkEndTop;
extern int UmnEventTextMakeWork;
extern void endPrintInit(void);
extern void GameCFSoundMenuPurge(int flag);
extern void xglRenderClearDepth(void);
extern int xglSoundSendSwd(void *swd, int bank);
extern int SsdSpuDmaCompleted(int wait);
extern int xglSoundSendSmd2(void *smd, int bank);
extern void xglSoundSequenceNormal2(int channel, int volume);
extern void xglFSrand(float seed);
extern void tyaUmlDispInit(void);
extern void tyaUmlDispInit2(unsigned char *work_buffer);
extern void *UmnInterface2Init(void *work);
extern void *UmnBgCubeInit(void *work);
extern void *eBattleWinInit2(void *arg);
extern int UmnMailHeaderCreate(int work);
extern int UmnHistoryTreeLoad(int work);
extern XglPacket *xglPacketGetCurrent(void);
extern void endPrintDirectFrameCopy(XglPacket *packet, int width, int height);
extern void xglSleep(void);
extern void *memset(void *dest, int value, unsigned int size);
typedef struct {
    unsigned char unmodeled_00[0x10];
    unsigned short width;
    unsigned short height;
    unsigned short height2;
} UmnMainRenderSize;
extern UmnMainRenderSize sRender;
extern int MenuModelWorkTop;
extern void MenuLoadInit(void);
extern void MenuModelMemoryInit(void *base, int size);
extern void MenuModelMemorySet(int count);
extern int UmnModelUkn;
extern int UmnModelSon;
extern void UmnChangeTopLevel(int level);
extern void GameSnapShotCheck(void);
extern void UmnTopMenu(void);
extern void UmnMail(int menuId);
extern void UmnDataBase(void);
extern void UmnSimulation(void);
extern void UmnPlugin(void);
extern void endPrintExtFunc(int kind, int id, void *data);
extern void UmnInterface2Main(void);
extern void MenuModelMain(void);
extern void MenuModelAllBreak(void);
extern void GameCFSoundMenuReload(void);

int tskUmnObjectTaskMain(XglTaskPrefix *base) {
    UmnObjectTask *task = (UmnObjectTask *)base;
    UmnObjectTaskWorker worker = task->worker;

    if (UmnWork.mode == 0xff)
        task->state = -1;

    switch (task->state) {
    case 0:
        worker(task, task->data);
        task->state = 2;
        /* fall through */
    case 2:
        worker(task, task->data);
        break;
    case -1:
        xglTaskWaitRemove(base);
        break;
    }
}

void UmnObjectTaskCreate(UmnObjectTaskWorker worker, void *data) {
    XglTaskScheduler *scheduler = (XglTaskScheduler *)umn_task;
    UmnObjectTask *task = (UmnObjectTask *)xglTaskEntryNext(
        scheduler, tskUmnObjectTaskMain,
        scheduler != 0 ? scheduler->active_tail : 0);
    int i;

    task->state = 0;
    task->worker = worker;
    task->data = data;
    for (i = 24; i >= 0; --i)
        task->zeroedOnCreate[i] = 0;
}

extern void UmnProcuratorSet(void);

void UmnChangeTopLevel(int level) {
    UmnWorkQuadword zero = 0;
    int index = 0;
    int remaining;

    for (remaining = 6; remaining >= 0; remaining--)
        UmnWork.section_state[index++] = zero;

    UmnWork.mode = level;
    UmnWork.stateOrCategory = 0;
    UmnProcuratorSet();
}

/* UmnTextLoad is ov02:0x00a0f578 (src/ov02/umn_event_text_symbol_check.c). */
extern int UmnTextLoad(int work, int mode);

/* "data\\endou\\umn\\cube.xtx", "data\\endou\\umn\\cube.lex" and
 * "data\\endou\\umn\\umn00.xtx" filename buffers loaded below. */
static const char D_00A11D40[24] = "data\\endou\\umn\\cube.xtx";
static const char D_00A11D58[24] = "data\\endou\\umn\\cube.lex";
static const char D_00A11D70[32] = "data\\endou\\umn\\umn00.xtx";
static const char D_00A11D90[32] = "data\\sound\\smd\\Umnmode.SWD";
static const char D_00A11DB0[32] = "data\\sound\\smd\\Umnmode.SMD";

extern int UmnBgCubeXtx;
extern int UmnBgCubeLex;
extern int UmnTexAddr;
extern int UmnWorkEnd;

static void UmnFirstLoad(void) {
    UmnWorkEnd = UmnTextLoad(UmnWorkEnd, 0);
    xglCdReadFile(D_00A11D40, (void *)UmnBgCubeXtx, 0, 1);
    xglCdReadFile(D_00A11D58, (void *)UmnBgCubeLex, 0, 1);
    xglCdReadFile(D_00A11D70, (void *)UmnTexAddr, 0, 1);
}

static void UmnInit(void) {
    int buffer;
    int base;
    int next;

    endPrintInit();
    GameCFSoundMenuPurge(1);
    UmnSimulationNo = 0;
    xglRenderClearDepth();

    buffer = (UmnMemory + 0xF) & ~0xF;
    xglCdReadFile(D_00A11D90, (void *)buffer, 0, 1);
    xglSoundSendSwd((void *)buffer, -5);
    while (SsdSpuDmaCompleted(0))
        ;

    xglCdReadFile(D_00A11DB0, (void *)buffer, 0, 1);
    xglSoundSendSmd2((void *)buffer, 4);
    xglSoundSequenceNormal2(4, 0x7F);
    xglFSrand(0.12345679f);
    tyaUmlDispInit();
    tyaUmlDispInit2((unsigned char *)UmnMemory);

    base = UmnMemory;
    UmnBgCubeXtx = base + 0x180000;
    UmnBgCubeLex = base + 0x190000;
    UmnTexAddr = base + 0x1A0000;
    UmnWorkEndTop = base + 0x1E0800;
    UmnMemory = base + 0x220800;
    UmnWorkEnd = UmnWorkEndTop;
    umn_task = (unsigned char *)UmnWorkEndTop;
    UmnWorkEnd = (int)xglTaskInitial((void *)UmnWorkEndTop, 0x20, 0);

    task_xmx = (unsigned char *)UmnWorkEnd;
    UmnWorkEnd = (int)xglTaskInitial((void *)UmnWorkEnd, 0x20, 0);
    UmnWorkEnd = (int)UmnInterface2Init((void *)UmnWorkEnd);
    UmnWorkEnd = (int)UmnBgCubeInit((void *)UmnWorkEnd);
    UmnWorkEnd = (int)eBattleWinInit2((void *)UmnWorkEnd);
    UmnWorkEnd = UmnMailHeaderCreate(UmnWorkEnd);
    next = UmnHistoryTreeLoad(UmnWorkEnd);

    UmnEventTextMakeWork = next;
    UmnWorkEnd = next + 0x2000;
}

void UmnMain(void) {
    XglPacket *packet;

    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.width << 5, sRender.height << 5);
    xglSleep();

    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.height << 5, sRender.height2 << 5);

    UmnMemory = MenuModelWorkTop;
    memset((void *)UmnMemory, 0, 0x480000);

    UmnInit();
    MenuLoadInit();
    MenuModelMemoryInit((void *)UmnMemory, 0x200000);
    MenuModelMemorySet(0);

    UmnModelUkn = 0;
    UmnModelSon = 0;
    UmnChangeTopLevel(0);
    UmnFirstLoad();

    for (;;) {
        GameSnapShotCheck();
        switch (UmnWork.mode) {
        case 0:
            UmnTopMenu();
            break;
        case 1:
            UmnMail(0);
            break;
        case 2:
            UmnDataBase();
            break;
        case 3:
            UmnSimulation();
            break;
        case 4:
            UmnPlugin();
            break;
        default:
            UmnChangeTopLevel(0);
            break;
        }

        endPrintExtFunc(0, 0x64, 0);
        UmnInterface2Main();
        xglTaskExecute((XglTaskScheduler *)task_xmx);
        MenuModelMain();
        xglTaskExecute((XglTaskScheduler *)umn_task);
        xglSleep();

        if (UmnWork.mode != 0xff) {
            if (UmnSimulationNo == 0)
                continue;
            UmnWork.mode = 0xff;
        }
        break;
    }

    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.width << 5, sRender.height << 5);
    xglTaskExecute((XglTaskScheduler *)task_xmx);
    xglTaskExecute((XglTaskScheduler *)umn_task);
    MenuModelAllBreak();
    xglSleep();
    GameCFSoundMenuReload();
}

extern StudioLight *xglStudioGetLight2(void);
extern void xglStudioChange(int mode);
extern void xglLightIntensityAmbient(void *light, void *values);
extern void xglLightIntensityParallel(void *light, int index, void *values);
extern void xglLightDirection(void *light, int index, void *values);
extern void MenuCfTaikiPush(void);
extern void MenuCfTaikiPop(void);
extern void MenuGameDataPush(void);
extern void MenuGameDataPop(void);
extern void MenuLoadCancel(void);
extern void MenuFontLoad(int mode, int id);
extern void WindowTexLoad(int mode, int id);
extern void *GameResourceWorkAlloc(int size);
extern void GameResourceWorkReload(void);
extern void GameCfPlayerLoadResource(int id);
extern void GameSnapShotCheck(void);
extern void VibrationEngine(void);
extern unsigned int GameLoopState[];

typedef struct UmnLightTable {
    UmnWorkQuadword ambient;
    UmnWorkQuadword parallel[3];
    UmnWorkQuadword direction[3];
} UmnLightTable;

extern UmnLightTable D_00A11DF0;

void UmnMain2(int menuId) {
    XglPacket *packet;
    StudioLight *light;
    UmnLightTable lights;

    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.width << 5, sRender.height << 5);
    xglSleep();
    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.height << 5, sRender.height2 << 5);

    GameLoopState[4] |= 0x04000000;
    MenuCfTaikiPush();
    MenuGameDataPush();
    GameCFSoundMenuPurge(0);
    MenuLoadInit();
    xglStudioChange(0);
    light = xglStudioGetLight2();
    lights = D_00A11DF0;
    xglLightIntensityAmbient(light, &lights.ambient);
    xglLightIntensityParallel(light, 0, &lights.parallel[0]);
    xglLightDirection(light, 0, &lights.direction[0]);
    xglLightIntensityParallel(light, 1, &lights.parallel[1]);
    xglLightDirection(light, 1, &lights.direction[1]);
    xglLightIntensityParallel(light, 2, &lights.parallel[2]);
    xglLightDirection(light, 2, &lights.direction[2]);

    UmnMemory = (int)GameResourceWorkAlloc(0x480000);
    memset((void *)UmnMemory, 0, 0x480000);

    UmnInit();
    MenuLoadInit();
    UmnWorkEnd = (int)MenuModelInit((void *)UmnWorkEnd);
    MenuModelMemoryInit((void *)UmnMemory, 0x200000);
    MenuModelMemorySet(0);

    UmnModelUkn = 0;
    UmnModelSon = 0;
    UmnChangeTopLevel(1);
    UmnFirstLoad();
    MenuFontLoad(1, 0);
    WindowTexLoad(0, 1);
    WindowTexLoad(0, 2);

    while (!(UmnModelUkn != 0 && ((MenuModelUnit *)UmnModelUkn)->active != 0)) {
        UmnInterface2Main();
        MenuModelMain();
        xglSleep();
    }

    if (UmnWork.mode == 1) {
        do {
            GameSnapShotCheck();
            UmnMail(menuId);
            UmnInterface2Main();
            endPrintExtFunc(0, 0x64, 0);
            xglTaskExecute((XglTaskScheduler *)task_xmx);
            MenuModelMain();
            xglTaskExecute((XglTaskScheduler *)umn_task);
            VibrationEngine();
            xglSleep();
        } while (UmnWork.mode == 1);
    }

    MenuLoadCancel();
    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.width << 5, sRender.height << 5);
    xglTaskExecute((XglTaskScheduler *)task_xmx);
    xglTaskExecute((XglTaskScheduler *)umn_task);
    MenuModelAllBreak();
    xglSleep();
    MenuCfTaikiPop();
    MenuFontLoad(0, 0);
    WindowTexLoad(0, 0);
    GameResourceWorkReload();
    GameCfPlayerLoadResource(1);
    MenuGameDataPop();
    GameCFSoundMenuReload();
    GameLoopState[4] &= 0xFBFFFFFF;
}
