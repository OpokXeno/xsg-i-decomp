#include "common.h"
#include "shared.h"

/* Only the command words touched before ACT_DrawShadowBegin sends the buffer. */
typedef struct {
    u8 unmodeled_00[0x50];
    u64 shadow_command_word_10;
    u8 unmodeled_58[0x38];
    u64 shadow_command_word_18;
    u8 unmodeled_98[8];
} ActShadowCommand;

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

extern ActShadowCommand Head_9;
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

void ACT_DrawShadowBegin(void)
{
    u16 shadow_state = sRender.shadow_state;

    Head_9.shadow_command_word_10 = shadow_state | 0xFFFFFF00080000ULL;
    Head_9.shadow_command_word_18 = shadow_state;
    /* Set the command-enable bit after initializing the state bits. */
    Head_9.shadow_command_word_18 |= 0x80000;
    nmlModelDirectSend(1, (u8 *)&Head_9, 10);
}

extern void nmlModelDirectSend(int mode, u8 *data, int count);
extern u8 Tail_10[];

void ACT_DrawShadowEnd(void)
{
    nmlModelDirectSend(1, Tail_10, 3);
}

INCLUDE_ASM("asm/main/nonmatchings/act_3", ACT_DrawShadow);

void ACT_DrawShadowInit(void)
{
}
