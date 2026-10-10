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
    int x;
    int y;
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

typedef unsigned int ActShadowQuadword __attribute__((mode(TI)));

/* DMA TTE carries DIRECT; its payload is one GIFtag and eight packed registers. */

/* The final direct packet has one packed A+D register write. */

/* The render-state halfword read by the shadow command builder is at +0x20. */

/* Each source point contributes three words to the rear-shadow packet. */

/* Only the packet words and four 16-byte output points written here are modeled. */

/* Status bytes and 16-byte source records immediately precede the sent packet. */

#include "main/xgl_2.h"

#include "main/xgl_studio.h"

typedef struct {
    u64 dma_tag;
    u32 vif_nop;
    u32 vif_direct;
    GifTag gif_tag;
    GifAdCommand set_shadow_alpha;
    GifAdCommand set_render_state;
    GifAdCommand unused_command;
} DropShadowHeadPacket;

typedef struct {
    u16 kind_mask;
    u16 kind_value;
    u8 *contacts;
} DropShadowDefineEntry;

typedef struct {
    DropShadowSourcePoint point;
    u32 valid;
} DropShadowSourceVertex;

typedef struct {
    u64 dma_tag;
    u32 vif_nop;
    u32 packet_tag_0c;
    u32 test_environment_control;
    u32 packet_header_14;
    u32 packet_header_18;
    u8 unmodeled_1c[4];
    u32 packet_header_20;
    u8 unmodeled_24[4];
    u32 test_environment_register;
    u8 unmodeled_2c[4];
    u32 render_state_register;
    u32 packet_header_34;
    u32 shadow_context_register;
    u32 unmodeled_3c;
    u32 packet_state[3];
    u32 draw_mode;
    DropShadowPacketPoint points[4];
} ActDropShadowPacket;

typedef struct {
    Matrix4 elements;
} ActShadowMatrix;

typedef struct {
    u8 unmodeled_00[0x9a0];
    DropShadowSourceVertex source_vertices[128];
    u8 back_shadow_status[16];
    DropShadowBackEntry back_shadow_entries[16];
} ActDropShadowWorkspace;

extern void xglMatrixMul(Matrix4 destination, Matrix4 left, Matrix4 right);

extern void xglRotTransPersN(void *destination, const Matrix4 matrix,
                             const void *vertices, int count, int camera_index);

extern float I2F(int value);

typedef struct {
    u8 quad_indices[14][4];
} DrawDropCircleIndexTable;

typedef union {
    ActShadowQuadword quadword;
    struct {
        u32 x;
        u32 y;
        u32 z;
        u32 w;
    } value;
} ActZeldaShadowPoint;

typedef union {
    ActShadowQuadword quadword;
    struct {
        u32 red;
        u32 green;
        u32 blue;
        u32 alpha;
    } value;
} ActZeldaShadowColor;

typedef struct {
    ActZeldaShadowColor color;
    ActZeldaShadowPoint point;
} ActZeldaShadowVertex;

typedef struct {
    u64 command_words[10];
    ActZeldaShadowVertex vertices[6];
    u64 trailer[2];
} ActZeldaShadowPacket;

typedef struct {
    /* Contact records index this matrix array through element 72. */
    ActShadowMatrix matrices[73];
} ActShadowMatrixSet;

typedef struct {
    u8 unmodeled_000[0x14];
    float base_height;
    u8 unmodeled_018[0x88];
    float direction[3];
    u8 unmodeled_0ac[0x430];
    float height_override;
    u8 unmodeled_4e0[0x344];
    ActShadowMatrixSet *shadow_matrices;
} ActZeldaShadowActor;

typedef struct {
    Vector4 translation;
    ActShadowMatrix matrix;
    ActShadowMatrix transform;
    Vector4 vertices[5];
    ActZeldaShadowPoint projected[5];
    float height;
    float angle;
    float radius;
    u32 camera_index;
    ActZeldaShadowPacket packet;
} ActShadowScratchpad;

static void DrawZeldaShadowSub(ActShadowScratchpad *scratchpad,
                              ActShadowMatrix *matrix);

extern void xglMatrixStackUnit(void);

extern void xglMatrixStackTrans(const float translation[4]);

