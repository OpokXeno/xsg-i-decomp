#include "common.h"
#include "shared.h"

extern void xglSoundEffectNormalID(int soundEffectId, int pitch);

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

#include "main/msg_init.h"
#include "main/ew_sprt_set_cursor_uv.h"

typedef struct TComponent {
    unsigned char unmodeled_00[0xc];
    unsigned short pixelWidth; /* +0xc */
    unsigned short pixelHeight;        /* +0xE */
    unsigned int flags;                /* +0x10 */
    unsigned char unmodeled_14[0x16 - 0x14];
    unsigned short kindAndGroup;       /* +0x16 */
    unsigned char unmodeled_18[0x20 - 0x18];
    float width;                       /* +0x20: TMENU_create/TMENU_init */
    float height;                      /* +0x24: TMENU_create/TMENU_init */
    unsigned char unmodeled_28[0x32 - 0x28];
    unsigned short closeState;         /* +0x32 */
    unsigned char unmodeled_34[0x44 - 0x34];
    Vector4 *ownerPosition;            /* +0x44: TW_setPos's owner anchor */
    unsigned char unmodeled_48[0x54 - 0x48];
    int ewHandle;                      /* +0x54 */
    unsigned char unmodeled_58[0x9C - 0x58];
    int *msgBuffer[2];                 /* +0x9C */
    unsigned char unmodeled_a4[0x5B0 - 0xA4];
} TComponent;

void TW_setPos(TComponent *component);

static TComponent tcomponent[4];

static unsigned char groupStatus[3];

static unsigned char groupByteState;

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

extern const char twsysResetControl[];
/* Text: {reset} */
const char twsysResetControl[8] = "\x0b";

/* Defined by xgl_font.c; `text` is a font control-code string. */

extern void xglFontPrintDirectOT(int ot, const char *text);

/*
 * The font control strings TWSYS_draw emits before a component's own drawing
 * code: the same setup for every window-like kind, and a variant for the
 * scene window, differing only in the two counts at offsets 1 and 4.
 */

/* Text: {effect 2}{effect params}{blend}{color #808080} */
#define TWSYS_DRAW_WINDOW_FONT "\x0d\x02\x0e\x02\x03\x18\x18\x18\x0f\x00\x00\x80\x0c\x80\x80\x80"

/* Text: {effect 3}{effect params}{blend}{color #808080} */
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
    unsigned char unmodeled_018[0x20 - 0x18];
    float x; /* +0x20 */
    float y; /* +0x24 */
    unsigned char unmodeled_028[0x30 - 0x28];
    short waitFrames; /* +0x30 */
    unsigned short closeState;     /* +0x32 */
    unsigned char unmodeled_034[0x50 - 0x34];
    unsigned short textVerticalOffset; /* +0x50 */
    unsigned char unmodeled_052[0x54 - 0x52];
    EwContainerWidget *rootWidget; /* +0x54 */
    unsigned char unmodeled_058[0x9c - 0x58];
    MsgBuffer *messageBuffer[2]; /* +0x9c */
    short currentPage;             /* +0xA4 */
    short line_pitch;              /* +0xA6 */
    char *textOutput;              /* +0xA8 */
    MsgQueue queue; /* +0xac */
    unsigned int frameCounter; /* +0xcc */
    void *widgets[8]; /* +0xd0 */
    unsigned int initializationWord0; /* +0xF0 */
    unsigned char unmodeled_f4[0x108 - 0xF4];
    unsigned int initializationWord1; /* +0x108 */
    unsigned char unmodeled_10c[0x120 - 0x10c];
    int pendingMessage; /* +0x120 */
    unsigned char unmodeled_124[0x14c - 0x124];
    unsigned short lineWidths[24]; /* +0x14c */
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
    unsigned short state;        /* +0x14 */
    unsigned char unmodeled_16[0x20 - 0x16];
    float x;                     /* +0x20 */
    float y;                     /* +0x24 */
    unsigned char unmodeled_28[0x30 - 0x28];
    short waitFrames;            /* +0x30: signed opening and countdown frames */
    unsigned char unmodeled_32[0x50 - 0x32];
    unsigned int closePolicy;     /* +0x50: bit 0 selects terminal state */
    signed char selectedItem;     /* +0x54 */
    signed char selectionResult;  /* +0x55: -1 for cancelled selection */
    unsigned char itemCount;       /* +0x56 */
    unsigned char unmodeled_57;
    short firstVisibleItem;       /* +0x58 */
    short visibleLineCount;        /* +0x5A */
    unsigned char unmodeled_5c[0x60 - 0x5C];
    MsgBuffer *msgBuffer[2];            /* +0x60 */
    unsigned char requestedSelection; /* +0x68: opening flag 0x200 copies this index */
    unsigned char unmodeled_69[0xD8 - 0x69];
    MsgQueue queryQueue;               /* +0xD8 */
    unsigned int frameCounter;         /* +0xF8: cursor animation frame */
    int ewHandle;                      /* +0xFC */
    void *widgets[16];                 /* +0x100 */
    unsigned char itemCapacity;    /* +0x140 */
    unsigned char queryLineCount;  /* +0x141: MSG_queueGetInfo line count */
    signed char currentQueryLine;       /* +0x142: signed index read by text reveal */
    signed char queryCharacterOffset;   /* +0x143: signed byte offset within a query line */
    char **queryLines;                 /* +0x144 */
    char *activeQueryId;               /* +0x148 */
    const char **itemTexts;       /* +0x14C */
    unsigned char unmodeled_150[0x154 - 0x150];
    char *nextQueryId;                 /* +0x154 */
    const char *itemStorage[16];       /* +0x158 */
    char *queryLineStorage[3];         /* +0x198 */
    char queryText[1];                /* +0x1A4: inline string workspace */
} TMenu;

int MSG_queuePush(int queue, int *message, int messageSize, int encoding);

extern unsigned char radixTenUnits[100];

extern unsigned char radixTenTenths[100];

extern unsigned char digits[16];

