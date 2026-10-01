#include "common.h"
#include "shared.h"

#define NULL ((void *)0)

typedef struct {
    u8 unmodeled_00[0x14];
    u16 tex0_state_14;
    u8 unmodeled_16[0x0a];
    u16 tex0_state_20;
    u16 tex0_state_22;
    u8 unmodeled_24[0x10];
    void (*callback)(XglPacket *packet);
} DefocusRenderState;

extern DefocusRenderState sRender;

/* Partial view of one GameDefocusParam entry: 16 entries, stride 0x44.
 * The first three bytes are used by the recovered state handlers and the
 * following sixteen words hold the settings copied by GameDefocusSet. */
typedef struct {
    u8 type;
    u8 state;
    u8 index;
    u8 unmodeled_03;
    int parameters[16];
} DefocusLayer;

extern DefocusLayer GameDefocusParam[16];
extern void DefocusMain(XglPacket *packet);
extern void DefocusMainType09Final(DefocusLayer *layer);

static u64 GetTex0(int type, int tex_cc)
{
    switch (type) {
    case 0:
        return ((u64)0x24020000 | (u64)(sRender.tex0_state_20 << 5) |
                ((u64)0x9000 << 18) | ((u64)(u32)tex_cc << 34));
    case 1:
        return ((u64)0x24020000 | (u64)(sRender.tex0_state_22 << 5) |
                ((u64)0x9000 << 18) | ((u64)(u32)tex_cc << 34));
    case 2:
        return ((u64)0x24020000 | (u64)(sRender.tex0_state_14 << 5) |
                ((u64)0x9000 << 18) | ((u64)(u32)tex_cc << 34));
    default:
        return 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType01);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType02);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType03);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType05);

typedef struct {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} DefocusSolidColor;
typedef struct {
    u8 unmodeled_00[4];
    u32 draw_color;
    DefocusSolidColor color_bytes;
    int x;
    int y;
    int width;
    int height;
} DefocusSolidRect;
typedef struct {
    u32 header[3];
    u32 gif_tag;
    u32 vif_command;
    u32 gs_registers;
    u32 register_count;
    u32 header_padding;
    u32 red;
    u32 green;
    u32 blue;
    u32 alpha;
    u32 left;
    u32 top;
    u32 color;
    u32 flags;
    u32 right;
    u32 bottom;
    u32 second_color;
    u32 second_flags;
} DefocusSolidPacket;
#define DEFOCUS_SCRATCH ((DefocusSolidPacket *)0x70000000)
#define DEFOCUS_SOLID_GIF_TAG 0x51000004
#define DEFOCUS_SOLID_VIF_COMMAND 0x8001
#define DEFOCUS_SOLID_GS_MODE 0x30234000
#define DEFOCUS_SOLID_GS_REGISTERS 0x551
#define DEFOCUS_SCREEN_X_BIAS 28664
#define DEFOCUS_SCREEN_Y_BIAS 29175
#define DEFOCUS_HEAD_WORDS 16
#define DEFOCUS_SOLID_WORDS 20
extern const u32 Head_6_00367240[];
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);

static void DefocusMainType06(DefocusSolidRect *rect, XglPacket *packet)
{
    DefocusSolidPacket *draw;
    DefocusSolidColor *color;
    u32 left;
    u32 top;
    u32 draw_color;

    draw = DEFOCUS_SCRATCH;
    color = &rect->color_bytes;
    draw->gif_tag = DEFOCUS_SOLID_GIF_TAG;
    draw->vif_command = DEFOCUS_SOLID_VIF_COMMAND;
    draw->gs_registers = DEFOCUS_SOLID_GS_MODE;
    draw->register_count = DEFOCUS_SOLID_GS_REGISTERS;
    draw->header[0] = 0;
    draw->header[1] = 0;
    draw->header[2] = 0;
    draw->header_padding = 0;
    draw->red = color->red;
    draw->green = color->green;
    draw->blue = color->blue;
    draw->alpha = color->alpha;
    left = ((u32)rect->x << 4) + DEFOCUS_SCREEN_X_BIAS;
    draw->left = left;
    draw->right = left + ((u32)rect->width << 4);
    top = ((u32)rect->y << 4) + DEFOCUS_SCREEN_Y_BIAS;
    draw->top = top;
    draw->bottom = top + ((u32)rect->height << 4);
    draw_color = rect->draw_color;
    draw->color = draw_color;
    draw->second_color = draw_color;
    draw->flags = 0;
    draw->second_flags = 0;
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, Head_6_00367240, DEFOCUS_HEAD_WORDS);
    sceVif1PkAddDataN(packet, DEFOCUS_SCRATCH, DEFOCUS_SOLID_WORDS);
}

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType07);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType08);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType09);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType09Final);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType10);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType11);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMain);

void GameDefocusCheck(void)
{
    int layer_index;
    int active;

    active = 0;
    layer_index = 0;
    if (GameDefocusParam[0].type == 0) {
next_layer:
        layer_index++;
        if (layer_index < 16) {
            if (GameDefocusParam[layer_index].type != 0) {
                goto layer_active;
            }
            goto next_layer;
        }
    } else {
layer_active:
        active = 1;
    }
    if (active == 0) {
        sRender.callback = NULL;
        return;
    }
    sRender.callback = DefocusMain;
}

