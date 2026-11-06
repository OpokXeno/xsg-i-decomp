/*
 * OV01 original TU 36: 0x00a3a090..0x00a3aac8 (5 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"
#include "main/xgl_packet.h"

typedef struct Kosbw02DrawWork {
    unsigned char unmodeled_00[0x40];
    int model_entry;
    const char *texture;
    unsigned char unmodeled_48[0x28];
    int frame;
    unsigned char unmodeled_74[0x8c];
    Vector4 position;
    unsigned char unmodeled_110[0x10];
    Vector4 rotation;
} Kosbw02DrawWork;

typedef struct Kosbw02GsParameter {
    u64 primitive_tag;
    float scale_u;
    float scale_v;
} Kosbw02GsParameter;

/* Partial views of the GS packet (same template as fnAMP02_DP000; the a1..a5
 * A+D stores go through the volatile vul/vuc members, which plain
 * MEfGsPacket/MEfGsQword fields did not reproduce in amp_02). */
typedef union Kosbw02QuadwordView {
    Quadword q;
    unsigned long ul[2];
    unsigned short us[8];
    unsigned char uc[16];
    volatile unsigned long vul[2];
    volatile unsigned char vuc[16];
} Kosbw02QuadwordView;

typedef struct Kosbw02PacketView {
    void *data;
    int capacity;
    Kosbw02QuadwordView *current;
    int count;
    int auxCount;
    unsigned char unmodeled_14[4];
} Kosbw02PacketView;

typedef struct Kosbw02CreateState {
    unsigned char unmodeled_00[0x08];
    Actor *source_actor;
    u32 source_part;
    Vector4 source_offset;
    s16 rotation_degrees;
    unsigned char unmodeled_22[0x02];
    Actor *target_actor;
    u32 target_part;
    unsigned char unmodeled_2c[0x1c];
    u64 model_texture;
    unsigned char unmodeled_50[0x20];
    int frame;
    unsigned char unmodeled_74[0x0c];
    Vector4 start_point;
    Vector4 target_point;
    Vector4 midpoint;
    float half_length;
    float angle;
    float angle_step;
    unsigned char unmodeled_bc[0x04];
    Vector4 matrix[4];
    Vector4 position;
    Vector4 previous_position;
    Vector4 rotation;
    Kosbw02PacketView draw_packet;
    u64 draw_parameter;
    u64 texture_parameter;
    u16 active[10];
    unsigned char unmodeled_16c[0x04];
    Vector4 points[30];
    float twist[30];
    float random_angle;
} Kosbw02CreateState;

struct Kosbw02Effect;

typedef struct Kosbw02Allocation {
    unsigned char unmodeled_00[0x04];
    void (*process_callback)(void *task, Kosbw02CreateState *state);
    void (*model_callback)(void *task, Kosbw02DrawWork *state);
    void (*packet_callback)(void *task, Kosbw02CreateState *state);
    void (*post_callback)(void *task, struct Kosbw02Effect *state);
    unsigned char unmodeled_14[0x0c];
    Kosbw02CreateState state;
} Kosbw02Allocation;

static void fnKOSBW02_PR000(void *task, Kosbw02CreateState *state);
static void fnKOSBW02_DM000(void *task, Kosbw02DrawWork *state);
static void fnKOSBW02_DP000(void *task, Kosbw02CreateState *state);
static void fnKOSBW02_PO000(void *task, struct Kosbw02Effect *state);

extern Vector4 *MMathVectorInterpolation(Vector4 *destination,
                                         const Vector4 *first,
                                         const Vector4 *second,
                                         float parameter);
extern Vector4 *MMathRotateMatrixYX(Vector4 *destination,
                                    const Vector4 *source,
                                    const Vector4 *angles);
extern float MMathCalcDist(const Vector4 *first, const Vector4 *second);
extern float MMathMakeRandom(void);
extern void MGsGPInit(void *packet, void *address, int size);
extern Kosbw02GsParameter *svGetPrmFromName(const char *name,
                                            Kosbw02GsParameter *output);
