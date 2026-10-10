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

DefocusLayer GameDefocusParam[16] = { 0 };

static void DefocusMain(XglPacket *packet);

extern void DefocusMainType09Final(DefocusLayer *layer);

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

/* The data reference in the original GIF/VIF packet points one word into VU memory. */

/* Partial model of the fixed packet template; unknown words stay explicit. */

typedef struct DefocusSolidPacketHeader {
    u8 unmodeled_00[0x0c];
    u32 packet_tag_0c;
    u32 vu_memory_address_10;
    u32 packet_header_14;
    u32 packet_header_18;
    u32 unmodeled_1c;
    u32 packet_header_20;
    u32 unmodeled_24;
    u32 packet_header_28;
    u32 unmodeled_2c;
    u32 packet_header_30;
    u32 unmodeled_34;
    u32 packet_header_38;
    u32 unmodeled_3c;
} DefocusSolidPacketHeader;

#define DEFOCUS_VU_MEMORY_WORD ((u32 *)0x8001)

static DefocusSolidPacketHeader defocus_solid_packet_header = {
    {0}, 0x51000003, (u32)DEFOCUS_VU_MEMORY_WORD, 0x20000000,
    0x000000EE, 0, 0x00071001, 0, 0x47, 0, 0x84, 0, 0x42, 0
};

extern void sceVif1PkCnt(XglPacket *packet, int count);

extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);

extern int printf(const char *format, ...);

typedef struct {
    u8 unmodeled_00[0x50];
    u64 texture;
    u8 unmodeled_58[8];
} DefocusTexturedHeader;

typedef struct {
    u32 header[3];
    u32 gif_tag;
    u32 vif_command;
    u32 gs_mode;
    u32 gs_registers;
    u32 header_padding;
    u32 red;
    u32 green;
    u32 blue;
    u32 alpha;
    u32 first_u;
    u32 first_v;
    u32 unmodeled_38[2];
    int left;
    int top;
    u32 color;
    u32 flags;
    u32 second_u;
    u32 second_v;
    u32 unmodeled_58[2];
    int right;
    int bottom;
    u32 second_color;
    u32 second_flags;
} DefocusTexturedPacket;

typedef struct {
    u64 unmodeled_00[12];
    u64 framebuffer;
    u64 frame_register;
    u64 texture;
    u64 unmodeled_78[15];
} DefocusCopyPacket;

typedef struct {
    u64 unmodeled_00[6];
    u64 source_frame;
    u64 frame_register;
    u64 color[2];
    u64 top_left;
    int first_color;
    u32 first_flags;
    u64 bottom_right;
    int second_color;
    u32 second_flags;
    u64 unmodeled_70[2];
    u64 destination_frame;
    u64 destination_register;
} DefocusTintPacket;

extern DefocusCopyPacket TestPrim_1;

extern DefocusTintPacket TestPrim_2;

extern u64 TestPrim_3[6];

extern DefocusTexturedHeader Head_4_00367170;

typedef struct {
    u32 unmodeled_00[20];
    u64 texture;
    u64 unmodeled_58;
    u64 texture_dimensions;
    u64 unmodeled_68;
} DefocusType05Header;

extern DefocusType05Header Head_5;

#define DEFOCUS_TYPE05_PACKET ((DefocusTexturedPacket *)0x70000000)

/* The data reference in the original GIF/VIF packet points one word into VU memory. */

/* Partial model of the fixed packet template; unknown words stay explicit. */

typedef struct {
    u8 unmodeled_00[0x20];
    u64 texture;
    u8 unmodeled_28[0x48];
} DefocusCopyTemplate;

extern DefocusCopyTemplate Copy_10;

extern DefocusTexturedHeader Head_11;

extern u64 Head_13[10];

extern u64 Tail_15[8];

extern void (*func_14[11])(DefocusLayer *layer, XglPacket *packet);

