#include "common.h"

#include "main/ew_sprt_set_cursor_uv.h"
#include "ew_sprt_set_cursor_uv.h"

#include "shared.h"
#include "main/xgl_font.h"
#include "main/xgl_prim_add.h"
#include "main/xgl_packet.h"

/* GIF tag words: EOP bit of word 0 (NLOOP in the low bits) and the A+D
 * register descriptor (NREG = 1, REGS = 0xE) of words 1 and 2. */
#define GIF_TAG_NREG_AD    0x10000000
#define GIF_TAG_REGS_AD    (-2)

/* The widgets sit in a 512x448 screen (EW_SCREEN_WIDTH/HEIGHT) whose origin is
 * at this offset in GS primitive coordinates (2048 - 256, 2048 - 224). */
#define EW_GS_ORIGIN_X      1792
#define EW_GS_ORIGIN_Y      1824

/* Draw passes: the packet pass (EW_sendPacket) and the text pass (EW_draw). */
#define EW_PASS_PACKET 1
#define EW_PASS_TEXT   2

/* Default line pitch of a new label. */
#define EW_LABEL_LINE_HEIGHT 24
/* Default alpha of a new frame (GS alpha range 0..0x80). */
#define EW_FRAME_ALPHA_DEFAULT 0x60
/* Border pieces of the frame (rows of frameUV) drawn by frame_put/label_put. */
#define EW_FRAME_PIECE_COUNT 8
/* label_put draws every border piece but the bottom edge (bit i = piece i). */
#define EW_LABEL_PIECE_MASK 0x3D

typedef struct EwFrameState {
    /* +0x00: cleared by frame_init; no reader found among this TU's own
       functions (frame_put reads only the shared header, +0x04..+0x0e). */
    short flags;
    /* +0x02: default alpha (PS2 GS 0-0x80 range; matches EwMaskState's
       own 0x60 default and the codebase's "alpha" term, e.g.
       include/ov12/rg_piclist.h); same "no reader in this TU" caveat as
       flags above. */
    signed char alpha;
} EwFrameState;

typedef struct EwGifRegister {
    u64 data;
    u64 reg;
} EwGifRegister;

typedef union EwGifQuad {
    unsigned int words[4];
    EwGifRegister reg;
} EwGifQuad;

typedef struct EwDrawEnvPacket {
    unsigned int tag[4];
    EwGifRegister regs[6];
} EwDrawEnvPacket;

typedef struct EwFrameWidget {
    unsigned short flags;
    unsigned short type;
    unsigned char unmodeled_04[0x0C];
    EwFrameState frame;
    unsigned char unmodeled_14[0x14];
} EwFrameWidget;

typedef struct EwTextState {
    /* +0x00: padding word, the init stores nothing here. */
    unsigned char unmodeled_00[2];
    /* +0x02: line pitch in pixels; EW_drawComoponent advances the y
       position by it after each line. */
    short lineHeight;
    /* +0x04: number of lines, or negative to count up to the first NULL
       entry of lines[]. */
    short lineCount;
    /* +0x06: index of the first line drawn. */
    short firstLine;
    /* +0x08: colour scale; EW_drawComoponent compares it with 128 (the
       neutral value) before it prints a second pass. */
    int colorScale;
    /* +0x0C: optional per-line widths used to centre each line. */
    unsigned short *lineWidths;
    /* +0x10: font control string handed to xglFontPrint. */
    const unsigned char *fontStyle;
    /* +0x14: NULL-terminated array of line strings. */
    char **lines;
} EwTextState;

typedef struct EwTextWidget {
    unsigned short flags;
    unsigned short type;
    unsigned char unmodeled_04[0x0C];
    EwTextState text;
} EwTextWidget;

/*
 * Shared 0x28-byte ewComponent table-slot header:
 * flags encodes both the widget type and its enabled bit. EW_sendPacket
 * and EW_draw test the type via (flags & 0xE000) == 0xC000; EW_drawContainer
 * tests only the enabled bit via flags & 0x4000.
 */
void EW_drawComoponent(FontDrawCursor *cursor, EwWidget *widget);

static void EW_setDrawEnv(FontDrawCursor *cursor);

static void container_init(EwContainerState *container);

static void frame_put(FontDrawCursor *cursor, EwWidget *widget);

/* The font queue calls these back with its draw cursor; EW_draw registers
 * EW_sendPacket with a null argument, the clip and mask widgets pass
 * themselves. */
static void label_put(FontDrawCursor *cursor, EwLabelWidget *widget);
static void mask_put(FontDrawCursor *cursor, void *arg);
static void set_clip(FontDrawCursor *cursor, void *arg);
static void sprt_put(FontDrawCursor *cursor, EwSpriteWidget *widget);
static int ew_send_mode = 0;