extern void xglMatrixStackRotY(float angle);

extern void xglMatrixStackMulVector(Vector4 *destination,
                                    const Vector4 *source);

typedef struct {
    u32 flags;
    u8 unmodeled_04[0xc];
    float position[4];
    u8 unmodeled_20[0x34];
    float yaw;
    u8 unmodeled_58[0x28];
    u8 slot;
    u8 unmodeled_81[5];
    u16 kind;
    u8 unmodeled_88[8];
    u8 shadow_type;
    u8 shadow_alpha;
    u8 unmodeled_92[0xe];
    Vector4 light_direction;
    short previous_frame;
    u8 unmodeled_b2[0x42a];
    float height_override;
    u8 unmodeled_4e0[0x210];
    u32 motion_flags;
    float motion_frame;
    u8 unmodeled_6f8[0xc];
    u16 motion;
    u8 unmodeled_706[0x11e];
    ActShadowMatrixSet *shadow_matrices;
    u8 unmodeled_828[0x1c0];
    float shadow_radius;
} ActShadowActor;

typedef struct {
    u32 flags;
    ActShadowActor *controlled_actor;
    u8 unmodeled_08[0x2a028];
} ActFootstepGameState;

extern ActFootstepGameState GameLoopState;

typedef struct {
    u8 unmodeled_00[0x100a4];
    u16 language;
} ActFootstepSaveData;

typedef struct {
    u8 unmodeled_00[0x37a2];
    signed char footstep_enabled;
    u8 unmodeled_37a3[0x10d];
} ActFootstepEnemyWork;

extern ActFootstepEnemyWork enepc[16];

typedef struct {
    u8 unmodeled_00[8];
    u16 flags;
    u8 unmodeled_0a[0xe];
    void *header;
    u8 unmodeled_1c[4];
    u64 material;
    u8 unmodeled_28[0x18];
} ActFootstepCollision;

extern u8 *D_00478CCC[];

extern float D_004D85C8;

extern int F2I(float value);

extern void UnduParamInit(void *collision);

extern void UnduCheck(const float *position, void *unused, void *collision);

extern void *UnduDataGetHeader(int map_index, int unit_index);

extern int RES_GetEnemySeBank(int kind);

extern void xglSoundEffectNormalID(int sound, int flags);

typedef struct {
    u32 color[4];
    DropShadowPacketPoint point;
} CircleShadowPoint;

typedef struct {
    u64 dma_tag;
    u64 vif_command;
    u64 gif_control;
    u64 gif_registers;
    CircleShadowPoint points[8];
} CircleShadowPacket;

typedef struct {
    float temporary[4];
    float transform[4][4];
    float model[4][4];
    Vector4 vertices[16];
    DropShadowPacketPoint projected[16];
    int camera_index;
    u8 unmodeled_294[0xc];
    CircleShadowPacket packet;
} CircleShadowScratchpad;

typedef struct {
    float s;
    float t;
    u8 unmodeled_08[8];
    DropShadowPacketPoint point;
} DropShadowTexturedPoint;

typedef struct {
    u8 unmodeled_00[0x0c];
    u32 vif_direct;
    GifTag gif_tag;
    u32 primitive;
    u8 unmodeled_24[0x10];
    u32 draw_state;
    u8 unmodeled_38[8];
    u32 color[4];
    DropShadowTexturedPoint points[4];
} DropShadowFrontPacket;

typedef struct {
    u8 unmodeled_00[0x11a0];
    u8 status[16];
    DropShadowBackEntry entries[16];
    u8 unmodeled_12b0[0xb];
    u8 alpha;
    u8 unmodeled_12bc[4];
    DropShadowFrontPacket packet;
} ActDropShadowFrontState;

typedef struct {
    ActShadowQuadword qwords[4];
} ActShadowMatrixStorage;

static u8 TestEnv_3_00478D80[0x30] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x51,
    0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
    0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};



