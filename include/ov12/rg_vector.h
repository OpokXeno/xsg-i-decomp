#ifndef INCLUDE_OV12_RG_VECTOR_H
#define INCLUDE_OV12_RG_VECTOR_H

#include "shared.h"

/*
 * The growable pointer array the CreateRgVector family operates on (not the
 * four-float RgVector of shared.h). CreateRgVector allocates 0x30 bytes for
 * it; m_szFile and m_nLine record the allocation site for the error
 * messages.
 */
typedef struct RgPtrVector {
    void **m_apList;
    unsigned int m_uCapa;
    unsigned int m_uSize;
    char m_szFile[32];
    int m_nLine;
} RgPtrVector;

/* Functions and data used by other translation units. */
RgPtrVector *CreateRgVector(unsigned int capacity, const char *file, int line);
void DisposeRgVector(RgPtrVector *vector, const char *file, int line);
void RgVectorPush(RgPtrVector *vector, void *element);
void *RgVectorIndex(RgPtrVector *vector, unsigned int index,
                    const char *file, int line);
unsigned int RgVectorSize(RgPtrVector *vector);
unsigned int RgVectorCapacity(RgPtrVector *vector);
int RgVectorRemove(RgPtrVector *vector, void *element, const char *file,
                   int line);
void RgVectorClear(RgPtrVector *vector);
int RgVectorFind_sub(RgPtrVector *vector, void *element, const char *file,
                     int line);
void RgVectorResize(RgPtrVector *vector, unsigned int size);
void RgVectorAssign(RgPtrVector *vector, unsigned int index, void *element);
void RgVectorDup(RgPtrVector *vector, void **dest);
void **RgVectorGetArray(RgPtrVector *vector);
void RgVectorDump(RgPtrVector *vector);

#endif /* INCLUDE_OV12_RG_VECTOR_H */
