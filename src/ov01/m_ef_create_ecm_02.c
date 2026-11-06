/*
 * OV01 original TU 31: 0x00a38190..0x00a387a8 (5 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "shared.h"
#include "main/m_ef_obj.h"
#include "main/xgl_packet.h"

extern void MMathAddRotateVectorY(void *destination, float angle, const Vector4 *base, const Vector4 *offset);

typedef struct ECM02InitState {
    unsigned char unmodeled_00[0x30];
    Vector4 source_vector;
    unsigned char unmodeled_40[0x30];
    short frame;
    unsigned char unmodeled_72[0x0E];
    Vector4 copied_vector;
    MEfGsQword color; /* one quadword: fnECM02_DP000 reads it with a single lq (0xA3851C); MEfCreate_ECM02 bytes unchanged (gate) */
    short visible[21];
    unsigned char unmodeled_ca[2];
    float radius[21];
    float angle[21];
    MEfGsPacket packet;
} ECM02InitState;

typedef void (*ECM02Callback)(void *self, ECM02InitState *work);

typedef struct ECM02Effect {
    u32 unmodeled_00;
    ECM02Callback process_callback;
    u32 unmodeled_08;
    ECM02Callback draw_packet_callback;
    ECM02Callback post_process_callback;
    unsigned char unmodeled_14[0x0C];
    ECM02InitState state;
} ECM02Effect;

static void fnECM02_PR000(void *self, ECM02InitState *work);
static void fnECM02_DP000(void *self, ECM02InitState *work);
static void fnECM02_PO000(void *self, ECM02InitState *work);
extern void MGsGPInit(MEfGsPacket *packet, void *address, int size);
extern float MMathMakeRandom2PI(void);

int MEfCreate_ECM02(MEfObjRecord *self)
{
    ECM02Effect *effect = (ECM02Effect *)self;
    ECM02InitState *state = &effect->state;
    u32 *color_cursor;
    u32 remaining_color;
    int color_index;

    state->frame = 0;
    /* Preserve the original four-word vector before replacing its second float. */
    __asm__ __volatile__(
        "lq $8, 0(%1)\n\t"
        "sq $8, 0(%0)"
        :
        : "r"(&state->copied_vector), "r"(&state->source_vector)
        : "$8", "memory"
    );
    state->copied_vector.y = 11.0f;
    memset(state->visible, 0, sizeof(state->visible));
    memset(state->radius, 0, sizeof(state->radius));
    memset(state->angle, 0, sizeof(state->angle));

    remaining_color = 128;
    color_cursor = state->color.channel;
    for (color_index = 2; color_index >= 0; color_index--) {
        *color_cursor = xglSRand() & 0x3F;
        if (*color_cursor > remaining_color) {
            *color_cursor = remaining_color;
        }
        remaining_color -= *color_cursor;
        color_cursor++;
    }

    state->color.channel[3] = 255;
    state->visible[0] = 1;
    state->radius[0] = 0.0f;
    state->angle[0] = MMathMakeRandom2PI();
    MGsGPInit(&state->packet, 0, 0);

    effect->process_callback = fnECM02_PR000;
    effect->draw_packet_callback = fnECM02_DP000;
    effect->post_process_callback = fnECM02_PO000;
    return 1;
}

static void makeCoord(void *destination, float radius, float angle, void *effect)
{
    unsigned char *base = (unsigned char *)effect;
    Vector4 local;

    /* VF00 is hardwired to (0.0, 0.0, 0.0, 1.0); this
     * materializes that constant into "local" in one quadword store instead of
     * four scalar stores. Only z (0.0) and w (1.0) survive from this store: x
     * and y are overwritten immediately below. */
    __asm__ __volatile__(
        "sqc2 $vf0, 0(%1)"
        : "=m"(local)
        : "r"(&local)
        : "memory"
    );

    local.x = radius * 0.454545468f + 0.5f;
    local.y = -radius;

    MMathAddRotateVectorY(destination, angle, (Vector4 *)(base + 0x80), &local);
}

/*
 * work's layout beyond +0x70 pairs a per-frame lifetime counter with three
 * parallel 21-entry trails (visible flag, radius, angle) for the head this
 * function advances: index 0 is the value fnECM02_PR000 updates each frame,
 * indices 1..20 its trailing history, oldest last.
 */
#define ECM02_HIT_FRAME 0x23

static void fnECM02_PR000(void *self, ECM02InitState *work)
{
    ECM02InitState *state = work;
    int i;

    for (i = 19; i >= 0; i--) {
        state->visible[i + 1] = state->visible[i];
        state->radius[i + 1] = state->radius[i];
        state->angle[i + 1] = state->angle[i];
    }
    /* i == -1 here: the store reaches visible[-1], the two bytes before
     * visible[0] that lie in the colour quadword. */
    state->visible[i] = 0;

    if (state->frame < ECM02_HIT_FRAME) {
        state->visible[0] = 1;
        state->radius[0] += 0.35f;
        state->angle[0] += 0.31415924f;
    }
}

