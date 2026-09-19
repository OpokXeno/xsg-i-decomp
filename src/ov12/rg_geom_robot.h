/*
 * TU-local declarations of ov12/tu054 (src/ov12/rg_geom_robot.c).
 */

#ifndef SRC_OV12_RG_GEOM_ROBOT_H
#define SRC_OV12_RG_GEOM_ROBOT_H

#include "shared.h"

/*
 * The five per-instance rotation fields the allocated accessors evidence, at
 * RgGeom + 0x80: rotate, rotVel, rotForce, rotResist and maxRotVel, in that
 * offset order.  _InitRgGeomRobot (ov12:0x00a2d290) initializes a robot as a
 * ball geometry (InitRgGeomBall, RgGeomSetType 2) and then clears rotate,
 * rotVel, rotForce and rotResist to 0 and sets maxRotVel to 1.0e8.  The
 * geometry header before them belongs to the TUs that model it
 * (rg_geom_poly.c, rg_geom_tray.c, rg_geom_pillar.c) and stays unmodeled here.
 */
typedef struct RgGeomRobotState {
    float rotate;
    float rotVel;
    float rotForce;
    float rotResist;
    float maxRotVel;
} RgGeomRobotState;

#define RgGeomRobotStateAt(geom) \
    ((RgGeomRobotState *)((unsigned char *)(geom) + 0x80))
#define RgGeomRobotStateConstAt(geom) \
    ((const RgGeomRobotState *)((const unsigned char *)(geom) + 0x80))

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void _InitRgGeomRobot(RgGeom *geom);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a55170 contains the assertion expression "pGeom != NIL".
 * ov12:0x00a55180 contains the source filename "../rg_geom_robot.euc.c".
 * ov12:0x00a55198 contains the assertion expression "pRobot != NIL".
 */
extern const char D_00A55170[];
extern const char D_00A55180[];
extern const char D_00A55198[];

RgGeom *CreateRgGeomRobot(void);
void InitRgGeomRobot(RgGeom *geom);
void RgGeomRobotSetRotate(RgGeom *geom, float rotate);
void RgGeomRobotSetRotVel(RgGeom *geom, float rotVel);
void RgGeomRobotAddRotForce(RgGeom *geom, float force);
void RgGeomRobotSetRotResist(RgGeom *geom, float resist);
void RgGeomRobotSetMaxRotVel(RgGeom *geom, float maxRotVel);
float RgGeomRobotGetRotate(const RgGeom *geom);
float RgGeomRobotGetRotVel(const RgGeom *geom);
float RgGeomRobotGetRotForce(const RgGeom *geom);

#endif /* SRC_OV12_RG_GEOM_ROBOT_H */
