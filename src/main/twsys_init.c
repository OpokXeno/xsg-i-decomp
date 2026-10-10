#include "common.h"

/*
 * One slot of the fixed-size `tcomponent` table TWSYS_createComponent hands
 * out (still assembler in this TU). Only the header fields every component
 * kind shares, evidenced from this allocation, are modeled:
 *  - +0x10 flags: TWIN_dispose/TMENU_dispose clear bits 0x11 on disposal;
 *    TWSYS_update tests bit 0x10 before dispatching the component's update.
 *  - +0x16 kindAndGroup: low nibble selects the update/draw handler
 *    (TMENU/TSLIDER/TWIN), bits 4-5 select the `groupStatus` slot.
 *  - +0x32 closeState: cleared with the flags when a slot is (re)claimed.
 *  - +0x54 ewHandle: TWIN_dispose passes it to EW_dispose (a window-kind
 *    slot's own layout; other kinds keep their own ewHandle elsewhere).
 *  - +0x9C/+0xA0 msgBuffer: TWIN_dispose passes each non-zero entry to
 *    MBUF_dispose.
 * The 0x5B0 stride between slots is TWSYS_init/TWSYS_update/TWSYS_draw's own
 * loop increment.
 */

typedef struct TComponent {
    unsigned char unmodeled_00[0x10];
    unsigned int flags;                /* +0x10 */
    unsigned char unmodeled_14[0x16 - 0x14];
    unsigned short kindAndGroup;       /* +0x16 */
    unsigned char unmodeled_18[0x20 - 0x18];
    float width;                       /* +0x20: TMENU_create/TMENU_init */
    float height;                      /* +0x24: TMENU_create/TMENU_init */
    unsigned char unmodeled_28[0x32 - 0x28];
    unsigned short closeState;         /* +0x32 */
    unsigned char unmodeled_34[0x54 - 0x34];
    int ewHandle;                      /* +0x54 */
    unsigned char unmodeled_58[0x9C - 0x58];
    int *msgBuffer[2];                 /* +0x9C */
    unsigned char unmodeled_a4[0x5B0 - 0xA4];
} TComponent;

static TComponent tcomponent[4];

static unsigned char groupStatus[3];

static unsigned char D_004DC593;

void EW_init(unsigned char *freeSpace);

void MBUF_init(void);

void TMENU_updateDefault(TComponent *component);

void TSLIDER_updateDefault(TComponent *component);

void TWIN_update2(TComponent *component);

void EW_draw(void);

void TMENU_drawDefault(TComponent *component);

void TSLIDER_drawDefault(TComponent *component);

void TWIN_draw2(TComponent *component);

void TWIN_drawScene2(TComponent *component);

extern const char D_004DA420[];
const char D_004DA420[8] = "\x0b";

/* Defined by xgl_font.c; `text` is a font control-code string. */

extern void xglFontPrintDirectOT(int ot, const char *text);

/*
 * The font control strings TWSYS_draw emits before a component's own drawing
 * code: the same setup for every window-like kind, and a variant for the
 * scene window, differing only in the two counts at offsets 1 and 4.
 */

#define TWSYS_DRAW_WINDOW_FONT "\x0d\x02\x0e\x02\x03\x18\x18\x18\x0f\x00\x00\x80\x0c\x80\x80\x80"

#define TWSYS_DRAW_SCENE_FONT  "\x0d\x03\x0e\x02\x02\x18\x18\x18\x0f\x00\x00\x80\x0c\x80\x80\x80"

/*
 * The text-window record TWSYS_createComponent hands back (still assembler
 * in this TU). Only the fields this allocation and its evidenced siblings
 * touch are modeled:
 *  - +0xC/+0xE width/height: TW_setPos (0x0025d660) reads both as unsigned
 *    halfwords for its on-screen centering math.
 *  - +0xA6 line_pitch: TWIN_drawScene2 (0x0025ec24, still assembler)
 *    multiplies the current line index (+0x185) by this halfword to place
 *    each text line, so it is the per-line vertical advance.
 *  - +0x186 layout_offset: set here and by TWIN_create2's own default
 *    (still assembler); no function recovered so far in this TU reads it
 *    back, so only that it is a byte is proven.
 *  - +0x187 line_count: TWIN_update2 and TWIN_popCF/TWIN_popScene (still
 *    assembler) loop up to this byte over the +0x17C line-pointer array.
 */

