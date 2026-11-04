#ifndef INCLUDE_OV12_RG_GEOM_BALL_H
#define INCLUDE_OV12_RG_GEOM_BALL_H

#include "shared.h"
#include "ov12/rg_geom.h"

/*
 * The contact record the ball-pair checks fill: the closest points of the two
 * balls, the separating vector (penetration) and the distance between them.
 */
typedef struct RgGeomBallContact {
    RgVector position1;
    RgVector position2;
    RgVector penetration;
    float distance;
} RgGeomBallContact;

/* Functions and data used by other translation units. */
void InitRgGeomBall(RgGeomBall *pBall, float radius, float weight);
RgGeomBall *CreateRgGeomBall(float radius, float weight);
void RgGeomBallSetRadius(RgGeomBall *pBall, float radius);
float RgGeomBallGetRadius(RgGeomBall *pBall);
void RgGeomBallPassTime(RgGeomBall *pBall, float deltaTime);
int RgGeomBallCheckBall(RgGeomBall *ball1, RgGeomBall *ball2,
                        RgGeomBallContact *contact);
int RgGeomBallCheckBallDist(RgGeomBall *ball1, RgGeomBall *ball2,
                            RgGeomBallContact *contact);

#endif /* INCLUDE_OV12_RG_GEOM_BALL_H */
