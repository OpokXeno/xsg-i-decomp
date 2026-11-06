/*
 * OV01 original TU 33: 0x00a39088..0x00a397c0 (4 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"
#include "quadword.h"
#include "vu0_macro.h"
#include "main/m_ef_obj.h"
#include "main/xgl_packet.h"

typedef struct EAD00ProcessWork {
    unsigned char unmodeled_00[8];
    Actor *actor;
    int part_id;
    unsigned char unmodeled_10[0x60];
    int frame;
    unsigned char unmodeled_74[0x0c];
    Vector4 start[3];
    Vector4 end[3];
    Vector4 displacement[3];
    Vector4 angle[3];
    MEfGsPacket packet;
} EAD00ProcessWork;

extern Vector4 *MMathSubVectorDivS(Vector4 *destination, const Vector4 *first,
                                    const Vector4 *second, float divisor);
extern Vector4 *MEfCalcAngle(Vector4 *destination, const Vector4 *from,
                           const Vector4 *to);

/* MGsGPInit (src/ov01/m_gs.c, ov01 TU 18): still TU-local there, declared
 * here for this TU until its header is published. MEfCreate_EAD00 calls it
 * with a null address and zero size, so it targets MGsGPInit's own default
 * scratchpad buffer and capacity. */
extern void MGsGPInit(void *packet, void *address, int size);
extern MEfObjCamParams mefCamParams;

/*
 * self is the effect object MEfObjExec1st/MEfObjExec2nd (src/main/m_ef_obj.c)
 * call back with (self, self's work area); MEfCreate_EAD00 installs its own
 * process, draw and lifetime callbacks in that object's callback slots.
 */
typedef void (*EAD00Callback)(void *self, EAD00ProcessWork *work);

/* The pooled effect object: callbacks in its prefix, the work record at +0x20
 * (its frame at +0x70 and the MGs direct-transfer packet at +0x140 are MEfCreate_EAD00's). */
typedef struct EAD00Object {
    unsigned char unmodeled_00[4];
    EAD00Callback process; /* +0x04 */
    unsigned char unmodeled_08[4];
    EAD00Callback draw;     /* +0x0C */
    EAD00Callback lifetime; /* +0x10 */
    unsigned char unmodeled_14[0x20 - 0x14];
    EAD00ProcessWork work; /* +0x20 */
} EAD00Object;

static void fnEAD00_PR000(void *self, EAD00ProcessWork *work);
static void fnEAD00_DP000(void *self, EAD00ProcessWork *work);
static void fnEAD00_PO000(void *self, EAD00ProcessWork *work);

int MEfCreate_EAD00(MEfObjRecord *self)
{
    EAD00Object *object = (EAD00Object *)self;

    object->work.frame = 0;
    MGsGPInit(&object->work.packet, 0, 0);
    object->process = fnEAD00_PR000;
    object->draw = fnEAD00_DP000;
    object->lifetime = fnEAD00_PO000;
    return 1;
}

/* From this frame on the strip fades out (both fnEAD00_PR000 and fnEAD00_DP000). */
#define EAD00_FADE_START 62

static void fnEAD00_PR000(void *self, EAD00ProcessWork *work)
{
    EAD00ProcessWork *state = work;
    Vector4 first_point;
    Vector4 second_point;
    Matrix4 actor_matrix;
    Vector4 transformed_point;
    float frame_scale;
    Vector4 *matrix_input;
    int remaining;
    unsigned int point_index;
    Actor *actor;
    int part_id;

    /* VU0's constant vf0 supplies zero xyz and unit w for both local points. */
    __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&first_point) : "memory");
    first_point.x = -0.1f;
    first_point.z = -0.2f;
    __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&second_point) : "memory");
    actor = state->actor;
    second_point.x = -9.0f;
    part_id = state->part_id;
    second_point.z = 28.0f;

    MEfGetActorMatrix(&actor_matrix, actor, part_id);
    frame_scale = 1.0f;
    if (state->frame < 8) {
        frame_scale = (float)state->frame * 0.125f;
    }
    if (state->frame >= EAD00_FADE_START) {
        frame_scale = (float)(69 - state->frame) * 0.125f;
    }

    matrix_input = &transformed_point;
    for (point_index = 0, remaining = 2; remaining >= 0; ) {
        /* Copy the complete aligned vector through the original scratch GPR. */
        __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)"
                             : : "r"(matrix_input), "r"(&first_point)
                             : "$8", "memory");
        transformed_point.x *= frame_scale;
        MMathApplyMatrix(&state->start[point_index], &actor_matrix,
                         matrix_input);
        first_point.x += 0.1f;

        __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)"
                             : : "r"(matrix_input), "r"(&second_point)
                             : "$8", "memory");
        remaining--;
        transformed_point.x *= frame_scale;
        MMathApplyMatrix(&state->end[point_index], &actor_matrix,
                         matrix_input);
        second_point.x += 9.0f;

        MMathSubVectorDivS(&state->displacement[point_index],
                           &state->end[point_index], &state->start[point_index], 50.0f);
        MEfCalcAngle(&state->angle[point_index], &state->start[point_index],
                      &state->end[point_index]);
        point_index++;
    }
}

