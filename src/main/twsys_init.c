#include "common.h"
#include "shared.h"
#include "main/db_light_write.h"
#include "main/msg_init.h"
#include "main/ew_sprt_set_cursor_uv.h"
#include "main/transform_vector.h"
#include "main/twsys_init.h"
#include "twsys_init.h"
#include "main/xgl_sound.h"
#include "vu0_macro.h"

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
 * The text-window kind of TComponent (include/main/twsys_init.h). Its width
 * at +0x0C is read as a signed short: TWIN_popScene compares a signed
 * halfword there, and TW_setPos sign-extends that halfword before its
 * centering calculation.
 *  - +0xC/+0xE pixelWidth/pixelHeight: TW_setPos reads both as signed
 *    halfwords for its on-screen centering math.
 *  - +0xA6 line_pitch: TWIN_drawScene2 multiplies the current line index
 *    (+0x185) by this halfword to place each text line, so it is the
 *    per-line vertical advance.
 *  - +0x186 layout_offset: set here and by TWIN_create2's default; no
 *    function in this file reads it back, so only its byte width is known.
 *  - +0x187 line_count: TWIN_update2, TWIN_popCF and TWIN_popScene loop up
 *    to this byte over the +0x17C line-pointer array.
 */

/*
 * TMenu belongs to main/tu143. TMENU_init initializes the pointer at +0x154
 * to its inline string workspace; TMENU_addItem advances it after each line.
 * The pointer at +0x148 retains the start of the active query text.
 */
/*
 * The menu-kind component TMENU_create returns. Only the fields this
 * allocation touches are modeled:
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
    union {
        int ewHandle;                  /* +0xFC: TMENU_dispose passes it to EW_dispose */
        EwContainerWidget *rootWidget; /* +0xFC: TMENU_init stores the type-4 root here */
    };
    EwWidget *widgets[TMENU_WIDGET_COUNT]; /* +0x100 */
    unsigned char itemCapacity;    /* +0x140 */
    unsigned char queryLineCount;  /* +0x141: MSG_queueGetInfo line count */
    signed char currentQueryLine;       /* +0x142: signed index read by text reveal */
    signed char queryCharacterOffset;   /* +0x143: signed byte offset within a query line */
    char **queryLines;                 /* +0x144 */
    char *activeQueryId;               /* +0x148 */
    const char **itemTexts;       /* +0x14C */
    unsigned char unmodeled_150[0x154 - 0x150];
    char *nextQueryId;                 /* +0x154 */
    const char *itemStorage[TMENU_ITEM_CAPACITY]; /* +0x158 */
    char *queryLineStorage[TMENU_QUERY_LINES];    /* +0x198 */
    char queryText[1];                /* +0x1A4: inline string workspace */
} TMenu;

typedef union EwLabelBody {
    EwLabelWidget label;
    EwContainerWidget container;
} EwLabelBody;

typedef struct WindowOwnerPrefix {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
} WindowOwnerPrefix;

/* A label's text-line slot as the menu fills it: the menu's item texts are
 * const strings (TMENU_setItem keeps the caller's text), the label reads the
 * same slot as its char ** line array. */
typedef union EwLabelSlot {
    char **textLines;
    const char **readOnlyTextLines;
} EwLabelSlot;

extern const char twsysResetControl[];
extern unsigned char radixTenUnits[100];
extern unsigned char radixTenTenths[100];
extern unsigned char digits[16];
extern const char twinFontParamControl[];
extern const char twinStyleControl[];
extern const unsigned char twinLabelStyleControl[];

void TW_setPos(TComponent *component);
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

/* Defined by xgl_font.c; `text` is a font control-code string. */
extern void xglFontPrintDirectOT(int ot, const char *text);

void EW_dispose(int handle);

void MBUF_dispose(int *mbuf);
void TMENU_init(TComponent *component);
int MSG_queuePush(int queue, int *message, int messageSize, int encoding);
extern void *memcpy(void *destination, const void *source, unsigned int size);
extern int MSG_copyln(char **destination, const char **source, int flags);

void xglStudioGetCamera(StudioCamera **camera, int index);
void *XTK_getWindowOwner(void);
void EW_sprtSetCursorUV(EwSpriteWidget *widget, int mode, int cell);
unsigned int strlen(const char *string);

/* EUC-JP: lead byte of the full-width digits and letters (row 3), and the first
 * lead byte of a two-byte character. */
#define EUC_FULLWIDTH_ALNUM_LEAD 0xA3

/* Layout offset and position TWIN_create2 starts from. */
#define TWIN_CREATE_LAYOUT_OFFSET 45
#define TWIN_CREATE_X 16.0f
#define TWIN_CREATE_Y 305.0f

/* Text: {reset} */
const char twsysResetControl[8] = "\x0b";

static Vector4 vSample = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };

/* Text: {style 0} */
const char twinStyleControl[3] = "\031\000";

static TComponent tcomponent[TWSYS_SLOT_COUNT];

static unsigned char groupStatus[3];

static unsigned char groupByteState;

/*
 * Clears every component slot and the group byte state, then initializes the
 * event widget and message buffer subsystems.
 */
