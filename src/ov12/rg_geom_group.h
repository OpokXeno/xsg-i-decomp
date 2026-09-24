/*
 * TU-local declarations of ov12/tu059 (src/ov12/rg_geom_group.c).
 */

#ifndef SRC_OV12_RG_GEOM_GROUP_H
#define SRC_OV12_RG_GEOM_GROUP_H

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/*
 * The group's own state is a single vector of member geometry pointers;
 * that is the only field this TU's allocated functions evidence (_InitGroup's
 * CreateRgVector store and RgGeomGroupAddElm's read, both at offset 0). The
 * vector container's own layout is not recovered anywhere yet, so it is kept
 * opaque.
 */
typedef struct RgGeomGroup {
    void *m_pList;
} RgGeomGroup;

extern void *CreateRgVector(int capacity, const char *source_file, int line);

extern int RgVectorFind_sub(void *vector, void *element,
                            const char *source_file, int line);

extern void RgVectorPush(void *vector, void *element);

static void _DisposeGroup(RgGeomGroup *group);

extern void RgGeomFree(RgGeom *geom);

extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);

extern RgHeap *InstanceOfRgHeap(void);

static void RgGeomGroupRemoveElm(RgGeomGroup *group, void *element);

static RgGeom *_CreatePointWithGroup(RgGeomGroup *group);
static RgGeom *_CreateBallWithGroup(RgGeomGroup *group);
static RgGeom *_CreateRobotWithGroup(RgGeomGroup *group);
static RgGeom *_CreatePolyWithGroup(RgGeomGroup *group);
static RgGeom *_CreateTrayWithGroup(RgGeomGroup *group);
static RgGeom *_CreatePillarWithGroup(RgGeomGroup *group);

extern void RgGeomSetParent(RgGeom *geom, RgGeom *parent);

extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);

extern int RgVectorRemove(void *vector, void *element,
                          const char *source_file, int line);

/*
 * InitRgGeomPoint and InitRgGeomBall are original functions of
 * rg_geom_point.c and rg_geom_ball.c; those TUs type their first parameter
 * as RgGeomPoint * / RgGeomBall *, both TU-local types this TU has no access
 * to, so this TU declares the same pointer it actually holds here, RgGeom *.
 */
extern void InitRgGeomPoint(RgGeom *point, float weight);
extern void InitRgGeomBall(RgGeom *ball, float radius, float weight);
extern void InitRgGeomRobot(RgGeom *geom);
extern void InitRgGeomPoly(RgGeom *geom);
extern void InitRgGeomTray(RgGeom *geom);
extern void InitRgGeomPillar(RgGeom *geom);

extern void RgGeomSetDestructMethod(RgGeom *geom, void (*method)(RgGeom *));

/*
 * The collision data a poly geometry points at (defined by ov12/tu062
 * rg_colidata); this TU only hands the pointer on to rg_geom_poly.c's
 * RgGeomPolySetColiData, so it stays incomplete here.
 */
typedef struct RgColiData RgColiData;

extern void RgGeomPolySetColiData(RgGeom *geom, RgColiData *coliData);

/* File-backed OV12 witnesses, this TU's own .rodata (scaffold-owned; kept
 * under their splat names, docs/naming.md "Scaffold-owned data keeps its
 * splat name"): 0x00a55338 "pGroup != NIL", 0x00a55348
 * "../rg_geom_group.euc.c", 0x00a55360 "pGroup != NIL && pElm != NIL",
 * 0x00a55380 "RgVectorFind(pGroup->m_pList,pElm) < 0", 0x00a553a8
 * "pGeom != NIL". */
extern const char D_00A55338[];
extern const char D_00A55348[];
extern const char D_00A55360[];
extern const char D_00A55380[];
extern const char D_00A553A8[];

#endif /* SRC_OV12_RG_GEOM_GROUP_H */
