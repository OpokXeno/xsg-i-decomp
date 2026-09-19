/*
 * OV01 original TU 22: 0x00a337a0..0x00a33f08 (11 functions)
 */
#include "common.h"
#include "m_ef.h"

/*
 * Rejects and logs an effect work request whose size exceeds the fixed
 * 0x400-byte work buffer; accepts everything at or below it.
 */
int MEfCheckWorkSize(const char *name, int size)
{
    if (size >= 0x401) {
        MOutputDebugStringWarn(D_00A512B0, name, size, 0x400);
        return 0;
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef", MEfCalcAngle);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef", MEfCalcAngleMatrix);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef", MEfGetActorMatrix);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef", MEfGetActorCoord);

/*
 * Optionally binds the entry's texture, applies its placement matrix, then
 * tail-calls the model system to draw the given entry.
 */
void MEfDrawModel(const Vector4 *place, int entry, const char *texture)
{
    if (texture != 0) {
        nmlModelSetTexture(texture);
    }
    nmlModelSetPlace(place);
    nmlModelEntry(entry);
}

/*
 * Builds the four corners of a square of the given half-size around source,
 * on the camera basis vectors in mefCamParams, transforms and perspective-
 * projects each of them, and returns how many the clipper rejected.
 */
int MEfMake4Vertex(Vector4 *dst, const Vector4 *source, float size)
{
    const MEfCamParams *cam = &mefCamParams;
    int rejected = 0;
    Vector4 corner[4];
    Vector4 offset;
    Vector4 *vertex;
    int i;

    /* The R5900's native 128-bit copy of the four basis rows; the original
       loads all four before storing any of them. */
    __asm__ __volatile__(
        "lq $8,0(%1)\n\t"
        "lq $9,16(%1)\n\t"
        "lq $10,32(%1)\n\t"
        "lq $11,48(%1)\n\t"
        "sq $8,0(%0)\n\t"
        "sq $9,16(%0)\n\t"
        "sq $10,32(%0)\n\t"
        "sq $11,48(%0)\n\t"
        :
        : "r"(corner), "r"(cam->basis)
        : "$8", "$9", "$10", "$11", "memory"
    );

    /* Put source in the translation row, forcing that row's w lane to the
       architectural VF0.w = 1 rather than keeping source's own w. */
    __asm__ __volatile__(
        "lqc2 $vf1,0(%1)\n\t"
        "vmove.w $vf1w,$vf0w\n\t"
        "sqc2 $vf1,48(%0)\n\t"
        :
        : "r"(corner), "r"(source)
        : "memory"
    );

    vertex = dst;
    for (i = 0; i < 4; i++) {
        /* Clear the offset to the origin with w = 1 (VF0). */
        __asm__ __volatile__("sqc2 $vf0,0(%0)" : : "r"(&offset) : "memory");

        offset.x = (i & 2) ? -size : size;
        offset.y = (i & 1) ? -size : size;

        MMathApplyMatrix(&offset, corner, &offset);
        if (MMathRotTransPersClip(vertex, cam->screen, cam->matrix, &offset)) {
            rejected++;
        }
        vertex++;
    }
    return rejected;
}

/*
 * As MEfMake4Vertex, but the corners sit at angleBase plus the per-corner
 * bearings of mefVertex2CornerTable, so the quad need not be axis aligned.
 */
int MEfMake4Vertex2(Vector4 *dst, const Vector4 *source, float size, float angleBase)
{
    const MEfCamParams *cam = &mefCamParams;
    int rejected = 0;
    MEfCornerAngles angleTable;
    Vector4 corner[4];
    Vector4 offset;
    Vector4 *vertex;
    float cosine;
    float sine;
    int i;

    angleTable = mefVertex2CornerTable;

    __asm__ __volatile__(
        "lq $8,0(%1)\n\t"
        "lq $9,16(%1)\n\t"
        "lq $10,32(%1)\n\t"
        "lq $11,48(%1)\n\t"
        "sq $8,0(%0)\n\t"
        "sq $9,16(%0)\n\t"
        "sq $10,32(%0)\n\t"
        "sq $11,48(%0)\n\t"
        :
        : "r"(corner), "r"(cam->basis)
        : "$8", "$9", "$10", "$11", "memory"
    );

    __asm__ __volatile__(
        "lqc2 $vf1,0(%1)\n\t"
        "vmove.w $vf1w,$vf0w\n\t"
        "sqc2 $vf1,48(%0)\n\t"
        :
        : "r"(corner), "r"(source)
        : "memory"
    );

    /* The corners sit on the diagonal, so the radius that reaches the same
       extent as MEfMake4Vertex's axis-aligned square is size times sqrt(2). */
    size = size * 1.414f;

    vertex = dst;
    for (i = 0; i < 4; i++) {
        float angle = angleBase + angleTable.angle[i];

        __asm__ __volatile__("sqc2 $vf0,0(%0)" : : "r"(&offset) : "memory");

        __asm__ __volatile__(
            "mfc1 $8,%1\n\t"
            "qmtc2.ni $8,$vf4\n\t"
            "vcallms 0xe8\n\t"
            "qmfc2.i $8,$vf1\n\t"
            "mtc1 $8,%0\n\t"
            : "=f"(cosine)
            : "f"(angle)
            : "$8", "memory"
        );
        offset.x = size * cosine;

        __asm__ __volatile__(
            "mfc1 $8,%1\n\t"
            "qmtc2.ni $8,$vf4\n\t"
            "vcallms 0x20\n\t"
            "qmfc2.i $8,$vf1\n\t"
            "mtc1 $8,%0\n\t"
            : "=f"(sine)
            : "f"(angle)
            : "$8", "memory"
        );
        offset.y = size * sine;

        MMathApplyMatrix(&offset, corner, &offset);
        if (MMathRotTransPersClip(vertex, cam->screen, cam->matrix, &offset)) {
            rejected++;
        }
        vertex++;
    }
    return rejected;
}