extern void *memcpy(void *destination, const void *source, unsigned int size);

extern int MSG_copyln(char **destination, const char **source, int flags);

#include "twsys_init.h"

typedef union EwRootView { EwContainerWidget container; EwWidget widget; } EwRootView;

typedef union EwLabelBody {
    EwLabelWidget label;
    EwContainerWidget container;
} EwLabelBody;

typedef struct Matrix4Block {
    Matrix4 value;
} Matrix4Block;

typedef struct TransformParams {
    Matrix4 matrix;
    Matrix4 world;
    Vector4 screenScale;
    Vector4 screenOffset;
    Matrix4 *local;
} TransformParams;

void transformVector(Vector4 *destination, const Vector4 *vectors, int count,
                     TransformParams *params);

void xglStudioGetCamera(StudioCamera **camera, int index);

int XTK_getWindowOwner(void);

typedef struct WindowOwnerPrefix {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
} WindowOwnerPrefix;

static Vector4 vSample = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };

typedef struct EwSpriteState {
    short flags;
    signed char alpha;
    unsigned short u;
    unsigned short v;
} EwSpriteState;
typedef struct EwSpriteWidget {
    unsigned short flags;
    unsigned short type;
    short x;
    short y;
    int z;
    short width;
    short height;
    EwSpriteState sprite;
    unsigned char unmodeled_18[0x10];
} EwSpriteWidget;

void EW_sprtSetCursorUV(EwSpriteWidget *widget, int mode, int cell);

extern const char twinFontParamControl[];

extern const char twinStyleControl[];

typedef union WindowCoord {
    float value;
    int bits;
} WindowCoord;

typedef union EwLabelSlot {
    unsigned short *lineWidths;
    const unsigned char *textStyle;
    char **textLines;
    const char **readOnlyTextLines;
    int bits;
} EwLabelSlot;

extern const unsigned char twinLabelStyleControl[];

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
    groupByte = &groupByteState;
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

TComponent *TWSYS_createComponent(int slotIndex, int kindAndGroup)
{
    int candidateIndex;
    TComponent *component;

    if (slotIndex < 0) {
        for (candidateIndex = 0; candidateIndex < 4; candidateIndex++) {
            component = &tcomponent[candidateIndex];
            if (!(component->flags & 0x10)) {
                slotIndex = candidateIndex;
                break;
            }
        }
        if (slotIndex < 0) {
            return 0;
        }
    }
    component = &tcomponent[slotIndex];
    component->kindAndGroup = (slotIndex << 8) | kindAndGroup;
    return component;
}

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
                xglFontPrintDirectOT(0xFFFFFF, twsysResetControl);
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

unsigned int strlen(const char *string);

int PARSE_int(const char *text, int length, int radix)
{
    int value = 0;
    int index = 0;
    int negative = 0;
    int limit;
    int cutoff;
    unsigned int digit;

    if (length < 0) {
        length = strlen(text);
    }
    if (text[0] == '-') {
        negative = 1;
        index = 1;
        limit = -2147483647 - 1;
    } else {
        limit = -2147483647;
    }
    cutoff = limit / radix;
    if (index < length) {
        digit = text[index++];
        if (digit - '0' < 10) {
            digit -= '0';
        } else if (digit - 'a' < 26) {
            digit -= 'a';
            digit += 10;
        } else if (digit - 'A' < 26) {
            digit -= 'A';
            digit += 10;
        }
        value = -digit;
        while (index < length) {
            digit = text[index++];
            if (digit - '0' < 10) {
                digit -= '0';
            } else if (digit - 'a' < 26) {
                digit -= 'a';
                digit += 10;
            } else if (digit - 'A' < 26) {
                digit -= 'A';
                digit += 10;
            }
            if (value < cutoff) {
                return 0;
            }
            value *= radix;
            if (value < limit + (int)digit) {
                return 0;
            }
            value -= digit;
        }
    }
    if (negative) {
        return value;
    }
    return -value;
}

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

void TW_setPos(TComponent *component)
{
    Vector4 anchorPosition;
    Vector4 direction;
    Vector4 cameraDelta;
    Matrix4 savedMatrix;
    Vector4 offsets;
    StudioCamera *cameras[1];
    StudioCamera *sourceCamera;
    Vector4 *cameraPosition;
    WindowOwnerPrefix *owner;
    Vector4 *ownerPosition;
    Vector4 *projected;
    TransformParams *params = (TransformParams *)0x70001800;
    int halfWidth;
    int halfHeight;
    float screenShiftX;
    float screenShiftY;
    float distanceOffset;
    float rightEdge;

    owner = (WindowOwnerPrefix *)XTK_getWindowOwner();
    if (owner == 0 || !(component->flags & 0x40))
        return;

    projected = (Vector4 *)0x70001680;
    ownerPosition = &owner->position;
    anchorPosition.x = ownerPosition->x;
    anchorPosition.y = ownerPosition->y + 1.6f;
    anchorPosition.z = ownerPosition->z;
    component->ownerPosition = ownerPosition;
    anchorPosition.w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&anchorPosition.x);
    xglMatrixStackSave(savedMatrix);
    xglStudioGetCamera(&cameras[0], 0);
    sourceCamera = cameras[0];
    params->local = &savedMatrix;
    *(Matrix4Block *)params->world = *(Matrix4Block *)sourceCamera->viewMatrix;
    params->screenOffset = sourceCamera->screenOffset;
    params->screenScale = sourceCamera->screenScale;
    transformVector(projected, &vSample, 1, params);
    cameraPosition = &cameras[0]->position;
    projected->x -= 2048.0f;
    projected->y = projected->y - 2048.0f;

    /* Original VU0 operation: distance from the anchor to the camera.
     * vf2/vf3 are local VU scratch state; the stored vector feeds length. */
    __asm__ __volatile__(
        "lqc2 $vf3,0(%1)\n"
        "lqc2 $vf2,0(%2)\n"
        "vsub.xyz $vf2xyz,$vf2xyz,$vf3xyz\n"
        "sqc2 $vf2,0(%0)"
        : : "r"(&cameraDelta), "r"(cameraPosition), "r"(&anchorPosition)
        : "memory");
    xglVectorLength(&projected->z, &cameraDelta);
    if (projected->z > 2.5f)
        projected->z = 2.5f;
    projected->z = 2.5f - projected->z;
    direction.x = 0.0f < projected->x ? -1.0f : 1.0f;
    direction.y = -1.0f;
    halfWidth = (short)component->pixelWidth / 2;
    halfHeight = (short)component->pixelHeight / 2;
    offsets.x = (float)(halfWidth + 16) * direction.x;
    offsets.y = -(float)(halfHeight + 16);
    screenShiftY = (float)(224 - halfHeight);
    distanceOffset = 40.0f * projected->z;
    {
        float shiftX = offsets.x + distanceOffset * direction.x;

        projected->x += shiftX;
    }
    projected->y += offsets.y + distanceOffset * direction.y;
    screenShiftX = (float)(256 - halfWidth);
    projected->x += screenShiftX;
    projected->y += screenShiftY;
    if (8.0f > projected->x)
        projected->x = 8.0f;
    rightEdge = (float)(504 - (short)component->pixelWidth);
    if (projected->x > rightEdge)
        projected->x = rightEdge;
    if (8.0f > projected->y) {
        float clampDelta = projected->y - 8.0f;

        offsets.z = clampDelta;
        projected->y = 8.0f;
        projected->x -= offsets.z * direction.x * 0.5f;
    }
    if (projected->x < 8.0f)
        projected->x = 8.0f;
    if (rightEdge < projected->x)
        projected->x = rightEdge;
    if (projected->y > 224.0f)
        projected->y = 224.0f;
    component->width = projected->x;
    component->height = projected->y;
}

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