extern float MMathMakeRandom2PI(void);
const char efKosbw02DeCld01Text[16] = "de_cld01";
const char efKosbw02DeCld02Text[16] = "de_cld02";
extern Vector4 *MMathRotateMatrixYXZ(Vector4 *destination,
                                     const Vector4 *source,
                                     const Vector4 *angles);
extern void MEfDrawModel(const Vector4 *place, int entry, const char *texture);

int MEfCreate_KOSBW02(MEfObjRecord *storage)
{
    Kosbw02Allocation *allocation = (Kosbw02Allocation *)storage;
    Kosbw02CreateState *state = &allocation->state;
    Kosbw02GsParameter parameter;
    Vector4 angle_vector;
    Matrix4 actor_matrix;

    state->frame = 0;
    MEfGetActorMatrix(&actor_matrix, state->source_actor, state->source_part);
    MMathApplyMatrix(&state->start_point, &actor_matrix, &state->source_offset);
    MEfGetActorCoord(&state->target_point, state->target_actor, state->target_part);
    MMathVectorInterpolation(&state->midpoint, &state->start_point,
                             &state->target_point, 0.5f);
    state->half_length = MMathCalcDist(&state->start_point, &state->target_point) * 0.5f;
    MEfCalcAngle(&angle_vector, &state->target_point, &state->start_point);
    angle_vector.z = (float)state->rotation_degrees * 0.017453292f;
    MMathRotateMatrixYXZ(state->matrix, 0, &angle_vector);
    __asm__ __volatile__(
        "lqc2 $vf1, 0(%1)\n"
        "vmove.w $vf1, $vf0\n"
        "sqc2 $vf1, 48(%0)\n"
        :
        : "r"(state->matrix), "r"(&state->midpoint)
        : "memory");
    state->angle = 0.0f;
    state->angle_step = 0.12566371f;
    __asm__ __volatile__(
        "lq $8, 0(%1)\n"
        "sq $8, 0(%0)\n"
        :
        : "r"(&state->position), "r"(&state->start_point)
        : "$8", "memory");
    __asm__ __volatile__(
        "lq $8, 0(%1)\n"
        "sq $8, 0(%0)\n"
        :
        : "r"(&state->previous_position), "r"(&state->start_point)
        : "$8", "memory");
    MEfCalcAngle(&state->rotation, &state->target_point, &state->start_point);
    MGsGPInit(&state->draw_packet, 0, 0);
    svGetPrmFromName(efKosbw02DeCld01Text, &parameter);
    state->draw_parameter = parameter.primitive_tag;
    svGetPrmFromName(efKosbw02DeCld02Text, &parameter);
    state->texture_parameter = parameter.primitive_tag;
    memset(state->active, 0, sizeof(state->active));
    memset(state->points, 0, sizeof(state->points));
    memset(state->twist, 0, sizeof(state->twist));
    state->random_angle = MMathMakeRandom2PI();
    allocation->process_callback = fnKOSBW02_PR000;
    allocation->model_callback = fnKOSBW02_DM000;
    allocation->packet_callback = fnKOSBW02_DP000;
    allocation->post_callback = fnKOSBW02_PO000;
    return 1;
}

