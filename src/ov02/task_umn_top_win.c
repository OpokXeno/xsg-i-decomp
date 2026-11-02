#include "common.h"
#include "umn_procurator.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/xgl_dma.h"
#include "main/e_message.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

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
    UmnTopOption option[4];
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

typedef struct UmnTopTask {
    unsigned char unmodeled_00[0x10];
    int state;
} UmnTopTask;

typedef struct UmnTopShortSet {
    short value[4];
} UmnTopShortSet;

typedef struct UmnTopIntSet {
    int value[4];
} UmnTopIntSet;

extern char *moji00_0[4];
extern char *moji01_1[4];
extern UmnTopShortSet D_00A12030;
extern UmnTopIntSet D_00A12038;
extern UmnTopShortSet D_00A12048;
extern int UmnTexAddr;

void eWindowSet(void *window);
void eSpriteSet(ESprite *sprite, int texture);
void eSpriteMain(ESprite *sprite);

/* UmnWork read as 16-byte quadword rows; row 2 holds the option ids. */
typedef struct UmnTopMenuRows {
    unsigned char row[4][16];
} UmnTopMenuRows;

void taskUmnTopWin(UmnTopTask *task, void *data)
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

    if (UmnWork.mode != 0) {
        task->state = -1;
        return;
    }
    work = data;
    switch (task->state) {
    case 0:
        work->x = 48;
        work->y = 256;
        work->graphics = 0xfc000;
        work->visible = 0;
        eWindowSet(&work->window);
        work->window.mode = 32;
        work->window.graphics = work->graphics;
        work->window.width = 576;
        work->window.height = 102;
        index = ((UmnTopMenuRows *)&UmnWork)->row[2][UmnWork.top.cursor] - 1;
        eMessageSet(&work->top_message, moji00_0[index]);
        eMessageModeChange(&work->top_message, 36);
        graphicsTemp = work->graphics;
        work->top_message.draw_parameter = graphicsTemp + 6;
        eMessageSet(&work->bottom_message, moji01_1[index]);
        eMessageModeChange(&work->bottom_message, 36);
        graphicsTemp = work->graphics;
        work->bottom_message.draw_parameter = graphicsTemp + 6;
        icons = D_00A12030;
        work->selection_span = 144;
        positions = D_00A12038;
        for (k = 0; k < UmnWork.top.count; k++) {
            eRibbonSet(&work->option[k].ribbon, 3);
            work->option[k].ribbon.width = 102;
            work->option[k].ribbon.height = 128;
            if (((UmnTopMenuRows *)&UmnWork)->row[2][k] != 0) {
                eSpriteSet((ESprite *)&work->option[k].sprite, icons.value[((UmnTopMenuRows *)&UmnWork)->row[2][k] - 1]);
            }
            work->option[k].x = 48;
            work->option[k].y = work->selection_span * positions.value[(unsigned int)(k - UmnWork.top.cursor) % (unsigned int)UmnWork.top.count] + 128;
            if (UmnWork.top.cursor == k) {
                work->option[k].fade = 1.0f;
            } else {
                work->option[k].fade = 0.5f;
            }
        }
        for (j = 0; j < 2; j++) {
            eSpriteSet(&work->group1[j], 266);
            work->group1[j].work = work->graphics + 8;
        }
        icons2 = D_00A12048;
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
    case 2:
        switch (work->state) {
        case 0:
            work->state = 20;
        case 20:
            nav = UmnWork.top.direction;
            if (nav != 0) {
                work->selected_option = UmnWork.top.cursor;
                if (((UmnTopMenuRows *)&UmnWork)->row[2][UmnWork.top.cursor] != 0) {
                    int message = ((UmnTopMenuRows *)&UmnWork)->row[2][UmnWork.top.cursor] - 1;

                    eMessageTextChange(&work->top_message, moji00_0[message]);
                    eMessageModeChange(&work->top_message, 36);
                    eMessageTextChange(&work->bottom_message, moji01_1[message]);
                    eMessageModeChange(&work->bottom_message, 36);
                }
                waiting = 0;
                work->state = 22;
            } else {
                waiting = 1;
            }
            if (waiting) {
                break;
            }
        case 22:
            for (v = 0; v < UmnWork.top.count; v++) {
                work->option[v].y -= UmnWork.top.direction * 16;
                if (work->option[v].y == 464) {
                    work->option[v].y = -112;
                } else if (work->option[v].y == -112) {
                    work->option[v].y = 464;
                }
            }
            cur = UmnWork.top.cursor;
            if (128 == work->option[cur].y) {
                if (((UmnTopMenuRows *)&UmnWork)->row[2][cur] == 0) {
                    UmnWork.top.cursor = cur + UmnWork.top.direction;
                    UmnWork.top.cursor = UmnWork.top.cursor % UmnWork.top.count;
                    work->state = 20;
                } else {
                    work->state = 20;
                    UmnWork.top.direction = 0;
                }
            }
        }
        if (work->visible != 0) {
            work->quad0.vertex[0].y = 0;
            work->quad0.vertex[1].x = 36;
            work->quad0.vertex[1].y = 448;
            work->quad0.vertex[1].graphics = work->graphics;
            work->quad0.vertex[0].x = 36;
            work->quad0.vertex[0].graphics = work->graphics;
            work->quad0.vertex[0].red = 56;
            work->quad0.vertex[1].red = 56;
            work->quad0.vertex[0].green = -84;
            work->quad0.vertex[1].green = -84;
            work->quad0.vertex[0].blue = -62;
            work->quad0.vertex[1].blue = -62;
            work->quad0.vertex[0].alpha = 84;
            work->quad0.vertex[1].alpha = 84;
            work->quad0.vertex[2].graphics = 0;
            endPrintExtFunc(0, 0, &work->quad0);
            work->quad1.vertex[1].x = 160;
            work->quad1.vertex[1].y = 448;
            work->quad1.vertex[1].graphics = work->graphics;
            work->quad1.vertex[0].x = 160;
            work->quad1.vertex[0].graphics = work->graphics;
            work->quad1.vertex[0].y = 0;
            work->quad1.vertex[0].red = 56;
            work->quad1.vertex[1].red = 56;
            work->quad1.vertex[0].green = -84;
            work->quad1.vertex[1].green = -84;
            work->quad1.vertex[0].blue = -62;
            work->quad1.vertex[1].blue = -62;
            work->quad1.vertex[0].alpha = 84;
            work->quad1.vertex[1].alpha = 84;
            work->quad1.vertex[2].graphics = 0;
            endPrintExtFunc(0, 0, &work->quad1);
            work->texture_addr = UmnTexAddr;
            endPrintExtFunc(0, 14, &work->texture_addr);
            for (k = 0; (int)k < UmnWork.top.count; k++) {
                id = ((UmnTopMenuRows *)&UmnWork)->row[2][k];
                if (id != 0) {
                    work->option[k].sprite.x = work->option[k].x;
                    work->option[k].sprite.y = work->option[k].y;
                    work->option[k].sprite.work = work->graphics - 2;
                    if (UmnWork.top.cursor == k) {
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
                        work->entry[k].vertex[point].red = 56;
                        work->entry[k].vertex[point].green = -84;
                        work->entry[k].vertex[point].blue = -62;
                        work->entry[k].vertex[point].alpha = 84;
                    }
                    work->entry[k].vertex[5].graphics = 0;
                    endPrintExtFunc(0, 0, &work->entry[k].vertex[0]);
                }
            }
            do {
                for (m = 0; m < 4; m++) {
                    work->group2[m].x = 176 + 64 * m;
                    work->group2[m].y = 392;
                    eSpriteMain(&work->group2[m]);
                }
            } while (0);
            endPrintExtFunc(0, 15, 0);
            work->window.x = work->x - 96;
            work->window.y = work->y;
            endPrintExtFunc(0, 14, &work->texture_addr);
            work->box.box_16 = 64;
            work->box.box_20 = work->y + 7;
            work->box.box_24 = work->graphics + 1;
            endPrintExtFunc(0, 8, &work->box);
            endPrintExtFunc(0, 15, 0);
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

extern void *UmnWorkEnd;
extern const char D_00A12050[];
extern unsigned char D_4A1A0C[];
void UmnChangeTopLevel(int arg);

void UmnTopMenu(void)
{
    unsigned char *slot;
    unsigned char *object;
    int count;
    int i;
    int pos;
    int arg;
    unsigned short pad;
    unsigned short pressed;
    unsigned short navPad;
    signed char nav;

    switch (UmnWork.stateOrCategory) {
    case 0:
        UmnObjectTaskCreate((UmnTaskFunction)taskUmnTopWin, (void *)(((int)UmnWorkEnd + 15) & -16));
        object = D_4A1A0C;
        slot = &UmnWork.top.slots[3];
        for (i = 3; i >= 0; i--) {
            *slot-- = 0;
        }
        pad = *(unsigned short *)&object[134];
        i = 4;
        if (pad & 0x8) {
            UmnWork.top.slots[0] = 3;
        }
        UmnWork.top.slots[1] = 1;
        if (pad & 0x2) {
            UmnWork.top.slots[2] = 2;
        }
        UmnWork.top.slots[3] = i;
        UmnWork.top.count = i;
        if (UmnWork.unmodeled_02 == 0) {
            i = 0;
            while (i < 4 && UmnWork.top.slots[i] == 0) {
                i++;
            }
            UmnWork.unmodeled_02 = i;
        }
        UmnWork.top.cursor = UmnWork.unmodeled_02;
        UmnWork.stateOrCategory = 32;
        UmnWork.top.repeat_delay = 8;
        /* falls through */
    case 32:
        UmnWork.stateOrCategory = 33;
        UmnWork.top.direction = 0;
        UmnWork.top.unmodeled_11 = 0;
        /* falls through */
    case 33:
        if (UmnWork.top.repeat_delay != 0) {
            break;
        }
        if (UmnWork.top.direction == 0) {
            if (UmnWork.top.count >= 2) {
                navPad = *(unsigned short *)&PadData.unmodeled_2c[4];
                if (navPad & 0x1000) {
                    UmnWork.top.direction = -1;
                }
                if (navPad & 0x4000) {
                    UmnWork.top.direction = 1;
                }
            }
            nav = UmnWork.top.direction;
            if (nav != 0) {
                UmnWork.top.cursor = UmnWork.top.cursor + UmnWork.top.direction;
                UmnWork.top.cursor = UmnWork.top.cursor % UmnWork.top.count;
                UmnWork.top.repeat_delay = 7;
                xglSoundEffectNormalID(3, 0);
            } else {
                pressed = PadData.half_2a;
                if (pressed & 0x20) {
                    UmnWork.unmodeled_02 = UmnWork.top.cursor;
                    pos = (unsigned char)UmnWork.top.cursor;
                    if (UmnWork.top.slots[pos] != 0) {
                        UmnChangeTopLevel(UmnWork.top.slots[pos]);
                        arg = 1;
                    } else {
                        arg = 5;
                    }
                    xglSoundEffectNormalID(arg, 0);
                } else if (pressed & 0x40) {
                    UmnWork.stateOrCategory = -16;
                    xglSoundEffectNormalID(2, 0);
                }
            }
        }
        break;
    case 240:
        UmnWork.mode = -1;
        break;
    }
    if (UmnWork.top.repeat_delay != 0) {
        UmnWork.top.repeat_delay = UmnWork.top.repeat_delay - 1;
    }
    xglFontDebugPrintf(0, 64, D_00A12050, UmnWork.top.cursor);
}

short *UmnManzaiText = 0;
unsigned int UmnManzaiFlag = 0;
UmnManzaiWinTask UmnManzaiWin[2] = {{0}, {0}};