void TWSYS_init(void)
{
    TComponent *component;
    TComponent *end;
    unsigned char *groupByte;
    int count;

    component = tcomponent;
    end = tcomponent + TWSYS_SLOT_COUNT;
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

/*
 * Returns the component at slotIndex, or the first inactive slot when
 * slotIndex is negative (NULL when none is free). The slot index is stored in
 * the high byte of kindAndGroup.
 */
TComponent *TWSYS_createComponent(int slotIndex, int kindAndGroup)
{
    int candidateIndex;
    TComponent *component;

    if (slotIndex < 0) {
        for (candidateIndex = 0; candidateIndex < TWSYS_SLOT_COUNT; candidateIndex++) {
            component = &tcomponent[candidateIndex];
            if (!(component->flags & TCOMP_FLAG_ACTIVE)) {
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

/* Runs the per-frame update of every active component whose group allows it. */
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
        if (!(groupStatus[group] & TWSYS_GROUP_UPDATE_OFF)) {
            kind = component->kindAndGroup & TCOMP_KIND_MASK;
            if (component->flags & TCOMP_FLAG_ACTIVE) {
                switch (kind) {
                case TCOMP_KIND_MENU:
                    TMENU_updateDefault(component);
                    break;
                case TCOMP_KIND_SLIDER:
                    TSLIDER_updateDefault(component);
                    break;
                case TCOMP_KIND_TEXT:
                case TCOMP_KIND_TEXT_FIELD:
                    TWIN_update2(component);
                    break;
                case TCOMP_KIND_TEXT_SCENE:
                    TWIN_update2(component);
                    break;
                }
            }
        }
        slotIndex += 1;
        component++;
    } while (slotIndex < TWSYS_SLOT_COUNT);
}

/*
 * Draws every active component whose group allows it, each after a font reset
 * and the font setup for its kind, then draws the event widgets.
 */
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
        if (!(groupStatus[group] & TWSYS_GROUP_DRAW_OFF)) {
            if (component->flags & TCOMP_FLAG_ACTIVE) {
                xglFontPrintDirectOT(EW_ORDER_TEXT, twsysResetControl);
                kind = component->kindAndGroup & TCOMP_KIND_MASK;
                switch (kind) {
                case TCOMP_KIND_MENU:
                    xglFontPrintDirectOT(EW_ORDER_TEXT, TWSYS_DRAW_WINDOW_FONT);
                    TMENU_drawDefault(component);
                    break;
                case TCOMP_KIND_SLIDER:
                    xglFontPrintDirectOT(EW_ORDER_TEXT, TWSYS_DRAW_WINDOW_FONT);
                    TSLIDER_drawDefault(component);
                    break;
                case TCOMP_KIND_TEXT:
                case TCOMP_KIND_TEXT_FIELD:
                    xglFontPrintDirectOT(EW_ORDER_TEXT, TWSYS_DRAW_WINDOW_FONT);
                    TWIN_draw2(component);
                    break;
                case TCOMP_KIND_TEXT_SCENE:
                    xglFontPrintDirectOT(EW_ORDER_TEXT, TWSYS_DRAW_SCENE_FONT);
                    TWIN_drawScene2(component);
                    break;
                }
            }
        }
        slotIndex += 1;
        component++;
    } while (slotIndex < TWSYS_SLOT_COUNT);
    EW_draw();
}

/*
 * Parses a signed integer in the given radix. A negative length means the
 * string is NUL-terminated. Returns 0 when the value overflows.
 */
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

/*
 * Writes value as decimal text (with a leading '-' when negative) into
 * destination, and returns a pointer past the last character written.
 */
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

/*
 * Converts an ASCII string to EUC-JP full-width letters and digits. Bytes below
 * the first multi-byte lead are dropped; two-byte characters are copied.
 */
void STRING_h2zEUC(char *destination, const char *source)
{
    int character;

    character = *source++;
    while (character != 0) {
        if (character >= '0' && character <= '9') {
            *destination++ = EUC_FULLWIDTH_ALNUM_LEAD;
            *destination++ = character - 0x80;
        } else if (character >= 'a' && character <= 'z') {
            *destination++ = EUC_FULLWIDTH_ALNUM_LEAD;
            *destination++ = character - 0x80;
        } else if (character >= 'A' && character <= 'Z') {
            *destination++ = EUC_FULLWIDTH_ALNUM_LEAD;
            *destination++ = character + 0x60;
        } else if (character < EUC_FIRST_MULTIBYTE_LEAD) {
        } else {
            *destination++ = character;
            *destination++ = *source++;
        }
        character = *source++;
    }
    *destination = 0;
}

/* Formats value into a local digit buffer in base 2^bitsPerDigit. The buffer is never read back. */
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

/*
 * Places a follow-owner component above its owner's position: projects the
 * owner's anchor through the camera, then clamps the box to the screen.
 */
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

    owner = XTK_getWindowOwner();
    if (owner == 0 || !(component->flags & TCOMP_FLAG_FOLLOW_OWNER))
        return;

    projected = (Vector4 *)0x70001680;
    ownerPosition = &owner->position;
    anchorPosition.x = ownerPosition->x;
    anchorPosition.y = ownerPosition->y + 1.6f;
    anchorPosition.z = ownerPosition->z;
    component->ownerPosition = ownerPosition;
    anchorPosition.w = 1.0f;
    /* Project the anchor through the camera into screen space. */
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
    cameraPosition = &cameras[0]->position.vector;
    projected->x -= 2048.0f;
    projected->y = projected->y - 2048.0f;

    /* Original VU0 operation: distance from the anchor to the camera.
     * vf2/vf3 are local VU scratch state; the stored vector feeds length. */
    Vu0SubVectorXYZ(&cameraDelta, &anchorPosition, cameraPosition);
    xglVectorLength(&projected->z, &cameraDelta);
    if (projected->z > 2.5f)
        projected->z = 2.5f;
    projected->z = 2.5f - projected->z;
    direction.x = 0.0f < projected->x ? -1.0f : 1.0f;
    direction.y = -1.0f;
    /* Offset the box from the anchor, scaled by the distance term. */
    halfWidth = (short)component->pixelWidth / 2;
    halfHeight = (short)component->pixelHeight / 2;
    offsets.x = (float)(halfWidth + 16) * direction.x;
    offsets.y = -(float)(halfHeight + 16);
    screenShiftY = (float)(EW_SCREEN_HEIGHT / 2 - halfHeight);
    distanceOffset = 40.0f * projected->z;
    {
        float shiftX = offsets.x + distanceOffset * direction.x;

        projected->x += shiftX;
    }
    projected->y += offsets.y + distanceOffset * direction.y;
    screenShiftX = (float)(EW_SCREEN_WIDTH / 2 - halfWidth);
    projected->x += screenShiftX;
    projected->y += screenShiftY;
    /* Keep the box on screen: x between 8 and rightEdge, y at least 8. */
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
    component->x = projected->x;
    component->y = projected->y;
}

/* Configures a field-conversation text window's line pitch, layout offset,
 * line count and on-screen size. */
void WIN_initCF(TComponent *window)
{
    window->line_pitch = TWIN_CF_LINE_PITCH;
    window->layout_offset = TWIN_CF_LAYOUT_OFFSET;
    window->line_count = TWIN_LINE_COUNT;
    window->pixelWidth = TWIN_CF_WIDTH;
    window->pixelHeight = TWIN_CF_HEIGHT;
}

/* Configures a scene-message text window's line pitch, layout offset, line
 * count and on-screen size. */
void WIN_initScene(TComponent *window)
{
    window->line_pitch = TWIN_SCENE_LINE_PITCH;
    window->layout_offset = TWIN_SCENE_LAYOUT_OFFSET;
    window->line_count = TWIN_LINE_COUNT;
    window->pixelWidth = TWIN_SCENE_WIDTH;
    window->pixelHeight = TWIN_SCENE_HEIGHT;
}

/*
 * Resets a text window's line state, lays out its line and caption buffers,
 * and marks it active, running, following its owner and opening.
 */
void TWIN_init2(TComponent *window)
{
    unsigned int kind;
    unsigned int flagsValue;
    unsigned int stateValue;
    char *lineBuffer;
    int lineIndex;

    window->initializationWord1 = 0;
    window->currentLineIndex = 0;
    kind = window->kindAndGroup & TCOMP_KIND_MASK;
    window->lineCharacterOffset = 0;
    window->currentPage = 0;
    switch (kind) {
    case TCOMP_KIND_TEXT_FIELD:
    default:
        WIN_initCF(window);
        break;
    case TCOMP_KIND_TEXT_SCENE:
        WIN_initScene(window);
        break;
    }

    lineBuffer = window->lineData;
    window->lineBufferPointers = (char **)lineBuffer;
    lineBuffer += window->line_count * sizeof(*window->lineBufferPointers);
    for (lineIndex = 0; lineIndex < window->line_count; lineIndex++) {
        window->lineBufferPointers[lineIndex] = lineBuffer;
        *lineBuffer = 0;
        lineBuffer += TWIN_LINE_BUFFER_SIZE;
    }
    window->textOutput = lineBuffer;
    flagsValue = TCOMP_FLAG_FOLLOW_OWNER | TCOMP_FLAG_ACTIVE | TCOMP_FLAG_RUN;
    *lineBuffer = 0;
    lineBuffer += TWIN_CAPTION_BUFFER_SIZE;
    stateValue = TCOMP_STATE_OPENING;
    window->textScratchBuffer = lineBuffer;
    window->state = (unsigned short)stateValue;
    window->flags = flagsValue;
    window->closeState = 0;
    window->initializationWord0 = 0;
}