static void fnKOSBW02_PR000(void *task, Kosbw02CreateState *work)
{
    Vector4 work_vector;
    Vector4 matrix[4];
    float sine;
    float cosine;
    int trail;
    int point;
    int point_index;
    Vector4 *shifted_point;
    Vector4 *source_point;
    Vector4 *trail_point;

    if (work->frame < 25) {
        __asm__ __volatile__(
            "lq $8, 0(%1)\n"
            "sq $8, 0(%0)\n"
            :
            : "r"(&work->previous_position), "r"(&work->position)
            : "$8", "memory");
        __asm__ __volatile__("sqc2 $vf0, 0(%0)"
                             :
                             : "r"(&work_vector)
                             : "memory");
        __asm__ __volatile__(
            "mfc1 $8, %1\n"
            "qmtc2 $8, $vf4\n"
            "vcallms 0x20\n"
            "qmfc2.i $8, $vf1\n"
            "mtc1 $8, %0\n"
            : "=f"(sine)
            : "f"(work->angle)
            : "$8", "memory");
        work_vector.x = work->half_length * sine;
        __asm__ __volatile__(
            "mfc1 $8, %1\n"
            "qmtc2 $8, $vf4\n"
            "vcallms 0xe8\n"
            "qmfc2.i $8, $vf1\n"
            "mtc1 $8, %0\n"
            : "=f"(cosine)
            : "f"(work->angle)
            : "$8", "memory");
        work_vector.z = work->half_length * cosine;
        MMathApplyMatrix(&work->position, work->matrix, &work_vector);
        work->angle = work->angle + work->angle_step;
        MEfCalcAngle(&work->rotation, &work->position,
                     &work->previous_position);
    }

    for (trail = 8; trail >= 0; trail--) {
        work->active[trail + 1] = work->active[trail];
        for (point = 0; point < 3; point++) {
            point_index = trail * 3 + point;
            shifted_point = &work->points[point_index + 3];
            source_point = &work->points[point_index];
            __asm__ __volatile__(
                "lq $8, 0(%1)\n"
                "sq $8, 0(%0)\n"
                :
                : "r"(shifted_point), "r"(source_point)
                : "$8", "memory");
            work->twist[point_index + 3] = work->twist[point_index];
        }
    }

    work->active[0] = 0;
    for (point = 0; point < 3; point++) {
        trail_point = &work->points[point];
        __asm__ __volatile__("sqc2 $vf0, 0(%0)"
                             :
                             : "r"(trail_point)
                             : "memory");
    }

    if (work->frame < 25) {
        work->active[0] = 1;
        for (point = 0; point < 3; point++) {
            MMathVectorInterpolation(&work_vector, &work->position,
                                     &work->previous_position,
                                     (float)point / 3.0f);
            MMathRotateMatrixYX(matrix, 0, &work->rotation);
            __asm__ __volatile__(
                "lqc2 $vf1, 0(%1)\n"
                "vmove.w $vf1, $vf0\n"
                "sqc2 $vf1, 48(%0)\n"
                :
                : "r"(matrix), "r"(&work_vector)
                : "memory");
            __asm__ __volatile__("sqc2 $vf0, 0(%0)"
                                 :
                                 : "r"(&work_vector)
                                 : "memory");
            work_vector.x = MMathMakeRandom() * 0.06f - 0.03f;
            work_vector.y = MMathMakeRandom() * 0.06f - 0.03f;
            MMathApplyMatrix(&work->points[point], matrix, &work_vector);
            work->twist[point] = MMathMakeRandom2PI();
        }
    }
}

static void fnKOSBW02_DM000(void *task, Kosbw02DrawWork *work)
{
    Vector4 matrix[4];

    if (work->frame < 25) {
        MMathRotateMatrixYXZ(matrix, 0, &work->rotation);
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%1)\n"
            "vmove.w $vf1, $vf0\n"
            "sqc2 $vf1, 48(%0)\n"
            :
            : "r"(matrix), "r"(&work->position)
            : "memory");
        MEfDrawModel(matrix, work->model_entry, work->texture);
    }
}

extern int MEfMake4Vertex(Vector4 *destination, const Vector4 *source, float size);
extern int MEfMake4Vertex2(Vector4 *destination, const Vector4 *source,
                           float size, float angleBase);
extern void MGsGPTerm(void *packet);
extern Quadword McEfSTQtbl[];
static const Quadword coltbl_0_00A518A0[10];
static const float sztbl_1[10];

