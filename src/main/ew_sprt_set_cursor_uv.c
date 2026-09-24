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

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", EW_sprtSetCursorUV);

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

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", EW_setDrawEnv);

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", EW_create);

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", EW_dispose);

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", EW_addComponent);

/*
 * Shared 0x28-byte ewComponent table-slot header (top-of-file comment):
 * flags encodes both the widget type and its enabled bit. EW_sendPacket
 * and EW_draw test the type via (flags & 0xE000) == 0xC000; EW_drawContainer
 * tests only the enabled bit via flags & 0x4000.
 */
typedef struct EwWidget {
    unsigned short flags;
    unsigned char unmodeled_02[0x26];
} EwWidget;

extern void EW_drawComoponent(int context, EwWidget *widget);

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

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", EW_drawComoponent);

extern EwWidget ewComponent[64];

void EW_init(void)
{
    int i;

    for (i = 63; i >= 0; i--)
    {
        ewComponent[i].flags = 0;
    }
}

extern void EW_setDrawEnv(int context);
extern void xglFontReloadTexture(int context, int mode);
extern int ew_send_mode;

void EW_sendPacket(int context)
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

/* Registers EW_sendPacket with the font print queue; the context argument
   position is passed 0 (unused by EW_sendPacket's own logic here). */
extern void xglFontPrintExtFunc(unsigned int flags, void (*draw)(int context), void *arg);

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

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", mask_put);

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", sprt_put);

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", set_clip);

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", checkN2);

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", frame_put);

INCLUDE_ASM("asm/main/nonmatchings/ew_sprt_set_cursor_uv", label_put);
