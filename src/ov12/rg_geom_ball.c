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
    unsigned char unmodeled_00[0x70];
    float radius;
    unsigned char unmodeled_74[0x0c];
} RgGeomBall;

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
extern const char D_00A55148[];
extern const char D_00A55158[];

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_ball", RgGeomBallCheckBallDist);
