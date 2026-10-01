/*
 * OV01 original TU 24: 0x00a346c8..0x00a34e90 (5 functions)
 */
#include "common.h"
#include "m_ef_create.h"
#include "shared.h"
#include "m_ef_create_bp_00.h"

extern void MEfGetActorMatrix(Matrix4 destination, unsigned int actor, unsigned int part);
extern void MEfGetActorCoord(Vector4 *destination, unsigned int actor, unsigned int coordinate);
extern Vector4 *MMathApplyMatrix(Vector4 *destination, const Matrix4 *matrix, const Vector4 *vector);
extern Vector4 *MMathCalcVectorMatrix(Vector4 *destination, const Matrix4 *matrix);
extern void MGsGPInit(void *packet, void *address, int size);

static void fnBP00_PR000(void *self, void *work);
static void fnBP00_DP000(void *self, void *work);
static void fnBP00_PO000(void *self, void *work);

int MEfCreate_BP00(MEfObjRecord *self)
{
    /* This constructor interprets its own variant of the pooled work. */
    BP00Object *object = (BP00Object *)self;
    BP00Work *work = &object->work;
    Vector4 direction;
    Matrix4 actorMatrix;

    work->frame = 0;
    work->counter = 0;
    MEfGetActorMatrix(actorMatrix, work->actorId, work->actorPart);
    /* GCC 2.96 does not implicitly add const through a pointer to a typedef'd
       array; the cast changes only the read-only view of the same matrix. */
    MMathApplyMatrix(&work->position, (const Matrix4 *)&actorMatrix, &work->creationOffset);
    work->position.w = 1.0f;
    MEfGetActorCoord(&work->actorCoordinates, work->coordinateActorId, -1);
    memset(work->visibility, 0, sizeof(work->visibility));
    memset(work->trail, 0, sizeof(work->trail));
    work->visibility[0].flag[0] = 1;
    /* Quadword copy: the first trail point starts at the current position. */
    __asm__ __volatile__(
        "lq $8,0(%1)\n\t"
        "sq $8,0(%0)\n\t"
        :
        : "r"(work->trail[0].point), "r"(&work->position)
        : "$8", "memory");
    MMathCalcVectorMatrix(&direction, (const Matrix4 *)&actorMatrix);
    /* vectorOrigin[0].xyz = direction.xyz * -0.65, the first strand's
       per-frame step back along the actor's facing. */
    __asm__ __volatile__(
        "lqc2 $vf1,0(%1)\n\t"
        "mfc1 $8,%2\n\t"
        "qmtc2 $8,$vf2\n\t"
        "vmulx.xyz $vf1xyz,$vf1xyz,$vf2x\n\t"
        "sqc2 $vf1,0(%0)\n\t"
        :
        : "r"(&work->vectorOrigin[0]), "r"(&direction), "f"(-0.65f)
        : "$8", "memory");
    MGsGPInit(work->packet, 0, 0);

    object->processCallback = fnBP00_PR000;
    object->drawCallback = fnBP00_DP000;
    object->postCallback = fnBP00_PO000;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_bp_00", fnBP00_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_bp_00", fnBP00_PR010);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_bp_00", fnBP00_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect at frame 0x21 and expiring
 * through MEfObjDestroy once it reaches 0x2D. counter, right after it, is
 * only ever incremented here in lockstep with frame; nothing recovered in
 * this TU reads it back, so its specific role (an "age" naming borrowed
 * from the unrelated SolbState.age, src/ov01/m_ef_create_solb.h) is not
 * evidenced here and is left neutral. */
#define BP00_HIT_FRAME 0x21
#define BP00_LIFETIME  0x2D

typedef struct BP00State {
    unsigned char unmodeled_00[0x70];
    int frame;   /* +0x70 */
    int counter; /* +0x74 */
} BP00State;

static void fnBP00_PO000(void *self, void *work)
{
    BP00State *state = (BP00State *)work;

    state->frame++;
    state->counter++;
    if (state->frame == BP00_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= BP00_LIFETIME) {
        MEfObjDestroy(self);
    }
}