TWindow *TWIN_create2(int index)
{
    TWindow *window;
    MsgBuffer *messageBuffer;
    EwContainerWidget *root;
    EwMaskWidget *mask;
    EwLabelWidget *label;
    EwWidget *widget;
    int childIndex;

    window = (TWindow *)TWSYS_createComponent(index, 0);
    if (window == 0) {
        return 0;
    }
    messageBuffer = MBUF_create2();
    if (messageBuffer == 0) {
        return 0;
    }
    window->layout_offset = 45;
    window->line_count = 3;
    window->x = 16.0f;
    window->y = 305.0f;
    window->textVerticalOffset = 0;
    TWIN_init2(window);
    for (childIndex = 7; childIndex >= 0; childIndex--) {
        window->widgets[childIndex] = 0;
    }

    root = (EwContainerWidget *)EW_create(-1, 4);
    root->container.children = window->widgets;
    root->container.childCapacity = 8;
    root->flags |= 0x4000;
    window->rootWidget = root;

    widget = EW_create(-1, 10);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, 1, &widget->flags);
    mask = (EwMaskWidget *)window->widgets[1];
    mask->mask.fix = 0x00808000;
    mask->mask.blend |= 0x100;

    widget = EW_create(-1, 9);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, 0, &widget->flags);

    widget = EW_create(-1, 7);
    label = (EwLabelWidget *)widget;
    label->textLines = window->lineBufferPointers;
    label->lineCount = -1;
    EW_addComponent(root, 2, &widget->flags);

    widget = EW_create(-1, 9);
    widget->width = 512;
    widget->height = 448;
    widget->x = 0;
    widget->y = 0;
    widget->drawValue = 0;
    EW_addComponent(root, 3, &widget->flags);
    widget->flags |= 0x4000;

    widget = EW_create(-1, 1);
    EW_addComponent(root, 4, &widget->flags);

    widget = EW_create(-1, 5);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, 5, &widget->flags);

    window->messageBuffer[0] = messageBuffer;
    window->queue.buffer = messageBuffer->cursor;
    window->queue.size = messageBuffer->capacity;
    window->messageBuffer[1] = 0;
    window->queue.head = 0;
    window->queue.tail = 0;
    window->queue.count = 0;
    window->queue.width = 0;
    switch (window->kindAndGroup & 15) {
    case 3:
    default:
        WIN_initCF(window);
        break;
    case 4:
        WIN_initScene(window);
        break;
    }
    return window;
}

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

void TWIN_popCF(TWindow *window)
{
    MsgQueue *queue;
    int code;

    if ((window->flags & 3) != 1) {
        return;
    }
    if (window->queue.tail <= 0) {
        return;
    }
    queue = &window->queue;
    window->queue.limit = (short)window->width - 24;
    window->lineCharacterOffset += MSG_queuePop(queue,
        (unsigned char *)window->lineBufferPointers[window->currentLineIndex]
            + window->lineCharacterOffset, 2);
    window->lineWidths[window->currentLineIndex] = window->queue.width;
    code = window->queue.last;
    if (code >= 128) {
        switch (code) {
        case 131:
            window->state = 13;
            window->waitFrames = window->queue.label;
            break;
        case 132:
            window->state = 2;
            break;
        case 129:
            window->queue.count = 0;
            window->state = 9;
            window->queue.width = 0;
            break;
        case 128:
            window->state = 8;
            window->pendingMessage = window->queue.label;
            break;
        case 133:
            strcpy(window->textOutput, (const char *)window->queue.label);
            break;
        }
    } else if (window->queue.width >= window->queue.limit || code == 10) {
        MSG_queueReset(queue, 10);
        if (window->currentLineIndex >= window->line_count - 1
            && window->currentPage >= window->line_count - 1) {
            window->state = 17;
        } else {
            window->state = 10;
        }
    }
    if (window->queue.head < window->queue.tail) {
        return;
    }
    MSG_queueReset(queue, 0);
}

