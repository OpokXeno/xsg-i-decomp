#include "common.h"
#include "shared.h"
#include "main/control_entry.h"
#include "main/game_id_light_set.h"
#include "main/game_bg_draw_type.h"
#include "main/xgl_2.h"
#include "main/xgl_render.h"

#define GAME_BG_DRAW_TYPE1_SCRATCH ((GameBgDrawType1Packet *)SCRATCHPAD_BASE)

#define GAME_BG_PARTICLE_MAX 256
#define GAME_BG_GIF_EOP_NREG_1_REG_AD 0x51000004
#define GAME_BG_VIF_DIRECT_COMMAND 0x8001
#define GAME_BG_CLEAR_COLOR 0x80000000
/* Scrolling star texture: one 32-texel row per frame, 64 rows. */
#define GAME_BG_STAR_TEXTURE_WIDTH 32
#define GAME_BG_STAR_TEXTURE_HEIGHT 64
#define GAME_BG_STAR_TEXTURE_PIXELS (GAME_BG_STAR_TEXTURE_WIDTH * GAME_BG_STAR_TEXTURE_HEIGHT)
#define GAME_BG_STAR_COLOR_MASK 0xBFBFBF
#define GAME_BG_STAR_COLOR_BASE 0x80404040

/* Hardware-reserved bits are not C padding. */
typedef struct GameBgSetupEnvironment {
    unsigned int vif_nops[3];
    struct {
        unsigned int quadwords : 16;
        unsigned int count : 8;
        unsigned int command : 7;
        unsigned int interrupt : 1;
    } directhl;
    struct {
        unsigned long loop_count : 15;
        unsigned long end_of_packet : 1;
        unsigned long reserved_10 : 30;
        unsigned long primitive_enable : 1;
        unsigned long primitive : 11;
        unsigned long format : 2;
        unsigned long register_count : 4;
    } gif_tag;
    unsigned long gif_registers;
    struct {
        unsigned long source_color : 2;
        unsigned long subtract_color : 2;
        unsigned long alpha_source : 2;
        unsigned long add_color : 2;
        unsigned long reserved_08 : 24;
        unsigned long fixed_alpha : 8;
        unsigned long reserved_28 : 24;
        unsigned long address;
    } alpha;
    struct {
        unsigned long alpha_test_enable : 1;
        unsigned long alpha_test_method : 3;
        unsigned long alpha_reference : 8;
        unsigned long alpha_fail : 2;
        unsigned long destination_alpha_test_enable : 1;
        unsigned long destination_alpha_test_method : 1;
        unsigned long depth_test_enable : 1;
        unsigned long depth_test_method : 2;
        unsigned long reserved_13 : 45;
        unsigned long address;
    } test;
    struct {
        unsigned long texture_base : 14;
        unsigned long buffer_width : 6;
        unsigned long pixel_format : 6;
        unsigned long width_log2 : 4;
        unsigned long height_log2 : 4;
        unsigned long color_components : 1;
        unsigned long texture_function : 2;
        unsigned long clut_base : 14;
        unsigned long clut_pixel_format : 4;
        unsigned long clut_storage : 1;
        unsigned long clut_offset : 5;
        unsigned long clut_load : 3;
        unsigned long address;
    } tex0;
    struct {
        unsigned long lod_method : 1;
        unsigned long reserved_01 : 1;
        unsigned long maximum_mipmap_level : 3;
        unsigned long magnification_filter : 1;
        unsigned long minification_filter : 3;
        unsigned long automatic_mipmap_base : 1;
        unsigned long reserved_0a : 9;
        unsigned long lod_scale : 2;
        unsigned long reserved_15 : 11;
        unsigned long lod_bias : 12;
        unsigned long reserved_2c : 20;
        unsigned long address;
    } tex1;
    struct {
        unsigned long wrap_s : 2;
        unsigned long wrap_t : 2;
        unsigned long minimum_u : 10;
        unsigned long maximum_u : 10;
        unsigned long minimum_v : 10;
        unsigned long maximum_v : 10;
        unsigned long reserved_2c : 20;
        unsigned long address;
    } clamp;
    struct {
        unsigned long source_base : 14;
        unsigned long reserved_0e : 2;
        unsigned long source_width : 6;
        unsigned long reserved_16 : 2;
        unsigned long source_format : 6;
        unsigned long reserved_1e : 2;
        unsigned long destination_base : 14;
        unsigned long reserved_2e : 2;
        unsigned long destination_width : 6;
        unsigned long reserved_36 : 2;
        unsigned long destination_format : 6;
        unsigned long reserved_3e : 2;
        unsigned long address;
    } bitbltbuf;
    struct {
        unsigned long source_x : 11;
        unsigned long reserved_0b : 5;
        unsigned long source_y : 11;
        unsigned long reserved_1b : 5;
        unsigned long destination_x : 11;
        unsigned long reserved_2b : 5;
        unsigned long destination_y : 11;
        unsigned long pixel_order : 2;
        unsigned long reserved_3d : 3;
        unsigned long address;
    } trxpos;
    struct {
        unsigned long width : 12;
        unsigned long reserved_0c : 20;
        unsigned long height : 12;
        unsigned long reserved_2c : 20;
        unsigned long address;
    } trxreg;
    struct {
        unsigned long direction : 2;
        unsigned long reserved_02 : 62;
        unsigned long address;
    } trxdir;
} GameBgSetupEnvironment;

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
    GameBgDrawType1Particle particles[GAME_BG_PARTICLE_MAX];
} GameBgDrawType1Parameters;