static void GameDefocusFinalize(int index)
{
    DefocusLayer *layer;

    layer = &GameDefocusParam[index];
    if (layer->type == 9) {
        DefocusMainType09Final(layer);
    }
    layer->index = index;
    layer->state = 0;
    layer->type = 0;
}

extern int printf(const char *format, ...);
extern const char D_004C0200[];

void GameDefocusSet(int index, int type, const int *settings)
{
    int clear_index;
    int parameter_index;
    DefocusLayer *layer;

    if (index >= 16) {
        printf(D_004C0200, index);
        return;
    }
    if (index < 0) {
        for (clear_index = 0; clear_index < 16; clear_index++) {
            GameDefocusFinalize(clear_index);
        }
    } else {
        layer = &GameDefocusParam[index];
        GameDefocusFinalize(index);
        layer->type = type;
        if (type > 0) {
            for (parameter_index = 0; parameter_index < 16; parameter_index++) {
                if (settings == NULL) {
                    layer->parameters[parameter_index] = 0;
                } else {
                    layer->parameters[parameter_index] = settings[parameter_index];
                }
            }
        }
    }
    GameDefocusCheck();
}

extern const char D_004C0228[];

void GameDefocusQuickSet(int group_index, int preset, int color, int intensity)
{
    DefocusLayer *layers;
    u32 first_index;
    u32 row_address;

    row_address = (u32)GameDefocusParam + (u32)group_index * 0x110u;
    if (group_index >= 4) {
        printf(D_004C0228, group_index);
        return;
    }

    layers = (DefocusLayer *)row_address;
    first_index = (u32)group_index * 4u;
    GameDefocusFinalize(first_index);
    GameDefocusFinalize(first_index + 1);
    GameDefocusFinalize(first_index + 2);
    GameDefocusFinalize(first_index + 3);

    switch (preset) {
    case 0:
        break;
    case 1:
        layers[0].type = 1;
        layers[0].parameters[0] = 0;
        layers[0].parameters[1] = 1;
        layers[0].parameters[2] = color;
        layers[0].parameters[3] = 0x40808080;
        layers[0].parameters[4] = intensity;
        layers[0].parameters[5] = 0;
        layers[0].parameters[6] = 0;
        layers[0].parameters[7] = 0;
        layers[1].type = 1;
        layers[1].parameters[0] = 0;
        layers[1].parameters[1] = 1;
        layers[1].parameters[2] = color;
        layers[1].parameters[3] = 0x40808080;
        layers[1].parameters[4] = 0;
        layers[1].parameters[5] = intensity;
        layers[1].parameters[6] = 0;
        layers[1].parameters[7] = 0;
        break;
    case 2:
        layers[0].type = 2;
        layers[0].parameters[0] = 0;
        layers[0].parameters[1] = color;
        layers[1].type = 3;
        layers[1].parameters[0] = 1;
        layers[1].parameters[1] = 0x40808080;
        layers[1].parameters[2] = intensity;
        layers[1].parameters[3] = 0;
        layers[1].parameters[4] = 0;
        layers[1].parameters[5] = 0;
        layers[2].type = 3;
        layers[2].parameters[0] = 1;
        layers[2].parameters[1] = 0x40808080;
        layers[2].parameters[2] = 0;
        layers[2].parameters[3] = intensity;
        layers[2].parameters[4] = 0;
        layers[2].parameters[5] = 0;
        layers[3].type = 3;
        layers[3].parameters[0] = 1;
        layers[3].parameters[1] = 0x40808080;
        layers[3].parameters[2] = intensity;
        layers[3].parameters[3] = intensity;
        layers[3].parameters[4] = 0;
        layers[3].parameters[5] = 0;
        break;
    case 4:
        layers[0].type = 1;
        layers[0].parameters[0] = 1;
        layers[0].parameters[1] = 1;
        layers[0].parameters[2] = color;
        layers[0].parameters[3] = 0x00808080u + ((u32)intensity << 24);
        layers[0].parameters[4] = 0;
        layers[0].parameters[5] = 0;
        layers[0].parameters[6] = 0;
        layers[0].parameters[7] = 0;
        break;
    case 5:
        layers[0].type = 5;
        layers[0].parameters[0] = color;
        layers[0].parameters[1] = 0x80808080;
        layers[0].parameters[4] = 512;
        layers[0].parameters[5] = 448;
        layers[0].parameters[2] = 0;
        layers[0].parameters[3] = 0;
        layers[0].parameters[6] = intensity;
        layers[0].parameters[7] = intensity;
        break;
    case 6:
        layers[0].type = 6;
        layers[0].parameters[0] = color;
        layers[0].parameters[1] = 0x80ffffff;
        layers[0].parameters[2] = 0;
        layers[0].parameters[3] = 0;
        layers[0].parameters[4] = 512;
        layers[0].parameters[5] = 448;
        break;
    case 9:
        layers[0].type = 9;
        break;
    case 10:
        layers[0].type = 10;
        layers[0].parameters[0] = (u32)intensity << 24;
        layers[0].parameters[1] = 0;
        layers[0].parameters[2] = 0;
        layers[0].parameters[3] = 0;
        layers[0].parameters[4] = 0;
        break;
    }
    GameDefocusCheck();
}