static const u8 D_004D1DA0[56] = {
    0x04, 0x00, 0x01, 0x06, 0x01, 0x06, 0x07, 0x02,
    0x07, 0x02, 0x03, 0x05, 0x0c, 0x09, 0x08, 0x0f,
    0x08, 0x0f, 0x0e, 0x0b, 0x0e, 0x0b, 0x0a, 0x0d,
    0x00, 0x04, 0x08, 0x0c, 0x04, 0x01, 0x0c, 0x09,
    0x01, 0x07, 0x09, 0x0f, 0x07, 0x03, 0x0f, 0x0b,
    0x03, 0x05, 0x0b, 0x0d, 0x05, 0x02, 0x0d, 0x0a,
    0x02, 0x06, 0x0a, 0x0e, 0x06, 0x00, 0x0e, 0x08,
};

static u8 *DrawDropShadowSub(ActShadowActor *actor,
                             ActDropShadowWorkspace *work, u8 *contacts);

typedef struct {
    u8 unmodeled_00[0x20];
    ActShadowQuadword direction;
    u8 unmodeled_30[0xc0];
} ActShadowLight;

typedef struct {
    u32 flags;
    u8 unmodeled_04[0x82];
    short kind;
    u8 unmodeled_88[8];
    u8 shadow_type;
    u8 unmodeled_91[0xf];
    ActShadowQuadword direction;
    u8 unmodeled_b0[0x460];
    ActShadowLight local_light;
    u8 unmodeled_600[0x224];
    ActShadowMatrixSet *shadow_matrices;
} ActDrawShadowActor;

static void DrawCircleShadow(void *actor);

static void DrawDropCircle(ActShadowActor *actor);

static void DrawDropShadow(ActShadowActor *actor);

extern ActShadowLight *xglStudioGetLight2(void);

/* DMA TTE carries DIRECT; its payload is one GIFtag and eight packed registers. */

/* The final direct packet has one packed A+D register write. */

/* The render-state halfword read by the shadow command builder is at +0x20. */

/* Each source point contributes three words to the rear-shadow packet. */

/* Only the packet words and four 16-byte output points written here are modeled. */

/* Status bytes and 16-byte source records immediately precede the sent packet. */

#define DROP_SHADOW_TEXTURE_U_ORIGIN 0x6ff8

#define DROP_SHADOW_TEXTURE_V_ORIGIN 0x71f7

#define DROP_SHADOW_TEXTURE_COORDINATE_SCALE (1.0f / 8192.0f)