void TWIN_popScene(TWindow *window)
{
    TComponent *component = (TComponent *)window;
    int code;
    MsgQueue *queue;

    if ((window->flags & 3) != 1) {
        return;
    }
    if (window->queue.tail <= 0) {
        return;
    }
    queue = &window->queue;
    do {
        window->queue.limit = component->pixelWidth;
        window->lineCharacterOffset += MSG_queuePop(queue,
            (unsigned char *)window->lineBufferPointers[window->currentLineIndex]
                + window->lineCharacterOffset, 24);
        window->lineWidths[window->currentLineIndex] = window->queue.width;
        code = window->queue.last;
        if (code >= 128) {
            switch (code) {
            case 131:
                window->state = 13;
                window->waitFrames = window->queue.label;
                break;
            case 132:
                window->state = 2;
                break;
            case 129:
                window->queue.count = window->queue.width = 0;
                window->state = 9;
                break;
            case 128:
                window->state = 8;
                window->pendingMessage = window->queue.label;
                break;
            case 133:
                strcpy(window->textOutput, (const char *)window->queue.label);
                break;
            }
            return;
        }
        if (window->queue.width >= (short)component->pixelWidth || code == 10) {
            MSG_queueReset(queue, 266);
            window->currentLineIndex++;
            window->lineCharacterOffset = 0;
            if ((unsigned int)window->currentLineIndex >= window->line_count) {
                window->currentLineIndex = window->line_count - 1;
                window->state = 12;
                window->pendingMessage = window->queue.label;
                return;
            }
        }
    } while (window->queue.head < window->queue.tail);
    window->queue.tail = window->queue.head = 0;
}

void TWIN_update2(TComponent *component)
{
    TWindow *window = (TWindow *)component;
    unsigned int flags;
    EwMaskWidget *mask;
    EwLabelWidget *label;
    int alpha;
    int index;
    int lineIndex;
    char *firstLine;

    window->frameCounter++;
    flags = window->flags;
    if (flags & 1) {
        mask = (EwMaskWidget *)window->widgets[1];
        mask->mask.blend |= 0x100;
        alpha = (mask->mask.fix >> 8) & 0xFF;
        if (alpha != 1) {
            alpha -= 4;
            if (alpha <= 0) {
                label = (EwLabelWidget *)window->widgets[2];
                label->colorScale = 128;
                alpha = 1;
            }
            mask->mask.fix = (alpha << 8) | (alpha << 16);
        }
    }
    switch (window->kindAndGroup & 15) {
    case 3:
    default:
        TWIN_popCF(window);
        break;
    case 4:
        TWIN_popScene(window);
        break;
    }
    if (!(flags & 2)) {
        switch (window->state) {
        case 0:
            break;
        case 1:
        case 8:
        case 9:
        case 11:
        case 12:
        case 17:
            window->waitFrames = 0;
            window->flags |= 2;
            break;
        case 13:
            window->flags |= 2;
            break;
        case 2:
            window->waitFrames = 10;
            window->flags |= 2;
            break;
        case 10:
            window->flags |= 2;
            window->waitFrames = 0;
            break;
        }
        return;
    }
    if (!(flags & 4)) {
        return;
    }
    switch (window->state) {
    case 8:
        window->waitFrames = (window->waitFrames + 1) & 31;
        if (PadData[0].pressed & 0x20) {
            window->flags &= ~2;
            window->state = 0;
            window->closeState = window->pendingMessage;
        }
        break;
    case 12:
        window->waitFrames = (window->waitFrames + 1) & 31;
        if (PadData[0].pressed & 0x20) {
            window->flags &= ~2;
            window->state = 0;
            window->lineCharacterOffset = 0;
            window->currentLineIndex = 0;
            for (lineIndex = 0; lineIndex < window->line_count; lineIndex++) {
                window->lineBufferPointers[lineIndex][0] = '\0';
            }
        }
        break;
    case 13:
        window->waitFrames--;
        if ((short)window->waitFrames < 0) {
            window->flags &= ~2;
            window->state = 0;
        }
        break;
    case 9:
        window->flags &= ~2;
        window->state = 0;
        window->currentPage = 0;
        window->lineCharacterOffset = 0;
        window->currentLineIndex = 0;
        for (lineIndex = 0; lineIndex < window->line_count; lineIndex++) {
            window->lineBufferPointers[lineIndex][0] = '\0';
        }
        break;
    case 2:
        window->waitFrames--;
        if ((short)window->waitFrames < 0) {
            window->closeState = 255;
            TWIN_dispose(component);
        }
        break;
    case 11:
        if (PadData[0].pressed & 0x20) {
            window->flags &= ~2;
            window->state = 0;
            window->currentLineIndex = 0;
            window->currentPage = 0;
            for (lineIndex = 0; lineIndex < window->line_count; lineIndex++) {
                window->lineBufferPointers[lineIndex][0] = '\0';
            }
        }
        window->waitFrames++;
        break;
    case 17:
        window->waitFrames++;
        if (PadData[0].pressed & 0x20) {
            window->waitFrames = 0;
            window->state = 10;
        }
        break;
    case 10:
        if (window->currentLineIndex >= window->line_count - 1) {
            window->textVerticalOffset = (int)((float)(short)window->waitFrames * -6.0f);
            window->waitFrames++;
            if ((short)window->waitFrames >= 5) {
                firstLine = window->lineBufferPointers[0];
                for (index = 0; index < window->line_count - 1; index++) {
                    window->lineWidths[index] = window->lineWidths[index + 1];
                    window->lineBufferPointers[index] = window->lineBufferPointers[index + 1];
                }
                firstLine[0] = '\0';
                window->state = 0;
                window->lineBufferPointers[window->currentLineIndex] = firstLine;
                window->flags &= ~2;
                window->lineCharacterOffset = 0;
                window->textVerticalOffset = 0;
                if (window->currentPage >= window->line_count - 1) {
                    window->currentPage = 0;
                } else {
                    window->currentPage++;
                }
            }
        } else {
            window->currentLineIndex++;
            window->currentPage++;
            window->flags &= ~2;
            window->state = 0;
            window->lineCharacterOffset = 0;
        }
        break;
    case 1:
        window->waitFrames++;
        if ((short)window->waitFrames < 2) {
            window->textVerticalOffset = 0;
        }
        if ((short)window->waitFrames >= 11) {
            window->state = 0;
            window->flags |= 1;
            window->flags &= ~2;
        }
        break;
    }
}

