/*
 * OV12 original TU 50: 0x00a2b5e8..0x00a2be48 (16 functions)
 */
#include "common.h"
#include "rg_vector.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", _VecAssert);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", _InitRgVector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", CreateRgVector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", DisposeRgVector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorPush);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorSize);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorCapacity);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorRemove);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorClear);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorFind_sub);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorResize);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorAssign);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorDup);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorGetArray);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_vector", RgVectorDump);