/* Returns 1 when an active component is in a group that is not update-disabled, else 0. */
int WIN_checkActiveWindow(void)
{
    TComponent *component;
    int slotIndex;
    unsigned int group;

    component = tcomponent;
    for (slotIndex = 0; slotIndex < TWSYS_SLOT_COUNT; slotIndex++, component++) {
        group = (component->kindAndGroup >> 4) & 3;
        if (groupStatus[group] & TWSYS_GROUP_UPDATE_OFF) {
            continue;
        }
        if (component->flags & TCOMP_FLAG_ACTIVE) {
            return 1;
        }
    }
    return 0;
}

/*
 * Creates a text window with its widget tree and message buffer. Returns 0
 * when no component slot or message buffer is free. The label widget's
 * textLines points at the window's line buffer pointers.
 */
TComponent *TWIN_create2(int index)
{
    TComponent *window;
    MsgBuffer *messageBuffer;
    EwContainerWidget *root;
    EwMaskWidget *mask;
    EwLabelWidget *label;
    EwWidget *widget;
    int childIndex;

    window = TWSYS_createComponent(index, 0);
    if (window == 0) {
        return 0;
    }
    messageBuffer = MBUF_create2();
    if (messageBuffer == 0) {
        return 0;
    }
    window->layout_offset = TWIN_CREATE_LAYOUT_OFFSET;
    window->line_count = TWIN_LINE_COUNT;
    window->x = TWIN_CREATE_X;
    window->y = TWIN_CREATE_Y;
    window->textVerticalOffset = 0;
    TWIN_init2(window);
    for (childIndex = TWIN_WIDGET_COUNT - 1; childIndex >= 0; childIndex--) {
        window->widgets[childIndex] = 0;
    }

    /* Root container, then mask, clip, label, screen clip, frame and cursor. */
    root = (EwContainerWidget *)EW_create(-1, EW_TYPE_CONTAINER);
    root->container.children = window->widgets;
    root->container.childCapacity = TWIN_WIDGET_COUNT;
    root->flags |= EW_FLAG_VISIBLE;
    window->rootWidget = root;

    widget = EW_create(-1, EW_TYPE_MASK_TEXT_PASS);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, TWIN_WIDGET_MASK, widget);
    mask = (EwMaskWidget *)window->widgets[TWIN_WIDGET_MASK];
    mask->mask.fix = EW_MASK_FIX_FULL;
    mask->mask.blend |= EW_MASK_GRADIENT_HORIZONTAL;

    widget = EW_create(-1, EW_TYPE_CLIP_TEXT_PASS);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, TWIN_WIDGET_CLIP, widget);

    widget = EW_create(-1, EW_TYPE_LABEL);
    label = (EwLabelWidget *)widget;
    label->textLines = window->lineBufferPointers;
    label->lineCount = -1;
    EW_addComponent(root, TWIN_WIDGET_LABEL, widget);

    widget = EW_create(-1, EW_TYPE_CLIP_TEXT_PASS);
    widget->width = EW_SCREEN_WIDTH;
    widget->height = EW_SCREEN_HEIGHT;
    widget->x = 0;
    widget->y = 0;
    widget->drawValue = 0;
    EW_addComponent(root, TWIN_WIDGET_SCREEN_CLIP, widget);
    widget->flags |= EW_FLAG_VISIBLE;

    widget = EW_create(-1, EW_TYPE_FRAME);
    EW_addComponent(root, TWIN_WIDGET_FRAME, widget);

    widget = EW_create(-1, EW_TYPE_SPRITE);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, TWIN_WIDGET_CURSOR, widget);

    /* Attach the message buffer and reset the text queue. */
    window->messageBuffer[0] = messageBuffer;
    window->queue.buffer = messageBuffer->cursor;
    window->queue.size = messageBuffer->capacity;
    window->messageBuffer[1] = 0;
    window->queue.head = 0;
    window->queue.tail = 0;
    window->queue.count = 0;
    window->queue.width = 0;
    switch (window->kindAndGroup & TCOMP_KIND_MASK) {
    case TCOMP_KIND_TEXT_FIELD:
    default:
        WIN_initCF(window);
        break;
    case TCOMP_KIND_TEXT_SCENE:
        WIN_initScene(window);
        break;
    }
    return window;
}

void TWIN_dispose(TComponent *window)
{
    window->flags &= ~(TCOMP_FLAG_ACTIVE | TCOMP_FLAG_OPEN);
    EW_dispose(window->ewHandle);
    if (window->msgBuffer[0] != 0) {
        MBUF_dispose(window->msgBuffer[0]);
    }
    if (window->msgBuffer[1] != 0) {
        MBUF_dispose(window->msgBuffer[1]);
    }
}

/*
 * Pops the next queued unit into the current line of an open, not-waiting
 * field-conversation window, handling control codes and line wrap.
 */
void TWIN_popCF(TComponent *window)
{
    MsgQueue *queue;
    int code;

    if ((window->flags & (TCOMP_FLAG_OPEN | TCOMP_FLAG_WAIT)) != TCOMP_FLAG_OPEN) {
        return;
    }
    if (window->queue.tail <= 0) {
        return;
    }
    queue = &window->queue;
    window->queue.limit = (short)window->pixelWidth - 24;
    window->lineCharacterOffset += MSG_queuePop(queue,
        (unsigned char *)window->lineBufferPointers[window->currentLineIndex]
            + window->lineCharacterOffset, 2);
    window->lineWidths[window->currentLineIndex] = window->queue.width;
    code = window->queue.last;
    if (code >= MSG_CTRL_WAITKEY) {
        switch (code) {
        case MSG_CTRL_WAIT:
            window->state = TCOMP_STATE_WAIT_FRAMES;
            window->waitFrames = window->queue.label.value;
            break;
        case MSG_CTRL_CLOSE:
            window->state = TCOMP_STATE_CLOSING;
            break;
        case MSG_CTRL_CLEAR:
            window->queue.count = 0;
            window->state = TCOMP_STATE_CLEAR_TEXT;
            window->queue.width = 0;
            break;
        case MSG_CTRL_WAITKEY:
            window->state = TCOMP_STATE_WAIT_RESULT;
            window->pendingMessage = window->queue.label.value;
            break;
        case MSG_CTRL_CAPTION:
            strcpy(window->textOutput, (const char *)window->queue.label.text);
            break;
        }
    } else if (window->queue.width >= window->queue.limit || code == '\n') {
        MSG_queueReset(queue, 10);
        if (window->currentLineIndex >= window->line_count - 1
            && window->currentPage >= window->line_count - 1) {
            window->state = TCOMP_STATE_WAIT_SCROLL;
        } else {
            window->state = TCOMP_STATE_SCROLL_LINE;
        }
    }
    if (window->queue.head < window->queue.tail) {
        return;
    }
    MSG_queueReset(queue, 0);
}