typedef struct TWindow {
    unsigned char unmodeled_00[0xC];
    unsigned short width;          /* +0xC */
    unsigned short height;         /* +0xE */
    unsigned int flags;            /* +0x10 */
    unsigned short state;          /* +0x14 */
    unsigned short kindAndGroup;   /* +0x16 */
    unsigned char unmodeled_18[0x32 - 0x18];
    unsigned short closeState;     /* +0x32 */
    unsigned char unmodeled_34[0xA4 - 0x34];
    short currentPage;             /* +0xA4 */
    short line_pitch;              /* +0xA6 */
    char *textOutput;              /* +0xA8 */
    unsigned char unmodeled_ac[0xF0 - 0xAC];
    unsigned int initializationWord0; /* +0xF0 */
    unsigned char unmodeled_f4[0x108 - 0xF4];
    unsigned int initializationWord1; /* +0x108 */
    unsigned char unmodeled_10c[0x17C - 0x10C];
    char **lineBufferPointers;     /* +0x17C */
    char *textScratchBuffer;       /* +0x180 */
    unsigned char lineCharacterOffset; /* +0x184 */
    unsigned char currentLineIndex; /* +0x185 */
    unsigned char layout_offset;   /* +0x186 */
    unsigned char line_count;      /* +0x187 */
    char *lineData[1];             /* +0x188; inline pointer and character buffers follow */
} TWindow;

/* Configures a field-conversation text window's line pitch, layout offset,
 * line count and on-screen size. */

/* Configures a scene-message text window's line pitch, layout offset, line
 * count and on-screen size. */

void EW_dispose(int handle);

void MBUF_dispose(int *mbuf);

TComponent *TWSYS_createComponent(int slotIndex, int kindAndGroup);

void TMENU_init(TComponent *component);

/*
 * The menu-kind component TMENU_create hands back (still assembler in this
 * TU). Only the fields this allocation touches are modeled:
 *  - +0x10 flags: TMENU_dispose clears bits 0x11 when it releases the menu.
 *  - +0x60/+0x64 msgBuffer: TMENU_dispose passes each non-zero entry to
 *    MBUF_dispose.
 *  - +0xD8 queryQueue: TMENU_addQuery pushes a converted message onto it
 *    (an opaque handle: the address is passed on, never dereferenced here).
 *  - +0xFC ewHandle: TMENU_dispose passes it to EW_dispose.
 *  - +0x148 activeQueryId: TMENU_addQuery leaves it alone once it holds a
 *    query.
 *  - +0x154 nextQueryId: copied into activeQueryId when that one is empty.
 */

typedef struct TMenu {
    unsigned char unmodeled_00[0x0C];
    short width;                   /* +0x0C */
    short height;                  /* +0x0E */
    unsigned int flags;                /* +0x10 */
    unsigned char unmodeled_14[0x56 - 0x14];
    unsigned char itemCount;       /* +0x56 */
    unsigned char unmodeled_57[0x5A - 0x57];
    short visibleLineCount;        /* +0x5A */
    unsigned char unmodeled_5c[0x60 - 0x5C];
    int *msgBuffer[2];                 /* +0x60 */
    unsigned char unmodeled_68[0xD8 - 0x68];
    unsigned char queryQueue;          /* +0xD8 */
    unsigned char unmodeled_d9[0xFC - 0xD9];
    int ewHandle;                      /* +0xFC */
    unsigned char unmodeled_100[0x140 - 0x100];
    unsigned char itemCapacity;    /* +0x140 */
    unsigned char unmodeled_141[0x148 - 0x141];
    int activeQueryId;                 /* +0x148 */
    const char **itemTexts;       /* +0x14C */
    unsigned char unmodeled_150[0x154 - 0x150];
    int nextQueryId;                   /* +0x154 */
} TMenu;

void MSG_convert(int *destination, int destinationSize, int messageId, int encoding);

int MSG_queuePush(int queue, int *message, int messageSize, int encoding);

extern unsigned char radixTenUnits[100];

extern unsigned char radixTenTenths[100];

extern unsigned char digits[16];

extern void *memcpy(void *destination, const void *source, unsigned int size);

extern int MSG_copyln(char **destination, const char **source, int flags);

void TWSYS_init(void)
{
    TComponent *component;
    TComponent *end;
    unsigned char *groupByte;
    int count;

    component = tcomponent;
    end = tcomponent + 4;
    do {
        component->flags = 0;
        component->closeState = 0;
        component++;
    } while ((int)component < (int)end);
    count = 3;
    groupByte = &D_004DC593;
    do {
        count -= 1;
        *groupByte = 0;
        groupByte -= 1;
    } while (count >= 0);
    EW_init((unsigned char *)end);
    MBUF_init();
}