/*
 * Places a point at a random bearing on the circle of the given radius around
 * center, in the XZ plane, and returns the output pointer.
 *
 */
Vector4 *MEfCalcCircumXZ(Vector4 *out, Vector4 *center, float radius)
{
    float angle;
    float cosine;
    float sine;
    Vector4 offset;

    angle = MMathMakeRandom2PI();

    /* Clear offset to the origin and set its w lane to 1 in one instruction:
       sqc2 stores the architectural quadword constant VF0 = (0,0,0,1). */
    __asm__ __volatile__("sqc2 $vf0,0(%0)" : : "r"(&offset) : "memory");

    /* Move the angle into VF4 through a GPR (uninterlocked: nothing is
       pending), launch the resident VU0 cosine microprogram at its fixed
       instruction-pair entry, then read VF1 back with an interlocked transfer,
       which waits for the microprogram to finish. The EE has no scalar cosine
       or sine instruction. */
    __asm__ __volatile__(
        "mfc1 $8,%1\n\t"
        "qmtc2.ni $8,$vf4\n\t"
        "vcallms 0xe8\n\t"
        "qmfc2.i $8,$vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(cosine)
        : "f"(angle)
        : "$8", "memory"
    );
    offset.x = radius * cosine;

    /* The same launch at the resident sine entry. */
    __asm__ __volatile__(
        "mfc1 $8,%1\n\t"
        "qmtc2.ni $8,$vf4\n\t"
        "vcallms 0x20\n\t"
        "qmfc2.i $8,$vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(sine)
        : "f"(angle)
        : "$8", "memory"
    );
    offset.z = radius * sine;

    /* Translate the point to center. The lane mask is the operation: only xyz
       is summed, so the w lane keeps the 1 stored above. */
    __asm__ __volatile__(
        "lqc2 $vf1,0(%1)\n\t"
        "lqc2 $vf2,0(%0)\n\t"
        "vadd.xyz $vf1xyz,$vf1xyz,$vf2xyz\n\t"
        "sqc2 $vf1,0(%0)\n\t"
        :
        : "r"(&offset), "r"(center)
        : "memory"
    );

    /* The R5900's native 128-bit copy, the shape the SDK's own vector copy
       uses. */
    __asm__ __volatile__(
        "lq $8,0(%1)\n\t"
        "sq $8,0(%0)\n\t"
        :
        : "r"(out), "r"(&offset)
        : "$8", "memory"
    );
    return out;
}

/*
 * Scatters a point randomly around source, inside the actor-relative XY plane
 * whose orientation is the bearing from actor to source, and returns the
 * output pointer.
 */
Vector4 *MEfScatterXY(Vector4 *out, const Vector4 *source, const Vector4 *actor, float spread)
{
    Vector4 angles;
    Vector4 matrix[4];
    float cosine;
    float sine;
    float randomAngle;
    float randomLength;

    MEfCalcAngle(&angles, actor, source);
    MMathRotateMatrixYX(matrix, (const Vector4 *)0, &angles);

    /* Put actor at the translation row of the matrix, forcing that row's w
       lane to the architectural VF0.w = 1 rather than keeping actor's own w.
       The trailing nop is the delay-slot filler the original assembler left
       before the call below: without it the assembler in reorder mode moves
       this sqc2 into that call's delay slot. */
    __asm__ __volatile__(
        "lqc2 $vf1,0(%1)\n\t"
        "vmove.w $vf1w,$vf0w\n\t"
        "sqc2 $vf1,48(%0)\n\t"
        "nop"
        :
        : "r"(matrix), "r"(actor)
        : "memory"
    );

    randomAngle = MMathMakeRandom2PI();
    randomLength = spread * MMathMakeRandom();

    /* Clear the offset to the origin with w = 1 (VF0). */
    __asm__ __volatile__("sqc2 $vf0,0(%0)" : : "r"(&angles) : "memory");

    /* The resident VU0 cosine and sine microprogram launches; the EE has no
       scalar cosine or sine instruction. */
    __asm__ __volatile__(
        "mfc1 $8,%1\n\t"
        "qmtc2.ni $8,$vf4\n\t"
        "vcallms 0xe8\n\t"
        "qmfc2.i $8,$vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(cosine)
        : "f"(randomAngle)
        : "$8", "memory"
    );
    angles.x = randomLength * cosine;

    __asm__ __volatile__(
        "mfc1 $8,%1\n\t"
        "qmtc2.ni $8,$vf4\n\t"
        "vcallms 0x20\n\t"
        "qmfc2.i $8,$vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(sine)
        : "f"(randomAngle)
        : "$8", "memory"
    );
    angles.y = randomLength * sine;

    MMathApplyMatrix(out, matrix, &angles);
    return out;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef", MEfCalcWeaponCoord);
