/*
 * TU-local declarations of ov12/tu052 (src/ov12/rg_geom_point.c).
 */

#ifndef SRC_OV12_RG_GEOM_POINT_H
#define SRC_OV12_RG_GEOM_POINT_H

#include "shared.h"

/*
 * The geometry helpers use four adjacent single-precision slots for a
 * position.  The historical public vector typedef is not established here.
 */
typedef struct RgPointVector {
    float x;
    float y;
    float z;
    float w;
} RgPointVector;

void __RgGeomPointGetPos(RgGeomPoint *point, RgPointVector *destination,
                         const char *source_file, int source_line);

void __RgGeomPointGetOldPos(RgGeomPoint *point, RgPointVector *destination,
                            const char *source_file, int source_line);

/* Linker witness for the original literal at ov12:0x00a550f0. */
extern const char rg_point_assert_expression[];

/*
 * This TU's own functions evidence the velocity vector at 0x40 (read by
 * RgGeomPointGetSpeed), the force accumulator at 0x50 (added into by
 * RgGeomPointAddForce) and the weight/moveResist/maxXYSpd floats that follow
 * it; CreateRgGeomPoint allocates sizeof(RgGeomPoint), 0x70 bytes. The
 * position/oldPosition pair at 0x20/0x30 is the one __RgGeomPointGetPos and
 * __RgGeomPointGetOldPos copy and agrees with rg_geom_pillar.c's view of the
 * same tag. That view also reads a radius at 0x70, past the end of this
 * allocation, and rg_geom_group.c keeps a group-made point's owner at the same
 * 0x70: the objects those TUs receive are larger than a plain point, and the
 * two views are not reconciled yet. The bytes no function here reads stay
 * unmodeled.
 */
struct RgGeomPoint {
    unsigned char unmodeled_00[0x20];
    RgVector position;
    RgVector oldPosition;
    RgVector velocity;
    RgVector force;
    float weight;
    float moveResist;
    float maxXYSpd;
    unsigned char unmodeled_6c[4];
};

#endif /* SRC_OV12_RG_GEOM_POINT_H */
