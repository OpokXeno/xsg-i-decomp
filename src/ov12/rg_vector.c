/*
 * OV12 original TU 50: 0x00a2b5e8..0x00a2be48 (16 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_vector.h"
#include "ov12/rg_error.h"
#include "ov12/xrg_v_log.h"


/* Defined at the end of this file. */
extern const char vectorErrorWithCreationSiteFormat[];
extern const char vectorNilErrorFormat[];
extern const char rg_vector_not_null_message[];
extern const char vectorSourcePath[]; /* "../rg_vector.euc.c", the TU's own __FILE__ */
extern const char vectorAssertCapacityPositive[]; /* "uCapa > 0" */
extern const char vectorAssertListNotNil[]; /* "pVector->m_apList != NIL" */
extern const char vectorAssertVecNotNil[]; /* "pVec != NIL" */
extern const char vectorCapacityOverFormat[]; /* "vec(%p) capa over (capa %d size %d)" */
extern const char rg_vector_index_message[];
extern const char vectorAssertSizeWithinCapacity[];
extern const char vectorAssertIndexWithinSize[]; /* "0 <= uIndex && uIndex < pVector->m_uSize" */
extern const char vectorDumpHeaderFormat[];
extern const char vectorDumpElementFormat[];

/* Reports a failed vector check; a non-null vector also reports its creation site. */
static void _VecAssert(RgPtrVector *vector, const char *message,
                       const char *file, int line)
{
    if (vector != 0) {
        RgError(vectorErrorWithCreationSiteFormat, file, line, vector, message, vector->m_szFile,
                vector->m_nLine);
    } else {
        RgError(vectorNilErrorFormat, file, line, message);
    }
}

/* Allocates the pointer array for a vector and resets its size and creation site. */
static void _InitRgVector(RgPtrVector *vector, unsigned int capacity)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, vectorSourcePath, 31);
    if (capacity == 0)
        _VecAssert(vector, vectorAssertCapacityPositive, vectorSourcePath, 32);
    vector->m_apList = RgHeapAlloc(InstanceOfRgHeap(),
                                   capacity * sizeof(void *), vectorSourcePath, 33);
    if (vector->m_apList == 0)
        _VecAssert(vector, vectorAssertListNotNil, vectorSourcePath, 34);
    vector->m_uCapa = capacity;
    vector->m_uSize = 0;
    vector->m_szFile[0] = 0;
    vector->m_nLine = 0;
}

/* Allocates a vector with the given capacity and records file and line as its creation site. */
RgPtrVector *CreateRgVector(unsigned int capacity, const char *file, int line)
{
    RgPtrVector *vector;

    vector = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgPtrVector), vectorSourcePath, 45);
    if (vector == 0)
        _VecAssert(vector, vectorAssertVecNotNil, file, line);
    _InitRgVector(vector, capacity);
    strncpy(vector->m_szFile, file, sizeof(vector->m_szFile) - 1);
    vector->m_szFile[sizeof(vector->m_szFile) - 1] = 0;
    vector->m_nLine = line;
    return vector;
}

void DisposeRgVector(RgPtrVector *vector, const char *file, int line)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, file, line);
    RgHeapFree(InstanceOfRgHeap(), vector->m_apList, vectorSourcePath, 60);
    RgHeapFree(InstanceOfRgHeap(), vector, vectorSourcePath, 61);
}

/* Appends element; asserts that the vector has room. */
void RgVectorPush(RgPtrVector *vector, void *element)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, vectorSourcePath, 70);
    if (vector->m_uCapa <= vector->m_uSize) {
        RgError(vectorCapacityOverFormat, vectorSourcePath, 73, vector, vector->m_uCapa,
               vector->m_uSize);
    }
    vector->m_apList[vector->m_uSize++] = element;
}

/*
 * Returns one element of the pointer array. Asserts that index is below the
 * current size.
 */
void *RgVectorIndex(RgPtrVector *vector, unsigned int index,
                    const char *file, int line)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, file, line);
    if (!(index < vector->m_uSize))
        _VecAssert(vector, rg_vector_index_message, file, line);
    return vector->m_apList[index];
}

unsigned int RgVectorSize(RgPtrVector *vector)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, vectorSourcePath, 89);
    return vector->m_uSize;
}

