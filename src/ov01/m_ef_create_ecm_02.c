/*
 * OV01 original TU 31: 0x00a38190..0x00a387a8 (5 functions)
 */
#include "common.h"
#include "m_ef_create.h"
#include "shared.h"

extern void MMathAddRotateVectorY(void *destination, float angle, const Vector4 *base, const Vector4 *offset);

typedef void (*ECM02Callback)(void *self, void *work);

typedef struct ECM02Packet {
    void *data;
    int capacity;
    void *current;
    int count;
    int aux_count;
} ECM02Packet;

typedef struct ECM02InitState {
    unsigned char unmodeled_00[0x30];
    Vector4 source_vector;
    unsigned char unmodeled_40[0x30];
    short frame;
    unsigned char unmodeled_72[0x0E];
    Vector4 copied_vector;
    u32 color[4];
    short visible[21];
    unsigned char unmodeled_ca[2];
    float radius[21];
    float angle[21];
    ECM02Packet packet;
} ECM02InitState;

typedef struct ECM02Effect {
    u32 unmodeled_00;
    ECM02Callback process_callback;
    u32 unmodeled_08;
    ECM02Callback draw_packet_callback;
    ECM02Callback post_process_callback;
    unsigned char unmodeled_14[0x0C];
    ECM02InitState state;
} ECM02Effect;

static void fnECM02_PR000(void *self, void *work);
static void fnECM02_DP000(void *self, void *work);
static void fnECM02_PO000(void *self, void *work);
extern void MGsGPInit(ECM02Packet *packet, void *address, int size);
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
    color_cursor = state->color;
    for (color_index = 2; color_index >= 0; color_index--) {
        *color_cursor = xglSRand() & 0x3F;
        if (*color_cursor > remaining_color) {
            *color_cursor = remaining_color;
        }
        remaining_color -= *color_cursor;
        color_cursor++;
    }

    state->color[3] = 255;
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

typedef struct ECM02Trail {
    unsigned char unmodeled_00[0x70];
    short frame;                /* +0x70 */
    unsigned char unmodeled_72[0x9E - 0x72];
    short unmodeled_9e;         /* +0x9E: cleared every frame; role beyond that unresolved */
    short visible[21];          /* +0xA0 */
    unsigned char unmodeled_ca[0xCC - 0xCA];
    float radius[21];           /* +0xCC */
    float angle[21];            /* +0x120 */
} ECM02Trail;

static void fnECM02_PR000(void *self, void *work)
{
    ECM02Trail *trail = (ECM02Trail *)work;
    int i;

    for (i = 19; i >= 0; i--) {
        trail->visible[i + 1] = trail->visible[i];
        trail->radius[i + 1] = trail->radius[i];
        trail->angle[i + 1] = trail->angle[i];
    }
    /* i == -1 here: this reaches unmodeled_9e, one slot before visible[0]. */
    trail->visible[i] = 0;

    if (trail->frame < ECM02_HIT_FRAME) {
        trail->visible[0] = 1;
        trail->radius[0] += 0.35f;
        trail->angle[0] += 0.31415924f;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ecm_02", fnECM02_DP000);

extern void sefHitEffect(void);

/*
 * work's layout beyond +0x70 is unresolved: the same per-frame lifetime
 * counter fnECM02_PR000 advances, expiring through MEfObjDestroy once it
 * reaches 0x37.
 */
#define ECM02_LIFETIME 0x37

typedef struct ECM02State {
    unsigned char unmodeled_00[0x70];
    short frame; /* +0x70 */
} ECM02State;

static void fnECM02_PO000(void *self, void *work)
{
    ECM02State *state = (ECM02State *)work;

    state->frame++;
    if (state->frame == ECM02_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= ECM02_LIFETIME) {
        MEfObjDestroy(self);
    }
}