/* Font parameters installed before ordinary text and scene-window labels. */
const char twinFontParamControl[19] = "\r\002\016\002\003\030\030\030\017\000\000\200\014\200\200\200\031\003";

/* Text: {effect 3}{effect params}{blend}{color #808080}{char width 4}{spacing 0,0} */
const unsigned char twinLabelStyleControl[26] = "\r\003\016\002\002\030\030\030\017\000\000\200\014\200\200\200\025\002\002\004\025\003\003\000\000";

void TWIN_draw2(TComponent *component)
{
    TWindow *window = (TWindow *)component;
    EwWidget *mask;
    EwWidget *widget;
    EwLabelBody *body;
    int x;
    int y;
    float *windowX;
    float *windowY;

    if (window->state == 1 || window->state == 2) {
        return;
    }
    windowX = &window->x;
    windowY = &window->y;
    x = (int)(16.0f + *windowX);
    y = (int)(*windowY + 16.0f + 32.0f);
    mask = (EwWidget *)window->widgets[1];
    widget = (EwWidget *)window->widgets[4];
    mask->drawValue = 0xFFFFF1;
    widget->drawValue = 0xFFFFF0;
    mask->x = x;
    mask->y = y;
    mask->width = window->width;
    mask->height = window->height;
    mask->flags |= 0x4000;
    widget->x = x;
    widget->y = y;
    widget->width = window->width;
    widget->height = window->height;
    widget->flags |= 0x4000;
    widget = (EwWidget *)window->widgets[0];
    widget->x = x;
    widget->y = y;
    widget->width = window->width;
    widget->drawValue = 0xFFFFFF;
    widget->height = window->height;
    widget->flags |= 0x4000;
    body = (EwLabelBody *)window->widgets[2];
    widget = (EwWidget *)body;
    body->label.colorScale = 0;
    widget->x = x + 12;
    widget->drawValue = 0x00FFFFFF;
    widget->y = window->textVerticalOffset + (y + 4);
    body->label.textLines = window->lineBufferPointers;
    body->label.lineCount = window->line_count;
    widget->flags |= 0x4000;

    widget = (EwWidget *)window->widgets[5];
    if (window->state == 11 || window->state == 8 || window->state == 17) {
        x = (short)window->lineWidths[window->currentLineIndex];
        y = window->currentLineIndex * 24;
        if (x + 16 > window->queue.limit) {
            x = 0 - 8;
            y = y + 24;
        } else if (x == 0 && window->currentLineIndex != 0) {
            int previousLineWidth = (short)window->lineWidths[window->currentLineIndex - 1];
            if (window->queue.limit >= previousLineWidth + 16) {
                x = previousLineWidth;
                y -= 24;
            } else {
                x = -8;
            }
        }
        widget->drawValue = 0x00FFFFFF;
        x = (int)((float)x + (window->x + 20.0f + 16.0f));
        y = (int)((float)y + (window->y + 12.0f + 16.0f + 32.0f));
        widget->x = x;
        widget->y = y;
        EW_sprtSetCursorUV((EwSpriteWidget *)widget, 3, window->frameCounter);
        widget->flags |= 0x4000;
    } else {
        widget->flags &= 0xBFFF;
    }
    x = (int)(window->x + 16.0f);
    y = (int)(window->y + 16.0f);
    xglFontPrintDirectOT(0x00FFFFF0, twinFontParamControl);
    xglFontPrint(x, y, 0x00FFFFF0, window->textOutput);
    xglFontPrintDirectOT(0x00FFFFF0, twinStyleControl);
}

/* Text: {style 0} */
const char twinStyleControl[3] = "\031\000";

void TWIN_drawScene2(TComponent *component)
{
    TWindow *window = (TWindow *)component;
    EwWidget *mask;
    EwWidget *widget;
    int x;
    int y;
    int panelDrawValue;

    if (window->state != 1 && window->state != 2) {
        widget = (EwWidget *)window->widgets[0];
        mask = (EwWidget *)window->widgets[1];
        x = (int)(16.0f + ((WindowCoord *)&window->x)->value);
        y = (int)(((WindowCoord *)&window->y)->value + 16.0f);
        mask->drawValue = 0x00FFFFF1;
        widget->drawValue = 0x00FFFFFF;
        panelDrawValue = widget->drawValue;
        mask->x = x;
        mask->y = y;
        mask->width = (unsigned short)window->width;
        mask->height = window->height;
        mask->flags = mask->flags | 0x4000;
        widget->x = x;
        widget->y = y;
        widget->width = window->width;
        widget->height = window->height;
        widget->flags = widget->flags | 0x4000;

        widget = (EwWidget *)window->widgets[2];
        {
            EwLabelWidget *label = (EwLabelWidget *)widget;

            widget->x = x;
            widget->drawValue = panelDrawValue;
            widget->y = window->textVerticalOffset + (y + 4);
            widget->width = window->width;
            widget->height = window->height;
            ((EwLabelSlot *)&label->textLines)->textLines = window->lineBufferPointers;
            label->lineCount = window->line_count;
            ((EwLabelSlot *)&label->lineWidths)->lineWidths = window->lineWidths;
            label->linePitch = 26;
            ((EwLabelSlot *)&label->textStyle)->textStyle = twinLabelStyleControl;
            widget->flags |= 0x4000;
        }

        widget = (EwWidget *)window->widgets[5];
        if (window->state == 11 || window->state == 8
            || window->state == 12 || window->state == 17) {
            float cursorOriginX = 16.0f + (window->x + 4.0f);
            float cursorOriginY = 16.0f + (window->y + 12.0f);
            x = (short)window->width / 2
                + (short)window->lineWidths[window->currentLineIndex] / 2;
            y = window->currentLineIndex * window->line_pitch;
            y = (int)((float)y + cursorOriginY);
            x = (int)((float)x + cursorOriginX);
            widget->y = y;
            widget->x = x;
            widget->drawValue = 0x00FFFFFF;
            EW_sprtSetCursorUV((EwSpriteWidget *)widget, 3, window->frameCounter);
            widget->flags |= 0x4000;
        } else {
            widget->flags = widget->flags & 0xBFFF;
        }
    }
}

