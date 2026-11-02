/*
 * OV02 original TU 7: UmnTopMenu, its window task and the manzai window records.
 */
#include "common.h"
#include "main/e_message.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/xgl_dma.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"
#include "ov02/task_umn_top_win.h"
#include "ov02/title_hdd_install.h"
#include "ov02/tsk_umn_object_task_main.h"
#include "ov02/umn_event_text_symbol_check.h"
#include "ov02/ov02.h"

/* Entries of the top menu: option_ids[] and UmnTopWinWork.option[]. */
#define UMN_TOP_OPTION_COUNT 4

/* UmnWork.tab_state steps of UmnTopMenu. */
#define UMN_TOP_STEP_OPEN 0x20
#define UMN_TOP_STEP_ACTIVE 0x21

/* UmnTopWinWork.state steps of taskUmnTopWin. */
#define UMN_TOP_WIN_INIT 0
#define UMN_TOP_WIN_WAIT 20
#define UMN_TOP_WIN_SCROLL 22

/* UmnMailSaveData.plugin_unlock_flags bits that add an option to the top menu. */
#define UMN_TOP_UNLOCK_DATA_BASE 0x2
#define UMN_TOP_UNLOCK_SIMULATION 0x8

/* Frame countdowns armed in UmnWork.wait. */
#define UMN_TOP_WAIT_OPEN 8
#define UMN_TOP_WAIT_MOVE 7

/* Option layout: the selected option rests at y 128; the others scroll 16 per frame and wrap at 464 / -112. */
#define UMN_TOP_OPTION_SELECTED_Y 128
#define UMN_TOP_OPTION_SCROLL_STEP 16
#define UMN_TOP_OPTION_WRAP_BOTTOM 464
#define UMN_TOP_OPTION_WRAP_TOP (-112)

/* Colour of the translucent panels and the outline of the empty option slots. */
#define UMN_TOP_PANEL_RED 56
#define UMN_TOP_PANEL_GREEN (-84)
#define UMN_TOP_PANEL_BLUE (-62)
#define UMN_TOP_PANEL_ALPHA 84

/* ESprite as this TU writes it: the colour bytes are named channels. */
typedef struct UmnTopSprite {
    signed char state;
    u8 unmodeled_001[3];
    short x;
    short y;
    int work;
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
    u8 unmodeled_010[0x14];
    short spriteId;
} UmnTopSprite;

typedef struct UmnTopOption {
    short x;
    short y;
    UmnTopSprite sprite;
    ERibbonMainData ribbon;
    float fade;
} UmnTopOption;

typedef struct UmnTopWindow {
    int x;
    int y;
    int graphics;
    int width;
    int height;
    unsigned char unmodeled_14[4];
    unsigned char mode;
    unsigned char unmodeled_19[0x57];
} UmnTopWindow;

typedef struct UmnTopVertex {
    short x;
    short y;
    int graphics;
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
} UmnTopVertex;

/* Vertex strips for endPrintExtFunc: the vertex after the last drawn one
   is the terminator (graphics == 0). */
typedef struct UmnTopEntry {
    UmnTopVertex vertex[6];
} UmnTopEntry;

typedef struct UmnTopQuad {
    UmnTopVertex vertex[3];
} UmnTopQuad;

typedef struct UmnTopBox {
    unsigned char unmodeled_00;
    unsigned char unmodeled_01;
    short unmodeled_02;
    unsigned char unmodeled_04[0x0c];
    int box_16;
    int box_20;
    int box_24;
} UmnTopBox;

typedef struct UmnTopColor {
    unsigned char c0;
    unsigned char c1;
    unsigned char c2;
    unsigned char c3;
} UmnTopColor;

typedef struct UmnTopWinWork {
    unsigned char state;
    unsigned char visible;
    short selection_span;
    short x;
    short y;
    int graphics;
    int selected_option;
    UmnTopOption option[UMN_TOP_OPTION_COUNT];
    UmnTopWindow window;
    EMessageParam top_message;
    EMessageParam bottom_message;
    ESprite group1[2];
    int texture_addr;
    ESprite group2[4];
    unsigned char unmodeled_45c[4];
    UmnTopBox box;
    unsigned char unmodeled_49c[0x34];
    UmnTopColor bar_color[4];
    UmnTopQuad quad0;
    UmnTopQuad quad1;
    UmnTopEntry entry[5];
} UmnTopWinWork;