extern void MGsGPTerm(MEfGsPacket *packet);
extern int MMathRotTransPersClip(Vector4 *destination, const StudioCamera *camera,
                                 Matrix4 *matrix, const Vector4 *point);
extern Matrix4 *MMathRotateMatrixYXZ(Matrix4 *destination, Matrix4 *matrix,
                                     const Vector4 *angles);
static MEfGsQword coltbl_0_00A437B0[3];

#define EAD00_RAYS 4
#define EAD00_SEGMENTS 51

static inline MEfGsQword *ead00OpenTag(MEfGsPacket *packet, int pre, int prim, int reg_count,
                                         u64 registers)
{
    MEfGsQword *qword = packet->current;

    qword->gif.tag = GIF_TAG_EOP | ((u64)pre << 46) | ((u64)prim << 47) | ((u64)reg_count << 60);
    qword->gif.registers = registers;
    packet->reg_count = reg_count;
    return qword + 1;
}

static inline void ead00SetRegister(MEfGsQword *qword, unsigned char address, u64 data)
{
    qword->ad.data = data;
    qword->ad.address = address;
}

static inline void ead00CloseTag(MEfGsPacket *packet, int nloop)
{
    int size;

    size = packet->reg_count * nloop + 1;
    packet->current->nloop |= nloop;
    packet->current += size;
    packet->count += size;
}

