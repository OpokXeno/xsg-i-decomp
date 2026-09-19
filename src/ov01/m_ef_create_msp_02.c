/*
 * OV01 original TU 28: 0x00a360e0..0x00a36be8 (6 functions)
 */
#include "common.h"
#include "shared.h"

extern float MMathCalcRotNear(float first, float second);
static float adjAngle(float first, float second, float bound);

extern float MMathMakeRandom(void);
extern float MMathMakeRandom2PI(void);
extern float MMathCalcLengthXZ(Vector4 *vector);
extern float MMathCalcLengthYZ(Vector4 *vector);
extern void MMathCalcAngle(Vector4 *angles, Vector4 *from, Vector4 *to);
extern void MMathVectorInterpolation(Vector4 *destination, Vector4 *first,
                                     Vector4 *second, float parameter);
extern void MMathRotateMatrixYX(Matrix4 matrix, int flags, Vector4 *angles);
extern void MMathApplyMatrix(Vector4 *destination, Matrix4 matrix, Vector4 *source);
extern float tanf(float value);

/*
 * The MSP02 effect record: the work area the MEf effect object carries at its
 * own byte offset 0x20 (MEfCreate_MSP02 takes the object in $4, derives the
 * record as $4+0x20 and installs this TU's four callbacks at object+0x04..0x10).
 * Every member below is evidenced by an access in this TU; the spans whose
 * interior this TU never resolves keep the one role it does evidence for them,
 * and nothing here is sized to round the record out (docs/naming.md).
 *
 *   creationParameters  read, never written, by MEfCreate_MSP02 alone: the
 *                       actor/part pairs it passes to MEfGetActorMatrix
 *                       (+0x08, +0x0c) and MEfGetActorCoord (+0x24, +0x28),
 *                       the Vector4 at +0x10 it rotates into spawnPoint, and
 *                       the pair fnMSP02_DM000 passes to MEfDrawModel (+0x40,
 *                       +0x44). Their producer is outside this TU, so their
 *                       individual roles are not recovered here.
 *   frame               cleared by MEfCreate_MSP02, incremented by
 *                       fnMSP02_PO000 (which fires sefHitEffect at 20 and
 *                       MEfObjDestroy at 30), gated against 20 here and in
 *                       fnMSP02_DM000.
 *   spawnPoint          MMathApplyMatrix writes it from the actor matrix and
 *                       the creation offset; MEfCreate_MSP02 copies it into
 *                       position, previousPosition, target and previousTarget.
 *   circumPoint         MEfCalcCircumXZ writes it; velocity is the step from
 *                       spawnPoint to it divided by twenty.
 *   position .. turn    seven contiguous Vector4 records, all seeded in order
 *                       by MEfCreate_MSP02. turn is a whole Vector4 and not
 *                       two floats: MMathCalcAngle writes four lanes into it
 *                       and fnMSP02_DM000 copies all sixteen bytes with one
 *                       lq/sq pair. This function uses only its x and y lanes.
 *   spread, angle       0.2 and MMathMakeRandom2PI() at creation, rewritten
 *                       here every seventh frame.
 *   drawPacket          MEfCreate_MSP02 calls MGsGPInit on +0x118 and stores
 *                       the two eight-byte svGetPrmFromName results at +0x130
 *                       and +0x138; fnMSP02_DP000 re-initialises the same
 *                       packet every frame, walks its pointer/counter fields
 *                       and picks one of the two eight-byte values per slot.
 *                       Its interior belongs to the MGs packet builder, which
 *                       is outside this TU.
 *   active              MEfCreate_MSP02 memsets exactly 0x14 bytes here;
 *                       fnMSP02_DP000 walks ten halfwords and skips the empty
 *                       slots. This function loads and stores them unsigned.
 *   points, twist       MEfCreate_MSP02 memsets 0x140 bytes at +0x160 and
 *                       0x50 bytes at +0x2a0 -- exactly these two arrays, and
 *                       the record ends there; fnMSP02_DP000 walks both in
 *                       step, sixteen and four bytes per point.
 *
 * The two twelve-byte spans at +0x074 and +0x154 carry no member. Each sits
 * immediately before a quadword field that this TU reaches with lq/sq/lqc2/
 * sqc2, which the R5900 only accepts on a sixteen-byte boundary, so the
 * original's vector type is sixteen-byte aligned and the compiler inserted
 * exactly these two spans itself. This dialect has no way to spell that
 * alignment without an attribute the route does not admit, so the spans are
 * declared, named for the alignment they reproduce and for nothing else, and
 * they remain the one part of this type a reviewer must pass on.
 *
 * The trail is a ten-slot history: `active` says whether a slot holds a
 * segment, `points` holds its two endpoints and `twist` their two roll angles.
 * Every frame the whole history is shifted one slot down and slot 0 is refilled.
 */
