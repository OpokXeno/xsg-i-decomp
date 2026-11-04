/*
 * OV12 original TU 53: 0x00a2ca28..0x00a2d290 (7 functions)
 */
#include "common.h"
#include "shared.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);

/*
 * The complete ball layout is outside this allocation. CreateRgGeomBall
 * allocates sizeof(RgGeomBall) (0x80) bytes -- rg_geom_group.c's own
 * RG_GEOM_BALL_GROUP_OFFSET, the byte immediately past it where a
 * group-owned ball keeps its owning RgGeomGroup. The first 0x70 bytes are
 * the ball's point body (InitRgGeomBall runs InitRgGeomPoint on it; the
 * point type's definition is local to src/ov12/rg_geom_point.c), and
 * InitRgGeomBall, RgGeomBallSetRadius and RgGeomBallGetRadius all keep the
 * radius at 0x70.
 */
typedef struct RgGeomBall {
    unsigned char unmodeled_00[0x20];
    float position[4];
    float oldPosition[4];
    unsigned char unmodeled_40[0x30];
    float radius;
    unsigned char unmodeled_74[0x0c];
} RgGeomBall;

typedef struct RgGeomBallContact {
    RgVector position1;
    RgVector position2;
    RgVector penetration;
    float distance;
} RgGeomBallContact;

extern float XrgNormalizeVector(RgVector destination, RgVector source);
extern float XrgLengthVector(RgVector vector);
extern void XrgNegateVector(RgVector destination, RgVector source);

/* InitRgGeomBall stores its first float at the radius and hands its second
 * to InitRgGeomPoint as the point's weight. */
extern void InitRgGeomBall(RgGeomBall *pBall, float radius, float weight);

/*
 * RgGeomBallPassTime forwards its pointer unmodified into
 * RgGeomPointPassTime (still assembly, src/ov12/rg_geom_point.c): a ball's
 * pass-time method is the point's, applied to the ball's point body.
 */
extern void RgGeomPointPassTime(RgGeomPoint *pPoint, float deltaTime);

/* This TU's own .rodata (scaffold-owned; kept under their splat names,
 * docs/naming.md "Scaffold-owned data keeps its splat name"): 0x00a55148
 * "pBall != NIL", 0x00a55158 "../rg_geom_ball.euc.c". */
const char D_00A55148[16] = "pBall != NIL";
const char D_00A55158[24] = "../rg_geom_ball.euc.c";

/*
 * InitRgGeomPoint, RgGeomSetType and RgGeomSetPassTimeMeshod are original
 * functions of other OV12 translation units, already recovered as C
 * (rg_geom_point.c's InitRgGeomPoint; rg_geom.c's RgGeomSetType and
 * RgGeomSetPassTimeMeshod).
 */
extern void InitRgGeomPoint(RgGeomPoint *point, float weight);
extern void RgGeomSetType(RgGeom *pGeom, int type);
extern void RgGeomSetPassTimeMeshod(RgGeom *pGeom,
                                     void (*method)(RgGeom *, float));

/* Forward declaration: RgGeomBallPassTime is defined further down this
 * file and InitRgGeomBall below installs it as the ball's pass-time
 * method. */
void RgGeomBallPassTime(RgGeomBall *pBall, float deltaTime);

/*
 * The type tag InitRgGeomBall passes to RgGeomSetType;
 * src/ov12/rg_geom_point.c's RG_GEOM_TYPE_POINT (0) precedes it in the
 * same sequence.
 */
#define RG_GEOM_TYPE_BALL 1

void InitRgGeomBall(RgGeomBall *pBall, float radius, float weight)
{
    if (pBall == 0) {
        assert_prog(D_00A55148, D_00A55158, 20);
    }
    InitRgGeomPoint((RgGeomPoint *)pBall, weight);
    RgGeomSetType((RgGeom *)pBall, RG_GEOM_TYPE_BALL);
    RgGeomSetPassTimeMeshod((RgGeom *)pBall,
                            (void (*)(RgGeom *, float))RgGeomBallPassTime);
    pBall->radius = radius;
}

