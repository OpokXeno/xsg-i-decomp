/*
 * OV01 original TU 22: 0x00a337a0..0x00a33f08 (11 functions)
 */
#include "common.h"
#include "m_ef.h"
#include "main/act_1.h"
#include "main/act_2.h"

extern Matrix4 *MMathRotateMatrixY(Matrix4 *out, Matrix4 *matrix, float angle);
const char D_00A512B0[64] =
    "MEfCheckWorkSize: The size of %s has overflowed [%d > %d]";

const char D_00A512F0[40] =
    "MEfGetActorMatrix: actor is NULL";
const char D_00A51318[40] =
    "MEfGetActorMatrix: Invalid point (%d)";
const char D_00A51340[48] =
    "MEfGetActorMatrix: point is out of range (%d)";

const MEfCornerAngles mefVertex2CornerTable = {
    {0.0f, 1.5707964f, 4.712389f, 3.1415927f}
};

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

/*
 * MEfCalcAngle: the pitch/yaw pair that turns "from" towards "to". out->z and
 * out->w come from VU0's hardwired VF0 = (0,0,0,1). The horizontal distance is
 * the VU0 square root of the XZ lengths; the height difference is from - to.
 */
Vector4 *MEfCalcAngle(Vector4 *destination, const Vector4 *from, const Vector4 *to)
{
    Vector4 direction;
    float distanceXZ = 0.0f;

    __asm__ __volatile__(
        "lqc2 $vf1,0(%2)\n\t"
        "lqc2 $vf2,0(%3)\n\t"
        "vsub.xz $vf3xz,$vf2xz,$vf1xz\n\t"
        "vmul.xz $vf4xz,$vf3xz,$vf3xz\n\t"
        "vaddz.x $vf4x,$vf4x,$vf4z\n\t"
        "vsqrt Q,$vf4x\n\t"
        "vsub.y $vf3y,$vf1y,$vf2y\n\t"
        "sqc2 $vf3,0(%4)\n\t"
        "sqc2 $vf0,0(%1)\n\t"
        "vwaitq\n\t"
        "vaddq.x $vf1x,$vf0x,Q\n\t"
        "qmfc2.ni $8,$vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "+f"(distanceXZ)
        : "r"(destination), "r"(from), "r"(to), "r"(&direction)
        : "$8", "memory"
    );
    destination->x = srsAtan2(direction.y, distanceXZ);
    destination->y = srsAtan2(direction.x, direction.z);
    return destination;
}

/*
 * MEfCalcAngleMatrix: recover the yaw/pitch pair of "matrix" from the
 * direction "matrix" rotates its local -Z axis (0,0,-1,1) to. out->z and
 * out->w come from the trailing "sqc2 vf0" (VU0's hardwired (0,0,0,1)) and
 * are never overwritten; out->x is pitch, out->y is yaw. Compare
 * src/main/m_math.c's MMathCalcAngleMatrix, which recovers the same pair from
 * a direction it reads directly out of the matrix's third row instead of by
 * rotating a fixed local axis, hence that sibling's un-negated
 * MMathCalcLengthXZ where this one negates it.
 */
Vector4 *MEfCalcAngleMatrix(Vector4 *out, const Vector4 *matrix)
{
    Vector4 direction;

    __asm__ __volatile__(
        "vmove.xyzw vf1, vf0\n\t"
        "vsubw.z vf1, vf0, vf0w\n\t"
        "sqc2 vf1, 0(%0)"
        :
        : "r"(&direction)
        : "memory"
    );
    MMathApplyMatrix(&direction, matrix, &direction);
    __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(out) : "memory");
    out->x = srsAtan2(direction.y, -MMathCalcLengthXZ(&direction));
    out->y = srsAtan2(direction.x, direction.z);
    return out;
}

/*
 * MEfGetActorMatrix: the world matrix of one point of an actor. coordinate
 * -1 is the actor's own yaw and position, -2 the same raised to a fixed
 * height of 1.5, and 0..0x27 an accessory joint whose matrix is copied out of
 * the actor's matrix block. Anything else, or no actor, leaves the identity.
 */