/* Runtime writes the layer entries through DefocusSet; the original table is
 * sixteen 0x44-byte records whose initial bytes are all zero. */

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

static void DefocusMainType02(DefocusLayer *layer, XglPacket *packet)
{
    int tint_color;

    TestPrim_1.framebuffer = (u64)sRender.tex0_state_14 | 0x80000;
    TestPrim_1.texture = (u64)0x24020000 | (sRender.tex0_state_20 << 5) |
                        ((u64)0xc800 << 19);
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, &TestPrim_1, 60);
    tint_color = layer->parameters[1];
    if (tint_color != 0) {
        TestPrim_2.first_color = tint_color;
        TestPrim_2.source_frame = (u64)sRender.tex0_state_14 | 0x80000;
        TestPrim_2.second_color = layer->parameters[1];
        TestPrim_2.destination_frame = (u64)sRender.tex0_state_20 | 0x80000;
        sceVif1PkCnt(packet, 0);
        sceVif1PkAddDataN(packet, &TestPrim_2, 36);
        return;
    }
    TestPrim_3[4] = (u64)sRender.tex0_state_20 | 0x80000;
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, TestPrim_3, 12);
}

static void DefocusMainType03(DefocusLayer *layer, XglPacket *packet)
{
    DefocusTexturedPacket *draw = (DefocusTexturedPacket *)0x70000000;
    u8 *color;
    int right;
    int bottom;
    int bottom_origin;
    int bottom_extent;
    u32 repeat = 0;

    Head_4_00367170.texture = GetTex0(2, 1);
    draw->gif_tag = 0x51000006;
    draw->vif_command = 0x8001;
    draw->gs_mode = 0x50ab4000;
    draw->gs_registers = 0x53531;
    draw->header[0] = 0;
    draw->header[1] = 0;
    draw->header[2] = 0;
    draw->header_padding = 0;
    color = (u8 *)&layer->parameters[1];
    draw->red = color[0];
    draw->green = color[1];
    draw->blue = color[2];
    draw->alpha = color[3];
    draw->second_u = 8192;
    draw->first_u = 0;
    draw->left = (layer->parameters[2] << 4) + 28664;
    right = layer->parameters[2] + layer->parameters[4];
    draw->second_v = 7168;
    draw->first_v = 0;
    draw->right = (right << 4) + 36856;
    draw->top = (layer->parameters[3] << 4) + 29175;
    bottom_origin = layer->parameters[3];
    bottom_extent = layer->parameters[5];
    draw->second_color = 0xffffffff;
    draw->color = 0xffffffff;
    bottom = bottom_origin + bottom_extent;
    draw->bottom = (bottom << 4) + 36343;
    draw->second_flags = 0;
    draw->flags = 0;
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, &Head_4_00367170, 24);
    if (layer->parameters[0] != 0) {
        do {
            repeat++;
            sceVif1PkAddDataN(packet, draw, 28);
        } while (repeat < (u32)layer->parameters[0]);
    }
}