typedef struct GameLoopBackgroundCallbacks {
    u8 unmodeled_00[0x2c];
    void (*background_draw)(void);
    void (*background_sync)(void);
    u8 unmodeled_34[0x2a030 - 0x34];
} GameLoopBackgroundCallbacks;

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

typedef struct GameBgScreenPosition {
    int x;
    int y;
    int z;
} GameBgScreenPosition;

typedef struct GameBgProjectedPoint {
    GameBgScreenPosition position;
    float q;
} GameBgProjectedPoint;

typedef struct GameBgGsPosition {
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int control;
} GameBgGsPosition;

typedef struct GameBgTextureCoordinates {
    float s;
    float t;
    float q;
    unsigned int unmodeled_0c;
} GameBgTextureCoordinates;

typedef struct GameBgGsColorComponent {
    unsigned int value : 8;
    unsigned int reserved : 24;
} GameBgGsColorComponent;

typedef struct GameBgBandVertex {
    GameBgTextureCoordinates texture;
    GameBgGsColorComponent red;
    GameBgGsColorComponent green;
    GameBgGsColorComponent blue;
    GameBgGsColorComponent alpha;
    GameBgGsPosition position;
} GameBgBandVertex;

typedef struct GameBgGifTagWord {
    unsigned long loop_count : 15;
    unsigned long end_of_packet : 1;
    unsigned long reserved_10 : 16;
    unsigned long reserved_20 : 14;
    unsigned long primitive_enable : 1;
    unsigned long primitive : 11;
    unsigned long format : 2;
    unsigned long register_count : 4;
} GameBgGifTagWord;

typedef struct GameBgGifRegisterList {
    unsigned long front_start_texture : 4;
    unsigned long front_start_color : 4;
    unsigned long front_start_position : 4;
    unsigned long back_start_texture : 4;
    unsigned long back_start_color : 4;
    unsigned long back_start_position : 4;
    unsigned long front_end_texture : 4;
    unsigned long front_end_color : 4;
    unsigned long front_end_position : 4;
    unsigned long back_end_texture : 4;
    unsigned long back_end_color : 4;
    unsigned long back_end_position : 4;
    unsigned long unused_registers : 16;
} GameBgGifRegisterList;

typedef struct GameBgGifTag {
    GameBgGifTagWord tag;
    GameBgGifRegisterList registers;
} GameBgGifTag;

typedef struct GameBgBandPacketHeader {
    unsigned long long vif_nops;
    unsigned long vif_command;
    GameBgGifTag gif;
} GameBgBandPacketHeader;

typedef struct GameBgBandPacket {
    GameBgBandPacketHeader header;
    GameBgBandVertex vertices[4];
} GameBgBandPacket;