static u64 sprt_base_register[4] = {
    0x2007EF0669343C00ULL, 0x2007EF0669343C00ULL,
    0x2007EF0669343C00ULL, 0x2007EF0669343C00ULL
};

/* Mutable font colour command. Opcode 0x0f consumes three parameter bytes,
 * including the embedded zero, before the terminal zero. */
char mutableFontColorCommand[5] = "\017D\000\200";

static char *tmp_0 = mutableFontColorCommand;

static unsigned int frameUV[8][4] = {
    { 0x000, 0x800, 0x800, 0x900 },
    { 0x000, 0x900, 0x800, 0xA00 },
    { 0x800, 0x000, 0x900, 0x800 },
    { 0x900, 0x000, 0xA00, 0x800 },
    { 0x800, 0x800, 0x900, 0x900 },
    { 0x900, 0x800, 0xA00, 0x900 },
    { 0x800, 0x900, 0x900, 0xA00 },
    { 0x900, 0x900, 0xA00, 0xA00 },
};

/* Retail maps ewComponent as a GLOBAL OBJECT at 0x0099CA30 with size 0xA00.
 * This table has 64 observed 0x28-byte slots; only flags are modeled here. */
EwWidget ewComponent[EW_SLOT_COUNT];

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const unsigned char labelFontStyleTable[0x20] = {
    0x0D, 0x02, 0x0E, 0x02, 0x03, 0x18, 0x18, 0x18,
    0x0F, 0x00, 0x00, 0x80, 0x0C, 0x80, 0x80, 0x80,
    0x15, 0x02, 0x02, 0x04, 0x19, 0x03, 0x15, 0x03,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void EW_sprtSetCursorUV(EwSpriteWidget *widget, int mode, int cell)
{
    EwSpriteState *sprite = &widget->sprite;
    int vCoord;
    int uBase;
    int mask = 3;
    int shift = 2;

    switch (mode) {
    case 0:
    default:
        uBase = 0;
        vCoord = 0xC0;
        mask = 7;
        shift = 1;
        break;
    case 1:
        uBase = 0x40;
        vCoord = 0xA0;
        break;
    case 2:
        uBase = 0;
        vCoord = 0xB0;
        break;
    case 3:
        uBase = 0x40;
        vCoord = 0xB0;
        break;
    }
    sprite->v = vCoord;
    sprite->u = uBase + (((cell >> shift) & mask) << 4);
}

static void frame_init(EwFrameState *frame)
{
    frame->flags = 0;
    frame->alpha = EW_FRAME_ALPHA_DEFAULT;
}

static void sprt_init(EwSpriteState *sprite)
{
    sprite->flags = 0;
    sprite->alpha = -0x80;
}

static void mask_init(EwMaskState *mask)
{
    mask->blend = 0x60;
    mask->fix = 0x80 << 8;
}

static void EW_setDrawEnv(FontDrawCursor *cursor)
{
    EwDrawEnvPacket *env;
    EwGifRegister *regs;
    EwGifRegister *entry;

    env = (EwDrawEnvPacket *) (((unsigned int)&cursor->work + 15) & ~15);
    env->tag[0] = GIF_TAG_EOP | 6;
    env->tag[1] = GIF_TAG_NREG_AD;
    env->tag[2] = GIF_TAG_REGS_AD;
    env->tag[3] = 0;
    regs = env->regs;
    entry = regs + 0;
    entry->data = 68;
    entry->reg = SCE_GS_ALPHA_1;
    entry = regs + 1;
    entry->data = 0;
    entry->reg = SCE_GS_TEXFLUSH;
    entry = regs + 2;
    entry->data = 0;
    entry->reg = SCE_GS_TEXA;
    entry = regs + 3;
    entry->data = 65;
    entry->reg = SCE_GS_TEX1_1;
    entry = regs + 4;
    entry->data = sprt_base_register[0];
    entry->reg = SCE_GS_TEX0_1;
    entry = regs + 5;
    entry->data = 0x31001;
    entry->reg = SCE_GS_TEST_1;
    xglPrimAddGifTagDirect(cursor->packet, env, 7);
}

/*
 * EW_create (main:0x0026c0e0) indexes a fixed table of 40-byte (0x28)
 * ewComponent slots (index*5*8); each slot holds a 0x10-byte shared
 * header (flags at +0x00, widget type at +0x02) followed by 0x18 bytes
 * of type-specific storage. It calls one of frame_init/sprt_init/
 * container_init/mask_init with a pointer to that type-specific storage
 * to initialize a newly created widget (type 1, 5, 4 and 8/10
 * respectively, per the jump table at 0x004c2360).
 */
EwWidget *EW_create(int index, int type)
{
    EwWidget *widget;
    int i;

    widget = 0;
    if (index < 0) {
        for (i = 0; i < EW_SLOT_COUNT; i++) {
            if ((ewComponent[i].flags & EW_FLAG_ALLOCATED) == 0) {
                index = i;
                break;
            }
        }
    }
    if ((unsigned int) index < EW_SLOT_COUNT) {
        widget = &ewComponent[index];
        widget->type = type;
        widget->flags = EW_FLAG_ALLOCATED;
        switch (widget->type) {
        case EW_TYPE_FRAME:
            frame_init(&((EwFrameWidget *) widget)->frame);
            break;
        case EW_TYPE_SPRITE:
            sprt_init(&((EwSpriteWidget *) widget)->sprite);
            break;
        case EW_TYPE_CONTAINER:
            container_init(&((EwContainerWidget *) widget)->container);
            break;
        case EW_TYPE_MASK:
        case EW_TYPE_MASK_TEXT_PASS:
            mask_init(&((EwMaskWidget *) widget)->mask);
            break;
        case EW_TYPE_LABEL: {
            EwTextWidget *label = (EwTextWidget *) widget;

            label->text.lineCount = -1;
            label->text.lineHeight = EW_LABEL_LINE_HEIGHT;
            label->text.fontStyle = labelFontStyleTable;
            label->text.colorScale = EW_COLOR_SCALE_NEUTRAL;
            label->text.lines = 0;
            label->text.firstLine = 0;
            label->text.lineWidths = 0;
            break;
        }
        }
    }
    return widget;
}

void EW_dispose(EwContainerWidget *widget)
{
    EwWidget **firstChild;
    EwWidget **children;
    EwWidget *child;
    int capacity;
    int remaining;

    if (widget->type == EW_TYPE_CONTAINER)
    {
        firstChild = widget->container.children;
        if (firstChild != 0)
        {
            capacity = widget->container.childCapacity;
            if (capacity > 0)
            {
                children = firstChild;
                remaining = capacity;
                do
                {
                    child = *children;
                    children++;
                    if (child != 0)
                    {
                        child->flags = 0;
                    }
                    remaining--;
                } while (remaining != 0);
            }
        }
    }
    widget->flags = 0;
}

int EW_addComponent(EwContainerWidget *widget, int index, EwWidget *child)
{
    EwWidget **children;
    int capacity;
    int i;

    if (widget->type != EW_TYPE_CONTAINER || (children = widget->container.children) == 0) {
        return -1;
    }
    if (child == 0) {
        return -1;
    }
    capacity = widget->container.childCapacity;
    if (index < 0) {
        if (index == -1) {
            for (i = 0; i < capacity; i++) {
                if (children[i] == 0) {
                    index = i;
                    break;
                }
            }
        } else {
            for (i = capacity - 1; i >= 0; i--) {
                if (children[i] == 0) {
                    index = i;
                    break;
                }
            }
        }
    }
    if (index >= 0 && index < capacity) {
        child->flags |= EW_FLAG_CHILD;
        children[index] = child;
    } else {
        index = -1;
    }
    return index;
}

void EW_drawContainer(FontDrawCursor *cursor, EwContainerState *container)
{
    EwWidget **firstChild;
    EwWidget **children;
    EwWidget *child;
    short capacity;
    int count;

    firstChild = container->children;
    if (firstChild != 0)
    {
        capacity = container->childCapacity;
        if (capacity > 0)
        {
            children = firstChild;
            count = capacity;
            do
            {
                child = *children;
                children++;
                if (child != 0 && (child->flags & EW_FLAG_VISIBLE))
                {
                    EW_drawComoponent(cursor, child);
                }
                count--;
            } while (count != 0);
        }
    }
}

void EW_drawComoponent(FontDrawCursor *cursor, EwWidget *widget)
{
    switch (widget->type)
    {
    case EW_TYPE_PANEL:
        if (ew_send_mode & EW_PASS_PACKET)
        {
            label_put(cursor, (EwLabelWidget *) widget);
        }
        break;
    case EW_TYPE_FRAME:
        if (ew_send_mode & EW_PASS_PACKET)
        {
            frame_put(cursor, widget);
        }
        break;
    case EW_TYPE_CONTAINER:
        EW_drawContainer(cursor, &((EwContainerWidget *) widget)->container);
        break;
    case EW_TYPE_SPRITE:
        if (ew_send_mode & EW_PASS_PACKET)
        {
            sprt_put(cursor, (EwSpriteWidget *) widget);
        }
        break;
    case EW_TYPE_CLIP:
        if (ew_send_mode & EW_PASS_PACKET)
        {
            set_clip(cursor, widget);
        }
        break;
    case EW_TYPE_MASK:
        if (ew_send_mode & EW_PASS_PACKET)
        {
            mask_put(cursor, widget);
        }
        break;
    case EW_TYPE_CLIP_TEXT_PASS:
        if (ew_send_mode & EW_PASS_TEXT)
        {
            xglFontPrintExtFunc(widget->drawValue, set_clip, widget);
        }
        break;
    case EW_TYPE_MASK_TEXT_PASS:
        if (ew_send_mode & EW_PASS_TEXT)
        {
            xglFontPrintExtFunc(widget->drawValue, mask_put, widget);
        }
        break;
    case EW_TYPE_LABEL:
    {
        typedef struct EwTextHeaderLocal
        {
            unsigned short flags;
            unsigned short type;
            short x;
            short y;
            int drawValue;
            unsigned short width;
            short height;
        } EwTextHeaderLocal;
        typedef struct EwTextPayloadLocal
        {
            /* Raw halfword at +0x10; this function does not establish its meaning. */
            unsigned short unmodeled_00;
            short lineHeight;
            short lineCount;
            short firstLine;
            int colorScale;
            unsigned short *lineWidths;
            const unsigned char *fontStyle;
            char **lines;
        } EwTextPayloadLocal;
        typedef struct EwTextWidgetLocal
        {
            EwTextHeaderLocal header;
            EwTextPayloadLocal text;
        } EwTextWidgetLocal;
        EwTextWidgetLocal *textWidget;
        char **lines;
        unsigned short *lineWidths;
        int lineHeight;
        int firstLine;
        int endLine;
        const unsigned char *fontStyle;
        int colorScale;
        int x;
        int y;
        int i;
        int centeredX;

        if ((ew_send_mode & EW_PASS_TEXT) == 0)
        {
            return;
        }
        textWidget = (EwTextWidgetLocal *) widget;
        lines = textWidget->text.lines;
        x = textWidget->header.x;
        y = textWidget->header.y;
        fontStyle = textWidget->text.fontStyle;
        colorScale = textWidget->text.colorScale;
        if (lines == 0)
        {
            return;
        }
        lineHeight = textWidget->text.lineHeight;
        firstLine = textWidget->text.firstLine;
        lineWidths = textWidget->text.lineWidths;
        endLine = textWidget->text.lineCount;
        if (endLine < 0)
        {
            i = firstLine;
            while (lines[i] != 0)
            {
                i++;
            }
            endLine = i;
        }
        else
        {
            endLine += firstLine;
        }
        xglFontPrint(x, y, textWidget->header.drawValue,
                     (const char *) fontStyle);
        if (colorScale != EW_COLOR_SCALE_NEUTRAL)
        {
            tmp_0[1] = 0x11;
            xglFontPrint(0, 0, textWidget->header.drawValue, tmp_0);
        }
        for (i = firstLine; i < endLine; i++)
        {
            x = textWidget->header.x;
            if (lines[i] != 0)
            {
                if (lineWidths != 0)
                {
                    centeredX = x + (short) textWidget->header.width / 2;
                    centeredX -= (short) lineWidths[i] / 2;
                    xglFontPrint(centeredX, y,
                                 textWidget->header.drawValue, lines[i]);
                }
                else
                {
                    xglFontPrint(x, y, textWidget->header.drawValue, lines[i]);
                }
            }
            y += lineHeight;
        }
        break;
    }
    case 2:
    case 3:
    default:
        break;
    }
}

void EW_init(void)
{
    int i;

    for (i = EW_SLOT_COUNT - 1; i >= 0; i--)
    {
        ewComponent[i].flags = 0;
    }
}

void EW_sendPacket(FontDrawCursor *cursor, void *argument)
{
    int i;

    xglFontReloadTexture(cursor, 2);
    ew_send_mode = EW_PASS_PACKET;
    EW_setDrawEnv(cursor);
    for (i = 0; i < EW_SLOT_COUNT; i++)
    {
        if ((ewComponent[i].flags & EW_FLAG_STATE_MASK) == EW_FLAG_TOP_LEVEL_VISIBLE)
        {
            EW_drawComoponent(cursor, &ewComponent[i]);
        }
    }
    xglFontReloadTexture(cursor, 1);
    ew_send_mode = 0;
}

void EW_draw(void)
{
    int i;

    xglFontPrintExtFunc(EW_ORDER_FRAME,EW_sendPacket, 0);
    ew_send_mode = EW_PASS_TEXT;
    for (i = 0; i < EW_SLOT_COUNT; i++)
    {
        if ((ewComponent[i].flags & EW_FLAG_STATE_MASK) == EW_FLAG_TOP_LEVEL_VISIBLE)
        {
            EW_drawComoponent(0, &ewComponent[i]);
        }
    }
}

static void container_init(EwContainerState *container)
{
    container->children = 0;
    container->childCapacity = 0;
    container->unusedByContainer[0] = 0;
    container->unusedByContainer[1] = 0;
}

/*
 * Draws a mask widget as one four-vertex Gouraud triangle strip (GIF
 * PRIM 0x20264000 with the ALPHA_1 value held in the mask state). The
 * top edge is filled with the intensity in bits 16-23 of the mask's fix
 * word and the bottom-right edge with bits 8-15; bit 0x100 of the blend
 * word swaps the two middle vertices, which turns the gradient from
 * vertical to horizontal.
 */
static void mask_put(FontDrawCursor *cursor, void *arg)
{
    EwMaskWidget *widget;
    EwMaskState *mask;
    unsigned int *gif;
    unsigned int *qword;
    int left;
    int top;
    int width;
    int height;
    int z;
    unsigned int color;

    widget = (EwMaskWidget *) arg;
    mask = &widget->mask;
    gif = (unsigned int *) (((unsigned int)&cursor->work + 15) & ~15);
    qword = gif;
    qword[0] = 1;
    qword[1] = GIF_TAG_NREG_AD;
    qword[2] = GIF_TAG_REGS_AD;
    qword[3] = 0;
    qword += 4;
    qword[0] = mask->blend;
    qword[1] = mask->fix;
    qword[2] = SCE_GS_ALPHA_1;
    qword += 4;
    qword[0] = GIF_TAG_EOP | 4;
    qword[1] = 0x20264000;
    qword[2] = -175;
    qword[3] = 0;
    qword += 4;
    width = widget->width << 4;
    height = widget->height << 4;
    left = (widget->x + EW_GS_ORIGIN_X) << 4;
    top = (widget->y + EW_GS_ORIGIN_Y) << 4;
    z = widget->z;
    if ((mask->blend & EW_MASK_GRADIENT_HORIZONTAL) == 0) {
        color = (unsigned char) (mask->fix >> 16);
        qword[0] = color;
        qword[1] = color;
        qword[2] = color;
        qword[3] = color;
        qword += 4;
        qword[0] = left;
        qword[1] = top;
        qword[2] = z;
        qword[3] = 0;
        qword += 4;
        qword[0] = color;
        qword[1] = color;
        qword[2] = color;
        qword[3] = color;
        qword += 4;
        qword[0] = left;
        qword[1] = top + height;
        qword[2] = z;
        qword[3] = 0;
        qword += 4;
        color = (unsigned char) (mask->fix >> 8);
        qword[0] = color;
        qword[1] = color;
        qword[2] = color;
        qword[3] = color;
        qword += 4;
        qword[0] = left + width;
        qword[1] = top;
        qword[2] = z;
        qword[3] = 0;
        qword += 4;
        qword[0] = color;
        qword[1] = color;
        qword[2] = color;
        qword[3] = color;
        qword += 4;
        qword[0] = left + width;
        qword[1] = top + height;
        qword[2] = z;
        qword[3] = 0;
        qword += 4;
    } else {
        color = (unsigned char) (mask->fix >> 16);
        qword[0] = color;
        qword[1] = color;
        qword[2] = color;
        qword[3] = color;
        qword += 4;
        qword[0] = left;
        qword[1] = top;
        qword[2] = z;
        qword[3] = 0;
        qword += 4;
        qword[0] = color;
        qword[1] = color;
        qword[2] = color;
        qword[3] = color;
        qword += 4;
        qword[0] = left + width;
        qword[1] = top;
        qword[2] = z;
        qword[3] = 0;
        qword += 4;
        color = (unsigned char) (mask->fix >> 8);
        qword[0] = color;
        qword[1] = color;
        qword[2] = color;
        qword[3] = color;
        qword += 4;
        qword[0] = left;
        qword[1] = top + height;
        qword[2] = z;
        qword[3] = 0;
        qword += 4;
        qword[0] = color;
        qword[1] = color;
        qword[2] = color;
        qword[3] = color;
        qword += 4;
        qword[0] = left + width;
        qword[1] = top + height;
        qword[2] = z;
        qword[3] = 0;
        qword += 4;
    }
    xglPrimAddGifTagDirect(cursor->packet, gif, (qword - gif) / 4);
}

static void sprt_put(FontDrawCursor *cursor, EwSpriteWidget *widget)
{
    unsigned int *packet;
    unsigned int *quad;
    unsigned int *setup;
    EwSpriteState *sprite;
    int texU;
    int texV;
    int width;
    int height;
    int screenX;
    int screenY;
    int z;

    packet = (unsigned int *) (((unsigned int)&cursor->work + 15) & ~15);
    quad = packet;
    quad[0] = 1;
    quad[1] = 0x20000000;
    quad[2] = -18;
    quad[3] = 0;
    quad = packet + 4;
    ((u64 *) quad)[0] = 0x7F1FC200;
    ((u64 *) quad)[1] = SCE_GS_CLAMP_1;
    setup = packet + 8;
    ((u64 *) setup)[0] = 68;
    ((u64 *) setup)[1] = SCE_GS_ALPHA_1;
    setup = packet + 12;
    setup[0] = GIF_TAG_EOP | 1;
    setup[1] = 0x50AB4000;
    setup[2] = 0xFFF53531;
    setup[3] = 0;
    sprite = &widget->sprite;
    texU = sprite->u << 4;
    texV = sprite->v << 4;
    screenX = (widget->x + EW_GS_ORIGIN_X) << 4;
    screenY = (widget->y + EW_GS_ORIGIN_Y) << 4;
    width = widget->width << 4;
    height = widget->height << 4;
    z = widget->z;
    quad = packet + 16;
    quad[0] = 128;
    quad[1] = 128;
    quad[2] = 128;
    quad[3] = 128;
    quad = packet + 20;
    quad[0] = texU;
    quad[1] = texV;
    quad = packet + 24;
    quad[0] = screenX;
    quad[1] = screenY;
    quad[2] = z;
    quad[3] = 0;
    quad = packet + 28;
    quad[0] = texU + width;
    quad[1] = texV + height;
    quad = packet + 32;
    quad[0] = screenX + width;
    quad[1] = screenY + height;
    quad[2] = z;
    quad[3] = 0;
    xglPrimAddGifTagDirect(cursor->packet, packet, 9);
}

static void set_clip(FontDrawCursor *cursor, void *arg)
{
    EwWidget *widget;
    EwDrawEnvPacket *env;
    EwGifRegister *entry;
    int top;
    int left;
    int bottom;
    int right;

    widget = (EwWidget *) arg;
    env = (EwDrawEnvPacket *) (((unsigned int)&cursor->work + 15) & ~15);
    left = widget->x;
    top = widget->y;
    right = left + widget->width - 1;
    bottom = top + widget->height - 1;
    if (top < 0) {
        top = 0;
    }
    if (left < 0) {
        left = 0;
    }
    if (right > EW_SCREEN_WIDTH - 1) {
        right = EW_SCREEN_WIDTH - 1;
    }
    if (bottom > EW_SCREEN_HEIGHT - 1) {
        bottom = EW_SCREEN_HEIGHT - 1;
    }
    env->tag[0] = GIF_TAG_EOP | 1;
    env->tag[1] = GIF_TAG_NREG_AD;
    env->tag[2] = GIF_TAG_REGS_AD;
    env->tag[3] = 0;
    entry = &env->regs[0];
    entry->data = (u64) left | (u64) right << 16 | (u64) top << 32 | (u64) (bottom - 2) << 48;
    entry->reg = SCE_GS_SCISSOR_1;
    xglPrimAddGifTagDirect(cursor->packet, env, 2);
}

static int checkN2(int value)
{
    int bitIndex;
    unsigned int bitMask;

    bitIndex = 0;
    do
    {
        bitMask = 1U << bitIndex;
        if ((unsigned int) value == bitMask)
        {
            return bitIndex;
        }
        bitIndex++;
    } while (bitIndex < 32);

    return 0;
}

static void frame_put(FontDrawCursor *cursor, EwWidget *widget)
{
    int xs[16];
    int ys[16];
    EwGifQuad *gif;
    unsigned int *qword;
    EwGifRegister *reg;
    int rawWidth;
    int rawHeight;
    int widthIsPow2;
    int heightIsPow2;
    int width;
    int height;
    int left;
    int top;
    int right;
    int bottom;
    int z;
    int i;

    gif = (EwGifQuad *) (((unsigned int)&cursor->work + 15) & ~15);
    qword = gif->words;
    qword[0] = 2;
    qword[1] = GIF_TAG_NREG_AD;
    qword[2] = GIF_TAG_REGS_AD;
    qword[3] = 0;
    qword += 4;
    reg = &gif[1].reg;
    reg->data = 0x7F0007FF;
    reg->reg = SCE_GS_CLAMP_1;
    reg = &gif[2].reg;
    reg->data = 68;
    reg->reg = SCE_GS_ALPHA_1;
    qword += 8;
    qword[0] = 1;
    qword[1] = 0x50AB4000;
    qword[2] = 0xFFF53531;
    qword[3] = 0;
    qword += 4;

    rawWidth = widget->width;
    rawHeight = widget->height;
    width = rawWidth << 4;
    widthIsPow2 = checkN2(rawWidth);
    if (widthIsPow2 != 0) {
        width += 16;
    }
    heightIsPow2 = checkN2(rawHeight);
    left = (widget->x + EW_GS_ORIGIN_X) << 4;
    top = (widget->y + EW_GS_ORIGIN_Y) << 4;
    z = widget->drawValue;
    height = rawHeight << 4;
    if (heightIsPow2 != 0) {
        height += 16;
    }

    qword[0] = 128;
    qword[1] = 128;
    qword[2] = 128;
    qword[3] = 96;
    qword += 4;
    qword[0] = 0;
    qword[1] = 0;
    qword += 4;
    qword[0] = left;
    qword[1] = top;
    qword[2] = z;
    qword[3] = 0;
    qword += 4;
    qword[0] = width;
    qword[1] = height;
    qword += 4;
    qword[0] = left + width;
    qword[1] = top + height;
    qword[2] = z;
    qword[3] = 0;
    qword += 4;

    qword[0] = 1;
    qword[1] = GIF_TAG_NREG_AD;
    qword[2] = GIF_TAG_REGS_AD;
    qword[3] = 0;
    qword += 4;
    reg = &gif[10].reg;
    reg->data = 5;
    reg->reg = SCE_GS_CLAMP_1;
    qword += 4;
    qword[0] = GIF_TAG_EOP | 8;
    qword[1] = 0x50AB4000;
    qword[2] = 0xFFF53531;
    qword[3] = 0;
    qword += 4;

    width = rawWidth << 4;
    if (widthIsPow2 != 0) {
        width += 16;
    }
    height = rawHeight << 4;
    if (heightIsPow2 != 0) {
        height += 16;
    }
    right = left + width;
    bottom = top + height;
    xs[0] = xs[2] = left;
    xs[1] = xs[3] = right;
    xs[4] = left - 256;
    xs[5] = left;
    xs[6] = right;
    xs[7] = right + 256;
    xs[8] = left - 256;
    xs[9] = left;
    xs[10] = right;
    xs[11] = right + 256;
    xs[12] = left - 256;
    xs[13] = left;
    xs[14] = right;
    xs[15] = right + 256;
    ys[0] = top - 256;
    ys[1] = top;
    ys[2] = bottom;
    ys[3] = bottom + 256;
    ys[4] = ys[6] = top;
    ys[5] = ys[7] = bottom;
    ys[10] = top - 256;
    ys[11] = top;
    ys[14] = bottom;
    ys[15] = bottom + 256;
    ys[8] = top - 256;
    ys[9] = top;
    ys[12] = bottom;
    ys[13] = bottom + 256;

    for (i = 0; i < EW_FRAME_PIECE_COUNT; i++) {
        qword[0] = 128;
        qword[1] = 128;
        qword[2] = 128;
        qword[3] = 96;
        qword += 4;
        qword[0] = frameUV[i][0];
        qword[1] = frameUV[i][1];
        qword += 4;
        qword[0] = xs[i * 2];
        qword[1] = ys[i * 2];
        qword[2] = 0;
        qword[3] = 0;
        qword += 4;
        qword[0] = frameUV[i][2];
        qword[1] = frameUV[i][3];
        qword += 4;
        qword[0] = xs[i * 2 + 1];
        qword[1] = ys[i * 2 + 1];
        qword[2] = 0;
        qword[3] = 0;
        qword += 4;
    }
    xglPrimAddGifTagDirect(cursor->packet, gif, (qword - gif->words) / 4);
}

/*
 * Draws a label widget: a four-vertex Gouraud-shaded sprite strip over the
 * widget rectangle (a power-of-two side widened by one pixel, as in
 * frame_put), then the border pieces of frameUV selected by the bits of
 * 0x3D: the top, left and right edges and all four corners, but not the
 * bottom edge.
 */
static void label_put(FontDrawCursor *cursor, EwLabelWidget *widget)
{
    int xs[16];
    int ys[16];
    EwGifQuad *gif;
    unsigned int *qword;
    EwGifRegister *reg;
    int rawWidth;
    int rawHeight;
    int width;
    int height;
    int left;
    int top;
    int right;
    int bottom;
    int z;
    int pieces;
    int i;

    gif = (EwGifQuad *) (((unsigned int)&cursor->work + 15) & ~15);
    qword = gif->words;
    qword[0] = 2;
    qword[1] = GIF_TAG_NREG_AD;
    qword[2] = GIF_TAG_REGS_AD;
    qword[3] = 0;
    qword += 4;
    reg = &gif[1].reg;
    reg->data = 0x7F0007FF;
    reg->reg = SCE_GS_CLAMP_1;
    reg = &gif[2].reg;
    reg->data = 68;
    reg->reg = SCE_GS_ALPHA_1;
    qword += 8;
    qword[0] = 4;
    qword[1] = 0x30A64000;
    qword[2] = 0xFFFFF531;
    qword[3] = 0;
    qword += 4;

    rawWidth = widget->width;
    rawHeight = widget->height;
    width = rawWidth << 4;
    if (checkN2(rawWidth) != 0) {
        width += 16;
    }
    height = rawHeight << 4;
    if (checkN2(rawHeight) != 0) {
        height += 16;
    }
    left = (widget->x + EW_GS_ORIGIN_X) << 4;
    top = (widget->y + EW_GS_ORIGIN_Y) << 4;
    z = widget->z;

    qword[0] = 80;
    qword[1] = 64;
    qword[2] = 112;
    qword[3] = 96;
    qword += 4;
    qword[0] = 0;
    qword[1] = 0;
    qword += 4;
    qword[0] = left;
    qword[1] = top;
    qword[2] = z;
    qword[3] = 0;
    qword += 4;
    qword[0] = 80;
    qword[1] = 64;
    qword[2] = 112;
    qword[3] = 96;
    qword += 4;
    qword[0] = 0;
    qword[1] = height;
    qword += 4;
    qword[0] = left;
    qword[1] = top + height;
    qword[2] = z;
    qword[3] = 0;
    qword += 4;
    qword[0] = 32;
    qword[1] = 32;
    qword[2] = 80;
    qword[3] = 96;
    qword += 4;
    qword[0] = width;
    qword[1] = 0;
    qword += 4;
    qword[0] = left + width;
    qword[1] = top;
    qword[2] = z;
    qword[3] = 0;
    qword += 4;
    qword[0] = 32;
    qword[1] = 32;
    qword[2] = 80;
    qword[3] = 96;
    qword += 4;
    qword[0] = width;
    qword[1] = height;
    qword += 4;
    qword[0] = left + width;
    qword[1] = top + height;
    qword[2] = z;
    qword[3] = 0;
    qword += 4;

    qword[0] = 1;
    qword[1] = GIF_TAG_NREG_AD;
    qword[2] = GIF_TAG_REGS_AD;
    qword[3] = 0;
    qword += 4;
    reg = &gif[17].reg;
    reg->data = 5;
    reg->reg = SCE_GS_CLAMP_1;
    qword += 4;
    qword[0] = GIF_TAG_EOP | 5;
    qword[1] = 0x50AB4000;
    qword[2] = 0xFFF53531;
    qword[3] = 0;
    qword += 4;

    width = rawWidth << 4;
    height = rawHeight << 4;
    right = left + width;
    bottom = top + height;
    xs[0] = xs[2] = left;
    xs[1] = xs[3] = right;
    xs[4] = left - 256;
    xs[5] = left;
    xs[6] = right;
    xs[7] = right + 256;
    xs[8] = left - 256;
    xs[9] = left;
    xs[10] = right;
    xs[11] = right + 256;
    xs[12] = left - 256;
    xs[13] = left;
    xs[14] = right;
    xs[15] = right + 256;
    ys[0] = top - 256;
    ys[1] = top;
    ys[2] = bottom;
    ys[3] = bottom + 256;
    ys[4] = ys[6] = top;
    ys[5] = ys[7] = bottom;
    ys[10] = top - 256;
    ys[11] = top;
    ys[14] = bottom;
    ys[15] = bottom + 256;
    ys[8] = top - 256;
    ys[9] = top;
    ys[12] = bottom;
    ys[13] = bottom + 256;

    pieces = EW_LABEL_PIECE_MASK;
    for (i = 0; i < EW_FRAME_PIECE_COUNT; i++) {
        if (pieces & 1) {
            qword[0] = 128;
            qword[1] = 128;
            qword[2] = 128;
            qword[3] = 96;
            qword += 4;
            qword[0] = frameUV[i][0];
            qword[1] = frameUV[i][1];
            qword += 4;
            qword[0] = xs[i * 2];
            qword[1] = ys[i * 2];
            qword[2] = z;
            qword[3] = 0;
            qword += 4;
            qword[0] = frameUV[i][2];
            qword[1] = frameUV[i][3];
            qword += 4;
            qword[0] = xs[i * 2 + 1];
            qword[1] = ys[i * 2 + 1];
            qword[2] = z;
            qword[3] = 0;
            qword += 4;
        }
        pieces >>= 1;
    }
    xglPrimAddGifTagDirect(cursor->packet, gif, (qword - gif->words) / 4);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* The original default resource path is stored in this TU's read-only data. */
const char EWDefaultResourcePath[23] = "host0:/home/xeno/work/";
