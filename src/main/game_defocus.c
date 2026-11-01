#include "common.h"
#include "shared.h"

#define NULL ((void *)0)
#define DEFOCUS_SCRATCH ((DefocusSolidPacket *)0x70000000)
#define DEFOCUS_SOLID_GIF_TAG 0x51000004
#define DEFOCUS_SOLID_VIF_COMMAND 0x8001
#define DEFOCUS_SOLID_GS_MODE 0x30234000
#define DEFOCUS_SOLID_GS_REGISTERS 0x551
#define DEFOCUS_SCREEN_X_BIAS 28664
#define DEFOCUS_SCREEN_Y_BIAS 29175
#define DEFOCUS_HEAD_WORDS 16
#define DEFOCUS_SOLID_WORDS 20
#define DEFOCUS_VU_MEMORY_WORD ((u32 *)0x8001)
#define DEFOCUS_TYPE05_PACKET ((DefocusTexturedPacket *)0x70000000)
#define DEFOCUS_WAVE_HEADER ((DefocusWaveHeader *)0x70000000)
#define DEFOCUS_WAVE_TABLE ((DefocusWaveTable *)0x70000000)
#define DEFOCUS_WAVE_SPRITE ((DefocusWaveSprite *)0x70000080)
#define DEFOCUS_RADIAL_VERTICES ((DefocusRadialVertex *)0x70000000)
#define DEFOCUS_STAR_HEADER ((DefocusStarHeader *)0x70000318)
#define DEFOCUS_STAR_STRIP ((DefocusStarVertex *)0x70000338)
#define DEFOCUS_STAR_WORD(index) (((int *)0x70000318)[index])
#define DEFOCUS_RING_TABLE ((DefocusRingTable *)0x70000000)

typedef struct {
    u8 unmodeled_00[0x14];
    u16 tex0_state_14;
    u8 unmodeled_16[0x0a];
    u16 tex0_state_20;
    u16 tex0_state_22;
    u8 unmodeled_24[0x10];
    void (*callback)(XglPacket *packet);
} DefocusRenderState;

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

/* GS registers inside the display-environment block at DispEnv. */
typedef struct {
    unsigned int en1 : 1;
    unsigned int en2 : 1;
    unsigned int crtmd : 3;
    unsigned int mmod : 1;
    unsigned int amod : 1;
    unsigned int slbg : 1;
    unsigned int alp : 8;
    unsigned int unmodeled_16 : 16;
    unsigned int unmodeled_32 : 32;
} DefocusPmode;

typedef struct {
    u64 fbp : 9;
    u64 unmodeled_09 : 55;
} DefocusDispfb;

typedef struct {
    unsigned int dx : 12;
    unsigned int dy : 11;
    unsigned int magh : 4;
    unsigned int magv : 2;
    unsigned int unmodeled_29 : 3;
    unsigned int dw : 12;
    unsigned int dh : 11;
    unsigned int unmodeled_55 : 9;
} DefocusDisplay;

typedef struct {
    unsigned int sx : 12;
    unsigned int sy : 11;
    unsigned int smph : 4;
    unsigned int smpv : 2;
    unsigned int unmodeled_29 : 3;
    unsigned int ww : 12;
    unsigned int wh : 11;
    unsigned int unmodeled_55 : 9;
} DefocusExtdata;

typedef struct {
    union {
        u64 flags;
        DefocusPmode pmode;
    };
    u8 unmodeled_08[8];
    u8 unmodeled_10[8];
    DefocusDisplay display1;
    u8 unmodeled_20[8];
    DefocusDispfb dispfb2;
    DefocusDisplay display2;
    u64 extbuf;
    DefocusExtdata extdata;
    u64 extwrite;
} DefocusDispEnv;

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

typedef struct {
    u32 unmodeled_00[20];
    u64 texture;
    u64 unmodeled_58;
    u64 texture_dimensions;
    u64 unmodeled_68;
} DefocusType05Header;

typedef struct {
    u8 unmodeled_00[0x20];
    u64 texture;
    u8 unmodeled_28[0x48];
} DefocusCopyTemplate;

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
    u32 sprite_tag_low;
    u32 sprite_tag_high;
    u32 sprite_registers;
    u32 sprite_padding;
} DefocusWaveHeader;

typedef struct {
    int x[17];
    int y[17];
} DefocusWaveTable;

typedef struct {
    u32 first_u;
    u32 first_v;
    u32 unmodeled_08[2];
    int left;
    int top;
    u32 color;
    u32 flags;
    u32 second_u;
    u32 second_v;
    u32 unmodeled_28[2];
    int right;
    int bottom;
    u32 second_color;
    u32 second_flags;
} DefocusWaveSprite;

