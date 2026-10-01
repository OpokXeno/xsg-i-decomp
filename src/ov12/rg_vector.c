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
extern void XrgLog(const char *format, const char *source_file, int line, ...);
extern const char D_00A54F40[];
extern const char D_00A54F68[];
extern const char D_00A55030[];
extern const char D_00A55090[];
extern const char D_00A550B8[];

static void _VecAssert(RgVectorPrefix *vector, const char *message,
                       const char *file, int line)
{
    if (vector != 0) {
        RgError(D_00A54F40, file, line, vector, message, vector->m_szFile,
                vector->m_nLine);
    } else {
        RgError(D_00A54F68, file, line, message);
    }
}

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

extern char *strncpy(char *dest, const char *src, unsigned int n);

RgVectorPrefix *CreateRgVector(unsigned int capacity, const char *file, int line)
{
    RgVectorPrefix *vector;

    vector = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgVectorPrefix), D_00A54F90, 45);
    if (vector == 0)
        _VecAssert(vector, D_00A54FD8, file, line);
    _InitRgVector(vector, capacity);
    strncpy(vector->m_szFile, file, sizeof(vector->m_szFile) - 1);
    vector->m_szFile[sizeof(vector->m_szFile) - 1] = 0;
    vector->m_nLine = line;
    return vector;
}

extern void RgHeapFree(void *heap, void *pointer, const char *source_file, int line);

void DisposeRgVector(RgVectorPrefix *vector, const char *file, int line)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, file, line);
    RgHeapFree(InstanceOfRgHeap(), vector->m_apList, D_00A54F90, 60);
    RgHeapFree(InstanceOfRgHeap(), vector, D_00A54F90, 61);
}

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

int RgVectorFind_sub(RgVectorPrefix *vector, void *element,
                     const char *file, int line)
{
    unsigned int index;

    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, file, line);
    for (index = 0; index < vector->m_uSize; index++) {
        if (vector->m_apList[index] == element)
            return index;
    }
    return -1;
}

void RgVectorResize(RgVectorPrefix *vector, unsigned int size)
{
    unsigned int index;

    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 145);
    if (size > vector->m_uCapa)
        _VecAssert(vector, D_00A55030, D_00A54F90, 146);
    if (size > vector->m_uSize) {
        for (index = vector->m_uSize; index < size; index++)
            vector->m_apList[index] = 0;
    }
    vector->m_uSize = size;
}

void RgVectorAssign(RgVectorPrefix *vector, unsigned int index, void *element)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 158);
    if (index >= vector->m_uSize)
        _VecAssert(vector, D_00A55060, D_00A54F90, 159);
    vector->m_apList[index] = element;
}

void RgVectorDup(RgVectorPrefix *vector, void **dest)
{
    void **src;
    int count;

    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 169);
    count = vector->m_uSize;
    src = vector->m_apList;
    if (count > 0) {
        do {
            *dest++ = *src++;
            count--;
        } while (count > 0);
    }
}

void **RgVectorGetArray(RgVectorPrefix *vector)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 178);
    return vector->m_apList;
}

void RgVectorDump(RgVectorPrefix *vector)
{
    unsigned int index;

    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, D_00A54F90, 187);
    XrgLog(D_00A55090, D_00A54F90, 189, vector, vector->m_uCapa,
           vector->m_uSize);
    for (index = 0; index < vector->m_uSize; index++) {
        XrgLog(D_00A550B8, D_00A54F90, 191, index,
               vector->m_apList[index]);
    }
}
