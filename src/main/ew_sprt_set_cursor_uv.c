#include "common.h"

/*
 * EW_create (main:0x0026c0e0) indexes a fixed table of 40-byte (0x28)
 * ewComponent slots (index*5*8); each slot holds a 0x10-byte shared
 * header (flags at +0x00, widget type at +0x02) followed by 0x18 bytes
 * of type-specific storage. It calls one of frame_init/sprt_init/
 * container_init/mask_init with a pointer to that type-specific storage
 * to initialize a newly created widget (type 1, 5, 4 and 8/10
 * respectively, per the jump table at 0x004c2360).
 */

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

typedef struct EwSpriteState {
    /* +0x00/+0x02: cleared/set by sprt_init; same "no reader in this TU"
       evidence tier as EwFrameState's fields above. */
    short flags;
    signed char alpha;
    /* +0x04/+0x06: texture cell coordinates. Not touched by sprt_init,
       but written by EW_sprtSetCursorUV and read back by sprt_put with
       lhu (main:0x0026bf80, 0x0026ca98), which fixes the width as
       unsigned short. */
    unsigned short u;
    unsigned short v;
} EwSpriteState;

typedef struct EwMaskState {
    /* +0x00: low 32 bits of the GS ALPHA_1 register value mask_put
       programs for this mask (main:0x0026c880 sends this word and the
       next one as the 64-bit data of GS register 0x42/ALPHA_1). Bits
       0-7 are the hardware A/B/C/D blend-source selectors; bit 0x100
       additionally selects one of mask_put's two drawing variants (its
       own test on this word, not a GS field). */
    int blend;
    /* +0x04: high 32 bits of the same ALPHA_1 value. Byte 0 (bits 32-39
       of the 64-bit register) is the hardware FIX coefficient; mask_put
       also reads bytes 1 and 2 back out of this word (lbu +0x05, +0x06)
       as two 8-bit fill intensities of its own -- bits the GS ALPHA_1
       register itself never reads. */
    int fix;
} EwMaskState;

typedef struct EwContainerState {
    /* +0x00: children[] slot count. Zeroed here; the container's owner
       sets the real value right after EW_create returns (TWIN_create2
       sets 8, TMENU_init sets 16, both immediately after creating a
       type-4 widget, main:0x0025dc68/0x0025ed60) and EW_addComponent,
       EW_drawContainer and EW_dispose all read it as the children[]
       bound. */
    short childCapacity;
    /* +0x04: caller-owned array of child widget-slot pointers, set by
       the same owners and read by the same three functions. */
    void **children;
    /* +0x08/+0x0C: zeroed here; no code reachable from this allocation
       (EW_create, EW_addComponent, EW_drawContainer, EW_dispose, this
       TU's set_clip/label_put, and TWIN_create2/TMENU_init, the only
       other callers found that build a type-4 container) reads either
       word again for a container. The label widget type reads the same
       slot bytes (EW_drawComoponent, slot+0x18/+0x1C) for its own data. */
    int unusedByContainer[2];
} EwContainerState;

typedef struct EwContainerWidget {
    unsigned short flags;
    unsigned short type;
    unsigned char unmodeled_04[0x0C];
    EwContainerState container;
    unsigned char unmodeled_20[0x08];
} EwContainerWidget;

/*
 * Shared 0x28-byte ewComponent table-slot header (top-of-file comment):
 * flags encodes both the widget type and its enabled bit. EW_sendPacket
 * and EW_draw test the type via (flags & 0xE000) == 0xC000; EW_drawContainer
 * tests only the enabled bit via flags & 0x4000.
 */

typedef struct EwWidget {
    unsigned short flags;
    unsigned short type;
    short x;
    short y;
    int drawValue;
    short width;
    short height;
    unsigned char unmodeled_10[0x18];
} EwWidget;

extern void EW_drawComoponent(int context, EwWidget *widget);

/* Retail maps ewComponent as a GLOBAL OBJECT at 0x0099CA30 with size 0xA00.
 * This table has 64 observed 0x28-byte slots; only flags are modeled here. */

EwWidget ewComponent[64];

static void EW_setDrawEnv(int context);

extern void xglFontReloadTexture(int context, int mode);

static int ew_send_mode = 0;

/* The font queue invokes callbacks with its render context and the saved
 * argument. EW_draw registers EW_sendPacket with a null saved argument. */