typedef struct {
    int u;
    int v;
    u32 unmodeled_08[2];
    int x;
    int y;
    u32 color;
    u32 flags;
} DefocusRadialVertex;

typedef struct {
    u32 header[3];
    u32 gif_tag;
    u32 vif_command;
    u32 gs_mode;
    u32 gs_registers;
    u32 header_padding;
} DefocusStarHeader;

typedef struct {
    int x;
    int y;
} DefocusRingPoint;

typedef struct {
    DefocusRingPoint inner[33];
    DefocusRingPoint middle[33];
    DefocusRingPoint outer[33];
} DefocusRingTable;

typedef struct {
    int red;
    int green;
    int blue;
    int alpha;
    int x;
    int y;
    int z;
    int flags;
} DefocusStarVertex;

extern DefocusRenderState sRender;
extern DefocusLayer GameDefocusParam[16];
extern DefocusDispEnv DispEnv;
extern int ScanLineInterpolate;
extern void (*func_14[11])(DefocusLayer *layer, XglPacket *packet);

static void DefocusMain(XglPacket *packet);
static void DefocusMainType09Final(DefocusLayer *layer);
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);
extern int printf(const char *format, ...);
extern float I2F(int value);
extern int F2I(float value);
extern float xglSin(float angle);
extern float xglCos(float angle);
extern float xglFRand(void);
extern void DefocusMainType01(DefocusLayer *layer, XglPacket *packet);
extern void DefocusMainType06(DefocusLayer *layer, XglPacket *packet);
extern void DefocusMainType02(DefocusLayer *layer, XglPacket *packet);
extern void DefocusMainType07(DefocusLayer *layer, XglPacket *packet);
extern void DefocusMainType11(DefocusLayer *layer, XglPacket *packet);
extern void DefocusMainType09(DefocusLayer *layer, XglPacket *packet);
extern void DefocusMainType03(DefocusLayer *layer, XglPacket *packet);
extern void DefocusMainType05(DefocusLayer *layer, XglPacket *packet);
extern void DefocusMainType08(DefocusLayer *layer, XglPacket *packet);
extern void DefocusMainType10(DefocusLayer *layer, XglPacket *packet);

static DefocusSolidPacketHeader Head_6_00367240;

static DefocusCopyPacket TestPrim_1;

static DefocusTintPacket TestPrim_2;

static u64 TestPrim_3[6];

static DefocusTexturedHeader Head_0;

static DefocusTexturedHeader Head_4_00367170;

static DefocusType05Header Head_5;

static DefocusCopyTemplate Copy_10;

static DefocusTexturedHeader Head_11;

static u64 Head_13[10];

static u64 Tail_15[8];

static DefocusTexturedHeader Head_12;

static DefocusTexturedHeader Head_7;

static DefocusSolidPacketHeader Head_8;

static u8 rate_9[37];

static void (*func_14[11])(DefocusLayer *layer, XglPacket *packet);

static DefocusTexturedHeader Head_0 = { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 81, 1, 128, 0, 0, 0, 0, 0, 64, 238, 238, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0 }, .texture = 0ULL, .unmodeled_58 = { 6, 0, 0, 0, 0, 0, 0, 0 } };

static DefocusCopyPacket TestPrim_1 = { .unmodeled_00 = { 0ULL, 5836665177201704960ULL, 15027174950396002305ULL, 4192666307718894ULL, 5117050880ULL, 78ULL, 462849ULL, 71ULL, 0ULL, 20ULL, 1ULL, 74ULL }, .framebuffer = 0ULL, .frame_register = 76ULL, .texture = 0ULL, .unmodeled_78 = { 6ULL, 549755814016ULL, 128ULL, 0ULL, 0ULL, 125305670889464ULL, 4294967295ULL, 30786325585920ULL, 0ULL, 156091996475384ULL, 4294967295ULL, 0ULL, 63ULL, 0ULL, 74ULL } };

static DefocusTintPacket TestPrim_2 = { .unmodeled_00 = { 0ULL, 5836665151431901184ULL, 8071365325922271233ULL, 249909742ULL, 458752ULL, 71ULL }, .source_frame = 0ULL, .frame_register = 76ULL, .color = { 0ULL, 0ULL }, .top_left = 125305670889464ULL, .first_color = 49152, .first_flags = 0U, .bottom_right = 156091996475384ULL, .second_color = 49152, .second_flags = 0U, .unmodeled_70 = { 0ULL, 63ULL }, .destination_frame = 0ULL, .destination_register = 76ULL };

