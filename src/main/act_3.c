#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/xgl_studio.h"
#include "main/Undulate.h"
#include "main/nml_model_set.h"
#include "main/res_get_path.h"
#include "main/xgl_sound.h"
#include "quadword.h"
#include "vu0_macro.h"

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

#define DROP_SHADOW_TEXTURE_U_ORIGIN 0x6ff8

#define DROP_SHADOW_TEXTURE_V_ORIGIN 0x71f7
#define DROP_SHADOW_TEXTURE_COORDINATE_SCALE (1.0f / 8192.0f)

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
    int z;
} DropShadowSourcePoint;

typedef struct {
    DropShadowSourcePoint *points[4];
} DropShadowBackEntry;

typedef struct {
    u32 x;
    u32 y;
    int z;
    u32 w;
} DropShadowPacketPoint;

typedef unsigned int ActShadowQuadword __attribute__((mode(TI)));

/* Partial packet storage: each element is one native quadword. */
typedef union {
    u32 words[4];
    ActShadowQuadword quad;
} ActShadowPacketQuadword;

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
    float s;
    float t;
    u8 unmodeled_08[8];
    DropShadowPacketPoint point;
} DropShadowTexturedPoint;

/* One shadow quad packet: the back passes send RGBAQ + four XYZ2 (GIF
 * registers 0x055551ee), the front pass RGBAQ + four ST/XYZ2 pairs
 * (0x252521ee), so the vertex block is either point kind. */
typedef struct {
    GifCommandData dma_tag;
    u32 vif_nop;
    u32 vif_direct;
    GifTag gif_tag;
    u32 packet_header_20;
    u8 unmodeled_24[4];
    u32 test_environment_register;
    u8 unmodeled_2c[4];
    u32 render_state_register;
    u32 packet_header_34;
    u32 shadow_context_register;
    u32 unmodeled_3c;
    u32 color[4];
    union {
        DropShadowPacketPoint points[4];
        DropShadowTexturedPoint textured_points[4];
    };
} ActDropShadowPacket;

typedef struct {
    Matrix4 elements;
} ActShadowMatrix;

/* One 16-byte scratch vector: indexed as four floats while it is built, and
   passed to the Vector4 helpers and VU blocks as a whole. */
typedef union {
    float values[4];
    Vector4 vector;
} ActShadowScratchVector;

typedef struct {
    ActShadowScratchVector temporary;
    Vector4 outer_offset;
    Vector4 inner_offset;
    Vector4 axial_offset;
    u8 unmodeled_40[0x10];
    Vector4 perpendicular;
    ActShadowMatrix model;
    ActShadowMatrix flatten;
    ActShadowMatrix translated_end;
    ActShadowMatrix first_world;
    ActShadowMatrix second_world;
    Vector4 vertices[128];
    DropShadowSourceVertex source_vertices[128];
    u8 back_shadow_status[16];
    DropShadowBackEntry back_shadow_entries[16];
    u8 unmodeled_12b0[8];
    u8 camera_index;
    u8 unmodeled_12b9[2];
    u8 alpha;
    u8 unmodeled_12bc[4];
    ActDropShadowPacket packet;
} ActDropShadowWorkspace;

typedef struct {
    u8 quad_indices[14][4];
} DrawDropCircleIndexTable;

typedef union {
    ActShadowQuadword quadword;
    struct {
        u32 x;
        u32 y;
        int z;
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
    ActShadowMatrix *shadow_matrices; /* +0x824; stride 0x40 */
    u8 unmodeled_828[0x1c0];
    float shadow_radius;
} ActShadowActor;

typedef struct {
    u32 flags;
    ActShadowActor *controlled_actor;
    u8 unmodeled_08[0x2a028];
} ActFootstepGameState;

typedef struct {
    u8 unmodeled_00[0x100a4];
    u16 language;
} ActFootstepSaveData;

typedef struct {
    u8 unmodeled_00[0x37a2];
    signed char footstep_enabled;
    u8 unmodeled_37a3[0x10d];
} ActFootstepEnemyWork;

typedef struct {
    u8 unmodeled_00[8];
    u16 flags;
    u8 unmodeled_0a[0xe];
    void *header;
    u8 unmodeled_1c[4];
    u64 material;
    u8 unmodeled_28[0x18];
} ActFootstepCollision;

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

/* Owner type from src/main/xgl_2.h:10-15; include-layer transport is pending. */
/* Private partial layouts: DrawDropShadowSub accesses actor +0x14/+0x91/+0xa0/
   +0x4dc/+0x824, and aligned matrices at scratch +0x60/+0xa0/+0xe0/
   +0x120/+0x160. Their defining-owner header transport remains pending. */
typedef struct {
    u8 unmodeled_000[0x10];
    float position[4];
    u8 unmodeled_020[0x71];
    u8 shadow_alpha;
    u8 unmodeled_092[0xe];
    Vector4 light_direction;
    u8 unmodeled_0b0[0x42c];
    float height_override;
    u8 unmodeled_4e0[0x344];
    Matrix *shadow_matrices;
} ActDropShadowSubMatrixActor;

typedef struct {
    float temporary[4];
    Vector4 outer_offset;
    Vector4 inner_offset;
    Vector4 axial_offset;
    u8 unmodeled_40[0x10];
    Vector4 perpendicular;
    Matrix model;
    Matrix flatten;
    Matrix translated_end;
    Matrix first_world;
    Matrix second_world;
    Vector4 vertices[128];
    DropShadowSourceVertex source_vertices[128];
    u8 back_shadow_status[16];
    DropShadowBackEntry back_shadow_entries[16];
    u8 unmodeled_12b0[8];
    u8 camera_index;
    u8 unmodeled_12b9[2];
    u8 alpha;
    u8 unmodeled_12bc[4];
    ActDropShadowPacket packet;
} ActDropShadowSubMatrixWorkspace;

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
    ActShadowQuadword qwords[4];
} ActShadowMatrixStorage;