static void Footstep(ActShadowActor *actor)
{
    ActFootstepCollision collision;
    float position[3];
    short previous = actor->previous_frame;
    unsigned int kind_index = actor->kind - 1;
    int sound;
    u16 motion;
    u8 *contacts;
    int frame;
    unsigned int material;
    int bank;
    int before_first;
    int before_second;
    u8 contact_frame;

    if (kind_index < 6 &&
        (actor == GameLoopState.controlled_actor ||
         enepc[actor->slot].footstep_enabled)) {
        motion = actor->motion;
        actor->previous_frame = 99;
        if ((motion == 1 || motion == 3) && !(actor->motion_flags & 2)) {
            sound = 0;
            contacts = D_00478CCC[((ActFootstepSaveData *)SaveData)->language];
            if (motion == 3) contacts += 2;
            frame = F2I(actor->motion_frame / D_004D85C8);
            contact_frame = contacts[0];
            before_first = previous < contact_frame;
            if ((before_first != 0 && frame >= contact_frame) ||
                (frame < previous && before_first != 0)) {
                sound = 0x20001;
                if (actor->motion == 3) sound = 0x20003;
            }
            contact_frame = contacts[1];
            before_second = previous < contact_frame;
            if ((before_second != 0 && frame >= contact_frame) ||
                (frame < previous && before_second != 0)) {
                sound = 0x20002;
                if (actor->motion == 3) sound = 0x20004;
            }
            actor->previous_frame = frame;
            if (sound != 0) {
                if (actor == GameLoopState.controlled_actor) {
                    UnduParamInit(&collision);
                    collision.header = UnduDataGetHeader(0x303, 0x8000);
                    collision.flags = 0x810;
                    position[0] = actor->position[0];
                    position[1] = actor->position[1];
                    position[2] = actor->position[2];
                    UnduCheck(position, 0, &collision);
                    material = (u32)(collision.material & 0xff00) - 0x2300;
                    if (material < 0x301) sound += material >> 6;
                } else {
                    bank = RES_GetEnemySeBank((short)actor->kind);
                    if (bank != -1) sound = bank + (sound & 0xffff);
                    else sound = 0;
                }
                if (sound != 0) xglSoundEffectNormalID(sound, 0);
            }
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawCircleShadow);

static void DrawZeldaShadowSub(ActShadowScratchpad *scratchpad,
                              ActShadowMatrix *matrix)
{
    float alpha;
    int i;
    ActZeldaShadowPoint *point;

    xglMatrixMul(scratchpad->transform.elements, scratchpad->matrix.elements,
                 matrix->elements);
    scratchpad->translation.x = scratchpad->transform.elements[3][0];
    scratchpad->translation.y = scratchpad->transform.elements[3][1];
    scratchpad->translation.z = scratchpad->transform.elements[3][2];
    scratchpad->translation.w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&scratchpad->translation.x);
    xglMatrixStackRotY(scratchpad->angle);

    scratchpad->translation.x = 0.0f;
    scratchpad->translation.y = 0.0f;
    scratchpad->translation.z = 0.0f;
    xglMatrixStackMulVector(&scratchpad->vertices[0], &scratchpad->translation);
    scratchpad->translation.x = -0.1f;
    xglMatrixStackMulVector(&scratchpad->vertices[1], &scratchpad->translation);
    scratchpad->translation.x = 0.1f;
    xglMatrixStackMulVector(&scratchpad->vertices[3], &scratchpad->translation);
    scratchpad->translation.x = 0.0f;
    scratchpad->translation.z = -0.15f;
    xglMatrixStackMulVector(&scratchpad->vertices[2], &scratchpad->translation);
    scratchpad->translation.z = scratchpad->radius;
    xglMatrixStackMulVector(&scratchpad->vertices[4], &scratchpad->translation);

    xglRotTransPersN(scratchpad->projected, 0, scratchpad->vertices, 5,
                     scratchpad->camera_index);
    for (point = scratchpad->projected, i = 0; i < 5; i++, point++) {
        if (point->value.x & 0xffff0000U) {
            return;
        }
        if (point->value.y & 0xffff0000U) {
            return;
        }
        if ((int)point->value.z < 0) {
            return;
        }
        point->value.w = 0;
    }

    scratchpad->packet.command_words[1] = 0x5100001100000000ULL;
    scratchpad->packet.command_words[2] = 0x0026c00000008001ULL;
    scratchpad->packet.command_words[3] = 0xe515151515151eeeULL;
    scratchpad->packet.command_words[4] = 0x0000000131000000ULL;
    scratchpad->packet.command_words[0] = 0;
    scratchpad->packet.command_words[5] = 0x4e;
    scratchpad->packet.command_words[6] = 0x53001;
    scratchpad->packet.command_words[7] = 0x47;
    scratchpad->packet.command_words[8] = 0x44;
    scratchpad->packet.command_words[9] = 0x42;
    alpha = (0.05f -
             ((matrix->elements[3][1] - scratchpad->height) -
              0.13f)) * 20.0f;
    if (alpha < 0.0f) {
        alpha = 0.0f;
    }
    if (alpha > 1.0f) {
        alpha = 1.0f;
    }

    scratchpad->packet.vertices[0].color.value.red = 0;
    scratchpad->packet.vertices[0].color.value.green = 0;
    scratchpad->packet.vertices[0].color.value.blue = 0;
    scratchpad->packet.vertices[0].color.value.alpha = F2I(alpha * 96.0f);
    scratchpad->packet.vertices[1].color.value.red = 0;
    scratchpad->packet.vertices[1].color.value.green = 0;
    scratchpad->packet.vertices[1].color.value.blue = 0;
    scratchpad->packet.vertices[1].color.value.alpha = 0;
    /* Vertex-copy block, kept as the original inline asm: vertex 0/1 points
     * from projected[0..1], then vertex 1's colour quadword (in $2) copied to
     * vertices 2..5 and the remaining points (in $3) from projected[1..4].
     * Plain 128-bit C copies emit the same lq/sq but cannot reproduce the
     * original $2/$3 allocation around the packet trailer. */
    __asm__ __volatile__(
        "lq $2, 0(%1)\n\t"
        "lq $3, 16(%1)\n\t"
        "sq $2, 16(%0)\n\t"
        "sq $3, 48(%0)\n\t"
        "lq $2, 32(%0)\n\t"
        "sq $2, 160(%0)\n\t"
        "sq $3, 176(%0)\n\t"
        "lq $3, 32(%1)\n\t"
        "sq $2, 64(%0)\n\t"
        "sq $3, 80(%0)\n\t"
        "lq $3, 48(%1)\n\t"
        "sq $2, 96(%0)\n\t"
        "sq $3, 112(%0)\n\t"
        "lq $3, 64(%1)\n\t"
        "sq $2, 128(%0)\n\t"
        "sq $3, 144(%0)\n\t"
        :
        : "r"(&scratchpad->packet.vertices[0].color),
          "r"(scratchpad->projected)
        : "$2", "$3", "memory");
    scratchpad->packet.trailer[0] = 0x31000000;
    scratchpad->packet.trailer[1] = 0x4e;
    nmlModelDirectSend(1, (u8 *)&scratchpad->packet, 0x12);
}

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawZeldaShadow);

