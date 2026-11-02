/*
 * OV02 original TU 11: 0x00a0baf8..0x00a0c488 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/e_battle_win_open.h"
#include "main/e_message.h"
#include "main/game.h"
#include "main/end_print.h"
#include "main/menu_1.h"
#include "main/menu_top_command_check.h"
#include "main/party.h"
#include "main/xgl_dma.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"
#include "ov02/ov02.h"
#include "ov02/title_game_init.h"
#include "ov02/title_hdd_install.h"
#include "ov02/tsk_umn_object_task_main.h"
#include "ov02/tsk_umn_simulation_pas.h"

/* Entries the scenario list can hold; the list array adds the "end" row and the terminator. */
#define UMN_SIM_LIST_MAX 7
/* Scenario numbers (MenuScenarioNoGet) from which the next simulations are listed, in unlock order. */
#define UMN_SIM_UNLOCK_SCENARIO_1 115 /* unlocks three entries */
#define UMN_SIM_UNLOCK_SCENARIO_2 162
#define UMN_SIM_UNLOCK_SCENARIO_3 301
#define UMN_SIM_UNLOCK_SCENARIO_4 345
#define UMN_SIM_UNLOCK_SCENARIO_5 388

/* tskUmnSimulationList states (UmnSimulationListWork.state). */
enum {
    UMN_SIM_LIST_IDLE = 0,
    UMN_SIM_LIST_WAIT_OPEN = 10,
    UMN_SIM_LIST_SLIDE_IN = 20,
    UMN_SIM_LIST_SELECT = 30,
    UMN_SIM_LIST_SLIDE_OUT = 40
};

typedef struct UmnSimulationPasWork {
    unsigned char unmodeled_00;
    unsigned char state;
    unsigned char unmodeled_02[2];
    int kind;
    WindowDX window;
    EMessageParam message;
    WindowDXRect box;
} UmnSimulationPasWork;

typedef struct UmnSlideTargets {
    short window;
    short message;
} UmnSlideTargets;

typedef union UmnInfoName {
    unsigned long long align;
    char text[10];
} UmnInfoName;

typedef struct UmnInfoPos {
    short x;
    short y;
} UmnInfoPos;

typedef struct UmnInfoWork {
    unsigned char mode;
    unsigned char opened;
    unsigned char unmodeled_02[2];
    unsigned char unmodeled_04[4];
    int color;
    WindowDX window;
    UmnInfoPos pos;
    unsigned char unmodeled_1a4[0x1ac - 0x1a4];
    char *text;
    unsigned char unmodeled_1b0[0x350 - 0x1b0];
    UmnInfoName text_buf;
} UmnInfoWork;

typedef struct UmnListBody {
    union {
        short slide[2];
        int slide_word;
    };
    int color;
    WindowSPCursor window;
    MenuListEntry items[UMN_SIM_LIST_MAX + 2];
} UmnListBody;

typedef struct UmnSimulationListWork {
    unsigned char state;
    unsigned char opened;
    unsigned char unmodeled_02[2];
    UmnListBody body;
} UmnSimulationListWork;

extern const UmnInfoName simulateInfoName;
extern const char simulationEmptyText[];
extern const char simulationPeriodText[];
extern const char simulationInfoTitleText[];
extern const char *ListText[];
extern int MenuScenarioNoGet(void);
extern int xglFontDebugHex(int x, int y, int value, int digits);

static int UmnSimulationScriptNoTbl[] = {
    0x000300EF,
    0x00040149,
    0x00010333,
    0x00010419,
    0x0001058B,
    0x0001097D,
    0x00010B35
};
static const char *msg00_0_00A10888 = "Environmental Simulator";
/* JP: シミュレーション
 * EN: Simulation */
const char simulationDebugLabel[] =
    "\245\267\245\337\245\345\245\354\241\274\245\267\245\347\245\363";