static void DefocusMainType05(DefocusLayer *layer, XglPacket *packet)
{
    const u8 *color;
    int x;
    int y;
    int right;
    int bottom;
    u32 draw_color;
    u64 texture;
    u64 width_dimensions;
    u64 height_dimensions;
    u64 texture_dimensions;

    texture = GetTex0(0, 0);
    Head_5.texture = texture;
    width_dimensions =
        ((u64)((long long)layer->parameters[6] ^ 0x3ffULL) << 4) | 15;
    height_dimensions =
        (u64)((long long)layer->parameters[7] ^ 0x3ffULL) << 24;
    texture_dimensions = width_dimensions | height_dimensions;
    Head_5.texture_dimensions = texture_dimensions;
    DEFOCUS_TYPE05_PACKET->gif_tag = 0x51000006;
    DEFOCUS_TYPE05_PACKET->vif_command = 0x8001;
    DEFOCUS_TYPE05_PACKET->header[0] = 0;
    DEFOCUS_TYPE05_PACKET->header[1] = 0;
    DEFOCUS_TYPE05_PACKET->header[2] = 0;
    DEFOCUS_TYPE05_PACKET->gs_mode = 0x50ab4000;
    DEFOCUS_TYPE05_PACKET->gs_registers = 0x53531;
    DEFOCUS_TYPE05_PACKET->header_padding = 0;
    color = (const u8 *)&layer->parameters[1];
    DEFOCUS_TYPE05_PACKET->red = color[0];
    DEFOCUS_TYPE05_PACKET->green = color[1];
    DEFOCUS_TYPE05_PACKET->blue = color[2];
    DEFOCUS_TYPE05_PACKET->alpha = color[3];
    x = layer->parameters[2] << 4;
    DEFOCUS_TYPE05_PACKET->first_u = x;
    DEFOCUS_TYPE05_PACKET->left = x + 28664;
    y = layer->parameters[3] << 4;
    DEFOCUS_TYPE05_PACKET->first_v = y;
    DEFOCUS_TYPE05_PACKET->top = y + 29175;
    right = (layer->parameters[2] + layer->parameters[4]) << 4;
    DEFOCUS_TYPE05_PACKET->second_u = right;
    DEFOCUS_TYPE05_PACKET->right = right + 28664;
    bottom = (layer->parameters[3] + layer->parameters[5]) << 4;
    DEFOCUS_TYPE05_PACKET->second_v = bottom;
    DEFOCUS_TYPE05_PACKET->bottom = bottom + 29175;
    draw_color = layer->parameters[0];
    DEFOCUS_TYPE05_PACKET->second_flags = 0;
    DEFOCUS_TYPE05_PACKET->second_color = draw_color;
    DEFOCUS_TYPE05_PACKET->color = draw_color;
    DEFOCUS_TYPE05_PACKET->flags = 0;
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, &Head_5, 28);
    sceVif1PkAddDataN(packet, DEFOCUS_TYPE05_PACKET, 28);
}

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
    sceVif1PkAddDataN(packet, &defocus_solid_packet_header, DEFOCUS_HEAD_WORDS);
    sceVif1PkAddDataN(packet, DEFOCUS_SCRATCH, DEFOCUS_SOLID_WORDS);
}

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType07);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType08);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType09);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType09Final);