/*
 * Pops queued units into the lines of an open, not-waiting scene window,
 * handling control codes and line wrap.
 */
void TWIN_popScene(TComponent *window)
{
    TComponent *component = window;
    int code;
    MsgQueue *queue;

    if ((window->flags & (TCOMP_FLAG_OPEN | TCOMP_FLAG_WAIT)) != TCOMP_FLAG_OPEN) {
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
        if (code >= MSG_CTRL_WAITKEY) {
            switch (code) {
            case MSG_CTRL_WAIT:
                window->state = TCOMP_STATE_WAIT_FRAMES;
                window->waitFrames = window->queue.label.value;
                break;
            case MSG_CTRL_CLOSE:
                window->state = TCOMP_STATE_CLOSING;
                break;
            case MSG_CTRL_CLEAR:
                window->queue.count = window->queue.width = 0;
                window->state = TCOMP_STATE_CLEAR_TEXT;
                break;
            case MSG_CTRL_WAITKEY:
                window->state = TCOMP_STATE_WAIT_RESULT;
                window->pendingMessage = window->queue.label.value;
                break;
            case MSG_CTRL_CAPTION:
                strcpy(window->textOutput, (const char *)window->queue.label.text);
                break;
            }
            return;
        }
        if (window->queue.width >= (short)component->pixelWidth || code == '\n') {
            MSG_queueReset(queue, 266);
            window->currentLineIndex++;
            window->lineCharacterOffset = 0;
            if ((unsigned int)window->currentLineIndex >= window->line_count) {
                window->currentLineIndex = window->line_count - 1;
                window->state = TCOMP_STATE_WAIT_SCENE;
                window->pendingMessage = window->queue.label.value;
                return;
            }
        }
    } while (window->queue.head < window->queue.tail);
    window->queue.tail = window->queue.head = 0;
}

/*
 * Advances one text window for a frame: fades its mask in, pops queued text
 * for its kind, and steps its wait and scroll state machine.
 */