void tskUmnSimulationPas(UmnObjectTask *task, void *data)
{
    UmnSimulationPasWork *work = data;
    unsigned char simulation_state = UmnWork.mode;

    if (simulation_state != UMN_MODE_SIMULATION) {
        task->state = UMN_TASK_END;
    } else {
        switch (task->state) {
        case UMN_TASK_INIT: {
            UmnWindowMessageView *view;
            int window_height = 32;
            unsigned char opening_state = 1;
            int message_x = 0x120;
            int message_y = 0x0b;

            work->kind = 0x00fffff0;
            WindowDXSet(&work->window);
            work->window.x = -0x110;
            work->window.y = 8;
            work->window.style = work->kind;
            work->window.width = 312;
            work->window.height = window_height;
            work->window.state = opening_state;
            WindowDXMain(&work->window);
            work->window.state = simulation_state;
            eMessageSet(&work->message, msg00_0_00A10888);
            view = (UmnWindowMessageView *)((unsigned char *)work + 0x190);
            view->message.mode = E_MODE_SHOW_START;
            view->message.x = message_x;
            view->message.y = message_y;
            work->state = opening_state;
            work->message.draw_parameter = work->kind + 2;
            break;
        }

        case UMN_TASK_RUN: {
            UmnSlideTargets targets;
            short *message_target;
            int tab_state = UmnWork.tab_state;

            targets.window = -0x10;
            message_target = &targets.message;
            *message_target = 288;
            if (tab_state < UMN_STEP_ACTIVE + 1) {
                if (tab_state >= UMN_STEP_OPEN) {
                    targets.message = 16;
                } else {
                    targets.window = -312;
                }
            } else {
                targets.window = -312;
            }

            if (work->state != 0) {
                MoveSlide(&work->window.x, &targets.window, 3.0f);
                WindowDXMain(&work->window);
                work->box.x = work->window.x + 3;
                work->box.y = work->window.y + 3;
                work->box.style = work->kind;
                work->box.width = work->window.width - 6;
                work->box.height = work->window.height - 6;
                endPrintExtFunc(work->kind, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET, &work->box);

                MoveSlide(&work->message.x, message_target, 3.0f);
                if (work->message.x < 256) {
                    eMessageMain(&work->message);
                }
                endPrintExtFunc(work->kind, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET, 0);
            }
            break;
        }
        }
    }
}

