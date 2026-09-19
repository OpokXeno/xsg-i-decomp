/*
 * TU-local declarations of ov12/tu060 (src/ov12/rg_game_collision.c).
 */

#ifndef SRC_OV12_RG_GAME_COLLISION_H
#define SRC_OV12_RG_GAME_COLLISION_H

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern RgHeap *InstanceOfRgHeap(void);

extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);

extern void *CreateRgVector(int capacity, const char *source_file, int line);

extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);

extern unsigned int RgVectorSize(void *vector);
extern void *RgVectorIndex(void *vector, unsigned int index,
                           const char *source_file, int line);
extern void RgVectorPush(void *vector, void *element);
extern void RgVectorClear(void *vector);
extern void DisposeRgVector(void *vector, const char *source_file, int line);

/*
 * The game-collision object's own state is a single vector of queued
 * collision-check arguments; that is the only field _InitGameCollision's
 * CreateRgVector store evidences here (offset 0). _AddCheck, _ClearCheck and
 * _JobCheck push, clear and iterate the same vector at this offset, but their
 * own bodies are still INCLUDE_ASM in this allocation, so the vector
 * container's own layout stays opaque.
 */
typedef struct RgGameCollision {
    void *m_pCheckList;
} RgGameCollision;

/*
 * RgGeomGroup is defined by ov12/tu059 (src/ov12/rg_geom_group.h); this TU
 * only stores and tests the pointers, so an incomplete type is enough here.
 */
typedef struct RgGeomGroup RgGeomGroup;

extern void RgGeomGroupCollision(RgGeomGroup *pGroup);

/*
 * A queued collision-check argument. _CreateColiArg's assert message
 * ("pSrc->m_pGroup1 != pSrc->m_pGroup2" at D_00A553F8, still INCLUDE_ASM in
 * this TU) names the two members RgGameCollisionAdd reads to decide whether
 * to enqueue the argument; RgGeomGroupCollision reads each one's vector at
 * offset 0, so both are geometry groups. _CreateColiArg copies six words
 * (0x18 bytes total) from this object when it constructs the queued entry,
 * but only these two are evidenced by this allocation's functions, so the
 * bytes between and after them stay unmodeled. InitRgGeomGroupColiArg
 * (ov12/tu059) clears all six words. The members name the group by its tag
 * so that include/ov12/rg_game_collision.h, which ov12/tu059 reads, carries
 * no second typedef of RgGeomGroup beside tu059's own definition.
 */
typedef struct RgGameColiArg {
    struct RgGeomGroup *m_pGroup1; /* +0x00 */
    unsigned int unmodeled_04;     /* +0x04 */
    unsigned int unmodeled_08;     /* +0x08 */
    struct RgGeomGroup *m_pGroup2; /* +0x0C */
    unsigned int unmodeled_10;     /* +0x10, copied verbatim by _CreateColiArg */
    unsigned int unmodeled_14;     /* +0x14, copied verbatim by _CreateColiArg */
} RgGameColiArg;

extern void InitRgGeomGroupColiArg(RgGameColiArg *pArg);

static void _DisposeGameCollision(RgGameCollision *pGameColi);
static void _AddCheck(RgGameCollision *pGameColi, RgGameColiArg *pArg);
static void _ClearCheck(RgGameCollision *pGameColi);
static void _JobCheck(RgGameCollision *pGameColi);

/* File-backed OV12 witnesses, this TU's own .rodata (scaffold-owned; kept
 * under their splat names, docs/naming.md "Scaffold-owned data keeps its
 * splat name"): 0x00a553c8 "../rg_game_collision.euc.c", 0x00a55420
 * "pArg != NIL", 0x00a55430 "pGameColi != NIL". */
extern const char D_00A553C8[];
extern const char D_00A55420[];
extern const char D_00A55430[];

/* File-backed OV12 witnesses, this TU's own .rodata (scaffold-owned; kept
 * under their splat names, docs/naming.md "Scaffold-owned data keeps its
 * splat name"): 0x00a553e8 "pSrc != NIL", 0x00a553f8
 * "pSrc->m_pGroup1 != pSrc->m_pGroup2". */
extern const char D_00A553E8[];
extern const char D_00A553F8[];

#endif /* SRC_OV12_RG_GAME_COLLISION_H */