static void fnEAD00_DP000(void *self, EAD00ProcessWork *work)
{
    EAD00ProcessWork *state = work;
    MEfGsQword position;
    Vector4 angles;
    MEfGsQword colour[2];
    Vector4 point;
    Matrix4 rotation;
    MEfGsQword vertex[2];
    StudioCamera *screen;
    Matrix4 *view;
    MEfGsPacket *first;
    MEfGsPacket *packet;
    MEfGsQword *setting;
    MEfGsQword *qword;
    float phase;
    float ray_angle;
    float width;
    int clip;
    int fan;
    int ray;
    int segment;
    int drawn;

    screen = mefCamParams.screen;
    view = mefCamParams.matrix;
    first = &state->packet;
    MGsGPInit(first, 0, 0);
    setting = first->current;
    setting[0].gif.tag = MEF_GS_SETUP_TAG;
    setting[0].gif.registers = MEF_GS_SETUP_REGS;
    first->reg_count = 1;
    ead00SetRegister(&setting[1], SCE_GS_TEST_1, MEF_GS_TEST_1_ALPHA_GREATER_ZGEQUAL);
    ead00SetRegister(&setting[2], SCE_GS_ZBUF_1, MEF_GS_ZBUF_1_PSMZ24_NO_DEPTH_WRITE);
    ead00SetRegister(&setting[3], SCE_GS_ALPHA_1, MEF_GS_ALPHA_1_ADDITIVE);
    ead00CloseTag(first, 3);
    MGsGPTerm(first);
    packet = first;
    first = 0;

    phase = (float)state->frame * -0.31415927f;
    colour[0].whole = coltbl_0_00A437B0[0].whole;
    colour[1].whole = coltbl_0_00A437B0[1].whole;
    for (fan = 0; fan < 3; fan++) {
        Vector4 *start;
        Vector4 *displacement;
        Vector4 *angle;

        MGsGPInit(packet, 0, 0);
        for (ray = 0; ray < EAD00_RAYS; ray++) {
            start = &state->start[fan];
            displacement = &state->displacement[fan];
            angle = &state->angle[fan];
            clip = 0;
            ray_angle = (float)ray * 1.5707963f + phase;
            QuadwordCopyT0(&position, start);
            qword = ead00OpenTag(&state->packet, 1, MEF_GS_STRIP_PRIM, 4, MEF_GS_STRIP_REGS);
            drawn = 0;
            for (segment = 0; segment < EAD00_SEGMENTS; segment++) {
                colour[0].channel[3] = 255;
                colour[1].channel[3] = 80;
                width = 0.05f;
                if (segment == 0 || segment == EAD00_SEGMENTS - 1) {
                    colour[0].channel[3] = 0;
                    colour[1].channel[3] = 0;
                    width = 0.0f;
                } else {
                    int fade = 255 - segment * 3;
                    int tail = 80 - segment;

                    colour[0].channel[3] = fade < 0 ? 0 : fade;
                    colour[1].channel[3] = tail < 0 ? 0 : tail;
                }
                if (state->frame >= EAD00_FADE_START) {
                    colour[0].channel[3] = colour[0].channel[3] * (70 - state->frame) / 8;
                    colour[1].channel[3] = colour[1].channel[3] * (70 - state->frame) / 8;
                }
                QuadwordCopyT0(&angles, angle);
                angles.z = ray_angle + (float)segment * 0.5235988f;
                MMathRotateMatrixYXZ(&rotation, 0, &angles);
                __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&position) : "memory");
                __asm__ __volatile__("vmove.w vf1, vf0" : : : "memory");
                __asm__ __volatile__("sqc2 vf1, 48(%0)" : : "r"(&rotation) : "memory");
                __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&position) : "memory");
                __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(displacement) : "memory");
                __asm__ __volatile__("vadd.xyz vf1, vf1, vf2" : : : "memory");
                __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(&position) : "memory");
                __asm__ __volatile__("lqc2 vf1, 48(%0)" : : "r"(&rotation) : "memory");
                __asm__ __volatile__("vmove.w vf1, vf0" : : : "memory");
                __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(&point) : "memory");

                if (MMathRotTransPersClip(&vertex[0].vector, screen, view, &point)) {
                    clip = 2;
                } else if (clip != 0) {
                    clip--;
                    vertex[0].xyz.adc = MEF_GS_XYZ_ADC;
                }
                Vu0SetOrigin(&point);
                point.x = width;
                MMathApplyMatrix(&point, &rotation, &point);
                if (MMathRotTransPersClip(&vertex[1].vector, screen, view, &point)) {
                    clip = 2;
                } else if (clip != 0) {
                    clip--;
                    vertex[1].xyz.adc = MEF_GS_XYZ_ADC;
                }
                qword[0].whole = colour[0].whole;
                qword[1].whole = vertex[0].whole;
                qword[2].whole = colour[1].whole;
                qword[3].whole = vertex[1].whole;
                qword += 4;
                drawn++;
            }
            ead00CloseTag(packet, drawn);
        }
        MGsGPTerm(packet);
    }
}

/* MEfObjDestroy is defined by src/main/m_ef_obj.c; sefHitEffect
 * is defined by src/main/sef.c. */
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect and expiring through
 * MEfObjDestroy on the same frame, 0x46. */
#define EAD00_LIFETIME 0x46

static void fnEAD00_PO000(void *self, EAD00ProcessWork *work)
{
    EAD00ProcessWork *state = work;

    state->frame++;
    if (state->frame == EAD00_LIFETIME) {
        sefHitEffect();
    }
    if (state->frame >= EAD00_LIFETIME) {
        MEfObjDestroy(self);
    }
}



static MEfGsQword coltbl_0_00A437B0[3] = {
    { .channel = { 0x80, 0x20, 0x20, 0xff } },
    { .channel = { 0x80, 0x10, 0x10, 0x50 } },
    { .channel = { 0x40, 0xff, 0x40, 0xff } }
};