static u64 TestPrim_3[6] = { 0x0000000000000000ULL, 0x5100000200000000ULL, 0x1000000000008001ULL, 0x000000000000000eULL, 0x0000000000000000ULL, 0x000000000000004cULL };

static DefocusTexturedHeader Head_4_00367170 = { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 81, 1, 128, 0, 0, 0, 0, 0, 64, 238, 238, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 13, 0, 7, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 64, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0 }, .texture = 0ULL, .unmodeled_58 = { 6, 0, 0, 0, 0, 0, 0, 0 } };

static DefocusType05Header Head_5 = { .unmodeled_00 = { 0U, 0U, 0U, 1358954502U, 32769U, 1342177280U, 978670U, 0U, 0U, 0U, 20U, 0U, 462849U, 0U, 71U, 0U, 68U, 0U, 66U, 0U }, .texture = 0ULL, .unmodeled_58 = 6ULL, .texture_dimensions = 0ULL, .unmodeled_68 = 8ULL };

static DefocusSolidPacketHeader Head_6_00367240 = {
    {0}, 0x51000003, 0x8001U, 0x20000000,
    0x000000EE, 0, 0x00071001, 0, 0x47, 0, 0x84, 0, 0x42, 0
};

static DefocusTexturedHeader Head_7 = { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 81, 1, 128, 0, 0, 0, 0, 0, 64, 238, 238, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0 }, .texture = 0ULL, .unmodeled_58 = { 6, 0, 0, 0, 0, 0, 0, 0 } };

static DefocusSolidPacketHeader Head_8 = { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, .packet_tag_0c = 1358954499U, .vu_memory_address_10 = 32769U, .packet_header_14 = 536870912U, .packet_header_18 = 238U, .unmodeled_1c = 0U, .packet_header_20 = 462849U, .unmodeled_24 = 0U, .packet_header_28 = 71U, .unmodeled_2c = 0U, .packet_header_30 = 68U, .unmodeled_34 = 0U, .packet_header_38 = 66U, .unmodeled_3c = 0U };

static u8 rate_9[37] = { 17, 9, 1, 0, 0, 0, 0, 8, 16, 24, 32, 48, 48, 48, 32, 24, 16, 8, 0, 0, 0, 0, 1, 9, 17, 25, 33, 49, 49, 49, 33, 25, 17, 9, 1, 0, 0 };

static DefocusCopyTemplate Copy_10 = { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 81, 1, 128, 0, 0, 0, 0, 0, 80, 238, 238, 14, 0, 0, 0, 0, 0 }, .texture = 0ULL, .unmodeled_28 = { 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 192, 1, 0, 0, 82, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0 } };

static DefocusTexturedHeader Head_11 = { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 81, 1, 128, 0, 0, 0, 0, 0, 64, 238, 238, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 13, 0, 7, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0 }, .texture = 0ULL, .unmodeled_58 = { 6, 0, 0, 0, 0, 0, 0, 0 } };

static DefocusTexturedHeader Head_12 = { .unmodeled_00 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 81, 1, 128, 0, 0, 0, 0, 0, 64, 238, 238, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0 }, .texture = 0ULL, .unmodeled_58 = { 6, 0, 0, 0, 0, 0, 0, 0 } };

static u64 Head_13[10] = { 0x0000000000000000ULL, 0x5100000400000000ULL, 0x3000000000008001ULL, 0x0000000000000eeeULL, 0x0000000000000000ULL, 0x000000000000003fULL, 0x0000000131000000ULL, 0x000000000000004eULL, 0x000006fc007fc005ULL, 0x0000000000000008ULL };

static void (*func_14[11])(DefocusLayer *layer, XglPacket *packet) = { DefocusMainType01, DefocusMainType02, DefocusMainType03, DefocusMainType01, DefocusMainType05, DefocusMainType06, DefocusMainType07, DefocusMainType08, DefocusMainType09, DefocusMainType10, DefocusMainType11 };

static u64 Tail_15[8] = { 0x0000000000000000ULL, 0x5100000300000000ULL, 0x2000000000008001ULL, 0x00000000000000eeULL, 0x0000000000000000ULL, 0x000000000000003fULL, 0x0000000031000000ULL, 0x000000000000004eULL };

/* Runtime writes the layer entries through DefocusSet; the original table is
 * sixteen 0x44-byte records whose initial bytes are all zero. */
DefocusLayer GameDefocusParam[16] = { 0 };

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

