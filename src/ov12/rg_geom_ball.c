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
    unsigned char unmodeled_30[0x40];
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_ball", RgGeomBallCheckBall);

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