typedef struct UmnTopShortSet {
    short value[4];
} UmnTopShortSet;

typedef struct UmnTopIntSet {
    int value[4];
} UmnTopIntSet;

static const char *moji00_0[4];
static const char *moji01_1[4];
extern const char menuCursorDebugFormat[];

void eWindowSet(void *window);
void eSpriteSet(ESprite *sprite, int texture);
void eSpriteMain(ESprite *sprite);

void taskUmnTopWin(UmnObjectTask *task, void *data)
{
    UmnTopWinWork *work;
    UmnTopOption *option;
    ESprite *sprite;
    UmnTopShortSet icons;
    UmnTopIntSet positions;
    UmnTopShortSet icons2;
    unsigned int k;
    int j;
    int m;
    int v;
    int i;
    int point;
    unsigned char cur;
    int id;
    int index;
    int nav;
    int waiting;
    int graphicsTemp;
    unsigned char *bar;
    float fade;

    if (UmnWork.mode != UMN_MODE_TOP_MENU) {
        task->state = UMN_TASK_END;
        return;
    }
    work = data;
    switch (task->state) {
    case UMN_TASK_INIT:
        work->x = 48;
        work->y = 256;
        work->graphics = 0xfc000;
        work->visible = 0;
        eWindowSet(&work->window);
        work->window.mode = E_MODE_SHOW_START;
        work->window.graphics = work->graphics;
        work->window.width = 576;
        work->window.height = 102;
        index = UmnWork.option_ids[UmnWork.menu_cursor] - 1;
        eMessageSet(&work->top_message, moji00_0[index]);
        eMessageModeChange(&work->top_message, EMESSAGE_MODE_LINE_START);
        graphicsTemp = work->graphics;
        work->top_message.draw_parameter = graphicsTemp + 6;
        eMessageSet(&work->bottom_message, moji01_1[index]);
        eMessageModeChange(&work->bottom_message, EMESSAGE_MODE_LINE_START);
        graphicsTemp = work->graphics;
        work->bottom_message.draw_parameter = graphicsTemp + 6;
        icons = (UmnTopShortSet){ .value = { 5120, 5121, 5122, 5123 } };
        work->selection_span = 144;
        positions = (UmnTopIntSet){ .value = { 0, 1, 2, -1 } };
        for (k = 0; k < UmnWork.menu_count; k++) {
            eRibbonSet(&work->option[k].ribbon, 3);
            work->option[k].ribbon.width = 102;
            work->option[k].ribbon.height = 128;
            if (UmnWork.option_ids[k] != 0) {
                eSpriteSet((ESprite *)&work->option[k].sprite, icons.value[UmnWork.option_ids[k] - 1]);
            }
            work->option[k].x = 48;
            work->option[k].y = work->selection_span * positions.value[(unsigned int)(k - UmnWork.menu_cursor) % (unsigned int)UmnWork.menu_count] + UMN_TOP_OPTION_SELECTED_Y;
            if (UmnWork.menu_cursor == k) {
                work->option[k].fade = 1.0f;
            } else {
                work->option[k].fade = 0.5f;
            }
        }
        for (j = 0; j < 2; j++) {
            eSpriteSet(&work->group1[j], 266);
            work->group1[j].work = work->graphics + 8;
        }
        icons2 = (UmnTopShortSet){ .value = { 6144, 6145, 6146, 6147 } };
        for (j = 0; j < 4; j++) {
            eSpriteSet(&work->group2[j], icons2.value[j]);
            work->group2[j].work = work->graphics + 8;
        }

        work->box.unmodeled_01 = 0;
        work->box.unmodeled_02 = 5131;
        work->box.unmodeled_00 = 0;
        for (i = 0; i < 4; i++) {
            work->bar_color[i].c0 = work->bar_color[i].c1 = work->bar_color[i].c2 = work->bar_color[i].c3 = -128;
        }
        work->bar_color[1].c3 = 112;
        work->bar_color[0].c3 = 112;
        work->bar_color[3].c3 = 0;
        work->bar_color[2].c3 = 0;
        work->state = 0;
        work->visible = 1;
        break;
    case UMN_TASK_RUN:
        switch (work->state) {
        case UMN_TOP_WIN_INIT:
            work->state = UMN_TOP_WIN_WAIT;
        case UMN_TOP_WIN_WAIT:
            nav = UmnWork.direction;
            if (nav != 0) {
                work->selected_option = UmnWork.menu_cursor;
                if (UmnWork.option_ids[UmnWork.menu_cursor] != 0) {
                    int message = UmnWork.option_ids[UmnWork.menu_cursor] - 1;

                    eMessageTextChange(&work->top_message, moji00_0[message]);
                    eMessageModeChange(&work->top_message, EMESSAGE_MODE_LINE_START);
                    eMessageTextChange(&work->bottom_message, moji01_1[message]);
                    eMessageModeChange(&work->bottom_message, EMESSAGE_MODE_LINE_START);
                }
                waiting = 0;
                work->state = UMN_TOP_WIN_SCROLL;
            } else {
                waiting = 1;
            }
            if (waiting) {
                break;
            }
        case UMN_TOP_WIN_SCROLL:
            for (v = 0; v < UmnWork.menu_count; v++) {
                work->option[v].y -= UmnWork.direction * UMN_TOP_OPTION_SCROLL_STEP;
                if (work->option[v].y == UMN_TOP_OPTION_WRAP_BOTTOM) {
                    work->option[v].y = UMN_TOP_OPTION_WRAP_TOP;
                } else if (work->option[v].y == UMN_TOP_OPTION_WRAP_TOP) {
                    work->option[v].y = UMN_TOP_OPTION_WRAP_BOTTOM;
                }
            }
            cur = UmnWork.menu_cursor;
            if (UMN_TOP_OPTION_SELECTED_Y == work->option[cur].y) {
                if (UmnWork.option_ids[cur] == 0) {
                    UmnWork.menu_cursor = cur + UmnWork.direction;
                    UmnWork.menu_cursor = UmnWork.menu_cursor % UmnWork.menu_count;
                    work->state = UMN_TOP_WIN_WAIT;
                } else {
                    work->state = UMN_TOP_WIN_WAIT;
                    UmnWork.direction = 0;
                }
            }
        }
        if (work->visible != 0) {
            work->quad0.vertex[0].y = 0;
            work->quad0.vertex[1].x = 36;
            work->quad0.vertex[1].y = OV02_SCREEN_HEIGHT;
            work->quad0.vertex[1].graphics = work->graphics;
            work->quad0.vertex[0].x = 36;
            work->quad0.vertex[0].graphics = work->graphics;
            work->quad0.vertex[0].red = UMN_TOP_PANEL_RED;
            work->quad0.vertex[1].red = UMN_TOP_PANEL_RED;
            work->quad0.vertex[0].green = UMN_TOP_PANEL_GREEN;
            work->quad0.vertex[1].green = UMN_TOP_PANEL_GREEN;
            work->quad0.vertex[0].blue = UMN_TOP_PANEL_BLUE;
            work->quad0.vertex[1].blue = UMN_TOP_PANEL_BLUE;
            work->quad0.vertex[0].alpha = UMN_TOP_PANEL_ALPHA;
            work->quad0.vertex[1].alpha = UMN_TOP_PANEL_ALPHA;
            work->quad0.vertex[2].graphics = 0;
            endPrintExtFunc(0, END_PRINT_CMD_LINE, &work->quad0);
            work->quad1.vertex[1].x = 160;
            work->quad1.vertex[1].y = OV02_SCREEN_HEIGHT;
            work->quad1.vertex[1].graphics = work->graphics;
            work->quad1.vertex[0].x = 160;
            work->quad1.vertex[0].graphics = work->graphics;
            work->quad1.vertex[0].y = 0;
            work->quad1.vertex[0].red = UMN_TOP_PANEL_RED;
            work->quad1.vertex[1].red = UMN_TOP_PANEL_RED;
            work->quad1.vertex[0].green = UMN_TOP_PANEL_GREEN;
            work->quad1.vertex[1].green = UMN_TOP_PANEL_GREEN;
            work->quad1.vertex[0].blue = UMN_TOP_PANEL_BLUE;
            work->quad1.vertex[1].blue = UMN_TOP_PANEL_BLUE;
            work->quad1.vertex[0].alpha = UMN_TOP_PANEL_ALPHA;
            work->quad1.vertex[1].alpha = UMN_TOP_PANEL_ALPHA;
            work->quad1.vertex[2].graphics = 0;
            endPrintExtFunc(0, END_PRINT_CMD_LINE, &work->quad1);
            work->texture_addr = UmnTexAddr;
            endPrintExtFunc(0, END_PRINT_CMD_FONT_TEX_CHANGE, &work->texture_addr);
            for (k = 0; (int)k < UmnWork.menu_count; k++) {
                id = UmnWork.option_ids[k];
                if (id != 0) {
                    work->option[k].sprite.x = work->option[k].x;
                    work->option[k].sprite.y = work->option[k].y;
                    work->option[k].sprite.work = work->graphics - 2;
                    if (UmnWork.menu_cursor == k) {
                        fade = work->option[k].fade + 0.1f;
                        work->option[k].fade = fade;
                        if (fade > 1.0f) {
                            work->option[k].fade = 1.0f;
                        }
                    } else {
                        fade = work->option[k].fade - 0.1f;
                        work->option[k].fade = fade;
                        if (0.5f > fade) {
                            work->option[k].fade = 0.5f;
                        }
                    }
                    work->option[k].sprite.red = (unsigned char)(work->option[k].fade * 128.0f);
                    work->option[k].sprite.green = (unsigned char)(work->option[k].fade * 128.0f);
                    work->option[k].sprite.blue = (unsigned char)(work->option[k].fade * 128.0f);
                    eSpriteMain((ESprite *)&work->option[k].sprite);
                } else {
                    int px[5] = { work->option[k].x, work->option[k].x + 100, work->option[k].x + 100, work->option[k].x, work->option[k].x };
                    int py[5] = { work->option[k].y, work->option[k].y, work->option[k].y + 128, work->option[k].y + 128, work->option[k].y };

                    for (point = 0; point < 5; point++) {
                        work->entry[k].vertex[point].x = px[point];
                        work->entry[k].vertex[point].y = py[point];
                        work->entry[k].vertex[point].graphics = work->graphics;
                        work->entry[k].vertex[point].red = UMN_TOP_PANEL_RED;
                        work->entry[k].vertex[point].green = UMN_TOP_PANEL_GREEN;
                        work->entry[k].vertex[point].blue = UMN_TOP_PANEL_BLUE;
                        work->entry[k].vertex[point].alpha = UMN_TOP_PANEL_ALPHA;
                    }
                    work->entry[k].vertex[5].graphics = 0;
                    endPrintExtFunc(0, END_PRINT_CMD_LINE, &work->entry[k].vertex[0]);
                }
            }
            do {
                for (m = 0; m < 4; m++) {
                    work->group2[m].x = 176 + 64 * m;
                    work->group2[m].y = 392;
                    eSpriteMain(&work->group2[m]);
                }
            } while (0);
            endPrintExtFunc(0, END_PRINT_CMD_FONT_TEX_RELOAD, 0);
            work->window.x = work->x - 96;
            work->window.y = work->y;
            endPrintExtFunc(0, END_PRINT_CMD_FONT_TEX_CHANGE, &work->texture_addr);
            work->box.box_16 = 64;
            work->box.box_20 = work->y + 7;
            work->box.box_24 = work->graphics + 1;
            endPrintExtFunc(0, END_PRINT_CMD_SPRITE_QUAD, &work->box);
            endPrintExtFunc(0, END_PRINT_CMD_FONT_TEX_RELOAD, 0);
            work->top_message.x = work->x + 108;
            work->top_message.y = work->y + 10;
            eMessageMain(&work->top_message);
            work->bottom_message.x = work->x + 108;
            work->bottom_message.y = work->y + 46;
            eMessageMain(&work->bottom_message);
        }
        break;
    }
}