static void DefocusMainType01(DefocusLayer *layer, XglPacket *packet)
{
    DefocusTexturedPacket *draw = (DefocusTexturedPacket *)0x70000000;
    u8 *color;
    int right;
    int bottom;
    u32 vertex_color;
    u32 repeat = 0;

    Head_0.texture = GetTex0(layer->parameters[0], 0);
    draw->gif_tag = 0x51000006;
    draw->vif_command = 0x8001;
    draw->gs_mode = 0x50ab4000;
    draw->gs_registers = 0x53531;
    draw->header[0] = 0;
    draw->header[1] = 0;
    draw->header[2] = 0;
    draw->header_padding = 0;
    color = (u8 *)&layer->parameters[3];
    draw->red = color[0];
    draw->green = color[1];
    draw->blue = color[2];
    draw->alpha = color[3];
    draw->second_u = 8192;
    draw->first_u = 0;
    draw->left = (layer->parameters[4] << 4) + 28664;
    right = layer->parameters[4] + layer->parameters[6];
    draw->second_v = 7168;
    draw->first_v = 0;
    draw->right = (right << 4) + 36856;
    draw->top = (layer->parameters[5] << 4) + 29175;
    bottom = layer->parameters[5] + layer->parameters[7];
    draw->bottom = (bottom << 4) + 36343;
    vertex_color = layer->parameters[2];
    draw->second_color = vertex_color;
    draw->color = vertex_color;
    draw->second_flags = 0;
    draw->flags = 0;
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, &Head_0, 24);
    if (layer->parameters[1] != 0) {
        do {
            repeat++;
            sceVif1PkAddDataN(packet, draw, 28);
        } while (repeat < (u32)layer->parameters[1]);
    }
}

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

static void DefocusMainType06(DefocusLayer *layer, XglPacket *packet)
{
    DefocusSolidPacket *draw;
    unsigned char *color;
    u32 left;
    u32 top;
    u32 draw_color;

    draw = DEFOCUS_SCRATCH;
    color = (unsigned char *)&layer->parameters[1];
    draw->gif_tag = DEFOCUS_SOLID_GIF_TAG;
    draw->vif_command = DEFOCUS_SOLID_VIF_COMMAND;
    draw->gs_registers = DEFOCUS_SOLID_GS_MODE;
    draw->register_count = DEFOCUS_SOLID_GS_REGISTERS;
    draw->header[0] = 0;
    draw->header[1] = 0;
    draw->header[2] = 0;
    draw->header_padding = 0;
    draw->red = color[0];
    draw->green = color[1];
    draw->blue = color[2];
    draw->alpha = color[3];
    left = ((u32)layer->parameters[2] << 4) + DEFOCUS_SCREEN_X_BIAS;
    draw->left = left;
    draw->right = left + ((u32)layer->parameters[4] << 4);
    top = ((u32)layer->parameters[3] << 4) + DEFOCUS_SCREEN_Y_BIAS;
    draw->top = top;
    draw->bottom = top + ((u32)layer->parameters[5] << 4);
    draw_color = layer->parameters[0];
    draw->color = draw_color;
    draw->second_color = draw_color;
    draw->flags = 0;
    draw->second_flags = 0;
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, &Head_6_00367240, DEFOCUS_HEAD_WORDS);
    sceVif1PkAddDataN(packet, DEFOCUS_SCRATCH, DEFOCUS_SOLID_WORDS);
}