void TMENU_init(TComponent *component)
{
    MsgBuffer *MBUF_create(int byteSize, int messageCount);
    TMenu *menu;
    const char **cursor;
    unsigned char **sourceWords;
    char **destinationWords;
    void *slots;
    int count;
    unsigned char capacity;
    MsgBuffer *messageBuffer;
    EwContainerWidget *root;
    EwMaskWidget *mask;
    EwLabelWidget *label;
    EwWidget *widget;
    char **textLines;
    const char **itemTexts;

    menu = (TMenu *)component;
    capacity = 0x10;
    slots = menu->itemStorage;
    menu->itemTexts = slots;
    slots = menu->queryLineStorage;
    menu->closePolicy = 1;
    messageBuffer = MBUF_create(0x80, 3);
    menu->queryQueue.buffer = messageBuffer->cursor;
    menu->queryQueue.size = 256;
    cursor = menu->itemTexts;
    menu->msgBuffer[0] = messageBuffer;
    menu->itemCapacity = capacity;
    menu->queryQueue.head = 0;
    menu->queryQueue.tail = 0;
    menu->queryQueue.count = 0;
    menu->queryQueue.width = 0;
    menu->msgBuffer[1] = 0;
    count = 15;
    do {
        cursor[count] = 0;
        count -= 1;
    } while (count >= 0);
    menu->queryLines = slots;
    slots = (char **)slots + 3;
    menu->currentQueryLine = 0;
    count = 0;
    menu->queryCharacterOffset = 0;
    menu->queryLineCount = 0;
    menu->activeQueryId = 0;
    sourceWords = menu->msgBuffer[0]->entry_table;
    destinationWords = menu->queryLines;
    do {
        destinationWords[count] = (char *)sourceWords[count];
        count += 1;
    } while (count < 3);
    menu->nextQueryId = (char *)slots;
    count = 15;
    menu->flags = 0x15;
    menu->state = 1;
    menu->x = 120.0f;
    menu->y = 100.0f;
    menu->height = 0x20;
    menu->visibleLineCount = 4;
    menu->itemCount = 0;
    menu->firstVisibleItem = 0;
    menu->selectionResult = 0;
    menu->selectedItem = 0;
    menu->width = 0;
    for (count = 15; count >= 0; count--) {
        menu->widgets[count] = 0;
    }
    count = 0x10;
    root = (EwContainerWidget *)EW_create(-1, 4);
    root->container.children = menu->widgets;
    root->container.childCapacity = 16;
    ((EwRootView *)root)->widget.flags |= 0x4000;
    menu->ewHandle = (int)root;

    widget = EW_create(-1, 9);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, 0, &widget->flags);

    widget = EW_create(-1, 10);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, 1, &widget->flags);
    mask = (EwMaskWidget *)menu->widgets[1];
    mask->mask.fix = 0x00808000;
    mask->mask.blend |= 0x100;

    widget = EW_create(-1, 7);
    label = (EwLabelWidget *)widget;
    itemTexts = menu->itemTexts;
    label->colorScale = 0;
    ((EwLabelBody *)label)->label.lineCount = -1;
    ((EwLabelSlot *)&label->textLines)->readOnlyTextLines = itemTexts;
    EW_addComponent(root, 2, &widget->flags);

    widget = EW_create(-1, 7);
    label = (EwLabelWidget *)widget;
    textLines = menu->queryLines;
    label->colorScale = 0;
    ((EwLabelBody *)label)->label.lineCount = -1;
    label->textLines = textLines;
    EW_addComponent(root, 3, &widget->flags);

    widget = EW_create(-1, 9);
    widget->width = 512;
    widget->height = 448;
    widget->x = 0;
    widget->y = 0;
    widget->drawValue = 0;
    EW_addComponent(root, 4, &widget->flags);
    widget->flags |= 0x4000;

    widget = EW_create(-1, 1);
    EW_addComponent(root, 5, &widget->flags);

    widget = EW_create(-1, 5);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, 6, &widget->flags);

    widget = EW_create(-1, 5);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, 7, &widget->flags);

    widget = EW_create(-1, 5);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, 8, &widget->flags);
    menu->flags = 0x55;
    menu->state = 1;
}

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
        MBUF_dispose(&menu->msgBuffer[0]->active);
    }
    if (menu->msgBuffer[1] != 0) {
        MBUF_dispose(&menu->msgBuffer[1]->active);
    }
}