static void DefocusMainType10(DefocusLayer *layer, XglPacket *packet)
{
    extern void GameDefocusSet(int index, int type, const int *settings);
    DefocusTexturedPacket *draw = (DefocusTexturedPacket *)0x70000000;
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
    u8 red_decrement;
    u8 green_decrement;
    u8 blue_decrement;
    u8 alpha_decrement;
    int right;
    u32 vertex_color;

    switch (layer->state) {
    case 0:
        Copy_10.texture = 0x80000 | (u64)(sRender.tex0_state_20 << 5) |
                          ((u64)sRender.tex0_state_14 << 37) |
                          (0x8000ULL << 36);
        sceVif1PkCnt(packet, 0);
        sceVif1PkAddDataN(packet, &Copy_10, 28);
        layer->parameters[5] = 0x80808080;
        layer->state = 1;
        layer->parameters[6] = 0;
        layer->parameters[7] = 0;
        layer->parameters[8] = 0;
        layer->parameters[9] = 0;
        /* The initial frame copy is followed by the first fade update. */
    case 1:
        red_decrement = *(const u8 *)&layer->parameters[0];
        green_decrement = ((const u8 *)&layer->parameters[0])[1];
        blue_decrement = ((const u8 *)&layer->parameters[0])[2];
        alpha_decrement = ((const u8 *)&layer->parameters[0])[3];
        red = *(u8 *)&layer->parameters[5];
        green = ((u8 *)&layer->parameters[5])[1];
        blue = ((u8 *)&layer->parameters[5])[2];
        alpha = ((u8 *)&layer->parameters[5])[3];
        red = red >= red_decrement ? red - red_decrement : 0;
        green = green >= green_decrement ? green - green_decrement : 0;
        blue = blue >= blue_decrement ? blue - blue_decrement : 0;
        alpha = alpha >= alpha_decrement ? alpha - alpha_decrement : 0;
        if (alpha == 0) {
            GameDefocusSet(layer->index, 0, NULL);
            return;
        }
        *((u8 *)&layer->parameters[5]) = red;
        draw->red = red;
        ((u8 *)&layer->parameters[5])[1] = green;
        draw->green = green;
        ((u8 *)&layer->parameters[5])[2] = blue;
        draw->blue = blue;
        ((u8 *)&layer->parameters[5])[3] = alpha;
        draw->alpha = alpha;
        Head_11.texture = GetTex0(2, 0);
        draw->vif_command = 0x8001;
        draw->gif_tag = 0x51000006;
        draw->gs_mode = 0x50ab4000;
        draw->gs_registers = 0x53531;
        draw->header[0] = 0;
        draw->header[1] = 0;
        draw->header[2] = 0;
        draw->header_padding = 0;
        layer->parameters[6] += layer->parameters[1];
        layer->parameters[7] += layer->parameters[2];
        layer->parameters[8] += layer->parameters[3];
        layer->parameters[9] += layer->parameters[4];
        draw->second_u = 8192;
        draw->first_u = 0;
        draw->left = (layer->parameters[6] << 4) + 28664;
        right = layer->parameters[6] + layer->parameters[8];
        draw->second_v = 7168;
        draw->first_v = 0;
        draw->right = (right << 4) + 36856;
        draw->top = (layer->parameters[7] << 4) + 29175;
        draw->bottom = ((layer->parameters[7] + layer->parameters[9]) << 4) + 36343;
        vertex_color = 0xffffffff;
        draw->second_color = vertex_color;
        draw->color = vertex_color;
        draw->second_flags = 0;
        draw->flags = 0;
        sceVif1PkCnt(packet, 0);
        sceVif1PkAddDataN(packet, &Head_11, 24);
        sceVif1PkAddDataN(packet, draw, 28);
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType11);

static void DefocusMain(XglPacket *packet)
{
    DefocusLayer *layer = GameDefocusParam;
    int remaining;
    int type;

    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, Head_13, 20);
    remaining = 15;
    do {
        type = layer->type;
        switch (type) {
        case 0:
            break;
        case 1: case 2: case 3: case 4: case 5: case 6:
        case 7: case 8: case 9: case 10: case 11:
            func_14[type - 1](layer, packet);
            break;
        }
        remaining--;
        layer++;
    } while (remaining >= 0);
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, Tail_15, 16);
}

void GameDefocusCheck(void)
{
    int layer_index;
    int active;

    active = 0;
    layer_index = 0;
    while (layer_index < 16) {
        if (GameDefocusParam[layer_index].type != 0) {
            active = 1;
            break;
        }
        layer_index++;
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

void GameDefocusSet(int index, int type, const int *settings)
{
    int clear_index;
    int parameter_index;
    DefocusLayer *layer;

    if (index >= 16) {
        printf("GameDefocusSet()\xA4\xCE\xB0\xFA\xBF\xF4\xA4\xAC\xB0\xDB\xBE\xEF\xA4\xC7\xA4\xB9(%d)\xA1\xA3\n", index);
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

void GameDefocusQuickSet(int group_index, int preset, int color, int intensity)
{
    DefocusLayer *layers;
    u32 first_index;
    u32 row_address;

    row_address = (u32)GameDefocusParam + (u32)group_index * 0x110u;
    if (group_index >= 4) {
        printf("GameDefocusQuickSet()\xA4\xCE\xB0\xFA\xBF\xF4\xA4\xAC\xB0\xDB\xBE\xEF\xA4\xC7\xA4\xB9(%d)\xA1\xA3\n", group_index);
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