static void DefocusMainType07(DefocusLayer *layer, XglPacket *packet)
{
    const u8 *color;
    float angle;
    float next_angle;
    float sin_angle;
    float cos_angle;
    float sin_next;
    float cos_next;
    float uv_radius_x;
    float uv_radius_y;
    float screen_radius_x;
    float screen_radius_y;
    int ring;
    int step;
    int offset_u;
    int offset_v;
    int offset_x;
    int offset_y;
    u32 flags;
    u32 skip_flag = 0x8000;
    DefocusWaveHeader *header = DEFOCUS_WAVE_HEADER;

    Head_7.texture = GetTex0(2, 0);
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, &Head_7, 24);
    header->gif_tag = 0x51000443;
    header->gs_mode = 0x10aa4000;
    header->vif_command = 1;
    header->gs_registers = 1;
    header->header[0] = 0;
    header->header[1] = 0;
    header->header[2] = 0;
    header->header_padding = 0;
    color = (const u8 *)&layer->parameters[1];
    header->red = color[0];
    header->green = color[1];
    header->blue = color[2];
    header->alpha = color[3];
    header->sprite_tag_low = 0x8220;
    header->sprite_tag_high = 0x20000000;
    header->sprite_registers = 0x53;
    header->sprite_padding = 0;
    sceVif1PkAddDataN(packet, DEFOCUS_WAVE_HEADER, 16);
    for (ring = 0; ring < 32; ring++) {
        angle = I2F(ring) * 0.03125f * 2.0f * 3.1415927f;
        next_angle = I2F(ring + 1) * 0.03125f * 2.0f * 3.1415927f;
        sin_angle = xglSin(angle);
        cos_angle = xglCos(angle);
        sin_next = xglSin(next_angle);
        cos_next = xglCos(next_angle);
        for (step = 0; step < 8; step++) {
            flags = 0;
            if (step == 0) {
                flags = skip_flag;
            }
            screen_radius_x = I2F(layer->parameters[4] * (8 - step)) * 0.125f;
            screen_radius_y = I2F(layer->parameters[5] * (8 - step)) * 0.125f;
            uv_radius_x = I2F(layer->parameters[4]) -
                          I2F(layer->parameters[4] * step * layer->parameters[6]) *
                              0.00048828125f;
            uv_radius_y = I2F(layer->parameters[5]) -
                          I2F(layer->parameters[5] * step * layer->parameters[6]) *
                              0.00048828125f;
            offset_u = F2I(cos_angle * uv_radius_x);
            offset_v = F2I(sin_angle * uv_radius_y);
            offset_x = F2I(cos_angle * screen_radius_x);
            offset_y = F2I(sin_angle * screen_radius_y);
            ((int *)0x70000000)[0] = (layer->parameters[2] + offset_u) << 4;
            ((int *)0x70000000)[1] = (layer->parameters[3] + offset_v) << 4;
            ((int *)0x70000000)[4] =
                ((layer->parameters[2] + offset_x) << 4) + 28664;
            ((int *)0x70000000)[5] =
                ((layer->parameters[3] + offset_y) << 4) + 29175;
            ((int *)0x70000000)[6] = layer->parameters[0];
            ((int *)0x70000000)[7] = flags;
            offset_u = F2I(cos_next * uv_radius_x);
            offset_v = F2I(sin_next * uv_radius_y);
            offset_x = F2I(cos_next * screen_radius_x);
            offset_y = F2I(sin_next * screen_radius_y);
            ((int *)0x70000000)[8] = (layer->parameters[2] + offset_u) << 4;
            ((int *)0x70000000)[9] = (layer->parameters[3] + offset_v) << 4;
            ((int *)0x70000000)[12] =
                ((layer->parameters[2] + offset_x) << 4) + 28664;
            ((int *)0x70000000)[13] =
                ((layer->parameters[3] + offset_y) << 4) + 29175;
            ((int *)0x70000000)[14] = layer->parameters[0];
            ((int *)0x70000000)[15] = flags;
            sceVif1PkAddDataN(packet, DEFOCUS_RADIAL_VERTICES, 16);
        }
        ((int *)0x70000000)[0] = layer->parameters[2] << 4;
        ((int *)0x70000000)[1] = layer->parameters[3] << 4;
        ((int *)0x70000000)[4] = (layer->parameters[2] << 4) + 28664;
        ((int *)0x70000000)[5] = (layer->parameters[3] << 4) + 29175;
        ((int *)0x70000000)[6] = layer->parameters[0];
        ((int *)0x70000000)[7] = 0;
        sceVif1PkAddDataN(packet, DEFOCUS_RADIAL_VERTICES, 8);
    }
}