void TMENU_addQuery(TMenu *menu, const char *text)
{
    int message[0x100];

    MSG_convert((unsigned char *)message, 0x400, text, -1);
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

void TMENU_addItem(int menuHandle, const char *text)
{
    TMenu *menu = (TMenu *)menuHandle;
    unsigned char output[0x400];
    char *outputCursor;
    const char *inputCursor;
    char *lineStart;
    unsigned int itemCount;
    int maximumWidth;
    int lineWidth;
    int itemHeight;
    int slot;
    int moreLines;

    MSG_convert(output, 0x400, text, -1);
    lineStart = (char *)menu->nextQueryId;
    itemCount = (int)menu->itemCount;
    maximumWidth = menu->width;
    outputCursor = lineStart;
    inputCursor = (const char *)output;
    do {
        menu->itemTexts[itemCount++] = lineStart;
        lineWidth = MSG_copyln(&outputCursor, &inputCursor, 0);
        moreLines = inputCursor != 0;
        lineStart = outputCursor + 1;
        if (lineWidth + 56 > maximumWidth) {
            maximumWidth = lineWidth + 56;
        }
        outputCursor = lineStart;
        menu->nextQueryId = outputCursor;
    } while (moreLines);

    itemCount = 0;
    for (slot = 0; slot < menu->itemCapacity; slot++) {
        if (menu->itemTexts[slot] != 0) {
            itemCount++;
        }
    }
    menu->itemCount = itemCount;
    if (menu->activeQueryId != 0) {
        itemHeight = menu->visibleLineCount * 24;
        menu->height = menu->queryLineCount * 24 + itemHeight + 16;
    } else {
        itemHeight = menu->visibleLineCount * 24 + 4;
        menu->height = itemHeight + menu->queryLineCount * 24;
    }
    if (menu->width < maximumWidth) {
        menu->width = maximumWidth;
    }
}

void TMENU_updateDefault(TComponent *component)
{
    TMenu *menu = (TMenu *)component;
    unsigned int flags;
    MsgQueueInfo info;
    EwMaskWidget *mask;
    EwWidget *widget;
    int oldSelection;
    int alpha;
    int secondaryAlpha;
    int code;

    flags = menu->flags;
    if (!(flags & 0x10)) {
        return;
    }
    if (menu->itemCount < menu->visibleLineCount) {
        menu->visibleLineCount = menu->itemCount;
    }
    menu->frameCounter++;
    if (!(flags & 2)) {
        switch (menu->state) {
        case 16: {
            EwMaskWidget *initialMask;
            menu->flags = flags | 2;
            initialMask = (EwMaskWidget *)menu->widgets[1];
            menu->waitFrames = 0;
            initialMask->mask.fix = 0x00808000;
            initialMask->mask.blend |= 0x100;
            break;
        }
        case 1:
            menu->flags = flags | 2;
            menu->waitFrames = 0;
            menu->height = menu->visibleLineCount * 24 + 4;
            MSG_queueGetInfo(&menu->queryQueue, &info, 0);
            if (info.lines > 0) {
                menu->height += info.lines * 24 + 12;
            }
            if (menu->width < info.width + 36) {
                menu->width = info.width + 36;
            }
            menu->queryLineCount = (unsigned char)info.lines;
            break;
        case 14:
        case 15:
            menu->flags = flags | 2;
            menu->waitFrames = 0;
            /* The initial input states continue through the wait-state setup. */
        case 13:
            menu->flags |= 2;
            break;
        case 2:
            menu->waitFrames = 10;
            menu->flags = flags | 2;
            widget = (EwWidget *)menu->ewHandle;
            widget->flags &= 0xBFFF;
            break;
        }
        return;
    }
    if (!(flags & 4)) {
        return;
    }
    switch (menu->state) {
    case 15:
        menu->flags = flags & ~2;
        if (menu->closePolicy & 1) {
            menu->state = 2;
        } else {
            menu->state = 0;
        }
        break;
    case 14:
        if (PadData[0].mappedPressed & 0x40) {
            xglSoundEffectNormalID(2, 0);
            menu->flags &= ~2;
            menu->state = 15;
            menu->selectionResult = -1;
            break;
        }
        oldSelection = menu->selectedItem;
        if (PadData[0].mappedPressed & 0x20) {
            xglSoundEffectNormalID(1, 0);
            menu->flags &= ~2;
            menu->state = 15;
            break;
        }
        menu->waitFrames = (menu->waitFrames + 1) & 0xFF;
        if ((PadData[0].mappedRepeat & 0x1000)
            && oldSelection > 0 && menu->selectionResult == 0) {
            xglSoundEffectNormalID(3, 0);
            menu->selectedItem--;
            menu->waitFrames = 0;
        }
        if ((PadData[0].mappedRepeat & 0x4000)
            && menu->selectedItem < menu->itemCount - 1) {
            xglSoundEffectNormalID(3, 0);
            menu->selectedItem++;
            menu->waitFrames = 0;
        }
        if (menu->selectedItem != oldSelection) {
            if (menu->selectedItem < 0) {
                menu->selectedItem = 0;
            }
            if (menu->selectedItem >= menu->firstVisibleItem + menu->visibleLineCount) {
                menu->firstVisibleItem = menu->firstVisibleItem + 1;
            }
            if (menu->selectedItem >= menu->itemCount) {
                menu->selectedItem = menu->itemCount - 1;
            }
            if (menu->firstVisibleItem > menu->selectedItem) {
                menu->firstVisibleItem = menu->firstVisibleItem - 1;
            }
            if (menu->firstVisibleItem < 0) {
                menu->firstVisibleItem = 0;
            }
            if (menu->firstVisibleItem >= menu->itemCount - menu->visibleLineCount) {
                menu->firstVisibleItem = menu->itemCount - menu->visibleLineCount;
            }
        }
        break;
    case 13:
        menu->waitFrames--;
        if (menu->waitFrames < 0) {
            menu->flags = flags & ~2;
            menu->state = 16;
        }
        break;
    case 2:
        menu->waitFrames--;
        if (menu->waitFrames < 0) {
            TMENU_dispose(menu);
        }
        break;
    case 1:
        menu->waitFrames++;
        TW_setPos(component);
        if (menu->waitFrames >= 11) {
            menu->flags |= 1;
            /* Both query modes enter the same waiting state. */
            if (menu->activeQueryId != 0) {
                menu->state = 16;
                menu->flags &= ~2;
            } else {
                menu->state = 16;
                menu->flags &= ~2;
            }
        }
        if (menu->flags & 0x200) {
            menu->flags &= ~0x200;
            menu->selectedItem = menu->requestedSelection;
        }
        break;
    case 16:
        if (menu->waitFrames < 8) {
            alpha = (int)(128.0f - ((float)menu->waitFrames * 0.125f) * 128.0f);
        } else {
            alpha = 1;
        }
        if (menu->waitFrames < 4) {
            secondaryAlpha = (int)(128.0f - ((float)menu->waitFrames * 0.25f) * 128.0f);
        } else {
            secondaryAlpha = 1;
        }
        menu->waitFrames++;
        mask = (EwMaskWidget *)menu->widgets[1];
        mask->mask.fix = ((unsigned int)secondaryAlpha << 16)
            | ((unsigned int)alpha << 8);
        if (menu->waitFrames >= 8) {
            EwLabelWidget *queryLabel = (EwLabelWidget *)menu->widgets[2];
            EwLabelWidget *itemLabel = (EwLabelWidget *)menu->widgets[3];
            queryLabel->colorScale = 128;
            itemLabel->colorScale = 128;
        }
        widget = (EwWidget *)mask;
        widget->drawValue = 0x00FFFFF1;
        widget->x = (int)(menu->x + 16.0f);
        widget->y = (int)(menu->y + 16.0f);
        widget->width = menu->width;
        widget->height = menu->height;
        if (menu->queryQueue.tail > 0) {
            menu->queryCharacterOffset += MSG_queuePop(&menu->queryQueue,
                (unsigned char *)menu->queryLines[menu->currentQueryLine]
                    + menu->queryCharacterOffset, 1);
            code = menu->queryQueue.last;
            if (code >= 128) {
                switch (code) {
                case 131:
                    menu->state = 13;
                    menu->waitFrames = menu->queryQueue.label;
                    break;
                case 128:
                case 129:
                case 130:
                case 132:
                case 133:
                default:
                    break;
                }
            } else if (menu->queryQueue.width >= menu->width || code == 10) {
                menu->queryCharacterOffset = 0;
                menu->currentQueryLine++;
                MSG_queueReset(&menu->queryQueue, 10);
            }
        }
        if (menu->queryQueue.head >= menu->queryQueue.tail && menu->waitFrames >= 9) {
            MSG_queueReset(&menu->queryQueue, 0);
            menu->flags &= ~2;
            menu->state = 14;
            mask = (EwMaskWidget *)menu->widgets[1];
            mask->mask.fix = 0x00010100;
        }
        break;
    default:
        menu->waitFrames++;
        break;
    }
}

void TMENU_drawDefault(TComponent *component)
{
    TMenu *menu = (TMenu *)component;
    EwWidget *widget;
    EwLabelBody *body;
    int x;
    int y;
    int queryHeight;

    if (!(menu->flags & 0x10)) {
        return;
    }
    if (menu->state == 1 || menu->state == 2) {
        return;
    }
    x = (int)menu->x + 16;
    y = (int)menu->y + 16;
    widget = (EwWidget *)menu->widgets[1];
    widget->x = (int)(menu->x + 16.0f);
    widget->drawValue = 0xFFFFF1;
    widget->y = (int)(menu->y + 16.0f);
    widget->width = menu->width;
    widget->height = menu->height;
    widget->flags |= 0x4000;
    widget = (EwWidget *)menu->widgets[5];
    widget->drawValue = 0x00FFFFF0;
    widget->x = x;
    widget->y = y;
    widget->width = menu->width;
    widget->height = menu->height;
    widget->flags |= 0x4000;
    if (menu->queryLineCount != 0) {
        queryHeight = menu->queryLineCount * 24 + 12;
    } else {
        queryHeight = 0x0;
    }
    widget = (EwWidget *)menu->widgets[0];
    widget->drawValue = 0x00FFFFF1;
    widget->x = x;
    widget->y = y;
    widget->width = menu->width;
    widget->height = menu->height;
    widget->flags |= 0x4000;
    widget = (EwWidget *)menu->widgets[3];
    body = (EwLabelBody *)widget;
    body->label.textLines = menu->queryLines;
    widget->drawValue = 0xFFFFFF;
    widget->x = x;
    widget->y = y;
    body->label.lineCount = (short)menu->queryLineCount;
    body->label.firstLine = 0x0;
    widget->flags |= 0x4000;
    widget = (EwWidget *)menu->widgets[2];
    body = (EwLabelBody *)widget;
    widget->x = x + 20;
    widget->drawValue = 0x00FFFFFF;
    widget->y = y + queryHeight;
    body->label.firstLine = (short)menu->firstVisibleItem;
    ((EwLabelSlot *)&body->label.textLines)->readOnlyTextLines = menu->itemTexts;
    body->label.lineCount = menu->visibleLineCount + 1;
    widget->flags |= 0x4000;

    widget = (EwWidget *)menu->widgets[6];
    widget->x = x + menu->width - 16;
    widget->y = y;
    widget->drawValue = 0x00FFFFFF;
    EW_sprtSetCursorUV((EwSpriteWidget *)widget, 1, menu->frameCounter);
    if (menu->firstVisibleItem > 0) {
        widget->flags = widget->flags | 0x4000;
    } else {
        widget->flags = widget->flags & 0xBFFF;
    }
    widget = (EwWidget *)menu->widgets[7];
    widget->drawValue = 0x00FFFFFF;
    widget->x = (unsigned short)menu->width + x - 16;
    widget->y = (unsigned short)menu->height + y - 16;
    EW_sprtSetCursorUV((EwSpriteWidget *)widget, 2, menu->frameCounter);
    if (menu->firstVisibleItem + menu->visibleLineCount < menu->itemCount) {
        widget->flags |= 0x4000;
    } else {
        widget->flags &= 0xBFFF;
    }
    y = 0xC;
    if (menu->selectedItem >= 0) {
        y = (menu->selectedItem - menu->firstVisibleItem) * 24 + 12;
    }
    if (menu->queryLineCount != 0) {
        queryHeight = 24 * menu->queryLineCount + 12;
    } else {
        queryHeight = 0;
    }
    widget = (EwWidget *)menu->widgets[8];
    widget->drawValue = 0x00FFFFFF;
    /* Retain both floating zero-offset additions in the cursor layout. */
    x = (int)(menu->x + 20.0f + 0.0f + 0.0f);
    y = (int)((float)y + (menu->y + 16.0f - 6.0f + (float)queryHeight));
    widget->x = x;
    widget->y = y;
    EW_sprtSetCursorUV((EwSpriteWidget *)widget, 0, menu->frameCounter);
    widget->flags |= 0x4000;
}