RgGeomBall *CreateRgGeomBall(float radius, float weight)
{
    RgGeomBall *pBall;

    pBall = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomBall), D_00A55158, 32);
    if (pBall == 0) {
        assert_prog(D_00A55148, D_00A55158, 33);
    }
    InitRgGeomBall(pBall, radius, weight);
    return pBall;
}

void RgGeomBallSetRadius(RgGeomBall *pBall, float radius)
{
    if (pBall == 0) {
        assert_prog(D_00A55148, D_00A55158, 44);
    }
    pBall->radius = radius;
}

float RgGeomBallGetRadius(RgGeomBall *pBall)
{
    if (pBall == 0) {
        assert_prog(D_00A55148, D_00A55158, 53);
    }
    return pBall->radius;
}

void RgGeomBallPassTime(RgGeomBall *pBall, float deltaTime)
{
    if (pBall == 0) {
        assert_prog(D_00A55148, D_00A55158, 61);
    }
    RgGeomPointPassTime((RgGeomPoint *)pBall, deltaTime);
}

int RgGeomBallCheckBall(RgGeomBall *ball1, RgGeomBall *ball2,
                        RgGeomBallContact *contact)
{
    RgVector motionNormal;
    RgVector motion1;
    RgVector motion2;
    RgVector oldPosition1;
    RgVector oldPosition2;
    RgVector startOffset;
    float speedSq1;
    float speedSq2;
    float overlap;
    float s;
    float t;

    __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                         "sqc2 $vf31, 0(%0)\n\t"
                         : : "r"(oldPosition1), "r"(ball1->oldPosition) : "memory");
    __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                         "sqc2 $vf31, 0(%0)\n\t"
                         : : "r"(oldPosition2), "r"(ball2->oldPosition) : "memory");
    __asm__ __volatile__("lqc2 $vf30, 0(%1)\n\t"
                         "lqc2 $vf31, 0(%2)\n\t"
                         "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                         "sqc2 $vf30, 0(%0)\n\t"
                         : : "r"(motion1), "r"(ball1->position), "r"(oldPosition1) : "memory");
    __asm__ __volatile__("lqc2 $vf30, 0(%1)\n\t"
                         "lqc2 $vf31, 0(%2)\n\t"
                         "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                         "sqc2 $vf30, 0(%0)\n\t"
                         : : "r"(motion2), "r"(ball2->position), "r"(oldPosition2) : "memory", "$2");
    __asm__ __volatile__("lqc2 $vf30, 0(%1)\n\t"
                         "lqc2 $vf31, 0(%2)\n\t"
                         "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                         "sqc2 $vf30, 0(%0)\n\t"
                         : : "r"(startOffset), "r"(oldPosition1), "r"(oldPosition2) : "memory");
    /* The outer product of the two motions is computed and stored, and
       nothing reads it back. */
    __asm__ __volatile__("lqc2 $vf30, 0(%1)\n\t"
                         "lqc2 $vf31, 0(%2)\n\t"
                         "vopmula.xyz ACC, vf30, vf31\n\t"
                         "vopmsub.xyz vf30, vf31, vf30\n\t"
                         "sqc2 $vf30, 0(%0)\n\t"
                         : : "r"(motionNormal), "r"(motion1), "r"(motion2) : "memory");

    speedSq1 = motion1[0] * motion1[0] + motion1[1] * motion1[1] +
               motion1[2] * motion1[2];
    speedSq2 = motion2[0] * motion2[0] + motion2[1] * motion2[1] +
               motion2[2] * motion2[2];

    if (0.005f <= speedSq1 && 0.005f <= speedSq2) {
        float one = 1.0f;
        float offsetDot1 = startOffset[0] * motion1[0] +
                           startOffset[1] * motion1[1] +
                           startOffset[2] * motion1[2];
        float offsetDot2 = startOffset[0] * motion2[0] +
                           startOffset[1] * motion2[1] +
                           startOffset[2] * motion2[2];
        float dot12 = motion1[0] * motion2[0] + motion1[1] * motion2[1] +
                      motion1[2] * motion2[2];
        float numerator = offsetDot2 * dot12 - speedSq2 * offsetDot1;
        s = numerator / (speedSq1 * speedSq2 - dot12 * dot12);
        t = (s * dot12 + offsetDot2) / speedSq2;
        if (s < 0.0f) {
            s = 0.0f;
        } else if (one < s) {
            s = one;
        }
        {
            float oneT = 1.0f;

            if (t < 0.0f) {
                t = 0.0f;
            } else if (oneT < t) {
                t = oneT;
            }
        }

        motion1[0] *= s;
        motion1[1] *= s;
        motion1[2] *= s;
        motion1[3] *= s;
        __asm__ __volatile__("lqc2 $vf30, 0(%0)\n\t"
                             "lqc2 $vf31, 0(%1)\n\t"
                             "vadd.xyzw $vf30, $vf30, $vf31\n\t"
                             "sqc2 $vf30, 0(%1)\n\t"
                             : : "r"(motion1), "r"(oldPosition1) : "memory");
        motion2[0] *= t;
        motion2[1] *= t;
        motion2[2] *= t;
        motion2[3] *= t;
        __asm__ __volatile__("lqc2 $vf30, 0(%0)\n\t"
                             "lqc2 $vf31, 0(%1)\n\t"
                             "vadd.xyzw $vf30, $vf30, $vf31\n\t"
                             "sqc2 $vf30, 0(%1)\n\t"
                             : : "r"(motion2), "r"(oldPosition2) : "memory");
        __asm__ __volatile__("lqc2 $vf30, 0(%1)\n\t"
                             "lqc2 $vf31, 0(%2)\n\t"
                             "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                             "sqc2 $vf30, 0(%0)\n\t"
                             : : "r"(motion1), "r"(oldPosition1), "r"(oldPosition2) : "memory");
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->position1), "r"(oldPosition1) : "memory");
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->position2), "r"(oldPosition2) : "memory");
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->penetration), "r"(motion1) : "memory");
        contact->distance = XrgLengthVector(motion1);
    } else if (0.005f <= speedSq1) {
        RgVector closest;
        float one = 1.0f;
        float *position2 = ball2->position;
        s = ((position2[0] * motion1[0] + position2[1] * motion1[1] +
              position2[2] * motion1[2]) -
             (oldPosition1[0] * motion1[0] + oldPosition1[1] * motion1[1] +
              oldPosition1[2] * motion1[2])) /
            speedSq1;
        if (s < 0.0f) {
            s = 0.0f;
        } else if (one < s) {
            s = one;
        }

        closest[0] = motion1[0] * s;
        closest[1] = motion1[1] * s;
        closest[2] = motion1[2] * s;
        closest[3] = motion1[3] * s;
        __asm__ __volatile__("lqc2 $vf30, 0(%0)\n\t"
                             "lqc2 $vf31, 0(%1)\n\t"
                             "vadd.xyzw $vf30, $vf30, $vf31\n\t"
                             "sqc2 $vf30, 0(%0)\n\t"
                             : : "r"(closest), "r"(oldPosition1) : "memory");
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->position1), "r"(closest) : "memory");
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->position2), "r"(ball2->position) : "memory");
        __asm__ __volatile__("lqc2 $vf30, 0(%0)\n\t"
                             "lqc2 $vf31, 0(%1)\n\t"
                             "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                             "sqc2 $vf30, 0(%0)\n\t"
                             : : "r"(closest), "r"(ball2->position) : "memory");
        contact->distance = XrgLengthVector(closest);
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->penetration), "r"(closest) : "memory");
    } else if (0.005f <= speedSq2) {
        RgVector closest;
        float one = 1.0f;
        float *position1 = ball1->position;
        s = ((position1[0] * motion2[0] + position1[1] * motion2[1] +
              position1[2] * motion2[2]) -
             (oldPosition2[0] * motion2[0] + oldPosition2[1] * motion2[1] +
              oldPosition2[2] * motion2[2])) /
            speedSq2;
        if (s < 0.0f) {
            s = 0.0f;
        } else if (one < s) {
            s = one;
        }

        closest[0] = motion2[0] * s;
        closest[1] = motion2[1] * s;
        closest[2] = motion2[2] * s;
        closest[3] = motion2[3] * s;
        __asm__ __volatile__("lqc2 $vf30, 0(%0)\n\t"
                             "lqc2 $vf31, 0(%1)\n\t"
                             "vadd.xyzw $vf30, $vf30, $vf31\n\t"
                             "sqc2 $vf30, 0(%0)\n\t"
                             : : "r"(closest), "r"(oldPosition2) : "memory");
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->position1), "r"(ball1->position) : "memory");
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->position2), "r"(closest) : "memory");
        __asm__ __volatile__("lqc2 $vf30, 0(%0)\n\t"
                             "lqc2 $vf31, 0(%1)\n\t"
                             "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                             "sqc2 $vf30, 0(%0)\n\t"
                             : : "r"(closest), "r"(ball1->position) : "memory");
        contact->distance = XrgLengthVector(closest);
        XrgNegateVector(contact->penetration, closest);
    } else {
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->position1), "r"(ball1->position) : "memory");
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->position2), "r"(ball2->position) : "memory");
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->penetration), "r"(startOffset) : "memory");
        contact->distance = XrgLengthVector(startOffset);
    }

    overlap = ball1->radius + ball2->radius - contact->distance;
    if (0.0f < overlap) {
        float *penetration = contact->penetration;
        XrgNormalizeVector(motion1, penetration);
        penetration[0] = motion1[0] * overlap;
        penetration[1] = motion1[1] * overlap;
        penetration[2] = motion1[2] * overlap;
        penetration[3] = motion1[3] * overlap;
        return 1;
    }
    return 0;
}