static void DefocusMainType08(DefocusLayer *layer, XglPacket *packet)
{
    DefocusStarHeader *header = DEFOCUS_STAR_HEADER;
    float base_angle;
    float spread;
    float angle;
    float sine;
    float cosine;
    u32 index = 0;
    int x;
    int y;
    int spark_depth;
    int spark_x;
    int spark_y;
    int along_x;
    int along_y;
    int across_x;
    int across_y;
    int segment;
    int depth;
    int ring;
    u32 row;
    u32 spawn_x;
    u32 spawn_y;
    u32 lifetime;
    u32 *slot;
    int *shared;
    DefocusStarVertex *strip;
    u32 phase;
    int shade0;
    int shade1;
    int shade2;
    int shade3;
    int shade4;
    int shade5;
    int level2;
    int level3;

    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, &Head_8, 16);
    header->gif_tag = 0x51000009;
    header->vif_command = 0x8001;
    header->gs_mode = 0x8026c000;
    header->gs_registers = 0x55555151;
    header->header[0] = 0;
    header->header[1] = 0;
    header->header[2] = 0;
    header->header_padding = 0;
    base_angle = I2F(layer->parameters[3]) / 180.0f * 3.1415927f;
    spread = I2F(layer->parameters[4]) / 180.0f * 3.1415927f;
    spark_depth = layer->parameters[0];
    if (layer->parameters[2] != 0) {
        do {
            index++;
            spark_x = xglSRand() & 0x1fff;
            spark_y = xglSRand() & 0x1fff;
            angle = base_angle + (xglFRand() - 1.5f) * spread;
            spark_x += 28672;
            spark_y += 29184;
            along_x = F2I(xglCos(angle) * 4.0f * 16.0f);
            along_y = F2I(xglSin(angle) * 4.0f * 16.0f);
            across_x = F2I(xglSin(angle) * 128.0f * 16.0f);
            across_y = F2I(-xglCos(angle) * 128.0f * 16.0f);
            DEFOCUS_STAR_WORD(8) = 255;
            DEFOCUS_STAR_WORD(9) = 255;
            DEFOCUS_STAR_WORD(10) = 255;
            DEFOCUS_STAR_WORD(11) = 64;
            DEFOCUS_STAR_WORD(12) = spark_x;
            DEFOCUS_STAR_WORD(13) = spark_y;
            DEFOCUS_STAR_WORD(14) = spark_depth;
            DEFOCUS_STAR_WORD(15) = 0;
            DEFOCUS_STAR_WORD(16) = 255;
            DEFOCUS_STAR_WORD(17) = 255;
            DEFOCUS_STAR_WORD(18) = 255;
            DEFOCUS_STAR_WORD(19) = 0;
            DEFOCUS_STAR_WORD(20) = spark_x + along_x;
            DEFOCUS_STAR_WORD(21) = spark_y + along_y;
            DEFOCUS_STAR_WORD(36) = spark_x + along_x;
            DEFOCUS_STAR_WORD(37) = spark_y + along_y;
            DEFOCUS_STAR_WORD(22) = spark_depth;
            DEFOCUS_STAR_WORD(38) = spark_depth;
            DEFOCUS_STAR_WORD(23) = 0;
            DEFOCUS_STAR_WORD(39) = 0;
            DEFOCUS_STAR_WORD(24) = spark_x + across_x;
            DEFOCUS_STAR_WORD(25) = spark_y + across_y;
            DEFOCUS_STAR_WORD(26) = spark_depth;
            DEFOCUS_STAR_WORD(27) = 0;
            DEFOCUS_STAR_WORD(28) = spark_x - along_x;
            DEFOCUS_STAR_WORD(29) = spark_y - along_y;
            DEFOCUS_STAR_WORD(30) = spark_depth;
            DEFOCUS_STAR_WORD(31) = 0;
            DEFOCUS_STAR_WORD(32) = spark_x - across_x;
            DEFOCUS_STAR_WORD(33) = spark_y - across_y;
            DEFOCUS_STAR_WORD(34) = spark_depth;
            DEFOCUS_STAR_WORD(35) = 0;
            sceVif1PkAddDataN(packet, DEFOCUS_STAR_HEADER, 40);
        } while (index < (u32)layer->parameters[2]);
    }
    for (ring = 0; ring <= 32; ring++) {
        sine = xglSin(I2F(ring) * 2.0f * 3.1415927f * (1.0f / 32.0f));
        cosine = xglCos(I2F(ring) * 2.0f * 3.1415927f * (1.0f / 32.0f));
        DEFOCUS_RING_TABLE->inner[ring].x = F2I(cosine * 8.0f * 16.0f);
        DEFOCUS_RING_TABLE->inner[ring].y = F2I(sine * 8.0f * 16.0f);
        DEFOCUS_RING_TABLE->middle[ring].x = F2I(cosine * 28.0f * 16.0f);
        DEFOCUS_RING_TABLE->middle[ring].y = F2I(sine * 28.0f * 16.0f);
        DEFOCUS_RING_TABLE->outer[ring].x = F2I(cosine * 48.0f * 16.0f);
        DEFOCUS_RING_TABLE->outer[ring].y = F2I(sine * 48.0f * 16.0f);
    }
    DEFOCUS_STAR_WORD(3) = 0x51000181;
    DEFOCUS_STAR_WORD(4) = 0x80c0;
    DEFOCUS_STAR_WORD(5) = 0x20264000;
    DEFOCUS_STAR_WORD(6) = 0x51;
    DEFOCUS_STAR_WORD(0) = 0;
    DEFOCUS_STAR_WORD(1) = 0;
    DEFOCUS_STAR_WORD(2) = 0;
    DEFOCUS_STAR_WORD(7) = 0;
    for (row = 0; row < (u32)layer->parameters[5]; row++) {
        slot = (u32 *)&layer->parameters[8 + row];
        strip = DEFOCUS_STAR_STRIP;
        if ((*slot >> 24) == 0) {
            spawn_x = xglSRand();
            spawn_y = xglSRand();
            lifetime = xglSRand();
            *slot = ((((spawn_x & 0x1fff) + 28672) & 0xfff0) << 8) + (((spawn_y & 0x1fff) + 29184) >> 4) +
                    ((lifetime % (u32)layer->parameters[6] + 1) << 24);
        }
        *slot -= 0x1000000;
        x = (*slot >> 8) & 0xfff0;
        y = (*slot << 4) & 0xfff0;
        shared = layer->parameters;
        depth = *shared;
        phase = xglSRand() & 3;
        for (segment = 0; segment < 32; segment++) {
            shade0 = 0;
            if (!(rate_9[phase + segment] & 1)) {
                shade0 = 255;
            }
            strip[0].red = shade0;
            strip[0].green = shade0;
            strip[0].blue = shade0;
            strip[0].alpha = 0;
            strip[0].x = x + DEFOCUS_RING_TABLE->inner[segment].x;
            strip[0].y = y + DEFOCUS_RING_TABLE->inner[segment].y;
            strip[0].z = depth;
            strip[0].flags = 0x8000;
            shade1 = 0;
            if (!(rate_9[phase + segment + 1] & 1)) {
                shade1 = 255;
            }
            strip[1].alpha = 0;
            strip[1].red = shade1;
            strip[1].green = shade1;
            strip[1].blue = shade1;
            strip[1].x = x + DEFOCUS_RING_TABLE->inner[segment + 1].x;
            strip[1].y = y + DEFOCUS_RING_TABLE->inner[segment + 1].y;
            strip[1].z = depth;
            strip[1].flags = 0x8000;
            level2 = rate_9[phase + segment];
            shade2 = 0;
            if (!(level2 & 1)) {
                shade2 = 255;
            }
            strip[2].alpha = level2;
            strip[2].red = shade2;
            strip[2].green = shade2;
            strip[2].blue = shade2;
            strip[2].x = x + DEFOCUS_RING_TABLE->middle[segment].x;
            strip[2].y = y + DEFOCUS_RING_TABLE->middle[segment].y;
            strip[2].z = depth;
            strip[2].flags = 0;
            level3 = rate_9[phase + segment + 1];
            shade3 = 0;
            if (!(level3 & 1)) {
                shade3 = 255;
            }
            strip[3].red = shade3;
            strip[3].green = shade3;
            strip[3].blue = shade3;
            strip[3].alpha = level3;
            strip[3].x = x + DEFOCUS_RING_TABLE->middle[segment + 1].x;
            strip[3].y = y + DEFOCUS_RING_TABLE->middle[segment + 1].y;
            strip[3].z = depth;
            strip[3].flags = 0;
            shade4 = 0;
            if (!(rate_9[phase + segment] & 1)) {
                shade4 = 255;
            }
            strip[4].red = shade4;
            strip[4].green = shade4;
            strip[4].blue = shade4;
            strip[4].alpha = 0;
            strip[4].x = x + DEFOCUS_RING_TABLE->outer[segment].x;
            strip[4].y = y + DEFOCUS_RING_TABLE->outer[segment].y;
            strip[4].z = depth;
            strip[4].flags = 0;
            shade5 = 0;
            if (!(rate_9[phase + segment + 1] & 1)) {
                shade5 = 255;
            }
            strip[5].red = shade5;
            strip[5].green = shade5;
            strip[5].blue = shade5;
            strip[5].alpha = 0;
            strip[5].x = x + DEFOCUS_RING_TABLE->outer[segment + 1].x;
            strip[5].y = y + DEFOCUS_RING_TABLE->outer[segment + 1].y;
            strip[5].z = depth;
            strip[5].flags = 0;
            strip += 6;
        }
        sceVif1PkAddDataN(packet, DEFOCUS_STAR_HEADER, 1544);
    }
}

