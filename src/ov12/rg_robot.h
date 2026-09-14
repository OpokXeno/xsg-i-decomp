/*
 * TU-local declarations of ov12/tu004 (src/ov12/rg_robot.c).
 */

#ifndef SRC_OV12_RG_ROBOT_H
#define SRC_OV12_RG_ROBOT_H

#include "shared.h"

typedef struct RgStatus RgStatus;

/* Partial RgBody layout: only the evidenced geometry word at byte offset
 * 0x60 is named. The 96 prefix bytes stay an opaque pad (no fields
 * invented); neighbouring body methods (_BodyGeomPassTime, _BodyDisp,
 * _InitBody) use the same offset, and the exact layout beyond this word
 * remains unresolved. This is the one changed element versus the opaque
 * RG_BODY_GEOMETRY macro form: struct-field access instead of macro. */
typedef struct RgBody {
    unsigned char pad[96];
    RgGeomPoint *geometry;
} RgBody;

extern void RgRobSubBreak(RgGeomPoint *geometry, float linear_scale,
                           float rotational_scale);

extern void _InitBreakingStatus(RgStatus *status, RgBody *body);

#endif /* SRC_OV12_RG_ROBOT_H */