typedef union {
    struct {
        signed char end_width;
        signed char midpoint_width;
    } segment;
    struct {
        signed char offset;
        signed char axis;
    } terminal;
} DropShadowContactPayload;

typedef struct {
    u8 matrix_index;
    DropShadowContactPayload payload;
} DropShadowContactRecord;

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

typedef struct {
    float temporary[4];
    ActShadowMatrix transform;
    ActShadowMatrix model;
    Vector4 vertices[16];
    DropShadowPacketPoint projected[16];
    int camera_index;
    u8 unmodeled_294[0xc];
    CircleShadowPacket packet;
} CircleShadowMatrixScratchpad;


typedef struct {
    u16 kind_mask;
    u16 kind_value;
    DropShadowContactRecord *contacts;
} DropShadowContactDefinition;

typedef float ActShadowMatrixRow[4];

extern ActShadowRenderState sRender;
extern ActFootstepGameState GameLoopState;
extern ActFootstepEnemyWork enepc[16];
extern u8 *actNullPointerTable[];
extern float footstepFrameTime;
extern const float contactCoordScale;
extern DropShadowContactDefinition DropShadowDefineTable[10];
extern const DrawDropCircleIndexTable dropCircleIndexTable;
extern const float dropCircleCornerScale;
extern const float dropCircleHeightOffset;
extern float zeldaShadowMinRadius;

extern void xglMatrixMul(Matrix4 destination, Matrix4 left, Matrix4 right);

extern void xglRotTransPersN(void *destination, const Matrix4 matrix,
                             const void *vertices, int count, int camera_index);

static void DrawZeldaShadowSub(ActShadowScratchpad *scratchpad,
                              ActShadowMatrix *matrix);

extern void UnduParamInit(void *collision);
extern void UnduCheck(const float *position, void *unused, void *collision);
static DropShadowContactRecord *DrawDropShadowSub(ActShadowActor *actor,
                            ActDropShadowWorkspace *work, DropShadowContactRecord *contacts);

static void DrawCircleShadow(void *actor_pointer);
static void DrawDropCircle(ActShadowActor *actor);
static void DrawDropShadow(void *actor_pointer);
extern ActShadowLight *xglStudioGetLight2(void);
extern void xglMatrixRotY(Matrix4 destination, Matrix4 source, float angle);

extern void xglMatrixTrans(Matrix4 destination, const Matrix4 source,
                            const float translation[4]);

extern float atan2f(float x, float z);

/* Original aligned matrices at scratchpad +0x60 and +0xa0. */
static const DrawDropCircleIndexTable dropShadowIndexTable = {
    {
    0x04, 0x00, 0x01, 0x06, 0x01, 0x06, 0x07, 0x02,
    0x07, 0x02, 0x03, 0x05, 0x0c, 0x09, 0x08, 0x0f,
    0x08, 0x0f, 0x0e, 0x0b, 0x0e, 0x0b, 0x0a, 0x0d,
    0x00, 0x04, 0x08, 0x0c, 0x04, 0x01, 0x0c, 0x09,
    0x01, 0x07, 0x09, 0x0f, 0x07, 0x03, 0x0f, 0x0b,
    0x03, 0x05, 0x0b, 0x0d, 0x05, 0x02, 0x0d, 0x0a,
    0x02, 0x06, 0x0a, 0x0e, 0x06, 0x00, 0x0e, 0x08,
    }
};

const DrawDropCircleIndexTable dropCircleIndexTable = { .quad_indices = { { 4, 0, 1, 6 }, { 1, 6, 7, 2 }, { 7, 2, 3, 5 }, { 12, 9, 8, 15 }, { 8, 15, 14, 11 }, { 14, 11, 10, 13 }, { 0, 4, 8, 12 }, { 4, 1, 12, 9 }, { 1, 7, 9, 15 }, { 7, 3, 15, 11 }, { 3, 5, 11, 13 }, { 5, 2, 13, 10 }, { 2, 6, 10, 14 }, { 6, 0, 14, 8 } } };

static u8 *foot_tbl[6];

static ActShadowPacketQuadword Head_1[5];

static u32 Tail_2[12];

static u8 rgbaw_0[16];

static DropShadowHeadPacket Head_4_00478DB0;

static ActShadowEndPacket Tail_5;

static DropShadowHeadPacket Head_6_00478E30;

static u32 TestEnv_7[12];

static ActShadowEndPacket Tail_8;

/* Original DATA order; zero alignment is supplied by allocation metadata. */
static u8 foot_sion[] = { 2, 16, 1, 12 };

static u8 foot_kosmos[] = { 2, 16, 1, 12 };

static u8 foot_chaos[] = { 2, 16, 1, 12 };

static u8 foot_momo[] = { 2, 15, 1, 12 };

static u8 foot_ziggy[] = { 2, 16, 1, 12 };

static u8 foot_jr[] = { 2, 16, 1, 12 };

