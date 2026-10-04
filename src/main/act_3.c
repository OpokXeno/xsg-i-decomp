#include "common.h"
#include "shared.h"

typedef union {
    u64 value;
    struct {
        u32 low;
        u32 high;
    } words;
} GifCommandData;

typedef struct {
    GifCommandData data;
    u32 register_address;
    u32 unused;
} GifAdCommand;

typedef struct {
    u32 control_low;
    u32 control_high;
    u32 registers_low;
    u32 registers_high;
} GifTag;

typedef struct {
    GifCommandData low_color;
    u32 blue_and_unused;
    u32 alpha_and_unused;
} GifRgbaq;

typedef struct {
    u16 x;
    u16 unused_x;
    u16 y;
    u16 unused_y;
    u32 z;
    u32 unused;
} GifXyz2;

#define GIF_TAG_NLOOP_1 1u
#define GIF_TAG_EOP (1u << 15)
#define GIF_TAG_PRE (1u << 14)
#define GIF_TAG_PRIM_SPRITE (6u << 15)
#define GIF_TAG_NREG_8 (8u << 28)
#define GIF_TAG_NREG_1 (1u << 28)
#define GIF_REG_A_D 0xEu
#define GIF_REG_RGBAQ 0x1u
#define GIF_REG_XYZ2 0x5u
#define SHADOW_GIF_REGISTERS \
    ((GIF_REG_A_D << 0) | (GIF_REG_A_D << 4) | \
     (GIF_REG_A_D << 8) | (GIF_REG_A_D << 12) | \
     (GIF_REG_RGBAQ << 16) | (GIF_REG_XYZ2 << 20) | \
     (GIF_REG_XYZ2 << 24) | (GIF_REG_A_D << 28))

#define GS_CLAMP_REGION_REPEAT 2u
#define GS_CLAMP_MIN_U 0u
#define GS_CLAMP_MAX_U 511u
#define GS_CLAMP_MIN_V 0u
#define GS_CLAMP_MAX_V 447u
#define GS_CLAMP_1_VALUE \
    ((u64)GS_CLAMP_REGION_REPEAT | ((u64)GS_CLAMP_REGION_REPEAT << 2) | \
     ((u64)GS_CLAMP_MIN_U << 4) | ((u64)GS_CLAMP_MAX_U << 14) | \
     ((u64)GS_CLAMP_MIN_V << 24) | ((u64)GS_CLAMP_MAX_V << 34))

/* DMA TTE carries DIRECT; its payload is one GIFtag and eight packed registers. */
typedef struct {
    u64 dma_tag;
    u32 vif_nop;
    u32 vif_direct;
    GifTag gif_tag;
    GifAdCommand set_gs_mode;
    GifAdCommand set_draw_environment;
    GifAdCommand set_clamp_1;
    GifAdCommand set_color_control;
    GifRgbaq base_color;
    GifXyz2 top_left;
    GifXyz2 bottom_right;
    GifAdCommand set_shadow_context;
} ActShadowCommand;

/* The final direct packet has one packed A+D register write. */
typedef struct {
    u64 dma_tag;
    u32 vif_nop;
    u32 vif_direct;
    GifTag gif_tag;
    GifAdCommand set_shadow_register;
} ActShadowEndPacket;

/* The render-state halfword read by the shadow command builder is at +0x20. */
typedef struct {
    u8 unmodeled_00[0x20];
    u16 shadow_state;
} ActShadowRenderState;

/* Each source point contributes three words to the rear-shadow packet. */
typedef struct {
    u32 x;
    u32 y;
    u32 z;
} DropShadowSourcePoint;

typedef struct {
    DropShadowSourcePoint *points[4];
} DropShadowBackEntry;

typedef struct {
    u32 x;
    u32 y;
    u32 z;
    u32 w;
} DropShadowPacketPoint;

/* Only the packet words and four 16-byte output points written here are modeled. */
typedef struct {
    u8 unmodeled_00[0x0c];
    u32 packet_tag_0c;
    u8 unmodeled_10[4];
    u32 packet_header_14;
    u32 packet_header_18;
    u8 unmodeled_1c[4];
    u32 packet_header_20;
    u8 unmodeled_24[0x10];
    u32 packet_header_34;
    u8 unmodeled_38[8];
    u32 packet_state[3];
    u32 draw_mode;
    DropShadowPacketPoint points[4];
} DropShadowBackPacket;

/* Status bytes and 16-byte source records immediately precede the sent packet. */
typedef struct {
    u8 unmodeled_00[0x11a0];
    u8 back_shadow_status[16];
    DropShadowBackEntry back_shadow_entries[17];
    DropShadowBackPacket back_shadow_packet;
} ActDropShadowState;

extern ActShadowRenderState sRender;
extern void nmlModelDirectSend(int mode, u8 *data, int count);

INCLUDE_ASM("asm/main/nonmatchings/act_3", Footstep);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawCircleShadow);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawZeldaShadowSub);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawZeldaShadow);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropShadowSubChk);

