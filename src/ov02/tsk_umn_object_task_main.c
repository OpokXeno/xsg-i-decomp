/*
 * OV02 original TU 5: 0x00a023a8..0x00a02d68 (7 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/e_battle_win_open.h"
#include "main/end_print.h"
#include "main/game.h"
#include "main/game_snap_shot.h"
#include "main/look_at_player.h"
#include "main/menu_model.h"
#include "main/res_get_path.h"
#include "main/vibration.h"
#include "main/party.h"
#include "main/xgl_cd.h"
#include "main/xgl_light.h"
#include "main/xgl_packet.h"
#include "main/xgl_sound.h"
#include "main/xgl_studio.h"
#include "main/xgl_task.h"
#include "main/xgl_thread.h"
#include "ov02/ov02.h"
#include "ov02/task_umn_top_win.h"
#include "ov02/title_game_init.h"
#include "ov02/title_hdd_install.h"
#include "ov02/tsk_umn_object_task_main.h"
#include "ov02/tsk_umn_plugin_pas.h"
#include "ov02/tsk_umn_simulation_pas.h"
#include "ov02/tsk_umn_mail_pas.h"
#include "ov02/tya_uml.h"
#include "ov02/umn_event_text_symbol_check.h"
#include "ov02/umn_guno_data_base_get.h"
#include "ov02/umn_interface2.h"
#include "ov02/umn_procurator.h"

/* Rows of UmnWork past the common prefix that UmnChangeTopLevel clears. */
#define UMN_WORK_SECTION_ROWS 7

/*
 * The memory block UmnMain reserves (UmnMemory): MenuModel memory comes first;
 * UmnInit places the cube texture, the cube model, the texture archive and
 * the task area at these offsets.
 */
#define UMN_ARENA_SIZE 0x480000
#define UMN_MODEL_MEMORY_SIZE 0x200000
#define UMN_CUBE_XTX_OFFSET 0x180000
#define UMN_CUBE_LEX_OFFSET 0x190000
#define UMN_TEXTURE_OFFSET 0x1A0000
#define UMN_WORK_OFFSET 0x1E0800
#define UMN_MEMORY_NEXT_OFFSET 0x220800

/* Entries of the two task schedulers (umn_task and task_xmx). */
#define UMN_TASK_COUNT 0x20

/* Sequence slot and volume the Umnmode.SMD plays at. */
#define UMN_SEQUENCE_SLOT 4
#define UMN_SEQUENCE_VOLUME 0x7F

typedef unsigned int UmnWorkQuadword __attribute__((mode(TI)));

/* UmnWork read as 16-byte rows: UmnChangeTopLevel clears rows 1..7 at once. */
typedef struct UmnWorkRows {
    unsigned char unmodeled_00[0x10];
    UmnWorkQuadword section_state[UMN_WORK_SECTION_ROWS];
} UmnWorkRows;

typedef struct {
    unsigned char unmodeled_00[0x10];
    unsigned short width;
    unsigned short height;
    unsigned short height2;
} UmnMainRenderSize;

typedef struct UmnLightTable {
    XglLightQuadword ambient;
    XglLightQuadword parallel[3];
    XglLightVector direction[3];
} UmnLightTable;

extern UmnMainRenderSize sRender;
extern unsigned char *MenuModelWorkTop;
extern void xglRenderClearDepth(void);
extern int SsdSpuDmaCompleted(int wait);
extern void xglFSrand(float seed);
extern void GameCFSoundMenuPurge(int flag);
extern void GameCFSoundMenuReload(void);
extern UmnLightTable D_00A11DF0;
extern int UmnEventTextMakeWork;

int tskUmnObjectTaskMain(XglTaskPrefix *base) {
    UmnObjectTask *task = (UmnObjectTask *)base;
    UmnObjectTaskWorker worker = task->worker;

    if (UmnWork.mode == UMN_MODE_NONE)
        task->state = UMN_TASK_END;

    switch (task->state) {
    case UMN_TASK_INIT:
        worker(task, task->data);
        task->state = UMN_TASK_RUN;
        /* fall through */
    case UMN_TASK_RUN:
        worker(task, task->data);
        break;
    case UMN_TASK_END:
        xglTaskWaitRemove(base);
        break;
    }
}