static u8 * foot_tbl[6] = { foot_sion, foot_kosmos, foot_chaos, foot_momo, foot_ziggy, foot_jr };

static u8 rgbaw_0[16] = { 2, 3, 1, 1, 1, 1, 1, 0, 2, 3, 1, 1, 1, 1, 1, 0 };

static ActShadowPacketQuadword Head_1[5] = {
    { .words = { 0x00000000U, 0x00000000U, 0x00000000U, 0x51000004U } },
    { .words = { 0x00008001U, 0x30000000U, 0x00000eeeU, 0x00000000U } },
    { .words = { 0x31000000U, 0x00000001U, 0x0000004eU, 0x00000000U } },
    { .words = { 0x00053001U, 0x00000000U, 0x00000047U, 0x00000000U } },
    { .words = { 0x00000044U, 0x00000000U, 0x00000042U, 0x00000000U } },
};

static u32 Tail_2[12] = { 0x00000000U, 0x00000000U, 0x00000000U, 0x51000002U, 0x00008001U, 0x10000000U, 0x0000000eU, 0x00000000U, 0x31000000U, 0x00000000U, 0x0000004eU, 0x00000000U };

static u8 TestEnv_3_00478D80[0x30] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x51,
    0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
    0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static DropShadowHeadPacket Head_4_00478DB0 = { .dma_tag = 0ULL, .vif_nop = 0U, .vif_direct = 1358954500U, .gif_tag = { .control_low = 32769U, .control_high = 805306368U, .registers_low = 3822U, .registers_high = 0U }, .set_shadow_alpha = { .data = { .value = 0ULL }, .register_address = 66U, .unused = 0U }, .set_render_state = { .data = { .value = 0ULL }, .register_address = 6U, .unused = 0U }, .unused_command = { .data = { .value = 0ULL }, .register_address = 20U, .unused = 0U } };

static ActShadowEndPacket Tail_5 = { .dma_tag = 0ULL, .vif_nop = 0U, .vif_direct = 1358954498U, .gif_tag = { .control_low = 32769U, .control_high = 268435456U, .registers_low = 14U, .registers_high = 0U }, .set_shadow_register = { .data = { .value = 0ULL }, .register_address = 76U, .unused = 0U } };

static DropShadowHeadPacket Head_6_00478E30 = { .dma_tag = 0ULL, .vif_nop = 0U, .vif_direct = 1358954500U, .gif_tag = { .control_low = 32769U, .control_high = 805306368U, .registers_low = 3822U, .registers_high = 0U }, .set_shadow_alpha = { .data = { .value = 0ULL }, .register_address = 66U, .unused = 0U }, .set_render_state = { .data = { .value = 0ULL }, .register_address = 6U, .unused = 0U }, .unused_command = { .data = { .value = 0ULL }, .register_address = 20U, .unused = 0U } };

static u32 TestEnv_7[12] = { 0x00000000U, 0x00000000U, 0x00000000U, 0x51000002U, 0x00008001U, 0x10000000U, 0x0000000eU, 0x00000000U, 0x00000000U, 0x00000000U, 0x0000003fU, 0x00000000U };

static ActShadowEndPacket Tail_8 = { .dma_tag = 0ULL, .vif_nop = 0U, .vif_direct = 1358954498U, .gif_tag = { .control_low = 32769U, .control_high = 268435456U, .registers_low = 14U, .registers_high = 0U }, .set_shadow_register = { .data = { .value = 0ULL }, .register_address = 76U, .unused = 0U } };

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

u8 * actNullPointerTable[] = { 0 };

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
            contacts = foot_tbl[((ActFootstepSaveData *)SaveData)->language - 1];
            if (motion == 3) contacts += 2;
            frame = F2I(actor->motion_frame / footstepFrameTime);
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

static inline ActShadowMatrixRow *ActShadowMatrixRows(ActShadowMatrix *matrix)
{
    return matrix->elements;
}

static inline const ActShadowMatrixRow *ActShadowMatrixConstRows(const ActShadowMatrix *matrix)
{
    return matrix->elements;
}