static void DrawDropShadowSubBack(ActDropShadowState *shadow, int vertex_count,
                                  int draw_mode)
{
    int remaining;
    u8 *status;
    DropShadowBackEntry *entry;
    int active_status;

    entry = shadow->back_shadow_entries;
    shadow->back_shadow_packet.packet_tag_0c = 0x51000008;
    shadow->back_shadow_packet.packet_header_14 = 0x70024000;
    shadow->back_shadow_packet.packet_header_18 = 0x055551ee;
    shadow->back_shadow_packet.packet_header_20 = 0x00071001;
    shadow->back_shadow_packet.packet_header_34 = 0x7fffffff;
    shadow->back_shadow_packet.draw_mode = draw_mode;
    shadow->back_shadow_packet.packet_state[0] = 0;
    shadow->back_shadow_packet.packet_state[1] = 0;
    shadow->back_shadow_packet.packet_state[2] = 0;
    shadow->back_shadow_packet.points[0].w = 0;
    shadow->back_shadow_packet.points[1].w = 0;
    shadow->back_shadow_packet.points[2].w = 0;
    shadow->back_shadow_packet.points[3].w = 0;

    if (vertex_count <= 0) {
        return;
    }

    remaining = vertex_count;
    active_status = 1;
    status = shadow->back_shadow_status;

    /* The original advances the status bytes and vertex records in lockstep. */
    do {
        if (*status == active_status) {
            shadow->back_shadow_packet.points[0].x = entry->points[0]->x;
            shadow->back_shadow_packet.points[0].y = entry->points[0]->y;
            shadow->back_shadow_packet.points[0].z = entry->points[0]->z;
            shadow->back_shadow_packet.points[1].x = entry->points[1]->x;
            shadow->back_shadow_packet.points[1].y = entry->points[1]->y;
            shadow->back_shadow_packet.points[1].z = entry->points[1]->z;
            shadow->back_shadow_packet.points[2].x = entry->points[2]->x;
            shadow->back_shadow_packet.points[2].y = entry->points[2]->y;
            shadow->back_shadow_packet.points[2].z = entry->points[2]->z;
            shadow->back_shadow_packet.points[3].x = entry->points[3]->x;
            shadow->back_shadow_packet.points[3].y = entry->points[3]->y;
            shadow->back_shadow_packet.points[3].z = entry->points[3]->z;
            nmlModelDirectSend(1, (u8 *)&shadow->back_shadow_packet, 9);
        }
        remaining--;
        status++;
        entry++;
    } while (remaining != 0);
}

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropShadowSubFront);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropShadowSub);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropShadow);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropCircle);

static ActShadowCommand Head_9 = {
        .dma_tag = 0,
        .vif_nop = 0,
        .vif_direct = 0x51000009,
        .gif_tag = {
            GIF_TAG_NLOOP_1 | GIF_TAG_EOP,
            GIF_TAG_NREG_8 | GIF_TAG_PRE | GIF_TAG_PRIM_SPRITE,
            SHADOW_GIF_REGISTERS,
            0
        },
        .set_gs_mode = { { .words = { 0x31000000, 1 } }, 0x4E, 0 },
        .set_draw_environment = { { .value = 0x00071001 }, 0x47, 0 },
        .set_clamp_1 = { { .value = GS_CLAMP_1_VALUE }, 0x08, 0 },
        .set_color_control = { { .value = 0 }, 0x4C, 0 },
        .base_color = { { .value = 0 }, 0, 0x80 },
        .top_left = { 0x6FF8, 0, 0x71F7, 0, 0x00F00000, 0 },
        .bottom_right = { 0x8FF8, 0, 0x8DF7, 0, 0x00F00000, 0 },
        .set_shadow_context = { { .value = 0 }, 0x4C, 0 },
};

static ActShadowEndPacket Tail_10 = {
    .dma_tag = 0,
    .vif_nop = 0,
    .vif_direct = 0x51000002,
    .gif_tag = {
        GIF_TAG_NLOOP_1 | GIF_TAG_EOP,
        GIF_TAG_NREG_1,
        GIF_REG_A_D,
        0
    },
    .set_shadow_register = { { .value = 0x31000000 }, 0x4E, 0 },
};

void ACT_DrawShadowBegin(void)
{
    u16 shadow_state = sRender.shadow_state;

    Head_9.set_color_control.data.value = shadow_state | 0xFFFFFF00080000ULL;
    Head_9.set_shadow_context.data.value = shadow_state;
    /* Set the command-enable bit after initializing the state bits. */
    Head_9.set_shadow_context.data.value |= 0x80000;
    nmlModelDirectSend(1, (u8 *)&Head_9, 10);
}

extern void nmlModelDirectSend(int mode, u8 *data, int count);
void ACT_DrawShadowEnd(void)
{
    nmlModelDirectSend(1, (u8 *)&Tail_10, 3);
}

INCLUDE_ASM("asm/main/nonmatchings/act_3", ACT_DrawShadow);

void ACT_DrawShadowInit(void)
{
}