static void DefocusMainType09(DefocusLayer *layer, XglPacket *packet)
{
    u64 *extbuf;

    if (layer->state == 0) {
        layer->parameters[0] = ScanLineInterpolate;
        layer->parameters[1] = (u32)DispEnv.flags & 1;
        if (layer->parameters[2] == 0) {
            ScanLineInterpolate = 2;
        }
        layer->state = 1;
    } else {
        ScanLineInterpolate = 2;
        DispEnv.dispfb2.fbp = sRender.tex0_state_14;
        DispEnv.pmode.en1 = 1;
        DispEnv.pmode.alp = 255;
        DispEnv.display2.dy = DispEnv.display1.dy;
    }
    extbuf = &DispEnv.extbuf;
    *extbuf = 0x2120000 | (sRender.tex0_state_14 << 5);
    DispEnv.extdata.sx = DispEnv.display1.dx + 7;
    DispEnv.extdata.sy = DispEnv.display1.dy / 2;
    DispEnv.extdata.smph = DispEnv.display1.magh;
    DispEnv.extdata.smpv = DispEnv.display1.magv;
    DispEnv.extdata.ww = 511;
    DispEnv.extdata.wh = 223;
    DispEnv.extwrite |= 1;
}

static void DefocusMainType09Final(DefocusLayer *layer)
{
    u64 enable = (u32)layer->parameters[1];

    ScanLineInterpolate = layer->parameters[0];
    DispEnv.pmode.en1 = enable;
    DispEnv.extwrite &= ~1ULL;
}

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