static void DrawCircleShadow(void *actor_pointer)
{
    float zero_coordinate;
    ActShadowActor *actor = actor_pointer;
    CircleShadowMatrixScratchpad *scratchpad = (CircleShadowMatrixScratchpad *)0x70000000;
    float radius;
    float corner;
    int camera_index;
    int i;
    int batch;
    DropShadowPacketPoint *projected;
    CircleShadowPoint *output;
    u8 *flags;

    xglMatrixUnit(ActShadowMatrixRows(&scratchpad->model));
    scratchpad->model.elements[3][0] = actor->position[0];
    scratchpad->model.elements[3][2] = actor->position[2];
    if (actor->height_override == -1000.0f) {
        scratchpad->model.elements[3][1] = actor->position[1] + 0.02f;
    } else {
        scratchpad->model.elements[3][1] = actor->height_override + 0.02f;
    }
    xglMatrixRotY(ActShadowMatrixRows(&scratchpad->transform),
                  ActShadowMatrixRows(&scratchpad->model), actor->yaw);
    for (camera_index = 0; camera_index < 8; camera_index++) {
        if (xglStudioGetCamera2(camera_index)->active == 1) {
            break;
        }
    }
    radius = actor->shadow_radius;
    corner = radius * 0.7f;
    scratchpad->vertices[0].x = scratchpad->vertices[1].x =
        scratchpad->vertices[5].x = scratchpad->vertices[14].x =
        scratchpad->vertices[15].x = 0.0f;
    scratchpad->vertices[6].x = scratchpad->vertices[7].x =
        scratchpad->vertices[8].x = scratchpad->vertices[9].x =
        scratchpad->vertices[13].x = -scratchpad->vertices[0].x;
    scratchpad->vertices[2].x = scratchpad->vertices[4].x =
        radius - (radius - corner);
    scratchpad->vertices[10].x = scratchpad->vertices[12].x =
        -scratchpad->vertices[2].x;
    scratchpad->vertices[3].x = radius;
    scratchpad->vertices[11].x = -radius;
    scratchpad->vertices[1].y =
        scratchpad->vertices[2].y = scratchpad->vertices[3].y =
        scratchpad->vertices[4].y = scratchpad->vertices[5].y =
        scratchpad->vertices[6].y = scratchpad->vertices[7].y =
        scratchpad->vertices[8].y = scratchpad->vertices[9].y =
        scratchpad->vertices[10].y = scratchpad->vertices[11].y =
        scratchpad->vertices[12].y = scratchpad->vertices[13].y =
        scratchpad->vertices[14].y = scratchpad->vertices[15].y =
        scratchpad->vertices[0].x;
    scratchpad->camera_index = camera_index;
    zero_coordinate = scratchpad->vertices[0].x;
    scratchpad->vertices[0].y = zero_coordinate;
    scratchpad->vertices[3].z = scratchpad->vertices[7].z =
        scratchpad->vertices[8].z = scratchpad->vertices[11].z =
        scratchpad->vertices[15].z = zero_coordinate;
    scratchpad->vertices[0].z = zero_coordinate;
    scratchpad->vertices[2].z = scratchpad->vertices[12].z = corner;
    scratchpad->vertices[4].z = scratchpad->vertices[10].z = -corner;
    scratchpad->vertices[1].z = scratchpad->vertices[13].z =
        scratchpad->vertices[14].z = radius;
    scratchpad->vertices[5].z = scratchpad->vertices[6].z =
        scratchpad->vertices[9].z = -radius;
    scratchpad->vertices[0].w = scratchpad->vertices[1].w = scratchpad->vertices[2].w =
        scratchpad->vertices[3].w = scratchpad->vertices[4].w =
        scratchpad->vertices[5].w = scratchpad->vertices[6].w =
        scratchpad->vertices[7].w = scratchpad->vertices[8].w =
        scratchpad->vertices[9].w = scratchpad->vertices[10].w =
        scratchpad->vertices[11].w = scratchpad->vertices[12].w =
        scratchpad->vertices[13].w = scratchpad->vertices[14].w =
        scratchpad->vertices[15].w = 1.0f;
    xglRotTransPersN(scratchpad->projected, ActShadowMatrixConstRows(&scratchpad->transform),
                    scratchpad->vertices, 16, camera_index);
    {
        DropShadowPacketPoint *clip = scratchpad->projected;
        int clip_index;
        for (clip_index = 0; clip_index < 16; clip_index++, clip++) {
            if ((clip->x & 0xffff0000) ||
                (clip->y & 0xffff0000) || clip->z < 0) {
                return;
            }
        }
    }
    nmlModelDirectSend(1, Head_1, 5);
    flags = rgbaw_0;
    scratchpad->packet.vif_command = 0x5100001100000000ULL;
    scratchpad->packet.gif_control = 0x0026c00000008001ULL;
    scratchpad->packet.gif_registers = 0x5151515151515151ULL;
    scratchpad->packet.dma_tag = 0;
    projected = scratchpad->projected;
    for (batch = 0; batch < 2; batch++) {
        output = scratchpad->packet.points;
        for (i = 7; i >= 0; i--, output++, projected++, flags++) {
            output->color[0] = 0;
            output->color[1] = 0;
            output->color[2] = 0;
            if ((*flags & 1) == 0) {
                output->color[3] = actor->shadow_alpha;
            } else {
                output->color[3] = 0;
            }
            output->point.x = projected->x;
            output->point.y = projected->y;
            output->point.z = projected->z;
            output->point.w = (*flags & 2) ? 0x8000 : 0;
        }
        nmlModelDirectSend(1, &scratchpad->packet, 18);
    }
    nmlModelDirectSend(1, Tail_2, 3);
}

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
        if (point->value.z < 0) {
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
    nmlModelDirectSend(1, &scratchpad->packet, 0x12);
}