Matrix4 *MEfGetActorMatrix(Matrix4 *destination, Actor *actor, u32 coordinate)
{
    int joint;

    /* The identity, built by rotating VF0 = (0,0,0,1) one lane per row. */
    __asm__ __volatile__(
        "vmr32.xyzw $vf1,$vf0\n\t"
        "vmr32.xyzw $vf2,$vf1\n\t"
        "vmr32.xyzw $vf3,$vf2\n\t"
        "sqc2 $vf0,48(%0)\n\t"
        "sqc2 $vf1,32(%0)\n\t"
        "sqc2 $vf2,16(%0)\n\t"
        "sqc2 $vf3,0(%0)\n\t"
        :
        : "r"(destination)
        : "memory"
    );

    if (actor == 0) {
        MOutputDebugStringWarn(D_00A512F0);
    } else if (coordinate == MEF_POINT_ACTOR) {
        MMathRotateMatrixY(destination, destination, actor->rotation.y);
        __asm__ __volatile__(
            "lqc2 $vf1,0(%1)\n\t"
            "vmove.w $vf1w,$vf0w\n\t"
            "sqc2 $vf1,48(%0)\n\t"
            :
            : "r"(destination), "r"(&actor->position)
            : "memory"
        );
    } else if (coordinate == MEF_POINT_ACTOR_RAISED) {
        MMathRotateMatrixY(destination, destination, actor->rotation.y);
        __asm__ __volatile__(
            "lqc2 $vf1,0(%1)\n\t"
            "vmove.w $vf1w,$vf0w\n\t"
            "sqc2 $vf1,48(%0)\n\t"
            :
            : "r"(destination), "r"(&actor->position)
            : "memory"
        );
        (*destination)[3][1] = 1.5f;
    } else if (coordinate < MEF_POINT_ACCESSORY_COUNT) {
        joint = ACT_jointGetAccessories((Actor *)actor, coordinate);
        if (joint < 0) {
            MOutputDebugStringWarn(D_00A51318, coordinate);
        } else {
            /* Integer form on purpose: the original adds the joint's byte
               offset to the block address with the offset as the first
               operand, which every pointer-arithmetic spelling (ptr + joint,
               &ptr[joint], via a named local) reverses ($2/$3 swap and an
               extra move). */
            Matrix4 *jointMatrix = (Matrix4 *)(joint * sizeof(Matrix4) + (u32)actor->jointMatrices);

            /* The R5900's native 128-bit copy of the joint's four rows. */
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
                : "r"(destination), "r"(jointMatrix)
                : "$8", "$9", "$10", "$11", "memory"
            );
        }
    } else {
        MOutputDebugStringWarn(D_00A51340, coordinate);
    }
    return destination;
}

/*
 * MEfGetActorCoord: the translation row of MEfGetActorMatrix, with w = 1.
 */
Vector4 *MEfGetActorCoord(Vector4 *destination, Actor *actor, u32 coordinate)
{
    Matrix4 matrix;

    MEfGetActorMatrix(&matrix, actor, coordinate);
    __asm__ __volatile__(
        "lqc2 $vf1,48(%1)\n\t"
        "vmove.w $vf1w,$vf0w\n\t"
        "sqc2 $vf1,0(%0)\n\t"
        :
        : "r"(destination), "r"(matrix)
        : "memory"
    );
    return destination;
}

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
       lane to the architectural VF0.w = 1 rather than keeping actor's own w. */
    __asm__ __volatile__(
        "lqc2 $vf1,0(%1)\n\t"
        "vmove.w $vf1w,$vf0w\n\t"
        "sqc2 $vf1,48(%0)"
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

Vector4 *MEfCalcWeaponCoord(Vector4 *destination, const MEfWeaponCoord *weapon)
{
    Vector4 actor_matrix[4];

    MEfGetActorMatrix(actor_matrix, weapon->actor, weapon->coordinate);
    MMathApplyMatrix(destination, actor_matrix, &weapon->local_position);
    destination->w = 1.0f;
    return destination;
}