void TWIN_update2(TComponent *component)
{
    TComponent *window = component;
    unsigned int flags;
    EwMaskWidget *mask;
    EwLabelWidget *label;
    int alpha;
    int index;
    int lineIndex;
    char *firstLine;

    window->frameCounter++;
    flags = window->flags;
    if (flags & TCOMP_FLAG_OPEN) {
        mask = (EwMaskWidget *)window->widgets[TWIN_WIDGET_MASK];
        mask->mask.blend |= EW_MASK_GRADIENT_HORIZONTAL;
        alpha = (mask->mask.fix >> 8) & 0xFF;
        if (alpha != 1) {
            alpha -= 4;
            if (alpha <= 0) {
                label = (EwLabelWidget *)window->widgets[TWIN_WIDGET_LABEL];
                label->colorScale = EW_COLOR_SCALE_NEUTRAL;
                alpha = 1;
            }
            mask->mask.fix = (alpha << 8) | (alpha << 16);
        }
    }
    /* Pop queued text for this window's kind. */
    switch (window->kindAndGroup & TCOMP_KIND_MASK) {
    case TCOMP_KIND_TEXT_FIELD:
    default:
        TWIN_popCF(window);
        break;
    case TCOMP_KIND_TEXT_SCENE:
        TWIN_popScene(window);
        break;
    }
    /* Not waiting: pick the wait state that the current state implies. */
    if (!(flags & TCOMP_FLAG_WAIT)) {
        switch (window->state) {
        case TCOMP_STATE_IDLE:
            break;
        case TCOMP_STATE_OPENING:
        case TCOMP_STATE_WAIT_RESULT:
        case TCOMP_STATE_CLEAR_TEXT:
        case TCOMP_STATE_WAIT_CLEAR:
        case TCOMP_STATE_WAIT_SCENE:
        case TCOMP_STATE_WAIT_SCROLL:
            window->waitFrames = 0;
            window->flags |= TCOMP_FLAG_WAIT;
            break;
        case TCOMP_STATE_WAIT_FRAMES:
            window->flags |= TCOMP_FLAG_WAIT;
            break;
        case TCOMP_STATE_CLOSING:
            window->waitFrames = TCOMP_CLOSE_FRAMES;
            window->flags |= TCOMP_FLAG_WAIT;
            break;
        case TCOMP_STATE_SCROLL_LINE:
            window->flags |= TCOMP_FLAG_WAIT;
            window->waitFrames = 0;
            break;
        }
        return;
    }
    if (!(flags & TCOMP_FLAG_RUN)) {
        return;
    }
    /* Waiting: advance on the circle button or when a timer runs out. */
    switch (window->state) {
    case TCOMP_STATE_WAIT_RESULT:
        window->waitFrames = (window->waitFrames + 1) & 31;
        if (PadData[0].pressed & PAD_CIRCLE) {
            window->flags &= ~TCOMP_FLAG_WAIT;
            window->state = TCOMP_STATE_IDLE;
            window->closeState = window->pendingMessage;
        }
        break;
    case TCOMP_STATE_WAIT_SCENE:
        window->waitFrames = (window->waitFrames + 1) & 31;
        if (PadData[0].pressed & PAD_CIRCLE) {
            window->flags &= ~TCOMP_FLAG_WAIT;
            window->state = TCOMP_STATE_IDLE;
            window->lineCharacterOffset = 0;
            window->currentLineIndex = 0;
            for (lineIndex = 0; lineIndex < window->line_count; lineIndex++) {
                window->lineBufferPointers[lineIndex][0] = '\0';
            }
        }
        break;
    case TCOMP_STATE_WAIT_FRAMES:
        window->waitFrames--;
        if (window->waitFrames < 0) {
            window->flags &= ~TCOMP_FLAG_WAIT;
            window->state = TCOMP_STATE_IDLE;
        }
        break;
    case TCOMP_STATE_CLEAR_TEXT:
        window->flags &= ~TCOMP_FLAG_WAIT;
        window->state = TCOMP_STATE_IDLE;
        window->currentPage = 0;
        window->lineCharacterOffset = 0;
        window->currentLineIndex = 0;
        for (lineIndex = 0; lineIndex < window->line_count; lineIndex++) {
            window->lineBufferPointers[lineIndex][0] = '\0';
        }
        break;
    case TCOMP_STATE_CLOSING:
        window->waitFrames--;
        if (window->waitFrames < 0) {
            window->closeState = 255;
            TWIN_dispose(component);
        }
        break;
    case TCOMP_STATE_WAIT_CLEAR:
        if (PadData[0].pressed & PAD_CIRCLE) {
            window->flags &= ~TCOMP_FLAG_WAIT;
            window->state = TCOMP_STATE_IDLE;
            window->currentLineIndex = 0;
            window->currentPage = 0;
            for (lineIndex = 0; lineIndex < window->line_count; lineIndex++) {
                window->lineBufferPointers[lineIndex][0] = '\0';
            }
        }
        window->waitFrames++;
        break;
    case TCOMP_STATE_WAIT_SCROLL:
        window->waitFrames++;
        if (PadData[0].pressed & PAD_CIRCLE) {
            window->waitFrames = 0;
            window->state = TCOMP_STATE_SCROLL_LINE;
        }
        break;
    /* Scroll: on the last line, shift every line up once the timer finishes. */
    case TCOMP_STATE_SCROLL_LINE:
        if (window->currentLineIndex >= window->line_count - 1) {
            window->textVerticalOffset = (int)((float)window->waitFrames * -6.0f);
            window->waitFrames++;
            if (window->waitFrames >= 5) {
                firstLine = window->lineBufferPointers[0];
                for (index = 0; index < window->line_count - 1; index++) {
                    window->lineWidths[index] = window->lineWidths[index + 1];
                    window->lineBufferPointers[index] = window->lineBufferPointers[index + 1];
                }
                firstLine[0] = '\0';
                window->state = TCOMP_STATE_IDLE;
                window->lineBufferPointers[window->currentLineIndex] = firstLine;
                window->flags &= ~TCOMP_FLAG_WAIT;
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
            window->flags &= ~TCOMP_FLAG_WAIT;
            window->state = TCOMP_STATE_IDLE;
            window->lineCharacterOffset = 0;
        }
        break;
    case TCOMP_STATE_OPENING:
        window->waitFrames++;
        if (window->waitFrames < 2) {
            window->textVerticalOffset = 0;
        }
        if (window->waitFrames >= TCOMP_OPEN_FRAMES) {
            window->state = TCOMP_STATE_IDLE;
            window->flags |= TCOMP_FLAG_OPEN;
            window->flags &= ~TCOMP_FLAG_WAIT;
        }
        break;
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* Font parameters installed before ordinary text and scene-window labels. */
const char twinFontParamControl[19] = "\r\002\016\002\003\030\030\030\017\000\000\200\014\200\200\200\031\003";

/* Text: {effect 3}{effect params}{blend}{color #808080}{char width 4}{spacing 0,0} */
const unsigned char twinLabelStyleControl[26] = "\r\003\016\002\002\030\030\030\017\000\000\200\014\200\200\200\025\002\002\004\025\003\003\000\000";

/*
 * Draws a field-conversation text window: positions its mask, frame, clip and
 * label, shows the cursor in the wait states, then prints the text.
 */
void TWIN_draw2(TComponent *window)
{
    EwWidget *mask;
    EwWidget *widget;
    EwLabelBody *body;
    int x;
    int y;
    unsigned short *textOffset;

    if (window->state == TCOMP_STATE_OPENING || window->state == TCOMP_STATE_CLOSING) {
        return;
    }
    x = (int)(16.0f + window->x);
    y = (int)(window->y + 16.0f + 32.0f);
    mask = window->widgets[TWIN_WIDGET_MASK];
    widget = window->widgets[TWIN_WIDGET_FRAME];
    mask->drawValue = EW_ORDER_MASK;
    widget->drawValue = EW_ORDER_FRAME;
    mask->x = x;
    mask->y = y;
    mask->width = window->pixelWidth;
    mask->height = window->pixelHeight;
    mask->flags |= EW_FLAG_VISIBLE;
    widget->x = x;
    widget->y = y;
    widget->width = window->pixelWidth;
    widget->height = window->pixelHeight;
    widget->flags |= EW_FLAG_VISIBLE;
    widget = window->widgets[TWIN_WIDGET_CLIP];
    widget->x = x;
    widget->y = y;
    widget->width = window->pixelWidth;
    widget->drawValue = EW_ORDER_TEXT;
    widget->height = window->pixelHeight;
    widget->flags |= EW_FLAG_VISIBLE;
    body = (EwLabelBody *)window->widgets[TWIN_WIDGET_LABEL];
    widget = (EwWidget *)body;
    body->label.colorScale = 0;
    widget->x = x + 12;
    widget->drawValue = EW_ORDER_TEXT;
    /* textVerticalOffset (+0x50) is a halfword that shifts the label vertically. */
    textOffset = &window->textVerticalOffset;
    widget->y = *textOffset + (y + 4);
    body->label.textLines = window->lineBufferPointers;
    body->label.lineCount = window->line_count;
    widget->flags |= EW_FLAG_VISIBLE;

    widget = window->widgets[TWIN_WIDGET_CURSOR];
    if (window->state == TCOMP_STATE_WAIT_CLEAR || window->state == TCOMP_STATE_WAIT_RESULT
        || window->state == TCOMP_STATE_WAIT_SCROLL) {
        /* Place the cursor after the current line's text, wrapping when it does not fit. */
        x = (short)window->lineWidths[window->currentLineIndex];
        y = window->currentLineIndex * TWIN_CF_LINE_PITCH;
        if (x + 16 > window->queue.limit) {
            x = 0 - 8;
            y = y + TWIN_CF_LINE_PITCH;
        } else if (x == 0 && window->currentLineIndex != 0) {
            int previousLineWidth = (short)window->lineWidths[window->currentLineIndex - 1];
            if (window->queue.limit >= previousLineWidth + 16) {
                x = previousLineWidth;
                y -= TWIN_CF_LINE_PITCH;
            } else {
                x = -8;
            }
        }
        widget->drawValue = EW_ORDER_TEXT;
        x = (int)((float)x + (window->x + 20.0f + 16.0f));
        y = (int)((float)y + (window->y + 12.0f + 16.0f + 32.0f));
        widget->x = x;
        widget->y = y;
        EW_sprtSetCursorUV((EwSpriteWidget *)widget, 3, window->frameCounter);
        widget->flags |= EW_FLAG_VISIBLE;
    } else {
        widget->flags &= ~EW_FLAG_VISIBLE;
    }
    x = (int)(window->x + 16.0f);
    y = (int)(window->y + 16.0f);
    xglFontPrintDirectOT(EW_ORDER_FRAME, twinFontParamControl);
    xglFontPrint(x, y, EW_ORDER_FRAME, window->textOutput);
    xglFontPrintDirectOT(EW_ORDER_FRAME, twinStyleControl);
}

/*
 * Draws a scene message window: positions its mask, clip and label, and shows
 * the cursor in the wait states. Draws nothing while opening or closing.
 */
void TWIN_drawScene2(TComponent *window)
{
    EwWidget *mask;
    EwWidget *widget;
    int x;
    int y;
    int panelDrawValue;

    if (window->state != TCOMP_STATE_OPENING && window->state != TCOMP_STATE_CLOSING) {
        widget = window->widgets[TWIN_WIDGET_CLIP];
        mask = window->widgets[TWIN_WIDGET_MASK];
        x = (int)(16.0f + window->x);
        y = (int)(window->y + 16.0f);
        mask->drawValue = EW_ORDER_MASK;
        widget->drawValue = EW_ORDER_TEXT;
        panelDrawValue = widget->drawValue;
        mask->x = x;
        mask->y = y;
        mask->width = window->pixelWidth;
        mask->height = window->pixelHeight;
        mask->flags = mask->flags | EW_FLAG_VISIBLE;
        widget->x = x;
        widget->y = y;
        widget->width = window->pixelWidth;
        widget->height = window->pixelHeight;
        widget->flags = widget->flags | EW_FLAG_VISIBLE;

        widget = window->widgets[TWIN_WIDGET_LABEL];
        {
            EwLabelBody *body = (EwLabelBody *)widget;

            widget->x = x;
            widget->drawValue = panelDrawValue;
            widget->y = window->textVerticalOffset + (y + 4);
            widget->width = window->pixelWidth;
            widget->height = window->pixelHeight;
            body->label.textLines = window->lineBufferPointers;
            body->label.lineCount = window->line_count;
            body->label.lineWidths = window->lineWidths;
            body->label.linePitch = 26;
            body->label.textStyle = twinLabelStyleControl;
            widget->flags |= EW_FLAG_VISIBLE;
        }

        widget = window->widgets[TWIN_WIDGET_CURSOR];
        if (window->state == TCOMP_STATE_WAIT_CLEAR || window->state == TCOMP_STATE_WAIT_RESULT
            || window->state == TCOMP_STATE_WAIT_SCENE || window->state == TCOMP_STATE_WAIT_SCROLL) {
            float cursorOriginX = 16.0f + (window->x + 4.0f);
            float cursorOriginY = 16.0f + (window->y + 12.0f);
            x = (short)window->pixelWidth / 2
                + (short)window->lineWidths[window->currentLineIndex] / 2;
            y = window->currentLineIndex * window->line_pitch;
            y = (int)((float)y + cursorOriginY);
            x = (int)((float)x + cursorOriginX);
            widget->y = y;
            widget->x = x;
            widget->drawValue = EW_ORDER_TEXT;
            EW_sprtSetCursorUV((EwSpriteWidget *)widget, 3, window->frameCounter);
            widget->flags |= EW_FLAG_VISIBLE;
        } else {
            widget->flags = widget->flags & ~EW_FLAG_VISIBLE;
        }
    }
}

/*
 * Initializes a menu component: points its item and query line arrays at its
 * own storage, sets the default geometry and state, and builds its widget tree.
 */
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
    capacity = TMENU_ITEM_CAPACITY;
    slots = menu->itemStorage;
    menu->itemTexts = slots;
    slots = menu->queryLineStorage;
    menu->closePolicy = 1;
    messageBuffer = MBUF_create(0x80, TMENU_QUERY_LINES);
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
    count = TMENU_ITEM_CAPACITY - 1;
    do {
        cursor[count] = 0;
        count -= 1;
    } while (count >= 0);
    /* Query lines use queryLineStorage, filled from the message buffer's entries below. */
    menu->queryLines = slots;
    slots = (char **)slots + TMENU_QUERY_LINES;
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
    } while (count < TMENU_QUERY_LINES);
    menu->nextQueryId = (char *)slots;
    count = TMENU_ITEM_CAPACITY - 1;
    menu->flags = TCOMP_FLAG_ACTIVE | TCOMP_FLAG_RUN | TCOMP_FLAG_OPEN;
    menu->state = TCOMP_STATE_OPENING;
    menu->x = 120.0f;
    menu->y = 100.0f;
    menu->height = 0x20;
    menu->visibleLineCount = 4;
    menu->itemCount = 0;
    menu->firstVisibleItem = 0;
    menu->selectionResult = 0;
    menu->selectedItem = 0;
    menu->width = 0;
    for (count = TMENU_WIDGET_COUNT - 1; count >= 0; count--) {
        menu->widgets[count] = 0;
    }
    count = TMENU_WIDGET_COUNT;
    /* Build the widget tree, starting with the root container. */
    root = (EwContainerWidget *)EW_create(-1, EW_TYPE_CONTAINER);
    root->container.children = menu->widgets;
    root->container.childCapacity = TMENU_WIDGET_COUNT;
    root->flags |= EW_FLAG_VISIBLE;
    menu->rootWidget = root;

    widget = EW_create(-1, EW_TYPE_CLIP_TEXT_PASS);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, TMENU_WIDGET_CLIP, widget);

    widget = EW_create(-1, EW_TYPE_MASK_TEXT_PASS);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, TMENU_WIDGET_MASK, widget);
    mask = (EwMaskWidget *)menu->widgets[TMENU_WIDGET_MASK];
    mask->mask.fix = EW_MASK_FIX_FULL;
    mask->mask.blend |= EW_MASK_GRADIENT_HORIZONTAL;

    widget = EW_create(-1, EW_TYPE_LABEL);
    label = (EwLabelWidget *)widget;
    itemTexts = menu->itemTexts;
    label->colorScale = 0;
    ((EwLabelBody *)label)->label.lineCount = -1;
    ((EwLabelSlot *)&label->textLines)->readOnlyTextLines = itemTexts;
    EW_addComponent(root, TMENU_WIDGET_ITEMS, widget);

    widget = EW_create(-1, EW_TYPE_LABEL);
    label = (EwLabelWidget *)widget;
    textLines = menu->queryLines;
    label->colorScale = 0;
    ((EwLabelBody *)label)->label.lineCount = -1;
    label->textLines = textLines;
    EW_addComponent(root, TMENU_WIDGET_QUERY, widget);

    widget = EW_create(-1, EW_TYPE_CLIP_TEXT_PASS);
    widget->width = EW_SCREEN_WIDTH;
    widget->height = EW_SCREEN_HEIGHT;
    widget->x = 0;
    widget->y = 0;
    widget->drawValue = 0;
    EW_addComponent(root, TMENU_WIDGET_SCREEN_CLIP, widget);
    widget->flags |= EW_FLAG_VISIBLE;

    widget = EW_create(-1, EW_TYPE_FRAME);
    EW_addComponent(root, TMENU_WIDGET_FRAME, widget);

    widget = EW_create(-1, EW_TYPE_SPRITE);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, TMENU_WIDGET_ARROW_UP, widget);

    widget = EW_create(-1, EW_TYPE_SPRITE);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, TMENU_WIDGET_ARROW_DOWN, widget);

    widget = EW_create(-1, EW_TYPE_SPRITE);
    widget->width = 16;
    widget->height = 16;
    EW_addComponent(root, TMENU_WIDGET_CURSOR, widget);
    menu->flags = TCOMP_FLAG_FOLLOW_OWNER | TCOMP_FLAG_ACTIVE | TCOMP_FLAG_RUN | TCOMP_FLAG_OPEN;
    menu->state = TCOMP_STATE_OPENING;
}