static void DrawZeldaShadow(void *actor_pointer)
{
    ActZeldaShadowActor *actor = actor_pointer;
    ActShadowScratchpad *scratchpad = (ActShadowScratchpad *)0x70000000;
    int camera_index;
    float *direction;

    if (actor->height_override == -1000.0f) {
        scratchpad->height = actor->base_height + 0.02f;
    } else {
        scratchpad->height = actor->height_override + 0.02f;
    }
    for (camera_index = 0; camera_index < 8; camera_index++) {
        if (xglStudioGetCamera2(camera_index)->active == 1) {
            break;
        }
    }
    scratchpad->camera_index = camera_index;
    {
        ActShadowMatrix *destination = &scratchpad->matrix;
        ActShadowMatrix *matrix = actor->shadow_matrices->matrices;
        /* Four aligned copies use the original scratch data register. */
        QuadwordCopyMatrix(destination, matrix);
    }
    direction = actor->direction;
    scratchpad->radius = __builtin_sqrtf(1.0f - direction[1] * direction[1]) * 0.5f;
    if (scratchpad->radius < zeldaShadowMinRadius) {
        scratchpad->radius = zeldaShadowMinRadius;
    }
    scratchpad->angle = atan2f(-direction[0], -direction[2]);
    DrawZeldaShadowSub(scratchpad, &actor->shadow_matrices->matrices[30]);
    DrawZeldaShadowSub(scratchpad, &actor->shadow_matrices->matrices[20]);
}

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
            int orientation_word;

            if (point0->valid == 0 || point1->valid == 0 ||
                point2->valid == 0 || point3->valid == 0) {
                return 0;
            }
            x0 = point0->point.x;
            x2 = point2->point.x;
            y0 = point0->point.y;
            y1 = point1->point.y;
            x1 = point1->point.x;
            y2 = point2->point.y;
            entry->points[3] = &point3->point;
            entry->points[0] = &point0->point;
            entry->points[1] = &point1->point;
            entry->points[2] = &point2->point;
            entry++;

            orientation_word = x2 * y0 + x0 * y1 + x1 * y2 - x2 * y1 -
                               x0 * y2 - x1 * y0;
            if ((float)orientation_word >= 0.0f) {
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

static void DrawDropShadowSubBack(ActDropShadowWorkspace *shadow, int vertex_count,
                                  int draw_mode)
{
    int remaining;
    u8 *status;
    DropShadowBackEntry *entry;
    int active_status;

    entry = shadow->back_shadow_entries;
    shadow->packet.vif_direct = 0x51000008;
    shadow->packet.gif_tag.control_high = 0x70024000;
    shadow->packet.gif_tag.registers_low = 0x055551ee;
    shadow->packet.packet_header_20 = 0x00071001;
    shadow->packet.packet_header_34 = 0x7fffffff;
    shadow->packet.color[3] = draw_mode;
    shadow->packet.color[0] = 0;
    shadow->packet.color[1] = 0;
    shadow->packet.color[2] = 0;
    shadow->packet.points[0].w = 0;
    shadow->packet.points[1].w = 0;
    shadow->packet.points[2].w = 0;
    shadow->packet.points[3].w = 0;

    if (vertex_count <= 0) {
        return;
    }

    remaining = vertex_count;
    active_status = 1;
    status = shadow->back_shadow_status;

    /* The original advances the status bytes and vertex records in lockstep. */
    do {
        if (*status == active_status) {
            /* Pointer form: a store through the vertex union itself is alias
             * set 0 and would pin the entry->points loads behind it. */
            (shadow->packet.points + 0)->x = entry->points[0]->x;
            (shadow->packet.points + 0)->y = entry->points[0]->y;
            (shadow->packet.points + 0)->z = entry->points[0]->z;
            (shadow->packet.points + 1)->x = entry->points[1]->x;
            (shadow->packet.points + 1)->y = entry->points[1]->y;
            (shadow->packet.points + 1)->z = entry->points[1]->z;
            (shadow->packet.points + 2)->x = entry->points[2]->x;
            (shadow->packet.points + 2)->y = entry->points[2]->y;
            (shadow->packet.points + 2)->z = entry->points[2]->z;
            (shadow->packet.points + 3)->x = entry->points[3]->x;
            (shadow->packet.points + 3)->y = entry->points[3]->y;
            (shadow->packet.points + 3)->z = entry->points[3]->z;
            nmlModelDirectSend(1, &shadow->packet, 9);
        }
        remaining--;
        status++;
        entry++;
    } while (remaining != 0);
}

static void DrawDropShadowSubFront(ActDropShadowWorkspace *shadow, int count)
{
    float coordinate_scale = DROP_SHADOW_TEXTURE_COORDINATE_SCALE;
    DropShadowTexturedPoint *points = shadow->packet.textured_points;
    DropShadowBackEntry *entry = shadow->back_shadow_entries;
    u8 *status;
    int remaining;

    shadow->packet.vif_direct = 0x5100000c;
    {
    u64 primitive = ((u64)shadow->alpha << 4) | 0x70009;
    u32 alpha = shadow->alpha;
    shadow->packet.gif_tag.control_high = 0xb02a4000;
    shadow->packet.gif_tag.registers_low = 0x252521ee;
    shadow->packet.gif_tag.registers_high = 0x525;
    shadow->packet.packet_header_20 = primitive;
    shadow->packet.color[3] = alpha;
    }
    shadow->packet.packet_header_34 = 0;
    shadow->packet.color[0] = 0;
    shadow->packet.color[1] = 0;
    shadow->packet.color[2] = 0;
    points[0].point.w = 0;
    points[1].point.w = 0;
    points[2].point.w = 0;
    points[3].point.w = 0;
    if (count > 0) {
        remaining = count;
        status = shadow->back_shadow_status;
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
                nmlModelDirectSend(1, &shadow->packet, 13);
            }
            status++;
            remaining--;
            entry++;
        } while (remaining != 0);
    }
}