unsigned int RgVectorCapacity(RgPtrVector *vector)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, vectorSourcePath, 96);
    return vector->m_uCapa;
}

/*
 * Removes every occurrence of element, shifting later entries down.
 * Returns 1 when at least one was removed, 0 otherwise.
 */
int RgVectorRemove(RgPtrVector *vector, void *element, const char *file,
                   int line)
{
    unsigned int index;
    unsigned int move;
    int removed;

    removed = 0;
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, file, line);
    index = 0;
    while (index < vector->m_uSize) {
        if (vector->m_apList[index] == element) {
            for (move = index; move < vector->m_uSize - 1; move++)
                vector->m_apList[move] = vector->m_apList[move + 1];
            vector->m_uSize--;
            removed = 1;
        } else {
            index++;
        }
    }
    return removed;
}

void RgVectorClear(RgPtrVector *vector)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, vectorSourcePath, 125);
    vector->m_uSize = 0;
}

/* Returns the index of the first entry equal to element, or -1 when none is. */
int RgVectorFind_sub(RgPtrVector *vector, void *element,
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

/* Sets the size; slots exposed by growing are set to 0. */
void RgVectorResize(RgPtrVector *vector, unsigned int size)
{
    unsigned int index;

    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, vectorSourcePath, 145);
    if (size > vector->m_uCapa)
        _VecAssert(vector, vectorAssertSizeWithinCapacity, vectorSourcePath, 146);
    if (size > vector->m_uSize) {
        for (index = vector->m_uSize; index < size; index++)
            vector->m_apList[index] = 0;
    }
    vector->m_uSize = size;
}

void RgVectorAssign(RgPtrVector *vector, unsigned int index, void *element)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, vectorSourcePath, 158);
    if (index >= vector->m_uSize)
        _VecAssert(vector, vectorAssertIndexWithinSize, vectorSourcePath, 159);
    vector->m_apList[index] = element;
}

/* Copies the entry pointers into dest. */
void RgVectorDup(RgPtrVector *vector, void **dest)
{
    void **src;
    int count;

    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, vectorSourcePath, 169);
    count = vector->m_uSize;
    src = vector->m_apList;
    if (count > 0) {
        do {
            *dest++ = *src++;
            count--;
        } while (count > 0);
    }
}

/* Returns the vector's pointer array. */
void **RgVectorGetArray(RgPtrVector *vector)
{
    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, vectorSourcePath, 178);
    return vector->m_apList;
}

/* Logs the vector's header and every entry pointer. */
void RgVectorDump(RgPtrVector *vector)
{
    unsigned int index;

    if (vector == 0)
        _VecAssert(vector, rg_vector_not_null_message, vectorSourcePath, 187);
    XrgLog(vectorDumpHeaderFormat, vectorSourcePath, 189, vector, vector->m_uCapa,
           vector->m_uSize);
    for (index = 0; index < vector->m_uSize; index++) {
        XrgLog(vectorDumpElementFormat, vectorSourcePath, 191, index,
               vector->m_apList[index]);
    }
}

const char vectorErrorWithCreationSiteFormat[40] = "vector(%p) error (%s)\ncreated at %s,%d";
const char vectorNilErrorFormat[24] = "vector nil error (%s)";
const char rg_vector_not_null_message[16] = "pVector != NIL";
const char vectorSourcePath[24] = "../rg_vector.euc.c";
const char vectorAssertCapacityPositive[16] = "uCapa > 0";
const char vectorAssertListNotNil[32] = "pVector->m_apList != NIL";
const char vectorAssertVecNotNil[16] = "pVec != NIL";
const char vectorCapacityOverFormat[40] = "vec(%p) capa over (capa %d size %d)";
const char rg_vector_index_message[32] = "pVector->m_uSize > uIndex";
const char vectorAssertSizeWithinCapacity[48] = "0U <= uSize && uSize <= pVector->m_uCapa";
const char vectorAssertIndexWithinSize[48] = "0 <= uIndex && uIndex < pVector->m_uSize";
const char vectorDumpHeaderFormat[40] = "--- dump vector (%p:capa=%d size=%d)\n";
const char vectorDumpElementFormat[16] = "  [%d] -> %p\n";