void tskUmnSimulationInfo(UmnObjectTask *task, void *data)
{
    UmnInfoWork *work = data;
    unsigned char simulation_state = UmnWork.mode;

    if (simulation_state != UMN_MODE_SIMULATION) {
        task->state = UMN_TASK_END;
    } else {
        int command = task->state;

        switch (command) {
        case UMN_TASK_INIT: {
            UmnInfoPos *pos;

            work->color = 0x00ff0000;
            __builtin_memcpy(work->text_buf.text, simulationEmptyText, 1);
            work->opened = 0;
            work->mode = 0;
            WindowDXSet(&work->window);
            work->window.x = -16;
            work->window.state = 0;
            work->window.y = 480;
            work->window.width = 544;
            work->window.height = 54;
            work->window.style = work->color;
            pos = &work->pos;
            work->window.close_callback = MenuInfoWindow;
            work->window.close_callback_arg = pos;
            pos->x = 0;
            work->window.state = WINDOWDX_STATE_OPEN;
            work->pos.y = 0;
            work->text = 0;
            WindowDXMain(&work->window);
            work->window.state = simulation_state;
            break;
        }

        case UMN_TASK_RUN: {
            short slide_target = 386;
            unsigned char mode = work->mode;

            if (mode != 0 && mode != command) {
                mode = 0;
            } else {
                int tab_state;

                if (mode == 0) {
                    work->mode = command;
                    work->opened = 1;
                }
                tab_state = UmnWork.tab_state;
                if (tab_state < UMN_STEP_ACTIVE + 1) {
                    if (tab_state >= UMN_STEP_OPEN) {
                        if (UmnWork.simulation.list_index >= 0) {
                            __builtin_memcpy(work->text_buf.text, simulateInfoName.text, 10);
                            strcat(work->text_buf.text, ListText[UmnWork.simulation.list_index]);
                            strcat(work->text_buf.text, simulationPeriodText);
                        }
                    } else {
                        slide_target = 512;
                    }
                } else {
                    slide_target = 512;
                }
            }
            work->text = work->text_buf.text;
            MoveSlide(&work->window.y, &slide_target, 3.0f);
            WindowDXMain(&work->window);
            break;
        }
        }
    }
}
void tskUmnSimulationList(UmnObjectTask *task, void *data)
{
    UmnSimulationListWork *list = data;
    unsigned char simulation_state = UmnWork.mode;

    if (simulation_state != UMN_MODE_SIMULATION) {
        task->state = UMN_TASK_END;
    } else {
        int command = task->state;

        switch (command) {
        case UMN_TASK_INIT: {
            int i;
            MenuListEntry *terminator;
            UmnListBody *body;
            int offset;

            list->body.slide[0] = 528;
            list->body.slide[1] = 112;
            list->body.color = 0x00FFFF00;

            for (i = 0; i < UmnWork.simulation.list_count; i++) {
                list->body.items[i].name = ListText[i];
                if ((GameLoopState.flags & GAME_FLAG_ON_EVENT_SYMBOL) != 0) {
                    list->body.items[i].flag = 0;
                } else {
                    list->body.items[i].flag = 1;
                }
                list->body.items[i].amount = 0;
            }
            list->body.items[i].name = simulationEmptyText;
            list->body.items[i].amount = 0;
            list->body.items[i].flag = 0;
            offset = (i + 1) * sizeof(MenuListEntry);
            body = &list->body;
            terminator = (MenuListEntry *)((char *)body->items + offset);

            list->body.window.x = list->body.slide[0];
            list->body.window.y = list->body.slide[1];
            list->body.window.style = list->body.color;
            terminator->name = 0;
            list->body.window.draw_flags = WINDOWSP_DRAW_CURSORS | WINDOWSP_DRAW_MESSAGES;
            list->body.window.title = simulationInfoTitleText;
            list->body.window.columns = 1;
            list->body.window.rows = UMN_SIM_LIST_MAX;
            list->body.window.width = 422;
            list->body.window.height = 174;
            list->body.window.items = list->body.items;
            WindowSPSet(&list->body.window);
            list->body.window.state = WINDOWSP_STATE_OPEN | (WINDOWSP_OPEN_NO_ANIMATION << 4);
            list->opened = 0;
            list->state = UMN_SIM_LIST_IDLE;
            break;
        }

        case UMN_TASK_RUN:
            switch (list->state) {
            case UMN_SIM_LIST_IDLE:
                list->opened = 0;
                list->state = UMN_SIM_LIST_WAIT_OPEN;
                /* fallthrough */
            case UMN_SIM_LIST_WAIT_OPEN:
                if (UmnWork.tab_state == UMN_STEP_ACTIVE) {
                    list->opened = 1;
                    list->state = UMN_SIM_LIST_SLIDE_IN;
                }
                break;
            case UMN_SIM_LIST_SLIDE_IN:
            case UMN_SIM_LIST_SLIDE_OUT: {
                short slide_target = 32;

                if (list->state != UMN_SIM_LIST_SLIDE_IN) {
                    slide_target = 528;
                } else {
                    slide_target = 32;
                }
                MoveSlide(&list->body.slide[0], &slide_target, 3.0f);
                if (list->body.slide[0] == slide_target) {
                    if (list->state == UMN_SIM_LIST_SLIDE_IN) {
                        list->state = UMN_SIM_LIST_SELECT;
                    } else {
                        list->state = UMN_SIM_LIST_IDLE;
                    }
                }
                break;
            }
            case UMN_SIM_LIST_SELECT:
                UmnWork.simulation.list_index = WindowSPSelect(&list->body.window, PadData.mappedRepeat);
                if (UmnWork.tab_state != UMN_STEP_ACTIVE) {
                    list->state = UMN_SIM_LIST_SLIDE_OUT;
                }
                break;
            }
            break;
        }

        if (list->opened != 0) {
            list->body.window.x = list->body.slide[0];
            list->body.window.y = list->body.slide[1];
            WindowSPMain(&list->body.window);
        }
    }
}
int UmnSimulation(void)
{
    unsigned char state = UmnWork.tab_state;

    switch (state) {
    case UMN_STEP_INIT: {
        int task_offset = UmnWorkEnd;
        int scenario;
        int list_offset;

        task_offset = (task_offset + 15) & ~15;

        UmnObjectTaskCreate(tskUmnSimulationPas, (void *)task_offset);
        task_offset = (task_offset + 507) & ~15;
        UmnObjectTaskCreate(tskUmnSimulationInfo, (void *)task_offset);
        list_offset = (task_offset + 927) & ~15;
        UmnObjectTaskCreate(tskUmnSimulationList, (void *)list_offset);

        scenario = MenuScenarioNoGet();
        UmnWork.simulation.list_count = 0;
        if (scenario >= UMN_SIM_UNLOCK_SCENARIO_5) {
            UmnWork.simulation.list_count = 1;
        }
        if (scenario >= UMN_SIM_UNLOCK_SCENARIO_4) {
            UmnWork.simulation.list_count++;
        }
        if (scenario >= UMN_SIM_UNLOCK_SCENARIO_3) {
            UmnWork.simulation.list_count++;
        }
        if (scenario >= UMN_SIM_UNLOCK_SCENARIO_2) {
            UmnWork.simulation.list_count++;
        }
        if (scenario >= UMN_SIM_UNLOCK_SCENARIO_1) {
            UmnWork.simulation.list_count += 3;
        }
        UmnWork.simulation.list_index = -1;
        if (UmnWork.simulation.list_count == 0) {
            UmnWork.tab_state = UMN_STEP_LEAVE;
        } else {
            UmnWork.tab_state = UMN_STEP_OPEN;
        }
        break;
    }

    case UMN_STEP_OPEN:
        UmnWork.tab_state = UMN_STEP_ACTIVE;
        UmnWork.wait = UMN_WAIT_OPEN;
        /* The newly armed countdown is handled by the active state. */

    case UMN_STEP_ACTIVE:
        if (UmnWork.wait == 0) {
            if (UmnWork.simulation.list_index >= 0 && (PadData.half_2a & PAD_CIRCLE) != 0) {
                if ((GameLoopState.flags & GAME_FLAG_ON_EVENT_SYMBOL) != 0) {
                    UmnWork.tab_state = UMN_STEP_LEAVE;
                    UmnWork.simulation.pending_script = UmnSimulationScriptNoTbl[UmnWork.simulation.list_index];
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            if ((PadData.half_2a & PAD_CROSS) != 0) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                UmnWork.tab_state = UMN_STEP_LEAVE;
            }
        }
        break;

    case UMN_STEP_LEAVE:
        UmnWork.tab_state = UMN_STEP_LEAVE_WAIT;
        UmnWork.wait = UMN_WAIT_LEAVE;

    case UMN_STEP_LEAVE_WAIT:
        if (UmnWork.wait == 0) {
            UmnSimulationNo = UmnWork.simulation.pending_script;
            UmnChangeTopLevel(0);
        }
        break;
    }

    if (UmnWork.wait != 0) {
        UmnWork.wait--;
    }
    xglFontDebugPrintf(32, 208, simulationDebugLabel);
    return xglFontDebugHex(0, 64, UmnWork.tab_state, 2);
}
