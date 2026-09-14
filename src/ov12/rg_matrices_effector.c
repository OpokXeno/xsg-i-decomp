/*
 * OV12 original TU 68: 0x00a36de8..0x00a380d8 (39 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_matrices_effector.h"

/*
 * The constraint's point-to vector.  MatrixConstraint's layout is not recovered
 * (src/ov12/rg_matrices_effector.h), so this one offset keeps the named-offset
 * fallback of docs/style.md rule 2: _InitConstraint seeds it with XrgUnitVector
 * (0x00a3783c) and _ConstraintStartFunc aims the constrained frame at it
 * (XrgSubVector against the root frame's translation, 0x00a37674).
 */
#define MATRIX_CONSTRAINT_POINT_TO(constraint) \
    ((float *)((unsigned char *)(constraint) + 0x30))

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ManiSet);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ManiGet);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _InitEffector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _DestructSingleEffector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _DestructDoubleEffector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _DestructEffector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _SingleEffectorJob);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _DoubleEffectorJob);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _EffectorSetActivity);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _EffectorAddUnit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _EffectorAddDoubleUnit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _EffectorJob);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _InitSingleEffector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _InitDoubleEffector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ConstraintStartFunc);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ConstraintFunc);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ConstraintDestructFunc);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _InitConstraint);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _CreateConstraint);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ConstraintSetRoot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ConstraintSetTop);

static void _ConstraintGetPointTo(MatrixConstraint *constraint, RgVector output)
{
    XrgCopyVector(output, MATRIX_CONSTRAINT_POINT_TO(constraint));
}

static void _ConstraintSetPointTo(MatrixConstraint *constraint, RgVector input)
{
    XrgCopyVector(MATRIX_CONSTRAINT_POINT_TO(constraint), input);
}

static void _CombineStartFunc(MatrixCombine *combine)
{
    if (combine->root == 0)
        return;
    if (combine->top == 0)
        return;

    _ManiGet(combine->root, combine->root_matrix);
    _ManiGet(combine->top, combine->top_matrix);
    XrgInvMatrix(combine->root_inverse, combine->root_matrix);
    XrgInvMatrix(combine->top_inverse, combine->top_matrix);

    {
        float weight = 0.0f;
        float step = 0.400000006f;
        float boundary;

        if (combine->effector.activity == 0)
            step = -0.150000006f;

        boundary = weight;
        weight = combine->weight + step;
        combine->weight = weight;
        if (weight < boundary) {
            combine->weight = boundary;
        } else {
            boundary = 1.0f;
            if (!(boundary < weight))
                boundary = weight;
            combine->weight = boundary;
        }
    }
}

static void _CombineFunc(MatrixCombine *combine, RgMatrix first, RgMatrix second)
{
    RgMatrix transform;
    RgVector x_axis;
    RgVector y_axis;
    RgVector translation;

    if (combine->weight > 0.0f) {
        XrgMulMatrix(transform, combine->top_inverse, second);
        XrgMulMatrix(transform, combine->root_matrix, transform);
        XrgLinearIntpVector(x_axis, transform, first, combine->weight);
        XrgLinearIntpVector(y_axis, transform + 4, first + 4, combine->weight);
        XrgLinearIntpVector(translation, transform + 12, first + 12, combine->weight);
        XrgCalcMatrixXtoY(first, x_axis, y_axis);
        XrgCopyVector(first + 12, translation);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _CombineDestructFunc);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _InitCombine);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", CreateRgMatEffCombine);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", RgMatEffCombineSetRoot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", RgMatricesEffectorSetActivity);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", DisposeRgMatricesEffector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", RgMatricesEffectorAddManipulator);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", RgMatricesEffectorAddDoubleMani);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", RgMatricesEffectorJob);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", CreateRgMatEffConstraint);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", RgMatEffConstraintSetRoot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", RgMatEffConstraintSetTop);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", RgMatEffConstraintSetTarget);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", RgMatEffConstraintGetTarget);