void UmnObjectTaskCreate(UmnObjectTaskWorker worker, void *data) {
    XglTaskScheduler *scheduler = umn_task;
    UmnObjectTask *task = (UmnObjectTask *)xglTaskEntryNext(
        scheduler, tskUmnObjectTaskMain,
        scheduler != 0 ? scheduler->active_tail : 0);
    int i;

    task->state = UMN_TASK_INIT;
    task->worker = worker;
    task->data = data;
    for (i = UMN_TASK_CLEAR_COUNT - 1; i >= 0; --i)
        task->zeroedOnCreate[i] = 0;
}

void UmnChangeTopLevel(int level) {
    UmnWorkQuadword zero = 0;
    int index = 0;
    int remaining;

    for (remaining = UMN_WORK_SECTION_ROWS - 1; remaining >= 0; remaining--)
        ((UmnWorkRows *)&UmnWork)->section_state[index++] = zero;

    UmnWork.mode = level;
    UmnWork.tab_state = 0;
    UmnProcuratorSet();
}

/* "data\\endou\\umn\\cube.xtx", "data\\endou\\umn\\cube.lex" and
 * "data\\endou\\umn\\umn00.xtx" filename buffers loaded below. */
static const char D_00A11D40[24] = "data\\endou\\umn\\cube.xtx";
static const char D_00A11D58[24] = "data\\endou\\umn\\cube.lex";
static const char D_00A11D70[32] = "data\\endou\\umn\\umn00.xtx";
static const char D_00A11D90[32] = "data\\sound\\smd\\Umnmode.SWD";
static const char D_00A11DB0[32] = "data\\sound\\smd\\Umnmode.SMD";

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
    xglSoundSendSmd2((void *)buffer, UMN_SEQUENCE_SLOT);
    xglSoundSequenceNormal2(UMN_SEQUENCE_SLOT, UMN_SEQUENCE_VOLUME);
    xglFSrand(0.12345679f);
    tyaUmlDispInit();
    tyaUmlDispInit2((unsigned char *)UmnMemory);

    base = UmnMemory;
    UmnBgCubeXtx = base + UMN_CUBE_XTX_OFFSET;
    UmnBgCubeLex = base + UMN_CUBE_LEX_OFFSET;
    UmnTexAddr = base + UMN_TEXTURE_OFFSET;
    UmnWorkEndTop = base + UMN_WORK_OFFSET;
    UmnMemory = base + UMN_MEMORY_NEXT_OFFSET;
    UmnWorkEnd = UmnWorkEndTop;
    umn_task = (XglTaskScheduler *)UmnWorkEndTop;
    UmnWorkEnd = (int)xglTaskInitial((void *)UmnWorkEndTop, UMN_TASK_COUNT, 0);

    task_xmx = (XglTaskScheduler *)UmnWorkEnd;
    UmnWorkEnd = (int)xglTaskInitial((void *)UmnWorkEnd, UMN_TASK_COUNT, 0);
    UmnWorkEnd = (int)UmnInterface2Init((void *)UmnWorkEnd);
    UmnWorkEnd = (int)UmnBgCubeInit((void *)UmnWorkEnd);
    UmnWorkEnd = (int)eBattleWinInit2((void *)UmnWorkEnd);
    UmnWorkEnd = UmnMailHeaderCreate(UmnWorkEnd);
    next = UmnHistoryTreeLoad(UmnWorkEnd);

    UmnEventTextMakeWork = next;
    UmnWorkEnd = next + UMN_EVENT_TEXT_BUFFER_SIZE;
}