int RgGeomBallCheckBallDist(RgGeomBall *ball1, RgGeomBall *ball2,
                            RgGeomBallContact *contact)
{
    RgVector direction;
    float radius_sum;
    float penetration;
    RgVector *penetration_vector;

    /* The position begins at byte 0x20 in the point body of each ball.
       Both operands are loaded before the difference overwrites VF30. */
    __asm__ __volatile__("lqc2 $vf30, 0(%0)\n\t"
                         "lqc2 $vf31, 0(%1)\n\t"
                         "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                         "sqc2 $vf30, 0(%2)"
                         :
                         : "r"(&ball1->position), "r"(&ball2->position),
                           "r"(direction)
                         : "memory");
    {
        float distance = XrgNormalizeVector(direction, direction);

        radius_sum = ball1->radius + ball2->radius;
        if (distance < radius_sum) {
            penetration = radius_sum - distance;
            penetration_vector = &contact->penetration;
            {
                float x_component = direction[0];
                float y_component = direction[1];

                y_component *= penetration;
                __asm__ __volatile__("" : : "f"(y_component));
                x_component *= penetration;
                (*penetration_vector)[1] = y_component;
                (*penetration_vector)[0] = x_component;
            }
            /* Keep each pair's store order and its scalar inputs visible. */
            __asm__ __volatile__("" : : "r"(penetration_vector));
            {
                float z_component = direction[2] * penetration;

                (*penetration_vector)[2] = z_component;
                __asm__ __volatile__("" : : "f"(z_component));
                (*penetration_vector)[3] = direction[3] * penetration;
            }
            __asm__ __volatile__("lqc2 $vf31, 0(%0)\n\t"
                                 "sqc2 $vf31, 0(%1)"
                                 :
                                 : "r"(&ball1->position),
                                   "r"(contact->position1)
                                 : "memory");
            __asm__ __volatile__("lqc2 $vf31, 0(%0)\n\t"
                                 "sqc2 $vf31, 0(%1)"
                                 :
                                 : "r"(&ball2->position),
                                   "r"(contact->position2)
                                 : "memory");
            contact->distance = distance;
            return 1;
        }
    }
    return 0;
}