extern void xglFontPrintExtFunc(unsigned int flags, void (*draw)(int, void *), void *arg);

#include "shared.h"

/*
 * EW_create (main:0x0026c0e0) indexes a fixed table of 40-byte (0x28)
 * ewComponent slots (index*5*8); each slot holds a 0x10-byte shared
 * header (flags at +0x00, widget type at +0x02) followed by 0x18 bytes
 * of type-specific storage. It calls one of frame_init/sprt_init/
 * container_init/mask_init with a pointer to that type-specific storage
 * to initialize a newly created widget (type 1, 5, 4 and 8/10
 * respectively, per the jump table at 0x004c2360).
 */

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

typedef struct EwMaskWidget {
    unsigned short flags;
    unsigned short type;
    short x;
    short y;
    int z;
    short width;
    short height;
    EwMaskState mask;
    unsigned char unmodeled_18[0x10];
} EwMaskWidget;

typedef struct EwLabelWidget {
    unsigned short flags;
    unsigned short type;
    short x;
    short y;
    int z;
    short width;
    short height;
    unsigned char unmodeled_10[0x18];
} EwLabelWidget;

static u64 sprt_base_register[4] = {
    0x2007EF0669343C00ULL, 0x2007EF0669343C00ULL,
    0x2007EF0669343C00ULL, 0x2007EF0669343C00ULL
};

extern void xglPrimAddGifTagDirect(XglPacket *packet, const void *data, int count);

typedef struct EwDrawContext {
    XglPacket *packet;
} EwDrawContext;

typedef struct EwGifRegister {
    u64 data;
    u64 reg;
} EwGifRegister;

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

static void container_init(EwContainerState *container);

const unsigned char D_004C2340[0x20] = {
    0x0D, 0x02, 0x0E, 0x02, 0x03, 0x18, 0x18, 0x18,
    0x0F, 0x00, 0x00, 0x80, 0x0C, 0x80, 0x80, 0x80,
    0x15, 0x02, 0x02, 0x04, 0x19, 0x03, 0x15, 0x03,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*
 * Shared 0x28-byte ewComponent table-slot header (top-of-file comment):
 * flags encodes both the widget type and its enabled bit. EW_sendPacket
 * and EW_draw test the type via (flags & 0xE000) == 0xC000; EW_drawContainer
 * tests only the enabled bit via flags & 0x4000.
 */

extern void frame_put(int context, EwWidget *widget);

static void label_put(EwDrawContext *context, EwLabelWidget *widget);

static void mask_put(int context, void *arg);

static void set_clip(int context, void *arg);

static void sprt_put(int context, EwSpriteWidget *widget);

extern void xglFontPrint(int x, int y, int color, const char *text);

extern char *tmp_0;

/* Retail maps ewComponent as a GLOBAL OBJECT at 0x0099CA30 with size 0xA00.
 * This table has 64 observed 0x28-byte slots; only flags are modeled here. */

/*
 * Draws a mask widget as one four-vertex Gouraud triangle strip (GIF
 * PRIM 0x20264000 with the ALPHA_1 value held in the mask state). The
 * top edge is filled with the intensity in bits 16-23 of the mask's fix
 * word and the bottom-right edge with bits 8-15; bit 0x100 of the blend
 * word swaps the two middle vertices, which turns the gradient from
 * vertical to horizontal.
 */

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

/*
 * Draws a label widget: a four-vertex Gouraud-shaded sprite strip over the
 * widget rectangle (a power-of-two side widened by one pixel, as in
 * frame_put), then the border pieces of frameUV selected by the bits of
 * 0x3D: the top, left and right edges and all four corners, but not the
 * bottom edge.
 */

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
    frame->alpha = 0x60;
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

static void EW_setDrawEnv(int context)
{
    EwDrawEnvPacket *env;
    EwGifRegister *regs;
    EwGifRegister *entry;

    env = (EwDrawEnvPacket *) ((context + 63) & -16);
    env->tag[0] = 0x8006;
    env->tag[1] = 0x10000000;
    env->tag[2] = -2;
    env->tag[3] = 0;
    regs = env->regs;
    entry = regs + 0;
    entry->data = 68;
    entry->reg = 66;
    entry = regs + 1;
    entry->data = 0;
    entry->reg = 63;
    entry = regs + 2;
    entry->data = 0;
    entry->reg = 59;
    entry = regs + 3;
    entry->data = 65;
    entry->reg = 20;
    entry = regs + 4;
    entry->data = sprt_base_register[0];
    entry->reg = 6;
    entry = regs + 5;
    entry->data = 0x31001;
    entry->reg = 71;
    xglPrimAddGifTagDirect(((EwDrawContext *) context)->packet, env, 7);
}

EwWidget *EW_create(int index, int type)
{
    EwWidget *widget;
    int i;

    widget = 0;
    if (index < 0) {
        for (i = 0; i < 64; i++) {
            if ((ewComponent[i].flags & 0x8000) == 0) {
                index = i;
                break;
            }
        }
    }
    if ((unsigned int) index < 64) {
        widget = &ewComponent[index];
        widget->type = type;
        widget->flags = 0x8000;
        switch (widget->type) {
        case 1:
            frame_init(&((EwFrameWidget *) widget)->frame);
            break;
        case 5:
            sprt_init(&((EwSpriteWidget *) widget)->sprite);
            break;
        case 4:
            container_init(&((EwContainerWidget *) widget)->container);
            break;
        case 8:
        case 10:
            mask_init(&((EwMaskWidget *) widget)->mask);
            break;
        case 7: {
            EwTextWidget *label = (EwTextWidget *) widget;

            label->text.lineCount = -1;
            label->text.lineHeight = 24;
            label->text.fontStyle = D_004C2340;
            label->text.colorScale = 128;
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
    void **firstChild;
    void **children;
    unsigned short *childFlags;
    int capacity;
    int remaining;

    if (widget->type == 4)
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
                    childFlags = *children;
                    children++;
                    if (childFlags != 0)
                    {
                        *childFlags = 0;
                    }
                    remaining--;
                } while (remaining != 0);
            }
        }
    }
    widget->flags = 0;
}

