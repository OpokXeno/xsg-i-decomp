#include "common.h"

#include "shared.h"

static unsigned char TestEnv_0_00367AA0[0x60] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x51, 0x01, 0x80, 0x00, 0x00, 0x00, 0x40, 0x03, 0x40, 0x1E, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x6F, 0x00, 0x00, 0xF7, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x8F, 0x00, 0x00, 0xF7, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00
};

/* Background draw type 0 draws nothing; Game calls it directly. */

/*
 * The do-nothing hook GameBgDrawType1Entry and GameBgDrawType2Entry install
 * at GameLoopState+0x30; GameDrawSync calls it with no arguments.
 */

typedef struct GameBgDrawType1Particle {
    u16 x;
    u16 y;
    s16 velocity_x;
    s16 velocity_y;
} GameBgDrawType1Particle;

typedef struct GameBgDrawType1Parameters {
    u8 initialized;
    u8 unmodeled_01[2];
    u8 particle_count;
    s16 spawn_x;
    s16 spawn_y;
    u16 entry_clear[2];
    GameBgDrawType1Particle particles[256];
} GameBgDrawType1Parameters;

typedef struct GameLoopBackgroundCallbacks {
    u8 unmodeled_00[0x2c];
    void (*background_draw)(void);
    void (*background_sync)(void);
    u8 unmodeled_34[0x2a030 - 0x34];
} GameLoopBackgroundCallbacks;

GameBgDrawType1Parameters GameBgDrawType1Param = {0};

extern GameLoopBackgroundCallbacks GameLoopState;

void GameBgDrawType1(void);

typedef struct GameBgDrawType1PacketHeader {
    u32 header_pad_00[3];
    u32 gif_tag;
    u32 vif_command;
    u32 packet_header_14;
    u32 packet_header_18;
    u32 header_pad_1c;
    u64 register_pair_20;
    u64 register_pair_28;
    u64 register_pair_30;
    u64 register_pair_38;
    u32 vu_memory_address;
    u32 gs_mode;
    u32 gs_registers;
    u32 header_pad_4c;
} GameBgDrawType1PacketHeader;

typedef struct GameBgDrawType1Vertex {
    u32 start_st[4];
    u32 x;
    u32 y;
    u32 start_zw[2];
    u32 red;
    u32 green;
    u32 blue;
    u32 alpha;
    u32 end_x;
    u32 end_y;
    u32 end_zw[2];
} GameBgDrawType1Vertex;

typedef struct GameBgDrawType1Packet {
    GameBgDrawType1PacketHeader header;
    GameBgDrawType1Vertex vertices[1];
} GameBgDrawType1Packet;

#define GAME_BG_DRAW_TYPE1_SCRATCH ((GameBgDrawType1Packet *)0x70000000)

extern void sceVif1PkCnt(XglPacket *packet, int count);

extern int sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);

/* Background draw type 0 draws nothing; Game calls it directly. */

/*
 * The do-nothing hook GameBgDrawType1Entry and GameBgDrawType2Entry install
 * at GameLoopState+0x30; GameDrawSync calls it with no arguments.
 */

static void clear(void)
{
    sceVif1PkRef(xglPacketGetCurrent(), TestEnv_0_00367AA0, 6, 0, 0, 0);
}

void GameBgDrawType0(void)
{
}

static void nullfunc(void)
{
}

void GameBgDrawType1(void)
{
    GameBgDrawType1Packet *scratch;
    XglPacket *packet;
    int i;
    int j;
    int k;
    GameBgDrawType1Vertex *vertex;
    GameBgDrawType1Parameters *param = &GameBgDrawType1Param;

    switch (param->initialized) {
    case 0:
        for (i = 0; i < 256; i++) {
            param->particles[i].x = 0;
            param->particles[i].y = 0;
        }
        param->initialized = 1;
        /* fallthrough */
    case 1:
        for (j = 0; j < param->particle_count; j++) {
            param->particles[j].x += param->particles[j].velocity_x;
            param->particles[j].y += param->particles[j].velocity_y;
            if ((u16)(param->particles[j].x + 0xC000) > 0x8000 || param->particles[j].y < 0x4000 || param->particles[j].y > 0xC000) {
                do {
                    param->particles[j].x = param->spawn_x;
                    param->particles[j].y = param->spawn_y;
                    param->particles[j].velocity_x = (xglSRand() & 0xFF) - 128;
                    param->particles[j].velocity_y = (xglSRand() & 0xFF) - 128;
                } while (param->particles[j].velocity_x == 0 && param->particles[j].velocity_y == 0);
            }
        }
        clear();
        packet = xglPacketGetCurrent();
        scratch = GAME_BG_DRAW_TYPE1_SCRATCH;
        scratch->header.header_pad_00[0] = 0;
        scratch->header.header_pad_00[1] = 0;
        scratch->header.header_pad_00[2] = 0;
        scratch->header.gif_tag = 0x51000004 + param->particle_count * 4;
        scratch->header.vif_command = 0x8001;
        scratch->header.packet_header_14 = 0x20000000;
        scratch->header.packet_header_18 = 0xEE;
        scratch->header.header_pad_1c = 0;
        scratch->header.register_pair_20 = 0x53001;
        scratch->header.register_pair_28 = 0x47;
        scratch->header.register_pair_30 = 0x44;
        scratch->header.register_pair_38 = 0x42;
        scratch->header.vu_memory_address = 0x8000 + param->particle_count;
        scratch->header.gs_mode = 0x4044C000;
        scratch->header.gs_registers = 0x5151;
        scratch->header.header_pad_4c = 0;
        vertex = scratch->vertices;
        for (k = 0; k < param->particle_count; k++) {
            vertex->start_st[0] = 0;
            vertex->start_st[1] = 0;
            vertex->start_st[2] = 0;
            vertex->start_st[3] = 0;
            vertex->x = param->particles[k].x;
            vertex->y = param->particles[k].y;
            vertex->start_zw[0] = 0;
            vertex->start_zw[1] = 0;
            vertex->red = 255;
            vertex->green = 255;
            vertex->blue = 255;
            vertex->alpha = 128;
            vertex->end_zw[0] = 0;
            vertex->end_zw[1] = 0;
            vertex->end_x = param->particles[k].x + param->particles[k].velocity_x;
            vertex->end_y = param->particles[k].y + param->particles[k].velocity_y;
            vertex++;
        }
        sceVif1PkCnt(packet, 0);
        sceVif1PkAddDataN(packet, scratch, param->particle_count * 16 + 20);
        packet = 0;
        break;
    }
}

void GameBgDrawType1Entry(void)
{
    GameBgDrawType1Param.particle_count = -16;
    GameBgDrawType1Param.entry_clear[1] = 0;
    GameBgDrawType1Param.spawn_x = -32768;
    GameBgDrawType1Param.entry_clear[0] = 0;
    GameLoopState.background_draw = GameBgDrawType1;
    GameLoopState.background_sync = nullfunc;
    GameBgDrawType1Param.spawn_y = -32768;
    GameBgDrawType1Param.initialized = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/game_bg_draw_type", GameBgDrawType2);

INCLUDE_ASM("asm/main/nonmatchings/game_bg_draw_type", GameBgDrawType2Entry);
