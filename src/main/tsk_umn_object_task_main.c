/* UMN code embedded in MAIN, original OV02 translation unit 5. */
#include "common.h"

#include "shared.h"
#include "main/xgl_cd.h"
#include "ov02/tsk_umn_object_task_main.h"
#include "main/xgl_task.h"

/* TU-local types of ov02/tu005 (src/ov02/tsk_umn_object_task_main.h). */
typedef struct UmnObjectTask UmnObjectTask;

typedef void (*UmnObjectTaskWorker)(UmnObjectTask *task, void *data);

/* xglTaskEntryNext allocates a 0x80-byte task; these are the fields read here. */
struct UmnObjectTask {
    XglTaskPrefix base;
    int state;
    UmnObjectTaskWorker worker;
    void *data;
    int zeroedOnCreate[0x19];
};

typedef struct {
    unsigned char unmodeled_00[0x10];
    unsigned short width;
    unsigned short height;
    unsigned short height2;
} UmnMainRenderSize;

extern UmnWorkData UmnWork;
extern int UmnSimulationNo;
extern int UmnMemory;
extern unsigned char *umn_task;
extern unsigned char *task_xmx;
extern int UmnWorkEndTop;
extern int UmnEventTextMakeWork;
extern UmnMainRenderSize sRender;
extern int MenuModelWorkTop;
extern int UmnModelUkn;
extern int UmnModelSon;

/* "data\\endou\\umn\\cube.xtx", "data\\endou\\umn\\cube.lex" and
 * "data\\endou\\umn\\umn00.xtx" filename buffers loaded below; original
 * embedded-OV02 storage, no new data definitions. */
extern const char D_00A11D40[24];

extern const char D_00A11D58[24];
extern const char D_00A11D70[32];
extern const char D_00A11D90[32];
extern const char D_00A11DB0[32];
extern int UmnBgCubeXtx;
extern int UmnBgCubeLex;
extern int UmnTexAddr;
extern int UmnWorkEnd;

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
extern void MenuLoadInit(void);
extern void MenuModelMemoryInit(void *base, int size);
extern void MenuModelMemorySet(int count);
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
extern void UmnProcuratorSet(void);

/* UmnTextLoad is ov02:0x00a0f578 (src/ov02/umn_event_text_symbol_check.c). */
extern int UmnTextLoad(int work, int mode);

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

INCLUDE_ASM("asm/main/nonmatchings/ov02/05_tsk_umn_object_task_main", UmnMain);

INCLUDE_ASM("asm/main/nonmatchings/ov02/05_tsk_umn_object_task_main", UmnMain2);