static void fnKOSBW02_DP000(void *task, Kosbw02CreateState *state)
{
    int i;
    long regs;
    int nreg;
    Vector4 verts[4];
    Kosbw02PacketView *pk;
    Kosbw02PacketView *gp;
    Kosbw02QuadwordView *q;
    Kosbw02QuadwordView *a1;
    Kosbw02QuadwordView *a2;
    Kosbw02QuadwordView *a3;
    Kosbw02QuadwordView *a4;
    Kosbw02QuadwordView *a5;
    Quadword color;
    unsigned long *tex;
    int j;
    int n;
    int lo;
    int nloop;
    int texReg;
    int off;
    unsigned char *rec;
    Vector4 *pt;
    float *tw;

    pk = &state->draw_packet;
    MGsGPInit(pk, 0, 0);
    q = pk->current;
    q[0].ul[0] = MEF_GS_SETUP_TAG;
    q[0].ul[1] = MEF_GS_SETUP_REGS;
    pk->auxCount = 1;
    a1 = q + 1;
    a2 = q + 2;
    a3 = q + 3;
    a4 = q + 4;
    a5 = q + 5;
    a1->vuc[8] = SCE_GS_TEST_1;
    a1->vul[0] = MEF_GS_TEST_1_ALPHA_GREATER_ZGEQUAL;
    a2->vuc[8] = SCE_GS_TEX1_1;
    a2->vul[0] = 0x60;
    a3->vuc[8] = SCE_GS_CLAMP_1;
    a3->vul[0] = 5;
    a4->vuc[8] = SCE_GS_ZBUF_1;
    a4->vul[0] = MEF_GS_ZBUF_1_PSMZ24_NO_DEPTH_WRITE;
    a5->vuc[8] = SCE_GS_ALPHA_1;
    a5->vul[0] = MEF_GS_ALPHA_1_ADDITIVE;
    nreg = 5;
    n = pk->auxCount * nreg + 1;
    pk->current->us[0] |= nreg;
    pk->current += n;
    pk->count += n;
    i = 0;
    rec = (unsigned char *)state + 8;
    for (; i < 10; i++) {
        if (((short *)rec)[i + 168] != 0) {
            color = coltbl_0_00A518A0[i];
            lo = i < 6;
            off = 328;
            if (lo) {
                off = 320;
            }
            tex = (unsigned long *)(rec + off);
            for (j = 0; j < 3; j++) {
                if (MEfMake4Vertex2(verts, &((Vector4 (*)[3])state->points)[i][j], sztbl_1[i], ((float (*)[3])state->twist)[i][j]) == 0) {
                    Kosbw02QuadwordView *t;
                    Kosbw02QuadwordView *g;
                    unsigned long texVal;
                    int n1;
                    int n2;

                    gp = &state->draw_packet;
                    texReg = SCE_GS_TEX0_1;
                    regs = MEF_GS_TEXTURED_STRIP_REGS;
                    nloop = 3;
                    t = gp->current;
                    t[0].ul[0] = MEF_GS_SETUP_TAG;
                    t[0].ul[1] = MEF_GS_SETUP_REGS;
                    gp->auxCount = 1;
                    a1 = t + 1;
                    texVal = *tex;
                    a1->uc[8] = texReg;
                    a1->ul[0] = texVal;
                    n1 = gp->auxCount + 1;
                    gp->current->us[0] |= 1;
                    gp->current += n1;
                    gp->count += n1;
                    g = gp->current;
                    g[0].ul[0] = MEF_GS_TEXTURED_STRIP_TAG;
                    g[0].ul[1] = regs;
                    gp->auxCount = nloop;
                    g[1].q = McEfSTQtbl[0];
                    g[2].q = color;
                    g[3].q = *(Quadword *)&verts[0];
                    g[4].q = McEfSTQtbl[1];
                    g[5].q = color;
                    g[6].q = *(Quadword *)&verts[1];
                    g[7].q = McEfSTQtbl[2];
                    g[8].q = color;
                    g[9].q = *(Quadword *)&verts[2];
                    g[10].q = McEfSTQtbl[3];
                    g[11].q = color;
                    g[12].q = *(Quadword *)&verts[3];
                    n2 = gp->auxCount * 4 + 1;
                    gp->current->us[0] |= 4;
                    gp->current += n2;
                    gp->count += n2;
                    n1 = i;
                    n2 = i;
                    n = i;
                    nreg = i;
                    t = g;
                    lo = i;
                    off = i;
                    a1 = g;
                    a2 = g;
                    a3 = g;
                    a4 = g;
                    a5 = g;
                    q = g;
                }
            }
        }
    }
    if (state->frame < 26) {
        if (MEfMake4Vertex(verts, &state->position, 0.5f) == 0) {
            Kosbw02QuadwordView *t;
            Kosbw02QuadwordView *g;
            unsigned long texture;
            long modelRegs;
            int modelReg;
            int n1;
            int n2;

            t = pk->current;
            t[0].ul[0] = MEF_GS_SETUP_TAG;
            t[0].ul[1] = MEF_GS_SETUP_REGS;
            pk->auxCount = 1;
            a1 = t + 1;
            modelReg = SCE_GS_TEX0_1;
            modelRegs = MEF_GS_TEXTURED_STRIP_REGS;
            texture = state->model_texture;
            a1->uc[8] = modelReg;
            a1->ul[0] = texture;
            color = (Quadword)0xFF000000280000002800000028;
            n1 = pk->auxCount + 1;
            pk->current->us[0] |= 1;
            pk->current += n1;
            pk->count += n1;
            g = pk->current;
            g[0].ul[0] = MEF_GS_TEXTURED_STRIP_TAG;
            g[0].ul[1] = modelRegs;
            pk->auxCount = 3;
            g[1].q = McEfSTQtbl[0];
            g[2].q = color;
            g[3].q = *(Quadword *)&verts[0];
            g[4].q = McEfSTQtbl[1];
            g[5].q = color;
            g[6].q = *(Quadword *)&verts[1];
            g[7].q = McEfSTQtbl[2];
            g[8].q = color;
            g[9].q = *(Quadword *)&verts[2];
            g[10].q = McEfSTQtbl[3];
            g[11].q = color;
            g[12].q = *(Quadword *)&verts[3];
            n2 = pk->auxCount * 4 + 1;
            pk->current->us[0] |= 4;
            pk->current += n2;
            pk->count += n2;
        }
    }
    MGsGPTerm(pk);
}

