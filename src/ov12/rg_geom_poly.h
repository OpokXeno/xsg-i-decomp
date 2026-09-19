/*
 * TU-local declarations of ov12/tu055 (src/ov12/rg_geom_poly.c).
 */

#ifndef SRC_OV12_RG_GEOM_POLY_H
#define SRC_OV12_RG_GEOM_POLY_H

#include "shared.h"

typedef struct RgGeomLocalMatrices {
    RgMatrix local;
    RgMatrix inverse_local;
} RgGeomLocalMatrices;

extern void XrgUnitMatrix(RgMatrix destination);

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void _InitRgGeomPoly(RgGeom *geom);

/*
 * The collision data a poly-type RgGeom points at (RgGeomPolySetColiData,
 * RgGeomPolyGetData): a distinct object this TU never dereferences, so its
 * layout is left incomplete here rather than restated from the TU that owns
 * it (src/ov12/rg_colidata.c).
 */
typedef struct RgColiData RgColiData;

/*
 * The per-poly window of a Poly-type RgGeom: not RgGeom's own layout (that
 * belongs to rg_geom.c, and include/shared.h only forward-declares the tag),
 * but a distinct TU-local type this TU casts a poly's RgGeom pointer to, the
 * same way RgGeomLocalMatricesAt already reaches +0x20 without completing
 * RgGeom. coliData at +0x18 is evidenced by RgGeomPolySetColiData/
 * RgGeomPolyGetData; local/inverseLocal at +0x20/+0x60 are the same matrices
 * RgGeomLocalMatricesAt already documents. The 4 bytes between coliData and
 * the matrices are unread by every function of this TU and stay unmodeled;
 * their size is consistent with what 16-byte-aligning the matrices at +0x20
 * would cost, which is likely why CreateRgGeomPoly's 0xA0-byte allocation
 * (0x18 header + 0x04 coliData + 0x04 pad + 0x40 local + 0x40 inverseLocal)
 * comes out even.
 */
typedef struct RgGeomPoly {
    unsigned char unmodeled_00[0x18];
    RgColiData *coliData;
    unsigned char unmodeled_1c[4];
    RgMatrix local;
    RgMatrix inverseLocal;
} RgGeomPoly;

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a551a8 contains the assertion expression "pData != NIL".
 * ov12:0x00a551b8 contains the source filename "../rg_geom_poly.euc.c".
 * ov12:0x00a551d0 contains the assertion expression "pPoly != NIL".
 */
extern const char D_00A551A8[];
extern const char D_00A551B8[];
extern const char D_00A551D0[];

RgGeom *CreateRgGeomPoly(void);

void InitRgGeomPoly(RgGeom *geom);

/*
 * RgGeomPolyCheckBall (this TU, still INCLUDE_ASM) passes the stored field to
 * RgColiDataVsBall as its first argument, an RgColiData *, which is the type
 * modeled here; rg_geom_group.c's RgGeomGroupCreatePoly hands the same
 * pointer through.
 */
void RgGeomPolySetColiData(RgGeom *geom, RgColiData *coliData);

RgColiData *RgGeomPolyGetData(const RgGeom *geom);

void RgGeomPolySetLocal(RgGeom *geom, const RgMatrix local);

void RgGeomPolyGetLocal(const RgGeom *geom, RgMatrix destination);

void RgGeomPolyGetInvLocal(const RgGeom *geom, RgMatrix destination);

#endif /* SRC_OV12_RG_GEOM_POLY_H */
