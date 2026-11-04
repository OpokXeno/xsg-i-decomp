/*
 * OV12 original TU 53: 0x00a2ca28..0x00a2d290 (7 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_geom_ball.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_rand_int.h"


/* This TU's own .rodata (scaffold-owned; kept under their splat names,
 * docs/naming.md "Scaffold-owned data keeps its splat name"): 0x00a55148
 * "pBall != NIL", 0x00a55158 "../rg_geom_ball.euc.c". */
const char geomBallAssertBallNotNil[16] = "pBall != NIL";

/* A ball whose squared motion this frame is below this is treated as at rest
 * by RgGeomBallCheckBall. */
#define RG_GEOM_BALL_MIN_MOTION_SQ 0.005f

const char geomBallSourcePath[24] = "../rg_geom_ball.euc.c";

void InitRgGeomBall(RgGeomBall *pBall, float radius, float weight)
{
    if (pBall == 0) {
        assert_prog(geomBallAssertBallNotNil, geomBallSourcePath, 20);
    }
    InitRgGeomPoint(&pBall->point, weight);
    RgGeomSetType(&pBall->point.geom, RG_GEOM_TYPE_BALL);
    RgGeomSetPassTimeMeshod(&pBall->point.geom,
                            (RgGeomPassTimeMethod)RgGeomBallPassTime);
    pBall->radius = radius;
}

RgGeomBall *CreateRgGeomBall(float radius, float weight)
{
    RgGeomBall *pBall;

    pBall = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomBall), geomBallSourcePath, 32);
    if (pBall == 0) {
        assert_prog(geomBallAssertBallNotNil, geomBallSourcePath, 33);
    }
    InitRgGeomBall(pBall, radius, weight);
    return pBall;
}

void RgGeomBallSetRadius(RgGeomBall *pBall, float radius)
{
    if (pBall == 0) {
        assert_prog(geomBallAssertBallNotNil, geomBallSourcePath, 44);
    }
    pBall->radius = radius;
}

float RgGeomBallGetRadius(RgGeomBall *pBall)
{
    if (pBall == 0) {
        assert_prog(geomBallAssertBallNotNil, geomBallSourcePath, 53);
    }
    return pBall->radius;
}

void RgGeomBallPassTime(RgGeomBall *pBall, float deltaTime)
{
    if (pBall == 0) {
        assert_prog(geomBallAssertBallNotNil, geomBallSourcePath, 61);
    }
    RgGeomPointPassTime(&pBall->point, deltaTime);
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
                         : : "r"(oldPosition1), "r"(ball1->point.oldPosition) : "memory");
    __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                         "sqc2 $vf31, 0(%0)\n\t"
                         : : "r"(oldPosition2), "r"(ball2->point.oldPosition) : "memory");
    __asm__ __volatile__("lqc2 $vf30, 0(%1)\n\t"
                         "lqc2 $vf31, 0(%2)\n\t"
                         "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                         "sqc2 $vf30, 0(%0)\n\t"
                         : : "r"(motion1), "r"(ball1->point.position), "r"(oldPosition1) : "memory");
    __asm__ __volatile__("lqc2 $vf30, 0(%1)\n\t"
                         "lqc2 $vf31, 0(%2)\n\t"
                         "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                         "sqc2 $vf30, 0(%0)\n\t"
                         : : "r"(motion2), "r"(ball2->point.position), "r"(oldPosition2) : "memory", "$2");
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

    if (RG_GEOM_BALL_MIN_MOTION_SQ <= speedSq1 && RG_GEOM_BALL_MIN_MOTION_SQ <= speedSq2) {
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
    } else if (RG_GEOM_BALL_MIN_MOTION_SQ <= speedSq1) {
        RgVector closest;
        float one = 1.0f;
        float *position2 = ball2->point.position;
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
                             : : "r"(contact->position2), "r"(ball2->point.position) : "memory");
        __asm__ __volatile__("lqc2 $vf30, 0(%0)\n\t"
                             "lqc2 $vf31, 0(%1)\n\t"
                             "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                             "sqc2 $vf30, 0(%0)\n\t"
                             : : "r"(closest), "r"(ball2->point.position) : "memory");
        contact->distance = XrgLengthVector(closest);
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->penetration), "r"(closest) : "memory");
    } else if (RG_GEOM_BALL_MIN_MOTION_SQ <= speedSq2) {
        RgVector closest;
        float one = 1.0f;
        float *position1 = ball1->point.position;
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
                             : : "r"(contact->position1), "r"(ball1->point.position) : "memory");
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->position2), "r"(closest) : "memory");
        __asm__ __volatile__("lqc2 $vf30, 0(%0)\n\t"
                             "lqc2 $vf31, 0(%1)\n\t"
                             "vsub.xyzw $vf30, $vf30, $vf31\n\t"
                             "sqc2 $vf30, 0(%0)\n\t"
                             : : "r"(closest), "r"(ball1->point.position) : "memory");
        contact->distance = XrgLengthVector(closest);
        XrgNegateVector(contact->penetration, closest);
    } else {
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->position1), "r"(ball1->point.position) : "memory");
        __asm__ __volatile__("lqc2 $vf31, 0(%1)\n\t"
                             "sqc2 $vf31, 0(%0)\n\t"
                             : : "r"(contact->position2), "r"(ball2->point.position) : "memory");
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
                         : "r"(ball1->point.position), "r"(ball2->point.position),
                           "r"(direction)
                         : "memory");
    {
        float distance = XrgNormalizeVector(direction, direction);

        radius_sum = ball1->radius + ball2->radius;
        if (distance < radius_sum) {
            penetration = radius_sum - distance;
            penetration_vector = &contact->penetration;
            (*penetration_vector)[1] = direction[1] * penetration;
            (*penetration_vector)[0] = direction[0] * penetration;
            do {
                (*penetration_vector)[2] = direction[2] * penetration;
            } while (0);
            do {
                (*penetration_vector)[3] = direction[3] * penetration;
            } while (0);
            __asm__ __volatile__("lqc2 $vf31, 0(%0)\n\t"
                                 "sqc2 $vf31, 0(%1)"
                                 :
                                 : "r"(ball1->point.position),
                                   "r"(contact->position1)
                                 : "memory");
            __asm__ __volatile__("lqc2 $vf31, 0(%0)\n\t"
                                 "sqc2 $vf31, 0(%1)"
                                 :
                                 : "r"(ball2->point.position),
                                   "r"(contact->position2)
                                 : "memory");
            contact->distance = distance;
            return 1;
        }
    }
    return 0;
}