/* Creates a menu in slotIndex (the first free slot when negative) at its default position. */
TComponent *TMENU_create(int slotIndex)
{
    TComponent *component;

    component = TWSYS_createComponent(slotIndex, TCOMP_KIND_MENU);
    if (component == 0) {
        return 0;
    }

    TMENU_init(component);
    component->x = 48.0f;
    component->y = 64.0f;
    return component;
}

void TMENU_dispose(TMenu *menu)
{
    menu->flags &= ~(TCOMP_FLAG_ACTIVE | TCOMP_FLAG_OPEN);
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

/*
 * Stores text as item itemIndex (the first empty slot when negative), then
 * grows the menu's height and width to fit the item.
 */
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
    menu->height = menu->visibleLineCount * TMENU_LINE_HEIGHT + 4;
    if (menu->width < lineWidth + 56) {
        menu->width = lineWidth + 56;
    }
}

/*
 * Converts text and adds each of its lines as a menu item, stored in the
 * menu's inline text area. Then recounts the items and resizes the menu.
 */
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
    lineStart = menu->nextQueryId;
    itemCount = menu->itemCount;
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
        itemHeight = menu->visibleLineCount * TMENU_LINE_HEIGHT;
        menu->height = menu->queryLineCount * TMENU_LINE_HEIGHT + itemHeight + 16;
    } else {
        itemHeight = menu->visibleLineCount * TMENU_LINE_HEIGHT + 4;
        menu->height = itemHeight + menu->queryLineCount * TMENU_LINE_HEIGHT;
    }
    if (menu->width < maximumWidth) {
        menu->width = maximumWidth;
    }
}

