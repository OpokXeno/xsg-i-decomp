/*
 * OV12 original TU 54: 0x00a2d290..0x00a2d900 (14 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_geom_robot.h"

/*
 * InitRgGeomBall, RgGeomSetType, RgGeomSetStatus and RgGeomSetPassTimeMeshod
 * are original functions of other OV12 translation units, already recovered
 * as C (rg_geom_ball.c's InitRgGeomBall; rg_geom.c's RgGeomSetType,
 * RgGeomSetStatus and RgGeomSetPassTimeMeshod).
 */

/*
 * The RgGeom type tag _InitRgGeomRobot passes to RgGeomSetType; 2 is the
 * value between rg_geom_ball.c's RG_GEOM_TYPE_BALL (1) and
 * rg_geom_poly.c's RG_GEOM_TYPE_POLY (3) in the same sequence.
 */
#define RG_GEOM_TYPE_ROBOT 2

extern void InitRgGeomBall(RgGeom *geom, float radius, float weight);
extern void RgGeomSetType(RgGeom *pGeom, int type);
extern void RgGeomSetStatus(RgGeom *pGeom, unsigned int flags);
extern void RgGeomSetPassTimeMeshod(RgGeom *pGeom,
                                    void (*method)(RgGeom *, float));

/* Defined later in this TU (still INCLUDE_ASM); installed by
 * _InitRgGeomRobot as the robot's pass-time method. */
void RgGeomRobotPassTime(RgGeom *geom, float deltaTime);

static void _InitRgGeomRobot(RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 30);
    }
    InitRgGeomBall(geom, 1.5f, 1.0f);
    RgGeomSetType(geom, RG_GEOM_TYPE_ROBOT);
    RgGeomSetStatus(geom, 0x1E);
    RgGeomRobotStateAt(geom)->rotate = 0.0f;
    RgGeomRobotStateAt(geom)->rotVel = 0.0f;
    RgGeomRobotStateAt(geom)->maxRotVel = 1.0e8f;
    RgGeomRobotStateAt(geom)->rotForce = 0.0f;
    RgGeomRobotStateAt(geom)->rotResist = 0.0f;
    RgGeomSetPassTimeMeshod(geom, RgGeomRobotPassTime);
}

RgGeom *CreateRgGeomRobot(void)
{
    RgGeom *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), 0xA0, D_00A55180, 51);
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 52);
    }
    _InitRgGeomRobot(geom);
    return geom;
}

void InitRgGeomRobot(RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(D_00A55198, D_00A55180, 59);
    }
    _InitRgGeomRobot(geom);
}

void RgGeomRobotSetRotate(RgGeom *geom, float rotate)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 68);
    }
    RgGeomRobotStateAt(geom)->rotate = rotate;
}

extern double atan2(double y, double x);

void RgGeomRobotSetDir(RgGeom *geom, RgVector direction)
{
    RgGeomRobotSetRotate(geom, atan2(direction[0], direction[2]));
}

void RgGeomRobotSetRotVel(RgGeom *geom, float rotVel)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 80);
    }
    RgGeomRobotStateAt(geom)->rotVel = rotVel;
}

void RgGeomRobotAddRotForce(RgGeom *geom, float force)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 86);
    }
    RgGeomRobotStateAt(geom)->rotForce += force;
}

void RgGeomRobotSetRotResist(RgGeom *geom, float resist)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 94);
    }
    RgGeomRobotStateAt(geom)->rotResist = resist;
}

void RgGeomRobotSetMaxRotVel(RgGeom *geom, float maxRotVel)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 101);
    }
    RgGeomRobotStateAt(geom)->maxRotVel = maxRotVel;
}

float RgGeomRobotGetRotate(const RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 111);
    }
    return RgGeomRobotStateConstAt(geom)->rotate;
}

float RgGeomRobotGetRotVel(const RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 117);
    }
    return RgGeomRobotStateConstAt(geom)->rotVel;
}

float RgGeomRobotGetRotForce(const RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 123);
    }
    return RgGeomRobotStateConstAt(geom)->rotForce;
}

extern void __RgGeomPointGetPos(RgGeomPoint *point, RgVector destination,
                                const char *source_file, int source_line);
extern float cosf(float angle);
extern float *XrgVectorY(void);
extern void XrgCalcMatrixYtoZ(RgMatrix matrix, RgVector z_axis,
                              RgVector y_axis);
extern void XrgCopyVectorXYZ(RgVector destination, RgVector source);

void RgGeomRobotCalcLocal(RgGeom *geom, RgMatrix matrix)
{
    RgVector position;
    RgVector direction;
    float rotate;

    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 135);
    }
    __RgGeomPointGetPos((RgGeomPoint *)geom, position, D_00A55180, 137);
    rotate = RgGeomRobotStateConstAt(geom)->rotate;
    XrgClearVector(direction);
    direction[0] = -sinf(rotate);
    direction[2] = -cosf(rotate);
    XrgCalcMatrixYtoZ(matrix, direction, XrgVectorY());
    XrgCopyVectorXYZ(&matrix[12], position);
}

#define RG_GEOM_ROBOT_PI 3.1415927f
#define RG_GEOM_ROBOT_TWO_PI 6.2831855f
#define RG_GEOM_ROBOT_STATUS_DAMPING 0x8

extern void RgGeomBallPassTime(RgGeom *geom, float deltaTime);
extern float RgGeomPointGetWeight(RgGeomPoint *point);
extern unsigned int RgGeomGetStatus(RgGeom *geom);

void RgGeomRobotPassTime(RgGeom *geom, float deltaTime)
{
    float resistance;
    float angle;
    float angularIncrement;

    if (geom == 0) {
        assert_prog(D_00A55170, D_00A55180, 157);
    }
    RgGeomBallPassTime(geom, deltaTime);

    angularIncrement = RgGeomRobotStateAt(geom)->rotForce * deltaTime /
                       RgGeomPointGetWeight((RgGeomPoint *)geom);
    RgGeomRobotStateAt(geom)->rotVel += angularIncrement;
    if (RgGeomRobotStateAt(geom)->rotVel <
        -RgGeomRobotStateAt(geom)->maxRotVel) {
        RgGeomRobotStateAt(geom)->rotVel =
            -RgGeomRobotStateAt(geom)->maxRotVel;
    } else if (RgGeomRobotStateAt(geom)->maxRotVel <
               RgGeomRobotStateAt(geom)->rotVel) {
        RgGeomRobotStateAt(geom)->rotVel =
            RgGeomRobotStateAt(geom)->maxRotVel;
    }

    angularIncrement = RgGeomRobotStateAt(geom)->rotVel * deltaTime;
    angle = RgGeomRobotStateAt(geom)->rotate + angularIncrement;
    while (angle > RG_GEOM_ROBOT_PI) {
        angle -= RG_GEOM_ROBOT_TWO_PI;
    }
    while (angle < -RG_GEOM_ROBOT_PI) {
        angle += RG_GEOM_ROBOT_TWO_PI;
    }
    RgGeomRobotStateAt(geom)->rotate = angle;

    if ((RgGeomGetStatus(geom) & RG_GEOM_ROBOT_STATUS_DAMPING) != 0) {
        resistance = RgGeomRobotStateAt(geom)->rotResist;
        RgGeomRobotStateAt(geom)->rotVel -=
            RgGeomRobotStateAt(geom)->rotVel *
            (resistance * deltaTime);
    }
    RgGeomRobotStateAt(geom)->rotForce = 0.0f;
}