static DropShadowContactRecord *DrawDropShadowSub(ActShadowActor *actor,
                            ActDropShadowWorkspace *work, DropShadowContactRecord *contacts)
{
    float ground_height;
    float length;
    int segments;
    int vertex_count;
    int visible;
    int i;
    ActShadowMatrix *translated_end;
    ActShadowMatrix *model;
    Vector4 *perpendicular;
    Vector4 *inner_offset;
    Vector4 *axial_offset;
    Matrix4 *first_world;
    Vector4 *light_direction;
    Matrix4 *second_world;
    Vector4 *points = work->vertices;
    DropShadowSourceVertex *projected;

    if (actor->height_override == -1000.0f) {
        ground_height = actor->position[1] - 0.5f;
    } else {
        ground_height = actor->height_override - 0.5f;
    }
    segments = 0;
    if (contacts[0].matrix_index != 255) {
        float *first_position;
        float *second_position;

        first_world = &work->first_world.elements;
        second_world = &work->second_world.elements;
        light_direction = &actor->light_direction;
        translated_end = &work->translated_end;
        model = &work->model;
        perpendicular = &work->perpendicular;
        inner_offset = &work->inner_offset;
        axial_offset = &work->axial_offset;
        first_position = (*first_world)[3];
        second_position = (*second_world)[3];
        do {
            ActShadowMatrix *first = &actor->shadow_matrices[contacts[segments].matrix_index];
            ActShadowMatrix *second;
            DropShadowContactRecord *contact = &contacts[segments];
            DropShadowContactRecord *next_contact = &contacts[segments + 1];

            if (next_contact->matrix_index != 255) {
                second = &actor->shadow_matrices[next_contact->matrix_index];
            } else {
                work->temporary.values[0] = 0.0f;
                work->temporary.values[1] = 0.0f;
                work->temporary.values[2] = 0.0f;
                work->temporary.values[next_contact->payload.terminal.axis] =
                    I2F(next_contact->payload.terminal.offset) * contactCoordScale;
                work->temporary.values[3] = 1.0f;
                xglMatrixTrans(ActShadowMatrixRows(translated_end),
                                ActShadowMatrixConstRows(first), work->temporary.values);
                second = translated_end;
            }
            xglMatrixMul(*first_world, ActShadowMatrixRows(model), ActShadowMatrixRows(first));
            xglMatrixMul(*second_world, ActShadowMatrixRows(model), ActShadowMatrixRows(second));

            /* Each VU block loads both complete vectors, changes XYZ, and retains
               the VF2 input's W. VF2/VF3 and arithmetic flags are scratch here;
               no incoming ACC/Q state is used and this compiler has no VF allocator. */
            Vu0SubVectorXYZ(work->temporary.values, second_position, first_position);
            xglVectorOuter(perpendicular, &work->temporary.vector,
                             light_direction);
            xglVectorLength(&length, perpendicular);
            xglVectorScaleXYZ(&work->outer_offset, perpendicular,
                                I2F(contact->payload.segment.end_width) * contactCoordScale / length);
            xglVectorScaleXYZ(inner_offset, perpendicular,
                                I2F(contact->payload.segment.midpoint_width) * contactCoordScale / length);
            xglVectorLength(&length, &work->temporary.vector);
            xglVectorScaleXYZ(axial_offset, &work->temporary.vector,
                                I2F(contact->payload.segment.end_width) * contactCoordScale / length);

            work->temporary.values[3] = ground_height;
            work->temporary.values[0] = first_position[0];
            work->temporary.values[1] = first_position[1];
            work->temporary.values[2] = first_position[2];
            Vu0AddVectorXYZ(&points[0], work->temporary.values, &work->outer_offset);
            xglVectorMulMat(&points[8], work->flatten.elements, &points[0]);
            points[0].w = 1.0f;
            points[8].w = 1.0f;
            Vu0SubVectorXYZ(&points[1], work->temporary.values, &work->outer_offset);
            xglVectorMulMat(&points[9], work->flatten.elements, &points[1]);
            points[1].w = 1.0f;
            points[9].w = 1.0f;
            Vu0SubVectorXYZ(&points[4], work->temporary.values, axial_offset);
            xglVectorMulMat(&points[12], work->flatten.elements, &points[4]);
            points[4].w = 1.0f;
            points[12].w = 1.0f;

            work->temporary.values[0] = second_position[0];
            work->temporary.values[1] = second_position[1];
            work->temporary.values[2] = second_position[2];
            Vu0AddVectorXYZ(&points[2], work->temporary.values, &work->outer_offset);
            xglVectorMulMat(&points[10], work->flatten.elements, &points[2]);
            points[2].w = 1.0f;
            points[10].w = 1.0f;
            Vu0SubVectorXYZ(&points[3], work->temporary.values, &work->outer_offset);
            xglVectorMulMat(&points[11], work->flatten.elements, &points[3]);
            points[3].w = 1.0f;
            points[11].w = 1.0f;
            Vu0AddVectorXYZ(&points[5], work->temporary.values, axial_offset);
            xglVectorMulMat(&points[13], work->flatten.elements, &points[5]);
            points[5].w = 1.0f;
            points[13].w = 1.0f;

            work->temporary.values[0] =
                (first_position[0] + second_position[0]) * 0.5f;
            work->temporary.values[1] =
                (first_position[1] + second_position[1]) * 0.5f;
            work->temporary.values[2] =
                (first_position[2] + second_position[2]) * 0.5f;
            Vu0AddVectorXYZ(&points[6], work->temporary.values, inner_offset);
            xglVectorMulMat(&points[14], work->flatten.elements, &points[6]);
            points[6].w = 1.0f;
            points[14].w = 1.0f;
            Vu0SubVectorXYZ(&points[7], work->temporary.values, inner_offset);
            xglVectorMulMat(&points[15], work->flatten.elements, &points[7]);
            points[7].w = 1.0f;
            points[15].w = 1.0f;
            points += 16;
            segments++;
        } while (contacts[segments].matrix_index != 255);
    }
    vertex_count = segments * 16;
    xglRotTransPersN(work->source_vertices, 0, work->vertices,
                      vertex_count, work->camera_index);
    visible = 0;
    projected = work->source_vertices;
    if (vertex_count > 0) {
        int remaining = vertex_count;
        do {
            if ((projected->point.x & 0xffff0000) ||
                (projected->point.y & 0xffff0000) || projected->point.z < 0) {
                projected->valid = 0;
            } else {
                visible = 1;
            }
            projected++;
        } while (--remaining != 0);
    }
    if (!visible) {
        return contacts + segments + 1;
    }
    work->alpha = actor->shadow_alpha;
    work->packet.gif_tag.control_low = 0x8001;
    work->packet.test_environment_register = 71;
    work->packet.render_state_register = sRender.shadow_state | 0x80000;
    work->packet.shadow_context_register = 76;
    work->packet.dma_tag.words.low = 0;
    work->packet.dma_tag.words.high = 0;
    work->packet.vif_nop = 0;
    for (i = 0; i < segments; i++) {
        DrawDropCircleIndexTable indices = dropShadowIndexTable;
        if (DrawDropShadowSubChk(work, &indices.quad_indices[0][0], i * 16, 14)) {
            DrawDropShadowSubBack(work, 14, 0);
            nmlModelDirectSend(2, TestEnv_3_00478D80, 3);
            DrawDropShadowSubFront(work, 14);
            DrawDropShadowSubBack(work, 14, 128);
        }
    }
    return contacts + segments + 1;
}

