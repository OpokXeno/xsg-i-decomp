#ifndef INCLUDE_OV12_RG_COLIDATA_H
#define INCLUDE_OV12_RG_COLIDATA_H

#include "shared.h"

/*
 * One collision triangle: a 128-byte record (_AllocDataMemory allocates
 * nCapa << 7 bytes, _AddTriangle indexes m_nTriangles << 7). Its layout is
 * private to src/ov12/rg_colidata.c.
 */
typedef struct RgColiTriangle RgColiTriangle;

/* The collision data owned by one geometry: a growable array of triangles. */
typedef struct RgColiData {
    int m_nTriangles;
    int m_nCapacity;
    RgColiTriangle *m_pTriangles;
} RgColiData;

/* The ball RgColiDataVsBall tests: a center point, then the radius just past
 * its four components. */
typedef struct RgColiBall {
    RgVector m_center;
    float m_radius;
} RgColiBall;

/* The hit record RgColiDataVsBall writes: the hit count at +0 and the
 * correction vector accumulated at +16 (+4..+15 are not evidenced). */
typedef struct RgColiBallResult {
    int m_nHits;
    unsigned char unmodeled_04[12];
    RgVector m_correction;
} RgColiBallResult;

/* The segment RgColiDataVsRay tests: its start point, then the vector to its
 * end point (the hit fraction runs over 0..1 along this vector). */
typedef struct RgColiSegment {
    RgVector m_start;
    RgVector m_vector;
} RgColiSegment;

/* The hit record RgColiDataVsRay writes: the plane of the first triangle the
 * segment crosses, then the fraction along the segment where it crosses. */
typedef struct RgColiSegmentHit {
    RgVector m_plane;
    float m_fraction;
} RgColiSegmentHit;

/* Functions and data used by other translation units. */
RgColiData *CreateRgColiData(void);
RgColiData *CreateRgColiDataQuadPrism(float width, float height);
void DisposeRgColiData(RgColiData *pData);
void RgColiDataInitTriangles(RgColiData *pData, int nCapa);
void RgColiDataAddTriangle(RgColiData *pData, void *pV0, void *pV1, void *pV2);
int RgColiDataVsBall(RgColiData *pPoly, RgColiBall *pBall,
                     RgColiBallResult *pResult);
int RgColiDataVsRay(RgColiData *pData, RgColiSegment *pSegment,
                    RgColiSegmentHit *pResult);

#endif /* INCLUDE_OV12_RG_COLIDATA_H */
