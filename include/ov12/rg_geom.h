#ifndef INCLUDE_OV12_RG_GEOM_H
#define INCLUDE_OV12_RG_GEOM_H

#include "shared.h"
#include "ov12/rg_colidata.h"

/*
 * The geometry family used by the battle collision code. Every object starts
 * with the RgGeom header; the kinds are laid out as follows (type tag in
 * brackets):
 *
 *   RgGeomPoint [0]  0x70 bytes  header, position, velocity, force, weight
 *   RgGeomBall  [1]  0x80 bytes  a point plus a radius
 *   RgGeomRobot [2]  0xa0 bytes  a ball plus the rotation state
 *   RgGeomPoly  [3]  0xa0 bytes  collision data and the local matrices
 *   RgGeomTray  [4]  0xa0 bytes  a width/height box and the local matrices
 *   RgGeomPillar [5] 0xa0 bytes  a tray re-tagged as a pillar
 *
 * RgGeomGroup allocates each kind with the owning group appended (see
 * rg_geom_group.h).
 */

typedef void (*RgGeomPassTimeMethod)(void *pGeom, float deltaTime);
typedef void (*RgGeomDestructMethod)(void *pGeom);

struct RgGeom {
    int type;
    void *parent;
    unsigned int status;
    int eventFlag;
    RgGeomPassTimeMethod passTimeMethod;
    RgGeomDestructMethod destructMethod;
};

#define RG_GEOM_TYPE_POINT 0
#define RG_GEOM_TYPE_BALL 1
#define RG_GEOM_TYPE_ROBOT 2
#define RG_GEOM_TYPE_POLY 3
#define RG_GEOM_TYPE_TRAY 4
#define RG_GEOM_TYPE_PILLAR 5

/* Number of geometry kinds: the collision handler table is indexed by the
 * types of both geometries. */
#define RG_GEOM_TYPE_COUNT 6

/* Gravity applied to geometries with RG_GEOM_STATUS_GRAVITY, in units per
 * second squared. */
#define RG_GEOM_GRAVITY 60.0f

/* Speed and rotation-speed limit of a geometry that has none set. */
#define RG_GEOM_NO_LIMIT 1.0e8f

/* RgGeom status bits. PUSH_OUT: when two geometries that both carry it
 * collide, the collision handlers move them out of the contact. */
#define RG_GEOM_STATUS_ACTIVE 1
#define RG_GEOM_STATUS_GRAVITY 2
#define RG_GEOM_STATUS_MOVE_RESIST 4
#define RG_GEOM_STATUS_ROT_DAMPING 8
#define RG_GEOM_STATUS_PUSH_OUT 0x10

struct RgGeomPoint {
    RgGeom geom;
    unsigned char unmodeled_18[8];
    RgVector position;
    RgVector oldPosition;
    RgVector velocity;
    RgVector force;
    float weight;
    float moveResist;
    float maxXYSpd;
    unsigned char unmodeled_6c[4];
};

typedef struct RgGeomBall {
    RgGeomPoint point;
    float radius;
    unsigned char unmodeled_74[0x0c];
} RgGeomBall;

typedef struct RgGeomRobot {
    RgGeomBall ball;
    float rotate;
    float rotVel;
    float rotForce;
    float rotResist;
    float maxRotVel;
    unsigned char unmodeled_94[0x0c];
} RgGeomRobot;

typedef struct RgGeomPoly {
    RgGeom geom;
    RgColiData *coliData;
    unsigned char unmodeled_1c[4];
    RgMatrix local;
    RgMatrix inverseLocal;
} RgGeomPoly;

/* A pillar is a tray tagged RG_GEOM_TYPE_PILLAR: its box extents are the
 * tray's width and depth. */
typedef struct RgGeomTray {
    RgGeom geom;
    float width;
    float height;
    RgMatrix local;
    RgMatrix inverseLocal;
} RgGeomTray;

/* Functions and data used by other translation units. */
void RgGeomInit(RgGeom *pGeom);
void RgGeomFree(RgGeom *pGeom);
void RgGeomSetParent(RgGeom *pGeom, void *parent);
void RgGeomSetType(RgGeom *pGeom, int type);
void RgGeomSetPassTimeMeshod(RgGeom *pGeom, RgGeomPassTimeMethod method);
void RgGeomSetDestructMethod(RgGeom *pGeom, RgGeomDestructMethod method);
void RgGeomSetStatus(RgGeom *pGeom, unsigned int flags);
void RgGeomResetStatus(RgGeom *pGeom, unsigned int flags);
void *RgGeomGetParent(RgGeom *pGeom);
int RgGeomGetType(RgGeom *pGeom);
unsigned int RgGeomGetStatus(RgGeom *pGeom);
int RgGeomGetEventFlag(RgGeom *pGeom);
void RgGeomPassTime(RgGeom *pGeom, float deltaTime);

#endif /* INCLUDE_OV12_RG_GEOM_H */