static void DefocusMainType11(DefocusLayer *layer, XglPacket *packet)
{
    const u8 *color;
    float phase_x;
    float phase_y;
    int index;
    int row;
    int column;
    int *wave_x;
    int *wave_y;
    int x;
    int y;
    int next_x;
    int next_y;
    int sprite_y;
    int sprite_next_y;
    DefocusWaveHeader *header = DEFOCUS_WAVE_HEADER;

    Head_12.texture = GetTex0(2, 0);
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, &Head_12, 24);
    header->gif_tag = 0x51000383;
    header->gs_mode = 0x10ab4000;
    header->header[0] = 0;
    header->header[1] = 0;
    header->header[2] = 0;
    header->vif_command = 1;
    header->header_padding = 0;
    header->gs_registers = 1;
    color = (const u8 *)&layer->parameters[1];
    header->red = color[0];
    header->green = color[1];
    header->blue = color[2];
    header->alpha = color[3];
    header->sprite_tag_low = 0x80e0;
    header->sprite_tag_high = 0x40000000;
    header->sprite_registers = 0x5353;
    header->sprite_padding = 0;
    sceVif1PkAddDataN(packet, DEFOCUS_WAVE_HEADER, 16);
    layer->parameters[8] += layer->parameters[4];
    layer->parameters[9] += layer->parameters[7];
    phase_x = I2F(layer->parameters[8] & 0xfff) * 0.0015339808f;
    phase_y = I2F(layer->parameters[9] & 0xfff) * 0.0015339808f;
    for (index = 0; index <= 16; index++) {
        DEFOCUS_WAVE_TABLE->x[index] =
            F2I(xglSin(phase_x) * I2F(layer->parameters[2]));
        phase_x += I2F(layer->parameters[3]) * (1.0f / 4096.0f);
        DEFOCUS_WAVE_TABLE->y[index] =
            F2I(xglSin(phase_y) * I2F(layer->parameters[5]));
        phase_y += I2F(layer->parameters[6]) * (1.0f / 4096.0f);
    }
    ((int *)0x70000000)[16] = 0;
    ((int *)0x70000000)[0] = 0;
    ((int *)0x70000000)[31] = 0;
    ((int *)0x70000000)[17] = 0;
    for (row = 0, wave_y = DEFOCUS_WAVE_TABLE->y, y = 0; row < 14; row++, y = row * 512) {
        column = 0;
        sprite_y = y + 29175;
        next_y = y + 512;
        sprite_next_y = y + 29687;
        wave_x = DEFOCUS_WAVE_TABLE->x;
        for (; column < 16; column++) {
            x = column * 512;
            next_x = x + 512;
            ((int *)0x70000000)[32] = x + wave_x[0];
            ((int *)0x70000000)[33] = y + wave_y[0];
            ((int *)0x70000000)[36] = x + 28664;
            ((int *)0x70000000)[37] = sprite_y;
            ((int *)0x70000000)[38] = layer->parameters[0];
            ((int *)0x70000000)[39] = 0;
            ((int *)0x70000000)[40] = next_x + wave_x[1];
            ((int *)0x70000000)[41] = next_y + wave_y[1];
            ((int *)0x70000000)[44] = x + 29176;
            ((int *)0x70000000)[45] = sprite_next_y;
            ((int *)0x70000000)[46] = layer->parameters[0];
            ((int *)0x70000000)[47] = 0;
            sceVif1PkAddDataN(packet, DEFOCUS_WAVE_SPRITE, 16);
            wave_x++;
        }
        wave_y++;
    }
}

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
        /* JP: GameDefocusSet()の引数が異常です(%d)。\n
         * EN: Invalid argument to GameDefocusSet() (%d).\n */
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
        /* JP: GameDefocusQuickSet()の引数が異常です(%d)。\n
         * EN: Invalid argument to GameDefocusQuickSet() (%d).\n */
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