#define MSP02_TRAIL_SLOTS 10
#define MSP02_TRAIL_POINTS 2
/* Record geometry the trail shift below walks directly. */
#define MSP02_POINT_BYTES 0x10
#define MSP02_SLOT_BYTES (MSP02_TRAIL_POINTS * MSP02_POINT_BYTES)
#define MSP02_POINTS_OFFSET 0x160

typedef struct MspEffect {
    unsigned char creationParameters[0x70];                        /* 0x000 */
    int frame;                                                     /* 0x070 */
    unsigned char alignmentBeforeSpawnPoint[0xc];                  /* 0x074 */
    Vector4 spawnPoint;                                            /* 0x080 */
    Vector4 circumPoint;                                           /* 0x090 */
    Vector4 position;                                              /* 0x0a0 */
    Vector4 previousPosition;                                      /* 0x0b0 */
    Vector4 velocity;                                              /* 0x0c0 */
    Vector4 rotation;                                              /* 0x0d0 */
    Vector4 target;                                                /* 0x0e0 */
    Vector4 previousTarget;                                        /* 0x0f0 */
    Vector4 turn;                                                  /* 0x100 */
    float spread;                                                  /* 0x110 */
    float angle;                                                   /* 0x114 */
    unsigned char drawPacket[0x28];                                /* 0x118 */
    unsigned short active[MSP02_TRAIL_SLOTS];                      /* 0x140 */
    unsigned char alignmentBeforePoints[0xc];                      /* 0x154 */
    Vector4 points[MSP02_TRAIL_SLOTS][MSP02_TRAIL_POINTS];         /* 0x160 */
    float twist[MSP02_TRAIL_SLOTS][MSP02_TRAIL_POINTS];            /* 0x2a0 */
} MspEffect;

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_msp_02", MEfCreate_MSP02);

/*
 * fnMSP02_PR000: per-frame update of the MSP02 effect: while the effect is younger than
 * twenty frames it steers a randomly swinging emitter toward its target, then
 * it shifts the ten-slot trail history down by one slot and refills slot 0.
 * Only the second parameter is read; the first is the effect-task slot of the
 * shared MEf per-frame callback signature.
 */