void TWSYS_setGRPStatus(int group, int status)
{
    groupStatus[group] = (unsigned char)status;
}

unsigned char TWSYS_getGRPStatus(int group)
{
    return groupStatus[group];
}

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWSYS_createComponent);

void TWSYS_update(void)
{
    TComponent *component;
    int slotIndex;
    unsigned int group;
    unsigned int kind;

    component = tcomponent;
    slotIndex = 0;
    do {
        group = (component->kindAndGroup >> 4) & 3;
        if (!(groupStatus[group] & 1)) {
            kind = component->kindAndGroup & 0xF;
            if (component->flags & 0x10) {
                switch (kind) {
                case 1:
                    TMENU_updateDefault(component);
                    break;
                case 2:
                    TSLIDER_updateDefault(component);
                    break;
                case 0:
                case 3:
                    TWIN_update2(component);
                    break;
                case 4:
                    TWIN_update2(component);
                    break;
                }
            }
        }
        slotIndex += 1;
        component++;
    } while (slotIndex < 4);
}

void TWSYS_draw(void)
{
    TComponent *component;
    int slotIndex;
    unsigned int group;
    unsigned int kind;

    component = tcomponent;
    slotIndex = 0;
    do {
        group = (component->kindAndGroup >> 4) & 3;
        if (!(groupStatus[group] & 2)) {
            if (component->flags & 0x10) {
                xglFontPrintDirectOT(0xFFFFFF, D_004DA420);
                kind = component->kindAndGroup & 0xF;
                switch (kind) {
                case 1:
                    xglFontPrintDirectOT(0xFFFFFF, TWSYS_DRAW_WINDOW_FONT);
                    TMENU_drawDefault(component);
                    break;
                case 2:
                    xglFontPrintDirectOT(0xFFFFFF, TWSYS_DRAW_WINDOW_FONT);
                    TSLIDER_drawDefault(component);
                    break;
                case 0:
                case 3:
                    xglFontPrintDirectOT(0xFFFFFF, TWSYS_DRAW_WINDOW_FONT);
                    TWIN_draw2(component);
                    break;
                case 4:
                    xglFontPrintDirectOT(0xFFFFFF, TWSYS_DRAW_SCENE_FONT);
                    TWIN_drawScene2(component);
                    break;
                }
            }
        }
        slotIndex += 1;
        component++;
    } while (slotIndex < 4);
    EW_draw();
}

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", PARSE_int);

unsigned char *STRING_int(unsigned char *destination, int value)
{
    unsigned char buffer[16];
    unsigned char *cursor;
    unsigned int valueBits;
    int magnitude;
    int pair;
    int radix;
    int negative;
    int start;

    magnitude = value;
    valueBits = (unsigned int)magnitude;
    negative = valueBits >> 31;
    if (negative) {
        magnitude = -magnitude;
    }
    radix = 100;
    cursor = &buffer[12];
    start = 12;
    do {
        pair = magnitude % radix;
        cursor--;
        start--;
        *cursor = radixTenUnits[pair];
        cursor--;
        start--;
        *cursor = radixTenTenths[pair];
        magnitude /= radix;
    } while (magnitude != 0);

    if (*cursor == '0') {
        start++;
    }
    if (negative) {
        start--;
        buffer[start] = '-';
    }
    memcpy(destination, &buffer[start], 12 - start);
    return destination - start + 12;
}

void STRING_h2zEUC(char *destination, const char *source)
{
    int character;

    character = *source++;
    while (character != 0) {
        if (character >= '0' && character <= '9') {
            *destination++ = (char)0xA3;
            *destination++ = character - 0x80;
        } else if (character >= 'a' && character <= 'z') {
            *destination++ = (char)0xA3;
            *destination++ = character - 0x80;
        } else if (character >= 'A' && character <= 'Z') {
            *destination++ = (char)0xA3;
            *destination++ = character + 0x60;
        } else if (character < 160) {
        } else {
            *destination++ = character;
            *destination++ = (unsigned char)*source++;
        }
        character = *source++;
    }
    *destination = 0;
}

void STRING_toUInt(void *unused, int value, int bitsPerDigit)
{
    char buffer[32];
    unsigned int digitMask;
    int position;

    digitMask = (1u << bitsPerDigit) - 1;
    position = 32;
    do {
        buffer[--position] = digits[value & digitMask];
        value >>= bitsPerDigit;
    } while (value != 0);
}

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TW_setPos);