void UmnTopMenu(void)
{
    unsigned char *slot;
    UmnMailSaveData *save;
    int count;
    int i;
    int pos;
    int arg;
    unsigned short pad;
    unsigned short pressed;
    unsigned short navPad;
    signed char nav;

    switch (UmnWork.tab_state) {
    case UMN_STEP_INIT:
        UmnObjectTaskCreate(taskUmnTopWin, (void *)(((int)UmnWorkEnd + 15) & -16));
        save = &umnMailSaveData;
        slot = &UmnWork.option_ids[UMN_TOP_OPTION_COUNT - 1];
        for (i = UMN_TOP_OPTION_COUNT - 1; i >= 0; i--) {
            *slot-- = 0;
        }
        pad = save->plugin_unlock_flags;
        i = UMN_TOP_OPTION_COUNT;
        if (pad & UMN_TOP_UNLOCK_SIMULATION) {
            UmnWork.option_ids[0] = UMN_MODE_SIMULATION;
        }
        UmnWork.option_ids[1] = UMN_MODE_MAIL;
        if (pad & UMN_TOP_UNLOCK_DATA_BASE) {
            UmnWork.option_ids[2] = UMN_MODE_DATA_BASE;
        }
        UmnWork.option_ids[3] = i;
        UmnWork.menu_count = i;
        if (UmnWork.menu_selected == 0) {
            i = 0;
            while (i < UMN_TOP_OPTION_COUNT && UmnWork.option_ids[i] == 0) {
                i++;
            }
            UmnWork.menu_selected = i;
        }
        UmnWork.menu_cursor = UmnWork.menu_selected;
        UmnWork.tab_state = UMN_TOP_STEP_OPEN;
        UmnWork.wait = UMN_TOP_WAIT_OPEN;
        /* falls through */
    case UMN_TOP_STEP_OPEN:
        UmnWork.tab_state = UMN_TOP_STEP_ACTIVE;
        UmnWork.direction = 0;
        UmnWork.moving = 0;
        /* falls through */
    case UMN_TOP_STEP_ACTIVE:
        if (UmnWork.wait != 0) {
            break;
        }
        if (UmnWork.direction == 0) {
            if (UmnWork.menu_count >= 2) {
                navPad = PadData.half_30;
                if (navPad & PAD_UP) {
                    UmnWork.direction = -1;
                }
                if (navPad & PAD_DOWN) {
                    UmnWork.direction = 1;
                }
            }
            nav = UmnWork.direction;
            if (nav != 0) {
                UmnWork.menu_cursor = UmnWork.menu_cursor + UmnWork.direction;
                UmnWork.menu_cursor = UmnWork.menu_cursor % UmnWork.menu_count;
                UmnWork.wait = UMN_TOP_WAIT_MOVE;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            } else {
                pressed = PadData.half_2a;
                if (pressed & PAD_CIRCLE) {
                    UmnWork.menu_selected = UmnWork.menu_cursor;
                    pos = (unsigned char)UmnWork.menu_cursor;
                    if (UmnWork.option_ids[pos] != 0) {
                        UmnChangeTopLevel(UmnWork.option_ids[pos]);
                        arg = SE_DECIDE;
                    } else {
                        arg = SE_BUZZER;
                    }
                    xglSoundEffectNormalID(arg, 0);
                } else if (pressed & PAD_CROSS) {
                    UmnWork.tab_state = UMN_STEP_LEAVE;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            }
        }
        break;
    case UMN_STEP_LEAVE:
        UmnWork.mode = UMN_MODE_NONE;
        break;
    }
    if (UmnWork.wait != 0) {
        UmnWork.wait = UmnWork.wait - 1;
    }
    xglFontDebugPrintf(0, 64, menuCursorDebugFormat, UmnWork.menu_cursor);
}

UmnEventTextRecord *UmnManzaiText = 0;
unsigned int UmnManzaiFlag = 0;
UmnManzaiWinTask UmnManzaiWin[UMN_MANZAI_WINDOW_COUNT] = {{0}, {0}};

extern const char topMenuDatabaseTitle[16];
extern const char topMenuMailTitle[8];
extern const char topMenuSimulatorTitle[32];
extern const char topMenuPluginTitle[16];
extern const char topMenuSimulatorHelp[56];
extern const char topMenuPluginHelp[48];
extern const char topMenuDatabaseHelp[64];
extern const char topMenuMailHelp[48];

static const char *moji00_0[4] = { topMenuMailTitle, topMenuDatabaseTitle, topMenuSimulatorTitle, topMenuPluginTitle };

static const char *moji01_1[4] = { topMenuMailHelp, topMenuDatabaseHelp, topMenuSimulatorHelp, topMenuPluginHelp };

const char topMenuPluginTitle[16] = "Plug-in Module";

const char topMenuSimulatorTitle[32] = "EVS (Environmental Simulator)";

const char topMenuDatabaseTitle[16] = "Database";

const char topMenuMailTitle[8] = "Mail";

const char topMenuPluginHelp[48] = "View list of Plug-ins currently\nequipped.";

const char topMenuSimulatorHelp[56] = "Simulate places that you have\nbeen to previously.";

const char topMenuDatabaseHelp[64] = "View database relating to the\ncharacters and the Gnosis.";

const char topMenuMailHelp[48] = "Read and reply to emails.\nDownload attachments.";

const char menuCursorDebugFormat[16] = "%d";