/*
 * Advances a menu by one frame: runs its opening, reveal, wait and closing
 * states, and moves the item selection from pad input while it is in input.
 */
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
    if (!(flags & TCOMP_FLAG_ACTIVE)) {
        return;
    }
    if (menu->itemCount < menu->visibleLineCount) {
        menu->visibleLineCount = menu->itemCount;
    }
    menu->frameCounter++;
    /* Not waiting: enter the wait that the current state needs. */
    if (!(flags & TCOMP_FLAG_WAIT)) {
        switch (menu->state) {
        case TCOMP_STATE_REVEAL: {
            EwMaskWidget *initialMask;
            menu->flags = flags | TCOMP_FLAG_WAIT;
            initialMask = (EwMaskWidget *)menu->widgets[TMENU_WIDGET_MASK];
            menu->waitFrames = 0;
            initialMask->mask.fix = EW_MASK_FIX_FULL;
            initialMask->mask.blend |= EW_MASK_GRADIENT_HORIZONTAL;
            break;
        }
        case TCOMP_STATE_OPENING:
            menu->flags = flags | TCOMP_FLAG_WAIT;
            menu->waitFrames = 0;
            menu->height = menu->visibleLineCount * TMENU_LINE_HEIGHT + 4;
            MSG_queueGetInfo(&menu->queryQueue, &info, 0);
            if (info.lines > 0) {
                menu->height += info.lines * TMENU_LINE_HEIGHT + 12;
            }
            if (menu->width < info.width + 36) {
                menu->width = info.width + 36;
            }
            menu->queryLineCount = (unsigned char)info.lines;
            break;
        case TCOMP_STATE_INPUT:
        case TCOMP_STATE_DONE:
            menu->flags = flags | TCOMP_FLAG_WAIT;
            menu->waitFrames = 0;
            /* The initial input states continue through the wait-state setup. */
        case TCOMP_STATE_WAIT_FRAMES:
            menu->flags |= TCOMP_FLAG_WAIT;
            break;
        case TCOMP_STATE_CLOSING:
            menu->waitFrames = TCOMP_CLOSE_FRAMES;
            menu->flags = flags | TCOMP_FLAG_WAIT;
            widget = (EwWidget *)menu->ewHandle;
            widget->flags &= ~EW_FLAG_VISIBLE;
            break;
        }
        return;
    }
    if (!(flags & TCOMP_FLAG_RUN)) {
        return;
    }
    /* Waiting and running: handle the input or timer of the current state. */
    switch (menu->state) {
    case TCOMP_STATE_DONE:
        menu->flags = flags & ~TCOMP_FLAG_WAIT;
        if (menu->closePolicy & 1) {
            menu->state = TCOMP_STATE_CLOSING;
        } else {
            menu->state = TCOMP_STATE_IDLE;
        }
        break;
    /* Input: cross cancels, circle decides, and up/down move the selection. */
    case TCOMP_STATE_INPUT:
        if (PadData[0].direction_pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            menu->flags &= ~TCOMP_FLAG_WAIT;
            menu->state = TCOMP_STATE_DONE;
            menu->selectionResult = -1;
            break;
        }
        oldSelection = menu->selectedItem;
        if (PadData[0].direction_pressed & PAD_CIRCLE) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            menu->flags &= ~TCOMP_FLAG_WAIT;
            menu->state = TCOMP_STATE_DONE;
            break;
        }
        menu->waitFrames = (menu->waitFrames + 1) & 0xFF;
        if ((PadData[0].direction_repeated & PAD_UP)
            && oldSelection > 0 && menu->selectionResult == 0) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            menu->selectedItem--;
            menu->waitFrames = 0;
        }
        if ((PadData[0].direction_repeated & PAD_DOWN)
            && menu->selectedItem < menu->itemCount - 1) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
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
    case TCOMP_STATE_WAIT_FRAMES:
        menu->waitFrames--;
        if (menu->waitFrames < 0) {
            menu->flags = flags & ~TCOMP_FLAG_WAIT;
            menu->state = TCOMP_STATE_REVEAL;
        }
        break;
    case TCOMP_STATE_CLOSING:
        menu->waitFrames--;
        if (menu->waitFrames < 0) {
            TMENU_dispose(menu);
        }
        break;
    case TCOMP_STATE_OPENING:
        menu->waitFrames++;
        TW_setPos(component);
        if (menu->waitFrames >= TCOMP_OPEN_FRAMES) {
            menu->flags |= TCOMP_FLAG_OPEN;
            /* With or without a query, the menu goes on to the reveal state. */
            if (menu->activeQueryId != 0) {
                menu->state = TCOMP_STATE_REVEAL;
                menu->flags &= ~TCOMP_FLAG_WAIT;
            } else {
                menu->state = TCOMP_STATE_REVEAL;
                menu->flags &= ~TCOMP_FLAG_WAIT;
            }
        }
        if (menu->flags & TCOMP_FLAG_PRESELECT) {
            menu->flags &= ~TCOMP_FLAG_PRESELECT;
            menu->selectedItem = menu->requestedSelection;
        }
        break;
    /* Reveal: fade the mask in and print the query one character at a time. */
    case TCOMP_STATE_REVEAL:
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
        mask = (EwMaskWidget *)menu->widgets[TMENU_WIDGET_MASK];
        mask->mask.fix = ((unsigned int)secondaryAlpha << 16)
            | ((unsigned int)alpha << 8);
        if (menu->waitFrames >= 8) {
            EwLabelWidget *queryLabel = (EwLabelWidget *)menu->widgets[TMENU_WIDGET_ITEMS];
            EwLabelWidget *itemLabel = (EwLabelWidget *)menu->widgets[TMENU_WIDGET_QUERY];
            queryLabel->colorScale = EW_COLOR_SCALE_NEUTRAL;
            itemLabel->colorScale = EW_COLOR_SCALE_NEUTRAL;
        }
        widget = (EwWidget *)mask;
        widget->drawValue = EW_ORDER_MASK;
        widget->x = (int)(menu->x + 16.0f);
        widget->y = (int)(menu->y + 16.0f);
        widget->width = menu->width;
        widget->height = menu->height;
        if (menu->queryQueue.tail > 0) {
            menu->queryCharacterOffset += MSG_queuePop(&menu->queryQueue,
                (unsigned char *)menu->queryLines[menu->currentQueryLine]
                    + menu->queryCharacterOffset, 1);
            code = menu->queryQueue.last;
            if (code >= MSG_CTRL_WAITKEY) {
                switch (code) {
                case MSG_CTRL_WAIT:
                    menu->state = TCOMP_STATE_WAIT_FRAMES;
                    menu->waitFrames = menu->queryQueue.label.value;
                    break;
                case MSG_CTRL_WAITKEY:
                case MSG_CTRL_CLEAR:
                case 130:
                case MSG_CTRL_CLOSE:
                case MSG_CTRL_CAPTION:
                default:
                    break;
                }
            } else if (menu->queryQueue.width >= menu->width || code == '\n') {
                menu->queryCharacterOffset = 0;
                menu->currentQueryLine++;
                MSG_queueReset(&menu->queryQueue, 10);
            }
        }
        if (menu->queryQueue.head >= menu->queryQueue.tail && menu->waitFrames >= 9) {
            MSG_queueReset(&menu->queryQueue, 0);
            menu->flags &= ~TCOMP_FLAG_WAIT;
            menu->state = TCOMP_STATE_INPUT;
            mask = (EwMaskWidget *)menu->widgets[TMENU_WIDGET_MASK];
            mask->mask.fix = EW_MASK_FIX_NONE;
        }
        break;
    default:
        menu->waitFrames++;
        break;
    }
}

