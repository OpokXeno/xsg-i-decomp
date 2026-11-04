/*
 * OV12 original TU 54: 0x00a2d290..0x00a2d900 (14 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_geom_ball.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_geom_robot.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_rand_int.h"


/* Collision radius of a robot's ball. */
#define RG_GEOM_ROBOT_RADIUS 1.5f

/* A robot starts with gravity, move resistance, rotation damping and
 * push-out. */
#define RG_GEOM_ROBOT_STATUS \
    (RG_GEOM_STATUS_GRAVITY | RG_GEOM_STATUS_MOVE_RESIST | \
     RG_GEOM_STATUS_ROT_DAMPING | RG_GEOM_STATUS_PUSH_OUT)

const char geomRobotAssertGeomNotNil[] = "pGeom != NIL";
const char geomRobotSourcePath[] = "../rg_geom_robot.euc.c";
const char geomRobotAssertRobotNotNil[] = "pRobot != NIL";

static void _InitRgGeomRobot(RgGeom *geom)
{
    RgGeomRobot *robot = (RgGeomRobot *)geom;

    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 30);
    }
    InitRgGeomBall(&robot->ball, RG_GEOM_ROBOT_RADIUS, 1.0f);
    RgGeomSetType(geom, RG_GEOM_TYPE_ROBOT);
    RgGeomSetStatus(geom, RG_GEOM_ROBOT_STATUS);
    robot->rotate = 0.0f;
    robot->rotVel = 0.0f;
    robot->maxRotVel = RG_GEOM_NO_LIMIT;
    robot->rotForce = 0.0f;
    robot->rotResist = 0.0f;
    RgGeomSetPassTimeMeshod(geom, (RgGeomPassTimeMethod)RgGeomRobotPassTime);
}

RgGeom *CreateRgGeomRobot(void)
{
    RgGeom *geom;

    geom = RgHeapAlloc(InstanceOfRgHeap(), 0xA0, geomRobotSourcePath, 51);
    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 52);
    }
    _InitRgGeomRobot(geom);
    return geom;
}

void InitRgGeomRobot(RgGeom *geom)
{
    if (geom == 0) {
        assert_prog(geomRobotAssertRobotNotNil, geomRobotSourcePath, 59);
    }
    _InitRgGeomRobot(geom);
}

void RgGeomRobotSetRotate(RgGeom *geom, float rotate)
{
    RgGeomRobot *robot = (RgGeomRobot *)geom;
    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 68);
    }
    robot->rotate = rotate;
}

void RgGeomRobotSetDir(RgGeom *geom, RgVector direction)
{
    RgGeomRobotSetRotate(geom, atan2(direction[0], direction[2]));
}

void RgGeomRobotSetRotVel(RgGeom *geom, float rotVel)
{
    RgGeomRobot *robot = (RgGeomRobot *)geom;
    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 80);
    }
    robot->rotVel = rotVel;
}

void RgGeomRobotAddRotForce(RgGeom *geom, float force)
{
    RgGeomRobot *robot = (RgGeomRobot *)geom;
    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 86);
    }
    robot->rotForce += force;
}

void RgGeomRobotSetRotResist(RgGeom *geom, float resist)
{
    RgGeomRobot *robot = (RgGeomRobot *)geom;
    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 94);
    }
    robot->rotResist = resist;
}

void RgGeomRobotSetMaxRotVel(RgGeom *geom, float maxRotVel)
{
    RgGeomRobot *robot = (RgGeomRobot *)geom;
    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 101);
    }
    robot->maxRotVel = maxRotVel;
}

float RgGeomRobotGetRotate(const RgGeom *geom)
{
    const RgGeomRobot *robot = (const RgGeomRobot *)geom;
    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 111);
    }
    return robot->rotate;
}

float RgGeomRobotGetRotVel(const RgGeom *geom)
{
    const RgGeomRobot *robot = (const RgGeomRobot *)geom;
    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 117);
    }
    return robot->rotVel;
}

float RgGeomRobotGetRotForce(const RgGeom *geom)
{
    const RgGeomRobot *robot = (const RgGeomRobot *)geom;
    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 123);
    }
    return robot->rotForce;
}

void RgGeomRobotCalcLocal(RgGeom *geom, RgMatrix matrix)
{
    RgGeomRobot *robot = (RgGeomRobot *)geom;
    RgVector position;
    RgVector direction;
    float rotate;

    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 135);
    }
    __RgGeomPointGetPos(&robot->ball.point, position, geomRobotSourcePath, 137);
    rotate = robot->rotate;
    XrgClearVector(direction);
    direction[0] = -sinf(rotate);
    direction[2] = -cosf(rotate);
    XrgCalcMatrixYtoZ(matrix, direction, XrgVectorY());
    XrgCopyVectorXYZ(&matrix[12], position);
}

#define RG_GEOM_ROBOT_PI 3.1415927f
#define RG_GEOM_ROBOT_TWO_PI 6.2831855f

void RgGeomRobotPassTime(RgGeom *geom, float deltaTime)
{
    RgGeomRobot *robot = (RgGeomRobot *)geom;
    float resistance;
    float angle;
    float angularIncrement;

    if (geom == 0) {
        assert_prog(geomRobotAssertGeomNotNil, geomRobotSourcePath, 157);
    }
    RgGeomBallPassTime(&robot->ball, deltaTime);

    angularIncrement = robot->rotForce * deltaTime /
                       RgGeomPointGetWeight(&robot->ball.point);
    robot->rotVel += angularIncrement;
    if (robot->rotVel <
        -robot->maxRotVel) {
        robot->rotVel =
            -robot->maxRotVel;
    } else if (robot->maxRotVel <
               robot->rotVel) {
        robot->rotVel =
            robot->maxRotVel;
    }

    angularIncrement = robot->rotVel * deltaTime;
    angle = robot->rotate + angularIncrement;
    while (angle > RG_GEOM_ROBOT_PI) {
        angle -= RG_GEOM_ROBOT_TWO_PI;
    }
    while (angle < -RG_GEOM_ROBOT_PI) {
        angle += RG_GEOM_ROBOT_TWO_PI;
    }
    robot->rotate = angle;

    if ((RgGeomGetStatus(geom) & RG_GEOM_STATUS_ROT_DAMPING) != 0) {
        resistance = robot->rotResist;
        robot->rotVel -=
            robot->rotVel *
            (resistance * deltaTime);
    }
    robot->rotForce = 0.0f;
}