static int DrawDropShadowSubChk(ActDropShadowWorkspace *shadow, u8 *quad_indices,
                               int source_index, int quad_count)
{
    int processed = 0;
    int status_sum = 0;
    u8 *status = shadow->back_shadow_status;
    DropShadowBackEntry *entry = shadow->back_shadow_entries;
    DropShadowSourceVertex *vertices;

    if (quad_count > 0) {
        vertices = &shadow->source_vertices[source_index];
        do {
            DropShadowSourceVertex *point0 = &vertices[quad_indices[0]];
            DropShadowSourceVertex *point1 = &vertices[quad_indices[1]];
            DropShadowSourceVertex *point2 = &vertices[quad_indices[2]];
            DropShadowSourceVertex *point3 = &vertices[quad_indices[3]];
            int x0;
            int x1;
            int x2;
            int y0;
            int y1;
            int y2;
            u32 orientation_word;

            if (point0->valid == 0 || point1->valid == 0 ||
                point2->valid == 0 || point3->valid == 0) {
                return 0;
            }
            x0 = (int)point0->point.x;
            x2 = (int)point2->point.x;
            y0 = (int)point0->point.y;
            y1 = (int)point1->point.y;
            x1 = (int)point1->point.x;
            y2 = (int)point2->point.y;
            entry->points[3] = &point3->point;
            entry->points[0] = &point0->point;
            entry->points[1] = &point1->point;
            entry->points[2] = &point2->point;
            entry++;

            orientation_word = (u32)x2 * (u32)y0 + (u32)x0 * (u32)y1 +
                               (u32)x1 * (u32)y2 - (u32)x2 * (u32)y1 -
                               (u32)x0 * (u32)y2 - (u32)x1 * (u32)y0;
            if ((float)(int)orientation_word >= 0.0f) {
                *status = 1;
            } else {
                *status = 2;
            }
            status_sum += *status;
            status++;
            quad_indices += 4;
            processed++;
        } while (processed < quad_count);
    }
    return status_sum;
}

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

