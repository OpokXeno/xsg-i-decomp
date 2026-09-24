/*
 * TU-local declarations of ov12/tu062 (src/ov12/rg_colidata.c).
 */

#ifndef SRC_OV12_RG_COLIDATA_H
#define SRC_OV12_RG_COLIDATA_H

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);

/*
 * One collision triangle: a 128-byte record (_AllocDataMemory allocates
 * nCapa << 7 bytes, _AddTriangle indexes m_nTriangles << 7). Its members are
 * not recovered here, so only pointers to it are used.
 */
typedef struct RgColiTriangle RgColiTriangle;

/*
 * The collision data owned by one geometry: a growable array of triangles.
 * m_nTriangles, m_nCapacity and m_pTriangles are the only fields any
 * allocated function of this TU evidences.
 */
typedef struct RgColiData {
    int m_nTriangles;
    int m_nCapacity;
    RgColiTriangle *m_pTriangles;
} RgColiData;

static void _DisposeColiData(RgColiData *pData);
static void _InitTriangles(RgColiData *pData, int nCapa);
static void _AddTriangle(RgColiData *pData, void *pV0, void *pV1, void *pV2);
static void _CheckIntersectBall(RgColiData *pPoly, void *pArg, void *pResult);

/* File-backed OV12 witnesses, this TU's own .rodata (scaffold-owned; kept
 * under their splat names, docs/naming.md "Scaffold-owned data keeps its
 * splat name"): 0x00a554f0 "pData != NIL", 0x00a55500
 * "../rg_colidata.euc.c", 0x00a55518 "nCapa > 0", 0x00a55528
 * "pData->m_pTriangles != NIL", 0x00a55670
 * "pPoly != NIL && pArg != NIL && pResult != NIL". */
extern const char D_00A554F0[];
extern const char D_00A55500[];
extern const char D_00A55518[];
extern const char D_00A55528[];
extern const char D_00A55670[];

/* 0x00a55560 "pData->m_nCapaOfTri > pData->m_nNumOfTri" */
extern const char D_00A55560[];

#endif /* SRC_OV12_RG_COLIDATA_H */