static void DrawDropShadow(void *actor_pointer)
{
    ActShadowActor *actor = actor_pointer;
    ActDropShadowWorkspace *work = (ActDropShadowWorkspace *)0x70000000;
    DropShadowContactDefinition *entry;
    DropShadowContactRecord *contacts;
    u16 actor_kind;
    int camera_index;

    {
        u64 alpha_descriptor = ((u64)actor->shadow_alpha << 32) | 100;
        Head_4_00478DB0.set_shadow_alpha.data.value = alpha_descriptor;
    }
    {
        int render_state_bits = 0x24020000 | (sRender.shadow_state << 5);
        Head_4_00478DB0.set_render_state.data.value =
            ((u64)0xc800 << 19) | render_state_bits;
    }
    nmlModelDirectSend(1, &Head_4_00478DB0, 5);
    for (camera_index = 0; camera_index < 8; camera_index++) {
        if (xglStudioGetCamera2(camera_index)->active == 1) {
            break;
        }
    }
    work->camera_index = camera_index;
    {
        ActShadowMatrix *matrix = actor->shadow_matrices;
        ActShadowMatrix *destination = &work->model;
        /* Match the original four aligned SDK copies through its scratch GPR. */
        QuadwordCopyMatrix(destination, matrix);
    }
    {
        float light_x = actor->light_direction.x;
        float light_z = actor->light_direction.z;
        work->flatten.elements[0][0] = 1.0f;
        work->flatten.elements[0][1] = 0.0f;
        work->flatten.elements[0][2] = 0.0f;
        work->flatten.elements[0][3] = 0.0f;
        work->flatten.elements[1][0] = -light_x;
        work->flatten.elements[1][1] = 0.0f;
        work->flatten.elements[1][2] = -light_z;
        work->flatten.elements[1][3] = 0.0f;
        work->flatten.elements[2][0] = 0.0f;
        work->flatten.elements[2][1] = 0.0f;
        work->flatten.elements[2][2] = 1.0f;
        work->flatten.elements[2][3] = 0.0f;
        work->flatten.elements[3][0] = light_x;
        work->flatten.elements[3][1] = 1.0f;
        work->flatten.elements[3][2] = light_z;
        work->flatten.elements[3][3] = 0.0f;
    }
    actor_kind = actor->kind;
    for (entry = DropShadowDefineTable; entry->contacts; entry++) {
        if ((entry->kind_mask & actor_kind) == entry->kind_value) {
            contacts = entry->contacts;
            while (contacts->matrix_index != 255) {
                /* Keep the separately authored partial workspace interface. */
                contacts = DrawDropShadowSub(actor, work, contacts);
            }
            break;
        }
    }
    {
        u64 shadow_register = (u64)sRender.shadow_state | 0x80000;
        Tail_5.set_shadow_register.data.value = shadow_register;
    }
    nmlModelDirectSend(1, &Tail_5, 3);
}