typedef struct GameBgBandScratch {
    Vector4 points[4];
    unsigned char unmodeled_40[64];
    GameBgProjectedPoint projected[4];
    unsigned char unmodeled_c0[64];
    GameBgBandPacketHeader header;
    GameBgBandVertex vertices[4];
} GameBgBandScratch;

typedef struct GameBgBand {
    float angular_velocity;
    float texture_velocity;
    float angle;
    float texture_phase;
    /* Interleaved x, y coordinates for the circular point ring. */
    float coordinates[128];
} GameBgBand;

typedef struct GameBgDepth {
    float z;
    unsigned int unmodeled_04;
} GameBgDepth;

typedef struct GameBgBandTexture GameBgBandTexture;

typedef struct GameBgBandParameters {
    unsigned char initialized;
    unsigned char unmodeled_01[2];
    unsigned char point_count;
    float radius;
    float angular_step;
    float scale;
    GameBgBand bands[4];
    GameBgDepth depths[7];
    unsigned char unmodeled_888[72];
    GameBgBandTexture *texture_packet;
} GameBgBandParameters;

struct GameBgBandTexture {
    unsigned int header[8];
    unsigned int pixels[GAME_BG_STAR_TEXTURE_PIXELS];
};

struct StageBackground;

extern GameLoopBackgroundCallbacks GameLoopState;

void GameBgDrawType1(void);
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);

extern void xglRotTransPersN(GameBgProjectedPoint *destination, Matrix4 *matrix,
                           const Vector4 *source, int count, int mode);

static unsigned char TestEnv_0_00367AA0[0x60] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x51, 0x01, 0x80, 0x00, 0x00, 0x00, 0x40, 0x03, 0x40, 0x1E, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x6F, 0x00, 0x00, 0xF7, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x8F, 0x00, 0x00, 0xF7, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00
};

/* Original TestEnv.1, main VA 0x00367b00, 11 quadwords. */
static GameBgSetupEnvironment TestEnv_1_00367B00[1] = {
    {
        .directhl = { .quadwords = 10, .command = 0x51 },
        .gif_tag = { .loop_count = 9, .end_of_packet = 1,
                     .format = 0, .register_count = 1 },
        .gif_registers = 0x0e,
        .alpha = { .source_color = 0, .subtract_color = 2,
                   .alpha_source = 0, .add_color = 1, .address = 0x42 },
        .test = { .alpha_test_enable = 1, .alpha_fail = 1,
                  .depth_test_enable = 1, .depth_test_method = 3,
                  .address = 0x47 },
        .tex0 = { .texture_base = 0x3800, .buffer_width = 8,
                  .pixel_format = 0, .width_log2 = 5, .height_log2 = 6,
                  .color_components = 1, .texture_function = 0,
                  .clut_load = 1, .address = 0x06 },
        .tex1 = { .magnification_filter = 1, .minification_filter = 1,
                  .address = 0x14 },
        .clamp = { .wrap_s = 0, .wrap_t = 0, .address = 0x08 },
        .bitbltbuf = { .destination_base = 0x3800,
                      .destination_width = 8, .destination_format = 0,
                      .address = 0x50 },
        .trxpos = { .source_x = 0, .source_y = 0, .destination_x = 0,
                    .destination_y = 0, .pixel_order = 0, .address = 0x51 },
        .trxreg = { .width = 32, .height = 64, .address = 0x52 },
        .trxdir = { .direction = 0, .address = 0x53 }
    }
};

GameBgDrawType1Parameters GameBgDrawType1Param = {0};

GameBgBandParameters GameBgDrawType2Param = {0};

IdLightSet GameIdLight[16] = {0};

static void clear(void)
{
    sceVif1PkRef(xglPacketGetCurrent(), TestEnv_0_00367AA0, 6, 0, 0, 0);
}

/* Background draw type 0 draws nothing; Game calls it directly. */
void GameBgDrawType0(void)
{
}