static void fnMSP02_PR000(void *task, MspEffect *effect)
{
    Vector4 work;
    Matrix4 matrix;
    Vector4 tip;
    Vector4 aim;
    Vector4 lead;
    float turn;
    unsigned char *record = (unsigned char *)effect;
    int trail;
    int slotOffset;
    int point;

    if (effect->frame < 20) {
        /* Quadword copy: previousPosition = position (VU0 library copy). */
        __asm__ __volatile__(
            "lq $8, 0(%1)\n"
            "sq $8, 0(%0)\n"
            :
            : "r"(&effect->previousPosition), "r"(&effect->position)
            : "$8", "memory");
        /* position.xyz += velocity.xyz, leaving position.w alone. */
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%1)\n"
            "lqc2 $vf2, 0(%2)\n"
            "vadd.xyz $vf1, $vf1, $vf2\n"
            "sqc2 $vf1, 0(%0)\n"
            :
            : "r"(&effect->position), "r"(&effect->position), "r"(&effect->velocity)
            : "memory");
        /* Quadword copy: previousTarget = target. */
        __asm__ __volatile__(
            "lq $8, 0(%1)\n"
            "sq $8, 0(%0)\n"
            :
            : "r"(&effect->previousTarget), "r"(&effect->target)
            : "$8", "memory");

        /* Every seventh frame the emitter picks a new swing. */
        if (effect->frame % 7 == 0) {
            if (effect->frame >= 13) {
                effect->spread = 0.1f;
                effect->angle += 3.14159274f;
            } else {
                effect->spread = MMathMakeRandom() * 0.3f + 0.6f;
                effect->angle += MMathMakeRandom() * 1.3962634f + 2.4434609f;
            }
        }

        MMathRotateMatrixYX(matrix, 0, &effect->rotation);
        /* matrix[3] = position, keeping the W lane of vf0. */
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%1)\n"
            "vmove.w $vf1, $vf0\n"
            "sqc2 $vf1, 48(%0)\n"
            :
            : "r"(matrix), "r"(&effect->position)
            : "memory");

        /* work = vf0, the VU0 constant (0, 0, 0, 1). */
        __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&work) : "memory");
        {
            /* The swing offset in the rotated frame, in polar form: the two
             * circular components of the current angle, scaled by the spread.
             * They come from the VU0 microprogram, whose entry 0xe8 is
             * Vu0CallCos and whose entry 0x20 is Vu0CallSin
             * (src/main/vu0/Vu0MicroCode.dvp),
             * so the cosine is computed first and lands in x. The angle goes in
             * through VF4 and the result comes back from VF1, so each one
             * crosses between the FPU register the EABI keeps a float in and the
             * GPR the quadword transfer reads: that is what the mfc1/mtc1 pair
             * is for. */
            float cosine;
            float sine;

            __asm__ __volatile__(
                "mfc1 $8, %1\n"
                "qmtc2.ni $8, $vf4\n"
                "vcallms 0xe8\n"
                "qmfc2.i $8, $vf1\n"
                "mtc1 $8, %0\n"
                : "=f"(cosine)
                : "f"(effect->angle)
                : "$8", "memory");
            work.x = cosine * effect->spread;

            __asm__ __volatile__(
                "mfc1 $8, %1\n"
                "qmtc2.ni $8, $vf4\n"
                "vcallms 0x20\n"
                "qmfc2.i $8, $vf1\n"
                "mtc1 $8, %0\n"
                : "=f"(sine)
                : "f"(effect->angle)
                : "$8", "memory");
            work.y = sine * effect->spread;
        }

        /* The swung emitter tip, and the same point pushed one unit forward. */
        MMathApplyMatrix(&tip, matrix, &work);
        work.z = 1.0f;
        MMathApplyMatrix(&work, matrix, &work);

        /* Turn toward the target, bounded to six degrees of yaw and five of
         * pitch per frame. */
        MMathCalcAngle(&aim, &effect->target, &work);
        work.x = adjAngle(effect->turn.x, aim.x, 0.10471976f);
        work.y = adjAngle(effect->turn.y, aim.y, 0.08726646f);
        effect->turn.x = work.x;
        effect->turn.y = work.y;

        /* lead = vf0; work = tip - target (xyz). */
        __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&lead) : "memory");
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%1)\n"
            "lqc2 $vf2, 0(%2)\n"
            "vsub.xyz $vf1, $vf1, $vf2\n"
            "sqc2 $vf1, 0(%0)\n"
            :
            : "r"(&work), "r"(&tip), "r"(&effect->target)
            : "memory");

        /* How far the target has to travel sideways to reach the new heading:
         * the distance left to cover, times the tangent of the angle still to
         * be turned through on that axis. */
        turn = MMathCalcRotNear(effect->rotation.x, effect->turn.x);
        lead.y = MMathCalcLengthYZ(&work) * tanf(turn);
        turn = MMathCalcRotNear(effect->rotation.y, effect->turn.y);
        lead.x = MMathCalcLengthXZ(&work) * tanf(turn);

        /* work = target + velocity (xyz), and that becomes the translation the
         * lead offset is rotated about. */
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%1)\n"
            "lqc2 $vf2, 0(%2)\n"
            "vadd.xyz $vf1, $vf1, $vf2\n"
            "sqc2 $vf1, 0(%0)\n"
            :
            : "r"(&work), "r"(&effect->target), "r"(&effect->velocity)
            : "memory");
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%1)\n"
            "vmove.w $vf1, $vf0\n"
            "sqc2 $vf1, 48(%0)\n"
            :
            : "r"(matrix), "r"(&work)
            : "memory");
        MMathApplyMatrix(&effect->target, matrix, &lead);
    }

    /*
     * Shift the whole trail history down one slot. The endpoints are copied
     * through the record's byte offsets: one slot is MSP02_SLOT_BYTES further
     * on than the previous one, which is the single address chain the original
     * walks for both endpoints of a slot.
     */
    for (trail = MSP02_TRAIL_SLOTS - 2, slotOffset = (MSP02_TRAIL_SLOTS - 2) * MSP02_SLOT_BYTES;
         trail >= 0; slotOffset -= MSP02_SLOT_BYTES, trail--) {
        effect->active[trail + 1] = effect->active[trail];
        for (point = 0; point < MSP02_TRAIL_POINTS; point++) {
            __asm__ __volatile__(
                "lq $8, 0(%1)\n"
                "sq $8, 0(%0)\n"
                :
                : "r"(&record[slotOffset + MSP02_POINTS_OFFSET + MSP02_SLOT_BYTES
                              + point * MSP02_POINT_BYTES]),
                  "r"(&record[slotOffset + MSP02_POINTS_OFFSET + point * MSP02_POINT_BYTES])
                : "$8", "memory");
            effect->twist[trail + 1][point] = effect->twist[trail][point];
        }
    }

    effect->active[0] = 0;
    for (point = 0; point < MSP02_TRAIL_POINTS; point++)
        __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&effect->points[0][point]) : "memory");

    if (effect->frame < 20) {
        effect->active[0] = 1;
        for (point = 0; point < MSP02_TRAIL_POINTS; point++) {
            MMathVectorInterpolation(&work, &effect->target, &effect->previousTarget,
                                     (float)point * 0.5f);
            MMathRotateMatrixYX(matrix, 0, &effect->rotation);
            __asm__ __volatile__(
                "lqc2 $vf1, 0(%1)\n"
                "vmove.w $vf1, $vf0\n"
                "sqc2 $vf1, 48(%0)\n"
                :
                : "r"(matrix), "r"(&work)
                : "memory");

            /* A small random spray around the interpolated trail point. */
            __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&work) : "memory");
            work.x = MMathMakeRandom() * 0.06f - 0.03f;
            work.y = MMathMakeRandom() * 0.06f - 0.03f;
            MMathApplyMatrix(&effect->points[0][point], matrix, &work);
            effect->twist[0][point] = MMathMakeRandom2PI();
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_msp_02", fnMSP02_DM000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_msp_02", fnMSP02_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void MEfObjDestroy(void *self);
extern void sefHitEffect(void);

/* Same frame gating as MSP00's fnMSP00_PO000 (see m_ef_create_msp_00.c),
 * against this TU's own thresholds (MspEffect.frame above). */
#define MSP02_HIT_FRAME 0x14
#define MSP02_LIFETIME  0x1E

static void fnMSP02_PO000(void *self, MspEffect *effect)
{
    effect->frame++;
    if (effect->frame == MSP02_HIT_FRAME) {
        sefHitEffect();
    }
    if (effect->frame >= MSP02_LIFETIME) {
        MEfObjDestroy(self);
    }
}

static float adjAngle(float first, float second, float bound)
{
    float value = MMathCalcRotNear(first, second);
    float negative = -bound;

    if (value < negative)
        value = negative;
    if (bound < value)
        value = bound;
    return first + value;
}