static void DrawDropCircle(ActShadowActor *actor)
{
    ActDropShadowWorkspace *work = (ActDropShadowWorkspace *)0x70000000;
    const float *center = work->temporary.values;
    Vector4 *vertices = work->vertices;
    float radius;
    float corner;
    int camera_index;
    int visible;
    u32 clip_mask;
    int i;
    DropShadowSourceVertex *projected;

    {
        u8 shadow_alpha = actor->shadow_alpha;
        u64 alpha_descriptor = ((u64)shadow_alpha << 32) | 100;
        Head_6_00478E30.set_shadow_alpha.data.value = alpha_descriptor;
    }
    {
        int render_state_bits = (sRender.shadow_state << 5) | 0x24020000;
        u64 render_state_value = ((u64)0xc800 << 19) | render_state_bits;
        Head_6_00478E30.set_render_state.data.value = render_state_value;
    }
    nmlModelDirectSend(1, &Head_6_00478E30, 5);
    for (camera_index = 0; camera_index < 8; camera_index++) {
        if (xglStudioGetCamera2(camera_index)->active == 1) {
            break;
        }
    }
    work->camera_index = camera_index;
    work->temporary.values[0] = actor->position[0];
    work->temporary.values[2] = actor->position[2];
    if (actor->height_override == -1000.0f) {
        work->temporary.values[1] = actor->position[1] + 0.02f;
    } else {
        work->temporary.values[1] = actor->height_override + 0.02f;
    }
    radius = actor->shadow_radius;
    corner = radius * dropCircleCornerScale;
    vertices[4].x = vertices[5].x = center[0];
    vertices[0].x = vertices[2].x = center[0] + corner;
    vertices[1].x = vertices[3].x = center[0] - corner;
    vertices[6].x = center[0] + radius;
    vertices[7].x = center[0] - radius;
    vertices[0].y = vertices[1].y = vertices[2].y =
        vertices[3].y = vertices[4].y = vertices[5].y =
        vertices[6].y = vertices[7].y = work->temporary.values[1] + dropCircleHeightOffset;
    vertices[6].z = vertices[7].z = center[2];
    vertices[0].z = vertices[1].z = center[2] + corner;
    vertices[2].z = vertices[3].z = center[2] - corner;
    vertices[4].z = center[2] + radius;
    vertices[5].z = center[2] - radius;
    vertices[0].w = vertices[1].w = vertices[2].w =
        vertices[3].w = vertices[4].w = vertices[5].w =
        vertices[6].w = vertices[7].w = 1.0f;
    vertices[8].x = vertices[0].x;
    vertices[8].z = vertices[0].z;
    vertices[9].x = vertices[1].x;
    vertices[9].z = vertices[1].z;
    vertices[10].x = vertices[2].x;
    vertices[10].z = vertices[2].z;
    vertices[11].x = vertices[3].x;
    vertices[11].z = vertices[3].z;
    vertices[12].x = vertices[4].x;
    vertices[12].z = vertices[4].z;
    vertices[13].x = vertices[5].x;
    vertices[13].z = vertices[5].z;
    vertices[14].x = vertices[6].x;
    vertices[14].z = vertices[6].z;
    vertices[15].x = vertices[7].x;
    vertices[15].z = vertices[7].z;
    vertices[8].y = vertices[9].y = vertices[10].y =
        vertices[11].y = vertices[12].y = vertices[13].y =
        vertices[14].y = vertices[15].y = center[1] - 0.5f;
    vertices[8].w = vertices[9].w = vertices[10].w =
        vertices[11].w = vertices[12].w = vertices[13].w =
        vertices[14].w = vertices[15].w = 1.0f;
    xglRotTransPersN(work->source_vertices, 0, vertices, 16, work->camera_index);
    projected = work->source_vertices;
    visible = 0;
    clip_mask = 0xffff0000;
    for (i = 15; i >= 0; i--, projected++) {
        if ((projected->point.x & clip_mask) ||
            (projected->point.y & clip_mask) || projected->point.z < 0) {
            projected->valid = 0;
        } else {
            visible = 1;
        }
    }
    if (visible) {
        DrawDropCircleIndexTable indices;
        work->alpha = actor->shadow_alpha;
        work->packet.gif_tag.control_low = 0x8001;
        work->packet.test_environment_register = 71;
        work->packet.render_state_register = sRender.shadow_state | 0x80000;
        work->packet.shadow_context_register = 76;
        indices = dropCircleIndexTable;
        work->packet.dma_tag.words.low = 0;
        work->packet.dma_tag.words.high = 0;
        work->packet.vif_nop = 0;
        if (DrawDropShadowSubChk(work, &indices.quad_indices[0][0], 0, 14)) {
            DrawDropShadowSubBack(work, 14, 0);
            nmlModelDirectSend(2, TestEnv_7, 3);
            DrawDropShadowSubFront(work, 14);
            DrawDropShadowSubBack(work, 14, 128);
            Tail_8.set_shadow_register.data.value = (u64)sRender.shadow_state | 0x80000;
            nmlModelDirectSend(1, &Tail_8, 3);
        }
    }
}

void ACT_DrawShadowBegin(void)
{
    u16 shadow_state = sRender.shadow_state;

    Head_9.set_color_control.data.value = shadow_state | 0xFFFFFF00080000ULL;
    Head_9.set_shadow_context.data.value = shadow_state;
    /* Set the command-enable bit after initializing the state bits. */
    Head_9.set_shadow_context.data.value |= 0x80000;
    nmlModelDirectSend(1, &Head_9, 10);
}

void ACT_DrawShadowEnd(void)
{
    nmlModelDirectSend(1, &Tail_10, 3);
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
        DrawZeldaShadow(actor);
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
    QuadwordCopy(&actor->direction, &light->direction);
    if ((actor->kind & 0xf000) == 0 && (actor->flags & 0x100) == 0) {
        Footstep((ActShadowActor *)actor);
    }
}

void ACT_DrawShadowInit(void)
{
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
/* Explicit scalar literals retained at the original compiler-mapped locations. */
float footstepFrameTime = 0.033333335071802139f;

float zeldaShadowMinRadius = 0.15000000596046448f;

const float contactCoordScale = 0.0099999997764825821f;

const float dropCircleCornerScale = 0.69999998807907104f;

const float dropCircleHeightOffset = 0.20000000298023224f;