static void DrawDropShadowSubFront(ActDropShadowFrontState *shadow, int count)
{
    float coordinate_scale = DROP_SHADOW_TEXTURE_COORDINATE_SCALE;
    DropShadowTexturedPoint *points = shadow->packet.points;
    DropShadowBackEntry *entry = shadow->entries;
    u8 *status;
    int remaining;

    shadow->packet.vif_direct = 0x5100000c;
    {
    u64 primitive = ((u64)shadow->alpha << 4) | 0x70009;
    u32 alpha = shadow->alpha;
    shadow->packet.gif_tag.control_high = 0xb02a4000;
    shadow->packet.gif_tag.registers_low = 0x252521ee;
    shadow->packet.gif_tag.registers_high = 0x525;
    shadow->packet.primitive = primitive;
    shadow->packet.color[3] = alpha;
    }
    shadow->packet.draw_state = 0;
    shadow->packet.color[0] = 0;
    shadow->packet.color[1] = 0;
    shadow->packet.color[2] = 0;
    points[0].point.w = 0;
    points[1].point.w = 0;
    points[2].point.w = 0;
    points[3].point.w = 0;
    if (count > 0) {
        remaining = count;
        status = shadow->status;
        do {
            if (*status == 2) {
                points[0].s = I2F(entry->points[0]->x - DROP_SHADOW_TEXTURE_U_ORIGIN) * coordinate_scale;
                points[0].t = I2F(entry->points[0]->y - DROP_SHADOW_TEXTURE_V_ORIGIN) * coordinate_scale;
                points[0].point.x = entry->points[0]->x;
                points[0].point.y = entry->points[0]->y;
                points[0].point.z = entry->points[0]->z;
                points[1].s = I2F(entry->points[1]->x - DROP_SHADOW_TEXTURE_U_ORIGIN) * coordinate_scale;
                points[1].t = I2F(entry->points[1]->y - DROP_SHADOW_TEXTURE_V_ORIGIN) * coordinate_scale;
                points[1].point.x = entry->points[1]->x;
                points[1].point.y = entry->points[1]->y;
                points[1].point.z = entry->points[1]->z;
                points[2].s = I2F(entry->points[2]->x - DROP_SHADOW_TEXTURE_U_ORIGIN) * coordinate_scale;
                points[2].t = I2F(entry->points[2]->y - DROP_SHADOW_TEXTURE_V_ORIGIN) * coordinate_scale;
                points[2].point.x = entry->points[2]->x;
                points[2].point.y = entry->points[2]->y;
                points[2].point.z = entry->points[2]->z;
                points[3].s = I2F(entry->points[3]->x - DROP_SHADOW_TEXTURE_U_ORIGIN) * coordinate_scale;
                points[3].t = I2F(entry->points[3]->y - DROP_SHADOW_TEXTURE_V_ORIGIN) * coordinate_scale;
                points[3].point.x = entry->points[3]->x;
                points[3].point.y = entry->points[3]->y;
                points[3].point.z = entry->points[3]->z;
                nmlModelDirectSend(1, (u8 *)&shadow->packet, 13);
            }
            status++;
            remaining--;
            entry++;
        } while (remaining != 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropShadowSub);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropShadow);

INCLUDE_ASM("asm/main/nonmatchings/act_3", DrawDropCircle);

void ACT_DrawShadowBegin(void)
{
    u16 shadow_state = sRender.shadow_state;

    Head_9.set_color_control.data.value = shadow_state | 0xFFFFFF00080000ULL;
    Head_9.set_shadow_context.data.value = shadow_state;
    /* Set the command-enable bit after initializing the state bits. */
    Head_9.set_shadow_context.data.value |= 0x80000;
    nmlModelDirectSend(1, (u8 *)&Head_9, 10);
}

void ACT_DrawShadowEnd(void)
{
    nmlModelDirectSend(1, (u8 *)&Tail_10, 3);
}

void ACT_DrawShadow(ActDrawShadowActor *actor)
{
    ActShadowLight *light;

    if (actor->flags & 0x1000) return;
    if (actor->shadow_matrices == 0) return;

    switch (actor->shadow_type) {
    case 0:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
        break;
    default:
    case 1:
        DrawCircleShadow(actor);
        break;
    case 2:
        DrawZeldaShadow((ActZeldaShadowActor *)actor);
        break;
    case 3:
        DrawDropCircle((ActShadowActor *)actor);
        break;
    case 4:
        DrawDropShadow((ActShadowActor *)actor);
        break;
    }
    if (actor->flags & 0x8000) {
        light = &actor->local_light;
    } else {
        light = xglStudioGetLight2();
    }
    /* sceVu0CopyVector shape: one quadword through a fixed scratch register. */
    __asm__ __volatile__("lq $2, 0(%1)\n"
                         "sq $2, 0(%0)\n"
                         :
                         : "r"(&actor->direction), "r"(&light->direction)
                         : "$2", "memory");
    if ((actor->kind & 0xf000) == 0 && (actor->flags & 0x100) == 0) {
        Footstep((ActShadowActor *)actor);
    }
}

void ACT_DrawShadowInit(void)
{
}
