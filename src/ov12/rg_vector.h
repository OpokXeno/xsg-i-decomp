/*
 * TU-local declarations of ov12/tu050 (src/ov12/rg_vector.c).
 *
 * CreateRgVector (0x00a2b718) allocates 0x30 bytes for RgVectorPrefix and
 * fills every field: offset 0 is m_apList, which _InitRgVector allocates
 * with RgHeapAlloc and checks against its own assert message
 * ("pVector->m_apList != NIL" at 0x00a54fb8), offsets 4/8 come from
 * RgVectorResize's assert string
 * "0U <= uSize && uSize <= pVector->m_uCapa" (0x00a55030), and the
 * remaining 0x24 bytes are the caller-supplied allocation site: CreateRgVector
 * strncpy()s its own source_file argument into offset 0xC for at most 0x1F
 * bytes, null-terminates at offset 0x2B, and stores its own line argument at
 * offset 0x2C.
 */

#ifndef SRC_OV12_RG_VECTOR_H
#define SRC_OV12_RG_VECTOR_H

typedef struct RgVectorPrefix {
    void **m_apList;
    unsigned int m_uCapa;
    unsigned int m_uSize;
    char m_szFile[32];
    int m_nLine;
} RgVectorPrefix;

RgVectorPrefix *CreateRgVector(unsigned int capacity, const char *file, int line);

void DisposeRgVector(RgVectorPrefix *vector, const char *file, int line);

void RgVectorDup(RgVectorPrefix *vector, void **dest);

void *RgVectorIndex(RgVectorPrefix *vector, unsigned int index,
                    const char *file, int line);

static void _VecAssert(RgVectorPrefix *vector, const char *message,
                       const char *file, int line);

void RgVectorPush(RgVectorPrefix *vector, void *element);

unsigned int RgVectorSize(RgVectorPrefix *vector);

unsigned int RgVectorCapacity(RgVectorPrefix *vector);

void RgVectorClear(RgVectorPrefix *vector);

void RgVectorAssign(RgVectorPrefix *vector, unsigned int index, void *element);

void **RgVectorGetArray(RgVectorPrefix *vector);

/* File-backed OV12 witnesses: 0x00a54f80 is "pVector != NIL" and
 * 0x00a55010 is "pVector->m_uSize > uIndex". */
extern const char rg_vector_not_null_message[];

extern const char rg_vector_index_message[];

/* Remaining OV12 witnesses used by this TU's own functions, none registered
 * with a name in config/symbols/ov12.txt. */
extern const char D_00A54F90[]; /* "../rg_vector.euc.c", the TU's own __FILE__ */
extern const char D_00A54FA8[]; /* "uCapa > 0" */
extern const char D_00A54FB8[]; /* "pVector->m_apList != NIL" */
extern const char D_00A54FE8[]; /* "vec(%p) capa over (capa %d size %d)" */
extern const char D_00A55060[]; /* "0 <= uIndex && uIndex < pVector->m_uSize" */

extern const char D_00A54FD8[]; /* "pVec != NIL" */

#endif /* SRC_OV12_RG_VECTOR_H */
