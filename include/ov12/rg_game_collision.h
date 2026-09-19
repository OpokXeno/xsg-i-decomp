#ifndef INCLUDE_OV12_RG_GAME_COLLISION_H
#define INCLUDE_OV12_RG_GAME_COLLISION_H

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

#endif /* INCLUDE_OV12_RG_GAME_COLLISION_H */
