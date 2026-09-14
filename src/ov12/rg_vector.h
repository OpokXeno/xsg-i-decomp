/*
 * TU-local declarations of ov12/tu050 (src/ov12/rg_vector.c).
 */

#ifndef SRC_OV12_RG_VECTOR_H
#define SRC_OV12_RG_VECTOR_H

typedef struct RgVectorPrefix {
    void **m_apList;
    unsigned int m_uCapacity;
    unsigned int m_uSize;
} RgVectorPrefix;

void *RgVectorIndex(RgVectorPrefix *vector, unsigned int index,
                    const char *file, int line);

extern void _VecAssert(RgVectorPrefix *vector, const char *message,
                       const char *file, int line);

/* File-backed OV12 witnesses: 0x00a54f80 is "pVector != NIL" and
 * 0x00a55010 is "pVector->m_uSize > uIndex". */
extern const char rg_vector_not_null_message[];

extern const char rg_vector_index_message[];

#endif /* SRC_OV12_RG_VECTOR_H */
