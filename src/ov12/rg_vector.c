/*
 * OV12 original TU 50: 0x00a2b5e8..0x00a2be48 (16 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_vector.h"

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgError(const char *message, const char *source_file, int line,
                    ...);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", _VecAssert);

static void _InitRgVector(RgVectorPrefix *vector, unsigned int capacity)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 31);
    if (capacity == 0)
        _VecAssert(vector, D_00A54FA8, D_00A54F90, 32);
    vector->m_apList = RgHeapAlloc(InstanceOfRgHeap(),
                                   capacity * sizeof(void *), D_00A54F90, 33);
    if (vector->m_apList == 0)
        _VecAssert(vector, D_00A54FB8, D_00A54F90, 34);
    vector->m_uCapa = capacity;
    vector->m_uSize = 0;
    vector->m_szFile[0] = 0;
    vector->m_nLine = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", CreateRgVector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", DisposeRgVector);

void RgVectorPush(RgVectorPrefix *vector, void *element)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 70);
    if (vector->m_uCapa <= vector->m_uSize) {
        RgError(D_00A54FE8, D_00A54F90, 73, vector, vector->m_uCapa,
               vector->m_uSize);
    }
    vector->m_apList[vector->m_uSize++] = element;
}

/*
 * RgVectorIndex reads one pointer-sized element from the vector's compact
 * pointer array. Only the proven three-word prefix is modeled; later vector
 * state is outside this function's access range. Message addresses are
 * retained from the original OV12 text/data image.
 */

void *RgVectorIndex(RgVectorPrefix *vector, unsigned int index,
                    const char *file, int line)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, file, line);
    if (!(index < vector->m_uSize))
        _VecAssert(vector, rg_vector_index_message, file, line);
    return vector->m_apList[index];
}

unsigned int RgVectorSize(RgVectorPrefix *vector)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 89);
    return vector->m_uSize;
}

unsigned int RgVectorCapacity(RgVectorPrefix *vector)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 96);
    return vector->m_uCapa;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorRemove);

void RgVectorClear(RgVectorPrefix *vector)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 125);
    vector->m_uSize = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorFind_sub);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorResize);

void RgVectorAssign(RgVectorPrefix *vector, unsigned int index, void *element)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 158);
    if (index >= vector->m_uSize)
        _VecAssert(vector, D_00A55060, D_00A54F90, 159);
    vector->m_apList[index] = element;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorDup);

void **RgVectorGetArray(RgVectorPrefix *vector)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 178);
    return vector->m_apList;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorDump);