/*
 * The do-nothing hook GameBgDrawType1Entry and GameBgDrawType2Entry install
 * at GameLoopState+0x30; GameDrawSync calls it with no arguments.
 */
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
        for (i = 0; i < GAME_BG_PARTICLE_MAX; i++) {
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
        scratch->header.gif_tag = GAME_BG_GIF_EOP_NREG_1_REG_AD + param->particle_count * 4;
        scratch->header.vif_command = GAME_BG_VIF_DIRECT_COMMAND;
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

void GameBgDrawType2(void)
{
    float point_component;
    unsigned long vif_command;
    XglPacket *packet = xglPacketGetCurrent();
    GameBgBandParameters *param = &GameBgDrawType2Param;
    GameBgBandTexture *texture;
    GameBgBandScratch *scratch;
    unsigned int *row;
    unsigned int *source_pixel;
    unsigned int *destination_pixel;
    int band_index;
    int draw_band_index;
    int depth_index;
    int point_index;
    int animation_point_index;
    int texture_row;
    int texture_pixel_index;
    int clip_index;
    int j;
    int star;
    int initial_star;
    unsigned int color;
    unsigned int clear_color;
    float phase;
    float band_angle;
    float full_turn;
    float half_turn;
    float negative_full_turn;
    float angle;
    float s_start;
    float s_end;
    float t_front;
    float t_back;
    float point_x;
    float point_y;
    float front_z;
    int clipped;

    switch (param->initialized) {
    case 0:
        xglRenderClearColor(GAME_BG_CLEAR_COLOR);
        param->texture_packet = (GameBgBandTexture *)(((unsigned int)WorkEnd + 15) & ~15);
        WorkEnd = (unsigned char *)(param->texture_packet + 1);
        param->texture_packet->header[3] = 0x51000201;
        param->texture_packet->header[4] = 0x8200;
        param->texture_packet->header[5] = 0x08000000;
        param->texture_packet->header[0] = 0;
        param->texture_packet->header[1] = 0;
        param->texture_packet->header[2] = 0;
        param->texture_packet->header[6] = 0;
        param->texture_packet->header[7] = 0;
        row = param->texture_packet->pixels;
        for (texture_row = 0; texture_row < GAME_BG_STAR_TEXTURE_HEIGHT; texture_row++) {
            for (j = 31; j >= 0; j--) {
                row[j] = 0;
            }
            initial_star = xglSRand();
            color = xglLRand();
            initial_star &= 31;
            row[initial_star] = (color & GAME_BG_STAR_COLOR_MASK) + GAME_BG_STAR_COLOR_BASE;
            row += 32;
        }
        param->initialized = 1;
        /* fallthrough */
    case 1:
        band_index = 0;
        do {
            band_angle = param->bands[band_index].angle;
            phase = param->bands[band_index].texture_phase +
                    param->bands[band_index].texture_velocity;
            while (phase > 1.0f) {
                phase -= 1.0f;
            }
            while (phase < -1.0f) {
                phase += 1.0f;
            }
            param->bands[band_index].texture_phase = phase;
            animation_point_index = 0;
            if (param->point_count != 0) {
                full_turn = 6.2831855f;
                half_turn = 3.1415927f;
                negative_full_turn = -6.2831855f;
                do {
                    angle = band_angle + (I2F(animation_point_index) * 2.0f * half_turn) /
                            I2F(param->point_count);
                    while (angle <= negative_full_turn) {
                        angle += full_turn;
                    }
                    while (full_turn <= angle) {
                        angle -= full_turn;
                    }
                    param->bands[band_index].coordinates[animation_point_index * 2] =
                        xglCos(angle) * param->radius;
                    param->bands[band_index].coordinates[animation_point_index * 2 + 1] =
                        xglSin(angle) * param->radius;
                    animation_point_index++;
                } while (animation_point_index < param->point_count);
            }
            band_index++;
        } while (band_index < 4);
        clear();
        texture = param->texture_packet;
        source_pixel = &texture->pixels[32];
        destination_pixel = texture->pixels;
        for (texture_pixel_index = GAME_BG_STAR_TEXTURE_PIXELS - GAME_BG_STAR_TEXTURE_WIDTH - 1; texture_pixel_index >= 0; texture_pixel_index--) {
            *destination_pixel++ = *source_pixel++;
        }
        clear_color = GAME_BG_CLEAR_COLOR;
        for (texture_pixel_index = 31; texture_pixel_index >= 0; texture_pixel_index--) {
            destination_pixel[texture_pixel_index] = clear_color;
        }
        star = xglSRand();
        color = xglLRand();
        star &= 31;
        destination_pixel[star] = (color & GAME_BG_STAR_COLOR_MASK) + GAME_BG_STAR_COLOR_BASE;
        sceVif1PkRef(packet, TestEnv_1_00367B00, 11, 0, 0, 0);
        sceVif1PkCnt(packet, 0);
        sceVif1PkAddDataN(packet, param->texture_packet, 2056);
        scratch = (GameBgBandScratch *)SCRATCHPAD_BASE;
        {
            vif_command = 0x5100000d00000000UL;
            {
                GameBgGifTagWord gif_tag = {1, 1, 0, 0, 1, 92, 0, 12};
                GameBgGifRegisterList gif_registers = {
                    2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 0
                };
                scratch->header.gif.registers = gif_registers;
                scratch->header.vif_command = vif_command;
                scratch->header.gif.tag = gif_tag;
                scratch->header.vif_nops = 0;
            }
        }
        for (draw_band_index = 0; draw_band_index < 4; draw_band_index++) {
            for (depth_index = 0; depth_index < 6; depth_index++) {
                t_front = param->depths[depth_index].z * 0.00025f + 0.5f +
                          param->bands[draw_band_index].texture_phase;
                t_back = param->depths[depth_index + 1].z * 0.00025f + 0.5f +
                         param->bands[draw_band_index].texture_phase;
                for (point_index = 0; point_index < param->point_count; point_index++) {
                    s_start = I2F(point_index) * 7.0f / I2F(param->point_count);
                    s_end = I2F(point_index + 1) * 7.0f / I2F(param->point_count);
                    if (point_index == 0) {
                        point_x = param->bands[draw_band_index].coordinates[(param->point_count - 1) * 2];
                        scratch->points[1].x = point_x;
                    } else {
                        point_x = param->bands[draw_band_index].coordinates[(point_index - 1) * 2];
                        scratch->points[1].x = point_x;
                    }
                    point_component = scratch->points[1].x;
                    scratch->points[0].x = point_component;
                    point_x = param->bands[draw_band_index].coordinates[point_index * 2];
                    scratch->points[3].x = point_x;
                    scratch->points[2].x = point_x;
                    if (point_index == 0) {
                        scratch->points[1].y = param->bands[draw_band_index].coordinates[(param->point_count - 1) * 2 + 1];
                    } else {
                        scratch->points[1].y = param->bands[draw_band_index].coordinates[(point_index - 1) * 2 + 1];
                    }
                    scratch->points[0].y = scratch->points[1].y;
                    point_y = param->bands[draw_band_index].coordinates[point_index * 2 + 1];
                    scratch->points[3].y = point_y;
                    scratch->points[2].y = point_y;
                    front_z = -param->depths[depth_index].z;
                    scratch->points[2].z = front_z;
                    scratch->points[0].z = front_z;
                    point_component = -param->depths[depth_index + 1].z;
                    scratch->points[3].z = point_component;
                    scratch->points[1].z = point_component;
                    scratch->points[0].w = scratch->points[1].w =
                        scratch->points[2].w = scratch->points[3].w = 1.0f;
                    xglRotTransPersN(scratch->projected, 0, scratch->points, 4, 0);
                    clipped = 0;
                    clip_index = 0;
                    do {
                        if (clip_index >= 4) {
                            break;
                        }
                        if ((scratch->projected[clip_index].position.x & 0xFFFF0000) != 0 ||
                            (scratch->projected[clip_index].position.y & 0xFFFF0000) != 0 ||
                            scratch->projected[clip_index].position.z < 0) {
                            clipped = 1;
                            break;
                        }
                        clip_index++;
                    } while (!clipped);
                    if (!clipped) {
                        float projected_q;
                        float front_start_s;
                        float front_start_t;
                        float back_start_s;
                        float back_start_t;
                        float front_end_s;
                        float back_end_s;
                        float back_end_t;
                        unsigned int front_position_x;
                        unsigned int front_position_y;
                        unsigned int front_position_z;
                        int back_position_x;
                        projected_q = scratch->projected[0].q;
                        scratch->vertices[0].texture.q = projected_q;
                        front_start_t = t_front * projected_q;
                        front_start_s = s_start * projected_q;
                        projected_q = scratch->projected[1].q;
                        scratch->vertices[1].texture.q = projected_q;
                        back_start_s = s_start * projected_q;
                        back_start_t = t_back * projected_q;
                        projected_q = scratch->projected[2].q;
                        front_end_s = s_end * projected_q;
                        front_position_x = scratch->projected[0].position.x;
                        front_position_y = scratch->projected[0].position.y;
                        front_position_z = scratch->projected[0].position.z;
                        scratch->vertices[0].position.x = front_position_x;
                        scratch->vertices[0].position.y = front_position_y;
                        back_position_x = scratch->projected[1].position.x;
                        scratch->vertices[0].texture.s = front_start_s;
                        scratch->vertices[0].texture.t = front_start_t;
                        scratch->vertices[0].position.z = front_position_z;
                        scratch->vertices[1].texture.s = back_start_s;
                        scratch->vertices[1].texture.t = back_start_t;
                        scratch->vertices[1].position.x = back_position_x;
                        scratch->vertices[1].position.y = scratch->projected[1].position.y;
                        scratch->vertices[1].position.z = scratch->projected[1].position.z;
                        scratch->vertices[2].texture.t = t_front * projected_q;
                        scratch->vertices[2].texture.s = front_end_s;
                        scratch->vertices[0].red = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[0].green = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[0].blue = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[0].alpha = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[0].position.control = 0;
                        scratch->vertices[1].red = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[1].green = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[1].blue = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[1].alpha = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[1].position.control = 0;
                        scratch->vertices[2].texture.q = projected_q;
                        projected_q = scratch->projected[3].q;
                        back_end_s = s_end * projected_q;
                        back_end_t = t_back * projected_q;
                        scratch->vertices[2].position.x = scratch->projected[2].position.x;
                        scratch->vertices[2].position.y = scratch->projected[2].position.y;
                        scratch->vertices[2].position.z = scratch->projected[2].position.z;
                        scratch->vertices[3].texture.s = back_end_s;
                        scratch->vertices[3].texture.t = back_end_t;
                        scratch->vertices[3].texture.q = projected_q;
                        scratch->vertices[3].position.x = scratch->projected[3].position.x;
                        scratch->vertices[3].position.y = scratch->projected[3].position.y;
                        scratch->vertices[3].position.z = scratch->projected[3].position.z;
                        scratch->vertices[2].red = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[2].green = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[2].blue = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[2].alpha = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[2].position.control = 0;
                        scratch->vertices[3].red = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[3].green = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[3].blue = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[3].alpha = (GameBgGsColorComponent){128, 0};
                        scratch->vertices[3].position.control = 0;
                        sceVif1PkAddDataN(packet, &scratch->header,
                                         sizeof(GameBgBandPacket) / sizeof(unsigned int));
                    }
                }
            }
        }
        return;
    }
}

void GameBgDrawType2Entry(int parameter, struct StageBackground *background)
{
    GameBgBandParameters *param = &GameBgDrawType2Param;
    float radius;
    const float texture_velocity = 0.01f;

    param->point_count = 16;
    param->angular_step = 0.3926991f;
    radius = 100.0f;
    param->radius = radius;
    param->bands[0].angular_velocity = 0.01f;
    param->scale = 1.0f;
    param->initialized = 0;
    param->bands[0].angle = xglFRand();
    param->bands[1].angular_velocity = 0.017f;
    param->bands[1].angle = xglFRand();
    param->bands[2].angular_velocity = -0.011f;
    param->bands[2].angle = xglFRand();
    param->bands[3].angular_velocity = -0.018f;
    param->bands[3].angle = xglFRand();
    param->bands[3].texture_velocity = texture_velocity;
    param->bands[1].texture_velocity = 0.001f;
    param->bands[0].texture_velocity = 0.0f;
    param->bands[2].texture_velocity = -0.01f;
    GameLoopState.background_draw = GameBgDrawType2;
    param->depths[0].z = -2000.0f;
    param->depths[1].z = -500.0f;
    param->depths[2].z = -100.0f;
    param->depths[3].z = 0.0f;
    param->depths[4].z = radius;
    param->depths[5].z = 500.0f;
    param->depths[6].z = 2000.0f;
    GameLoopState.background_sync = nullfunc;
}