void WIN_initCF(TWindow *window)
{
    window->line_pitch = 0x18;
    window->layout_offset = 0x2A;
    window->line_count = 3;
    window->width = 0x1CC;
    window->height = 0x50;
}

void WIN_initScene(TWindow *window)
{
    window->line_pitch = 0x1A;
    window->layout_offset = 0x30;
    window->line_count = 3;
    window->width = 0x1E0;
    window->height = 0x56;
}

void TWIN_init2(TWindow *window)
{
    unsigned int kind;
    unsigned int flagsValue;
    unsigned int stateValue;
    char *lineBuffer;
    int lineIndex;

    window->initializationWord1 = 0;
    window->currentLineIndex = 0;
    kind = window->kindAndGroup & 0xF;
    window->lineCharacterOffset = 0;
    window->currentPage = 0;
    switch (kind) {
    case 3:
    default:
        WIN_initCF(window);
        break;
    case 4:
        WIN_initScene(window);
        break;
    }

    lineBuffer = (char *)window->lineData;
    window->lineBufferPointers = (char **)lineBuffer;
    lineBuffer += window->line_count * sizeof(*window->lineBufferPointers);
    for (lineIndex = 0; lineIndex < window->line_count; lineIndex++) {
        window->lineBufferPointers[lineIndex] = lineBuffer;
        *lineBuffer = 0;
        lineBuffer += 0xC0;
    }
    window->textOutput = lineBuffer;
    flagsValue = 0x54;
    *lineBuffer = 0;
    lineBuffer += 0x80;
    stateValue = 1;
    window->textScratchBuffer = lineBuffer;
    window->state = (unsigned short)stateValue;
    window->flags = flagsValue;
    window->closeState = 0;
    window->initializationWord0 = 0;
}

int WIN_checkActiveWindow(void)
{
    TComponent *component;
    int slotIndex;
    unsigned int group;

    component = tcomponent;
    for (slotIndex = 0; slotIndex < 4; slotIndex++, component++) {
        group = (component->kindAndGroup >> 4) & 3;
        if (groupStatus[group] & 1) {
            continue;
        }
        if (component->flags & 0x10) {
            return 1;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_create2);

void TWIN_dispose(TComponent *window)
{
    window->flags &= ~0x11;
    EW_dispose(window->ewHandle);
    if (window->msgBuffer[0] != 0) {
        MBUF_dispose(window->msgBuffer[0]);
    }
    if (window->msgBuffer[1] != 0) {
        MBUF_dispose(window->msgBuffer[1]);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_popCF);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_popScene);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_update2);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_draw2);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_drawScene2);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_init);

TComponent *TMENU_create(int slotIndex)
{
    TComponent *component;

    component = TWSYS_createComponent(slotIndex, 1);
    if (component == 0) {
        return 0;
    }

    TMENU_init(component);
    component->width = 48.0f;
    component->height = 64.0f;
    return component;
}

void TMENU_dispose(TMenu *menu)
{
    menu->flags &= ~0x11;
    EW_dispose(menu->ewHandle);
    if (menu->msgBuffer[0] != 0) {
        MBUF_dispose(menu->msgBuffer[0]);
    }
    if (menu->msgBuffer[1] != 0) {
        MBUF_dispose(menu->msgBuffer[1]);
    }
}

void TMENU_addQuery(TMenu *menu, int messageId)
{
    int message[0x100];

    MSG_convert(message, 0x400, messageId, -1);
    MSG_queuePush((int)&menu->queryQueue, message, 0x400, -1);
    if (menu->activeQueryId == 0) {
        menu->activeQueryId = menu->nextQueryId;
    }
}

void TMENU_setItem(TMenu *menu, int itemIndex, const char *text)
{
    char output[0x400];
    char *outputCursor;
    const char *inputCursor;
    int candidateIndex;
    int lineWidth;
    unsigned char itemCapacity;

    itemCapacity = menu->itemCapacity;
    if (itemIndex < 0) {
        for (candidateIndex = 0; candidateIndex < itemCapacity; candidateIndex++) {
            if (menu->itemTexts[candidateIndex] == 0) {
                itemIndex = candidateIndex;
                break;
            }
        }
        if (itemIndex < 0)
            return;
        menu->itemCount++;
    }

    menu->itemTexts[itemIndex] = text;
    outputCursor = output;
    inputCursor = text;
    lineWidth = MSG_copyln(&outputCursor, &inputCursor, 0);
    menu->height = menu->visibleLineCount * 24 + 4;
    if (menu->width < lineWidth + 56) {
        menu->width = lineWidth + 56;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_addItem);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_updateDefault);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_drawDefault);
