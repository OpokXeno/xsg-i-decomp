/*
 * OV01 original TU 33: 0x00a39088..0x00a397c0 (4 functions)
 */
#include "common.h"

/* MGsGPInit (src/ov01/m_gs.c, ov01 TU 18): still TU-local there, declared
 * here for this TU until its header is published. MEfCreate_EAD00 calls it
 * with a null address and zero size, so it targets MGsGPInit's own default
 * scratchpad buffer and capacity. */
extern void MGsGPInit(void *packet, void *address, int size);

/*
 * self is the effect object MEfObjExec1st/MEfObjExec2nd (src/main/m_ef_obj.c)
 * call back with (self, self's work area); MEfCreate_EAD00 installs its own
 * process, draw and lifetime callbacks in that object's callback slots.
 */
typedef void (*EAD00Callback)(void *self, void *work);

typedef struct EAD00Object {
    unsigned char unmodeled_00[4];
    EAD00Callback process; /* +0x04 */
    unsigned char unmodeled_08[4];
    EAD00Callback draw;     /* +0x0C */
    EAD00Callback lifetime; /* +0x10 */
} EAD00Object;

/*
 * MEfCreate_EAD00's own view of self's work area (self+0x20): the frame
 * counter fnEAD00_PO000 advances (see EAD00State below) and the MGs direct-
 * transfer packet MGsGPInit initializes at work+0x140; the packet's own
 * record belongs to MGsPacket (src/ov01/m_gs.c).
 */
typedef struct EAD00Init {
    unsigned char unmodeled_00[0x70];
    int frame; /* +0x70 */
    unsigned char unmodeled_74[0x140 - 0x74];
    unsigned char packet[0x14]; /* +0x140 */
} EAD00Init;

static void fnEAD00_PR000(void *self, void *work);
static void fnEAD00_DP000(void *self, void *work);
static void fnEAD00_PO000(void *self, void *work);

int MEfCreate_EAD00(void *self)
{
    EAD00Object *object = (EAD00Object *)self;
    EAD00Init *init = (EAD00Init *)((unsigned char *)self + 0x20);

    init->frame = 0;
    MGsGPInit(init->packet, 0, 0);
    object->process = fnEAD00_PR000;
    object->draw = fnEAD00_DP000;
    object->lifetime = fnEAD00_PO000;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ead_00", fnEAD00_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ead_00", fnEAD00_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void MEfObjDestroy(void *self);
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect and expiring through
 * MEfObjDestroy on the same frame, 0x46. */
#define EAD00_LIFETIME 0x46

typedef struct EAD00State {
    unsigned char unmodeled_00[0x70];
    int frame; /* +0x70 */
} EAD00State;

static void fnEAD00_PO000(void *self, void *work)
{
    EAD00State *state = (EAD00State *)work;

    state->frame++;
    if (state->frame == EAD00_LIFETIME) {
        sefHitEffect();
    }
    if (state->frame >= EAD00_LIFETIME) {
        MEfObjDestroy(self);
    }
}