extern void MGsGPTerm(MEfGsPacket *packet);
extern int MMathRotTransPersClip(Vector4 *destination, const StudioCamera *camera,
                                 Matrix4 *matrix, const Vector4 *point);
extern MEfObjCamParams mefCamParams;

#define ECM02_TRAIL_DRAWN 20
#define ECM02_FADE_START 10


/* Writes the GIF tag that opens the packet's next primitive and returns
 * the first data quadword after it. */
static inline MEfGsQword *ecm02OpenTag(MEfGsPacket *packet, int pre, int prim, int reg_count,
                                         u64 registers)
{
    MEfGsQword *qword = packet->current;

    qword->gif.tag = GIF_TAG_EOP | ((u64)pre << 46) | ((u64)prim << 47) | ((u64)reg_count << 60);
    qword->gif.registers = registers;
    packet->reg_count = reg_count;
    return qword + 1;
}

/* Writes one A+D register setting. */
static inline void ecm02SetRegister(MEfGsQword *qword, unsigned char address, u64 data)
{
    qword->ad.data = data;
    qword->ad.address = address;
}

/* Stores NLOOP into the open GIF tag and moves past its data. */
static inline void ecm02CloseTag(MEfGsPacket *packet, int nloop)
{
    int size;

    size = packet->reg_count * nloop + 1;
    packet->current->nloop |= nloop;
    packet->current += size;
    packet->count += size;
}

static void fnECM02_DP000(void *self, ECM02InitState *work)
{
    ECM02InitState *state = work;
    MEfGsQword colour[2];
    Vector4 point[2];
    MEfGsQword vertex[2];
    StudioCamera *screen;
    Matrix4 *view;
    MEfGsPacket *packet;
    MEfGsQword *setting;
    MEfGsQword *qword;
    float width;
    int clip;
    int drawn;
    int side;
    int segment;

    screen = mefCamParams.screen;
    view = mefCamParams.matrix;
    packet = &state->packet;
    MGsGPInit(packet, 0, 0);
    setting = packet->current;
    setting[0].gif.tag = MEF_GS_SETUP_TAG;
    setting[0].gif.registers = MEF_GS_SETUP_REGS;
    packet->reg_count = 1;
    ecm02SetRegister(&setting[1], SCE_GS_TEST_1, MEF_GS_TEST_1_ALPHA_GREATER_ZGEQUAL);
    ecm02SetRegister(&setting[2], SCE_GS_ZBUF_1, MEF_GS_ZBUF_1_PSMZ24_NO_DEPTH_WRITE);
    ecm02SetRegister(&setting[3], SCE_GS_ALPHA_1, MEF_GS_ALPHA_1_ADDITIVE);
    ecm02CloseTag(packet, 3);

    for (side = -1; side < 2; side += 2) {
        clip = 0;
        drawn = 0;
        qword = ecm02OpenTag(&state->packet, 1, MEF_GS_STRIP_PRIM, 4, MEF_GS_STRIP_REGS);
        for (segment = 0; segment < ECM02_TRAIL_DRAWN; segment++) {
            if (state->visible[segment] == 0) {
                continue;
            }
            colour[1].whole = colour[0].whole = state->color.whole;
            colour[1].channel[3] = 32;
            if (segment >= ECM02_FADE_START) {
                colour[0].channel[3] = colour[0].channel[3] * (20 - segment) / 10;
                colour[1].channel[3] = colour[1].channel[3] * (20 - segment) / 10;
            }
            if (segment != 0 && segment != ECM02_TRAIL_DRAWN - 1) {
                width = (float)side * 0.15f;
            } else {
                width = 0.0f;
            }
            makeCoord(&point[0], state->radius[segment], state->angle[segment], state);
            makeCoord(&point[1], state->radius[segment] + width, state->angle[segment], state);

            if (MMathRotTransPersClip(&vertex[0].vector, screen, view, &point[0])) {
                clip = 2;
            } else if (clip != 0) {
                clip--;
                vertex[0].xyz.adc = MEF_GS_XYZ_ADC;
            }
            if (MMathRotTransPersClip(&vertex[1].vector, screen, view, &point[1])) {
                clip = 2;
            } else if (clip != 0) {
                clip--;
                vertex[1].xyz.adc = MEF_GS_XYZ_ADC;
            }
            drawn++;
            qword[0].whole = colour[0].whole;
            qword[1].whole = vertex[0].whole;
            qword[2].whole = colour[1].whole;
            qword[3].whole = vertex[1].whole;
            qword += 4;
        }
        if (drawn > 0) {
            ecm02CloseTag(packet, drawn);
        }
    }
    MGsGPTerm(packet);
}

extern void sefHitEffect(void);

/*
 * work's layout beyond +0x70 is unresolved: the same per-frame lifetime
 * counter fnECM02_PR000 advances, expiring through MEfObjDestroy once it
 * reaches 0x37.
 */
#define ECM02_LIFETIME 0x37

static void fnECM02_PO000(void *self, ECM02InitState *work)
{
    ECM02InitState *state = work;

    state->frame++;
    if (state->frame == ECM02_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= ECM02_LIFETIME) {
        MEfObjDestroy(self);
    }
}