extern void sefHitEffect(void);

/*
 * The KOSBW02 effect record: the work area MEfCreate_KOSBW02 (outside this
 * allocation) installs fnKOSBW02_PO000 into. `frame` is the same counter role
 * the sibling MSP02 effect keeps at this same offset (MspEffect,
 * src/ov01/m_ef_create_msp_02.c): incremented once per call, it fires
 * sefHitEffect at 25 and MEfObjDestroy at 35.
 */
typedef struct Kosbw02Effect {
    unsigned char unmodeled_00[0x70];
    int frame;
} Kosbw02Effect;

static void fnKOSBW02_PO000(void *self, Kosbw02Effect *work)
{
    work->frame++;
    if (work->frame == 25) {
        sefHitEffect();
    }
    if (work->frame >= 35) {
        MEfObjDestroy(self);
    }
}



static const Quadword coltbl_0_00A518A0[10] = {
    0x20000000800000008000000080,
    0x28000000800000008000000080,
    0x28000000800000008000000080,
    0x28000000800000008000000080,
    0x28000000800000008000000080,
    0x24000000800000008000000080,
    0x20000000800000008000000080,
    0x18000000800000008000000080,
    0x10000000800000008000000080,
    0x8000000800000008000000080
};

static const float sztbl_1[10] = { 0.10000000149011612f, 0.12999999523162842f, 0.15999999642372131f, 0.18000000715255737f, 0.20000000298023224f, 0.2199999988079071f, 0.23999999463558197f, 0.25999999046325684f, 0.2800000011920929f, 0.25f };