/*
 * Draws a menu: its mask, frame, clip, query and item labels, scroll arrows and
 * selection cursor. Draws nothing while opening or closing.
 */
void TMENU_drawDefault(TComponent *component)
{
    TMenu *menu = (TMenu *)component;
    EwWidget *widget;
    EwLabelBody *body;
    int x;
    int y;
    int queryHeight;

    if (!(menu->flags & TCOMP_FLAG_ACTIVE)) {
        return;
    }
    if (menu->state == TCOMP_STATE_OPENING || menu->state == TCOMP_STATE_CLOSING) {
        return;
    }
    x = (int)menu->x + 16;
    y = (int)menu->y + 16;
    widget = menu->widgets[TMENU_WIDGET_MASK];
    widget->x = (int)(menu->x + 16.0f);
    widget->drawValue = EW_ORDER_MASK;
    widget->y = (int)(menu->y + 16.0f);
    widget->width = menu->width;
    widget->height = menu->height;
    widget->flags |= EW_FLAG_VISIBLE;
    widget = menu->widgets[TMENU_WIDGET_FRAME];
    widget->drawValue = EW_ORDER_FRAME;
    widget->x = x;
    widget->y = y;
    widget->width = menu->width;
    widget->height = menu->height;
    widget->flags |= EW_FLAG_VISIBLE;
    if (menu->queryLineCount != 0) {
        queryHeight = menu->queryLineCount * TMENU_LINE_HEIGHT + 12;
    } else {
        queryHeight = 0x0;
    }
    widget = menu->widgets[TMENU_WIDGET_CLIP];
    widget->drawValue = EW_ORDER_MASK;
    widget->x = x;
    widget->y = y;
    widget->width = menu->width;
    widget->height = menu->height;
    widget->flags |= EW_FLAG_VISIBLE;
    widget = menu->widgets[TMENU_WIDGET_QUERY];
    body = (EwLabelBody *)widget;
    body->label.textLines = menu->queryLines;
    widget->drawValue = EW_ORDER_TEXT;
    widget->x = x;
    widget->y = y;
    body->label.lineCount = menu->queryLineCount;
    body->label.firstLine = 0x0;
    widget->flags |= EW_FLAG_VISIBLE;
    widget = menu->widgets[TMENU_WIDGET_ITEMS];
    body = (EwLabelBody *)widget;
    widget->x = x + 20;
    widget->drawValue = EW_ORDER_TEXT;
    widget->y = y + queryHeight;
    body->label.firstLine = menu->firstVisibleItem;
    ((EwLabelSlot *)&body->label.textLines)->readOnlyTextLines = menu->itemTexts;
    body->label.lineCount = menu->visibleLineCount + 1;
    widget->flags |= EW_FLAG_VISIBLE;

    /* Scroll arrows show only when items lie outside the visible window. */
    widget = menu->widgets[TMENU_WIDGET_ARROW_UP];
    widget->x = x + menu->width - 16;
    widget->y = y;
    widget->drawValue = EW_ORDER_TEXT;
    EW_sprtSetCursorUV((EwSpriteWidget *)widget, 1, menu->frameCounter);
    if (menu->firstVisibleItem > 0) {
        widget->flags = widget->flags | EW_FLAG_VISIBLE;
    } else {
        widget->flags = widget->flags & ~EW_FLAG_VISIBLE;
    }
    widget = menu->widgets[TMENU_WIDGET_ARROW_DOWN];
    widget->drawValue = EW_ORDER_TEXT;
    widget->x = menu->width + x - 16;
    widget->y = menu->height + y - 16;
    EW_sprtSetCursorUV((EwSpriteWidget *)widget, 2, menu->frameCounter);
    if (menu->firstVisibleItem + menu->visibleLineCount < menu->itemCount) {
        widget->flags |= EW_FLAG_VISIBLE;
    } else {
        widget->flags &= ~EW_FLAG_VISIBLE;
    }
    y = 0xC;
    if (menu->selectedItem >= 0) {
        y = (menu->selectedItem - menu->firstVisibleItem) * TMENU_LINE_HEIGHT + 12;
    }
    if (menu->queryLineCount != 0) {
        queryHeight = TMENU_LINE_HEIGHT * menu->queryLineCount + 12;
    } else {
        queryHeight = 0;
    }
    /* Place the cursor on the selected item's line, below the query lines. */
    widget = menu->widgets[TMENU_WIDGET_CURSOR];
    widget->drawValue = EW_ORDER_TEXT;
    /* Retain both floating zero-offset additions in the cursor layout. */
    x = (int)(menu->x + 20.0f + 0.0f + 0.0f);
    y = (int)((float)y + (menu->y + 16.0f - 6.0f + (float)queryHeight));
    widget->x = x;
    widget->y = y;
    EW_sprtSetCursorUV((EwSpriteWidget *)widget, 0, menu->frameCounter);
    widget->flags |= EW_FLAG_VISIBLE;
}