void UmnMain(void) {
    XglPacket *packet;

    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.width << 5, sRender.height << 5);
    xglSleep();

    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.height << 5, sRender.height2 << 5);

    UmnMemory = (int)MenuModelWorkTop;
    memset((void *)UmnMemory, 0, UMN_ARENA_SIZE);

    UmnInit();
    MenuLoadInit();
    MenuModelMemoryInit((void *)UmnMemory, UMN_MODEL_MEMORY_SIZE);
    MenuModelMemorySet(0);

    UmnModelUkn = 0;
    UmnModelSon = 0;
    UmnChangeTopLevel(UMN_MODE_TOP_MENU);
    UmnFirstLoad();

    for (;;) {
        GameSnapShotCheck();
        switch (UmnWork.mode) {
        case UMN_MODE_TOP_MENU:
            UmnTopMenu();
            break;
        case UMN_MODE_MAIL:
            UmnMail(0);
            break;
        case UMN_MODE_DATA_BASE:
            UmnDataBase();
            break;
        case UMN_MODE_SIMULATION:
            UmnSimulation();
            break;
        case UMN_MODE_PLUGIN:
            UmnPlugin();
            break;
        default:
            UmnChangeTopLevel(UMN_MODE_TOP_MENU);
            break;
        }

        endPrintExtFunc(0, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, 0);
        UmnInterface2Main();
        xglTaskExecute(task_xmx);
        MenuModelMain();
        xglTaskExecute(umn_task);
        xglSleep();

        if (UmnWork.mode != UMN_MODE_NONE) {
            if (UmnSimulationNo == 0)
                continue;
            UmnWork.mode = UMN_MODE_NONE;
        }
        break;
    }

    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.width << 5, sRender.height << 5);
    xglTaskExecute(task_xmx);
    xglTaskExecute(umn_task);
    MenuModelAllBreak();
    xglSleep();
    GameCFSoundMenuReload();
}

void UmnMain2(int menuId) {
    XglPacket *packet;
    StudioLight *light;
    UmnLightTable lights;

    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.width << 5, sRender.height << 5);
    xglSleep();
    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.height << 5, sRender.height2 << 5);

    GameLoopState.flags |= GAME_FLAG_UMN;
    MenuCfTaikiPush();
    MenuGameDataPush();
    GameCFSoundMenuPurge(0);
    MenuLoadInit();
    xglStudioChange(0);
    light = xglStudioGetLight2();
    lights = D_00A11DF0;
    xglLightIntensityAmbient(light, &lights.ambient);
    xglLightIntensityParallel(light, 0, &lights.parallel[0]);
    xglLightDirection(light, 0, &lights.direction[0].value);
    xglLightIntensityParallel(light, 1, &lights.parallel[1]);
    xglLightDirection(light, 1, &lights.direction[1].value);
    xglLightIntensityParallel(light, 2, &lights.parallel[2]);
    xglLightDirection(light, 2, &lights.direction[2].value);

    UmnMemory = (int)GameResourceWorkAlloc(UMN_ARENA_SIZE);
    memset((void *)UmnMemory, 0, UMN_ARENA_SIZE);

    UmnInit();
    MenuLoadInit();
    UmnWorkEnd = (int)MenuModelInit((void *)UmnWorkEnd);
    MenuModelMemoryInit((void *)UmnMemory, UMN_MODEL_MEMORY_SIZE);
    MenuModelMemorySet(0);

    UmnModelUkn = 0;
    UmnModelSon = 0;
    UmnChangeTopLevel(UMN_MODE_MAIL);
    UmnFirstLoad();
    MenuFontLoad(1, 0);
    WindowTexLoad(0, 1);
    WindowTexLoad(0, 2);

    while (!(UmnModelUkn != 0 && UmnModelUkn->active != 0)) {
        UmnInterface2Main();
        MenuModelMain();
        xglSleep();
    }

    if (UmnWork.mode == UMN_MODE_MAIL) {
        do {
            GameSnapShotCheck();
            UmnMail(menuId);
            UmnInterface2Main();
            endPrintExtFunc(0, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, 0);
            xglTaskExecute(task_xmx);
            MenuModelMain();
            xglTaskExecute(umn_task);
            VibrationEngine();
            xglSleep();
        } while (UmnWork.mode == UMN_MODE_MAIL);
    }

    MenuLoadCancel();
    packet = xglPacketGetCurrent();
    endPrintDirectFrameCopy(packet, sRender.width << 5, sRender.height << 5);
    xglTaskExecute(task_xmx);
    xglTaskExecute(umn_task);
    MenuModelAllBreak();
    xglSleep();
    MenuCfTaikiPop();
    MenuFontLoad(0, 0);
    WindowTexLoad(0, 0);
    GameResourceWorkReload();
    GameCfPlayerLoadResource(1);
    MenuGameDataPop();
    GameCFSoundMenuReload();
    GameLoopState.flags &= ~GAME_FLAG_UMN;
}