int EW_addComponent(EwContainerWidget *widget, int index, unsigned short *child)
{
    void **children;
    int capacity;
    int i;

    if (widget->type != 4 || (children = widget->container.children) == 0) {
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
        *child |= 0x2000;
        children[index] = child;
    } else {
        index = -1;
    }
    return index;
}

void EW_drawContainer(int context, EwContainerState *container)
{
    EwWidget **firstChild;
    EwWidget **children;
    EwWidget *child;
    short capacity;
    int count;

    firstChild = (EwWidget **) container->children;
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
                if (child != 0 && (child->flags & 0x4000))
                {
                    EW_drawComoponent(context, child);
                }
                count--;
            } while (count != 0);
        }
    }
}

void EW_drawComoponent(int context, EwWidget *widget)
{
    switch (widget->type)
    {
    case 0:
        if (ew_send_mode & 1)
        {
            label_put((EwDrawContext *) context, (EwLabelWidget *) widget);
        }
        break;
    case 1:
        if (ew_send_mode & 1)
        {
            frame_put(context, widget);
        }
        break;
    case 4:
        EW_drawContainer(context, &((EwContainerWidget *) widget)->container);
        break;
    case 5:
        if (ew_send_mode & 1)
        {
            sprt_put(context, (EwSpriteWidget *) widget);
        }
        break;
    case 6:
        if (ew_send_mode & 1)
        {
            set_clip(context, widget);
        }
        break;
    case 8:
        if (ew_send_mode & 1)
        {
            mask_put(context, (EwMaskWidget *) widget);
        }
        break;
    case 9:
        if (ew_send_mode & 2)
        {
            xglFontPrintExtFunc(widget->drawValue, set_clip, widget);
        }
        break;
    case 10:
        if (ew_send_mode & 2)
        {
            xglFontPrintExtFunc(widget->drawValue, mask_put, widget);
        }
        break;
    case 7:
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

        if ((ew_send_mode & 2) == 0)
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
        if (colorScale != 128)
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

    for (i = 63; i >= 0; i--)
    {
        ewComponent[i].flags = 0;
    }
}

void EW_sendPacket(int context, void *argument)
{
    int i;

    xglFontReloadTexture(context, 2);
    ew_send_mode = 1;
    EW_setDrawEnv(context);
    for (i = 0; i < 64; i++)
    {
        if ((ewComponent[i].flags & 0xE000) == 0xC000)
        {
            EW_drawComoponent(context, &ewComponent[i]);
        }
    }
    xglFontReloadTexture(context, 1);
    ew_send_mode = 0;
}

void EW_draw(void)
{
    int i;

    xglFontPrintExtFunc(0x00FFFFF0, EW_sendPacket, 0);
    ew_send_mode = 2;
    for (i = 0; i < 64; i++)
    {
        if ((ewComponent[i].flags & 0xE000) == 0xC000)
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

static void mask_put(int context, void *arg)
{
    EwDrawContext *drawContext;
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

    drawContext = (EwDrawContext *) context;
    widget = (EwMaskWidget *) arg;
    mask = &widget->mask;
    gif = (unsigned int *) (((int) context + 63) & -16);
    qword = gif;
    qword[0] = 1;
    qword[1] = 0x10000000;
    qword[2] = -2;
    qword[3] = 0;
    qword += 4;
    qword[0] = mask->blend;
    qword[1] = mask->fix;
    qword[2] = 66;
    qword += 4;
    qword[0] = 0x8004;
    qword[1] = 0x20264000;
    qword[2] = -175;
    qword[3] = 0;
    qword += 4;
    width = widget->width << 4;
    height = widget->height << 4;
    left = (widget->x + 1792) << 4;
    top = (widget->y + 1824) << 4;
    z = widget->z;
    if ((mask->blend & 0x100) == 0) {
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
    xglPrimAddGifTagDirect(drawContext->packet, gif, (qword - gif) / 4);
}

static void sprt_put(int context, EwSpriteWidget *widget)
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

    packet = (unsigned int *) ((context + 63) & -16);
    quad = packet;
    quad[0] = 1;
    quad[1] = 0x20000000;
    quad[2] = -18;
    quad[3] = 0;
    quad = packet + 4;
    ((u64 *) quad)[0] = 0x7F1FC200;
    ((u64 *) quad)[1] = 8;
    setup = packet + 8;
    ((u64 *) setup)[0] = 68;
    ((u64 *) setup)[1] = 66;
    setup = packet + 12;
    setup[0] = 0x8001;
    setup[1] = 0x50AB4000;
    setup[2] = 0xFFF53531;
    setup[3] = 0;
    sprite = &widget->sprite;
    texU = sprite->u << 4;
    texV = sprite->v << 4;
    screenX = (widget->x + 1792) << 4;
    screenY = (widget->y + 1824) << 4;
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
    xglPrimAddGifTagDirect(((EwDrawContext *) context)->packet, packet, 9);
}

static void set_clip(int context, void *arg)
{
    EwWidget *widget;
    EwDrawEnvPacket *env;
    EwGifRegister *entry;
    int top;
    int left;
    int bottom;
    int right;

    widget = (EwWidget *) arg;
    env = (EwDrawEnvPacket *) ((context + 63) & -16);
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
    if (right > 511) {
        right = 511;
    }
    if (bottom > 447) {
        bottom = 447;
    }
    env->tag[0] = 0x8001;
    env->tag[1] = 0x10000000;
    env->tag[2] = -2;
    env->tag[3] = 0;
    entry = &env->regs[0];
    entry->data = (u64) left | (u64) right << 16 | (u64) top << 32 | (u64) (bottom - 2) << 48;
    entry->reg = 64;
    xglPrimAddGifTagDirect(((EwDrawContext *) context)->packet, env, 2);
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

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", frame_put);

static void label_put(EwDrawContext *context, EwLabelWidget *widget)
{
    int xs[16];
    int ys[16];
    unsigned int *gif;
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

    gif = (unsigned int *) (((int) context + 63) & -16);
    qword = gif;
    qword[0] = 2;
    qword[1] = 0x10000000;
    qword[2] = -2;
    qword[3] = 0;
    qword += 4;
    reg = (EwGifRegister *) qword;
    reg->data = 0x7F0007FF;
    reg->reg = 8;
    reg = (EwGifRegister *) (qword + 4);
    reg->data = 68;
    reg->reg = 66;
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
    left = (widget->x + 1792) << 4;
    top = (widget->y + 1824) << 4;
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
    qword[1] = 0x10000000;
    qword[2] = -2;
    qword[3] = 0;
    qword += 4;
    reg = (EwGifRegister *) qword;
    reg->data = 5;
    reg->reg = 8;
    qword += 4;
    qword[0] = 0x8005;
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

    pieces = 0x3D;
    for (i = 0; i < 8; i++) {
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
    xglPrimAddGifTagDirect(context->packet, gif, (qword - gif) / 4);
}
