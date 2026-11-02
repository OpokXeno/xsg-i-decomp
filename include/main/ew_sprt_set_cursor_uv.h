#ifndef INCLUDE_MAIN_EW_SPRT_SET_CURSOR_UV_H
#define INCLUDE_MAIN_EW_SPRT_SET_CURSOR_UV_H

/* Screen the widgets are laid out in. */
#define EW_SCREEN_WIDTH  512
#define EW_SCREEN_HEIGHT 448

/* Number of slots in the ewComponent widget table. */
#define EW_SLOT_COUNT 64

/* Widget kinds (EwWidget.type), as dispatched by EW_drawComoponent. The
   *_TEXT_PASS kinds are drawn in the second (text) pass of EW_draw instead of
   the packet pass. */
enum {
    EW_TYPE_PANEL = 0,          /* label_put: textured panel with a border */
    EW_TYPE_FRAME = 1,          /* frame_put: nine-patch frame */
    EW_TYPE_CONTAINER = 4,      /* owns a children[] array */
    EW_TYPE_SPRITE = 5,         /* sprt_put */
    EW_TYPE_CLIP = 6,           /* set_clip in the packet pass */
    EW_TYPE_LABEL = 7,          /* lines of text */
    EW_TYPE_MASK = 8,           /* mask_put in the packet pass */
    EW_TYPE_CLIP_TEXT_PASS = 9, /* set_clip in the text pass */
    EW_TYPE_MASK_TEXT_PASS = 10 /* mask_put in the text pass */
};

/* EwWidget.drawValue: depth/order key the widget is queued with. */
#define EW_ORDER_FRAME 0x00FFFFF0 /* also the key of the EW_sendPacket pass */
#define EW_ORDER_MASK  0x00FFFFF1
#define EW_ORDER_TEXT  0x00FFFFFF

/* Neutral colour scale of a label; other values print a second pass. */
#define EW_COLOR_SCALE_NEUTRAL 128

/* EwMaskState.blend bit: the gradient runs horizontally instead of vertically. */
#define EW_MASK_GRADIENT_HORIZONTAL 0x100

/* EwMaskState.fix: the two fill intensities (bits 8-15 and 16-23). */
#define EW_MASK_FIX_FULL 0x00808000 /* both 0x80 */
#define EW_MASK_FIX_NONE 0x00010100 /* both 1 */

/* Widget flag bits (EwWidget.flags). Top-level widgets drawn by EW_draw have
   ALLOCATED and VISIBLE set and CHILD clear. */
#define EW_FLAG_CHILD     0x2000 /* owned by a container (EW_addComponent) */
#define EW_FLAG_VISIBLE   0x4000
#define EW_FLAG_ALLOCATED 0x8000 /* slot in use (EW_create) */
#define EW_FLAG_STATE_MASK 0xE000
#define EW_FLAG_TOP_LEVEL_VISIBLE (EW_FLAG_ALLOCATED | EW_FLAG_VISIBLE)

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
    struct EwWidget **children;
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
    unsigned char unmodeled_10[2];
    short linePitch;
    short lineCount;
    short firstLine;
    int colorScale;
    unsigned short *lineWidths;
    const unsigned char *textStyle;
    char **textLines;
} EwLabelWidget;

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

#endif /* INCLUDE_MAIN_EW_SPRT_SET_CURSOR_UV_H */
