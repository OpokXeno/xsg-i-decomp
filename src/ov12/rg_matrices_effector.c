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

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern const char D_00A565D8[];
extern const char D_00A56610[];

extern void _InitCombine(MatrixCombine *combine);
extern void _EffectorSetActivity(MatrixEffector *effector, int activity);
extern void _DestructEffector(MatrixEffector *effector);
extern void _EffectorJob(MatrixEffector *effector);
extern MatrixConstraint *_CreateConstraint(void);
extern void _ConstraintSetRoot(MatrixConstraint *constraint, void *root,
                               void *joint);
extern void _ConstraintSetTop(MatrixConstraint *constraint, void *top);

/*
 * The per-unit RgVector container (ov12/tu050, src/ov12/rg_vector.c) that
 * MatrixEffector.units points at.  Its RgVectorPrefix layout belongs to that
 * TU, so these four entry points keep the generic pointer this TU already
 * uses for the field.
 */
extern unsigned int RgVectorSize(void *vector);
extern void *RgVectorIndex(void *vector, unsigned int index,
                           const char *source_file, int line);
extern void RgVectorPush(void *vector, void *element);
extern void DisposeRgVector(void *vector, const char *source_file, int line);

/*
 * MatrixConstraint's three still-unrecovered manipulator slots.  Like the
 * point-to vector above, docs/style.md rule 2's named-offset fallback:
 * _ConstraintDestructFunc (0x00a37738) only frees each through RgHeapFree
 * when non-null, so no store site evidences a field name yet.
 */
#define MATRIX_CONSTRAINT_MANIPULATOR_1(constraint) \
    (*(void **)((unsigned char *)(constraint) + 0x20))
#define MATRIX_CONSTRAINT_MANIPULATOR_2(constraint) \
    (*(void **)((unsigned char *)(constraint) + 0x24))
#define MATRIX_CONSTRAINT_MANIPULATOR_3(constraint) \
    (*(void **)((unsigned char *)(constraint) + 0x28))

/*
 * RgHeapAlloc's byte count for one MatrixCombine: sizeof(MatrixCombine) plus
 * the twelve trailing bytes documented on struct MatrixCombine
 * (src/ov12/rg_matrices_effector.h) that no instruction in the six images
 * reads or writes.
 */
#define MATRIX_COMBINE_ALLOC_BYTES 0x140

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ManiSet);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ManiGet);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _InitEffector);

static void _DestructSingleEffector(MatrixEffector *effector)
{
    unsigned int count;
    unsigned int index;
    void *unit;

    index = 0;
    count = RgVectorSize(effector->units);
    if (count != 0) {
        do {
            unit = RgVectorIndex(effector->units, index, D_00A565D8, 100);
            index++;
            if (unit != 0) {
                RgHeapFree(InstanceOfRgHeap(), unit, D_00A565D8, 102);
            }
        } while (index < count);
    }
    DisposeRgVector(effector->units, D_00A565D8, 104);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _DestructDoubleEffector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _DestructEffector);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _SingleEffectorJob);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _DoubleEffectorJob);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _EffectorSetActivity);

static void _EffectorAddUnit(MatrixEffector *effector, void *unit)
{
    if (unit != 0) {
        RgVectorPush(effector->units, unit);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _EffectorAddDoubleUnit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _EffectorJob);

static void _InitEffector(MatrixEffector *effector);
extern int s_inSingleIdentifier;
static void _SingleEffectorJob(MatrixEffector *effector);

static void _InitSingleEffector(MatrixEffector *effector)
{
    _InitEffector(effector);
    effector->identifier = &s_inSingleIdentifier;
    effector->job = _SingleEffectorJob;
}

extern int s_inDoubleIdentifier;
static void _DoubleEffectorJob(MatrixEffector *effector);

static void _InitDoubleEffector(MatrixEffector *effector)
{
    _InitEffector(effector);
    effector->identifier = &s_inDoubleIdentifier;
    effector->job = _DoubleEffectorJob;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ConstraintStartFunc);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ConstraintFunc);

static void _ConstraintDestructFunc(MatrixConstraint *constraint)
{
    if (MATRIX_CONSTRAINT_MANIPULATOR_3(constraint) != 0) {
        RgHeapFree(InstanceOfRgHeap(), MATRIX_CONSTRAINT_MANIPULATOR_3(constraint),
                  D_00A565D8, 326);
    }
    if (MATRIX_CONSTRAINT_MANIPULATOR_1(constraint) != 0) {
        RgHeapFree(InstanceOfRgHeap(), MATRIX_CONSTRAINT_MANIPULATOR_1(constraint),
                  D_00A565D8, 328);
    }
    if (MATRIX_CONSTRAINT_MANIPULATOR_2(constraint) == 0) {
        return;
    }
    RgHeapFree(InstanceOfRgHeap(), MATRIX_CONSTRAINT_MANIPULATOR_2(constraint),
              D_00A565D8, 330);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _InitConstraint);

static void _InitConstraint(MatrixConstraint *constraint);

static MatrixConstraint *_CreateConstraint(void)
{
    MatrixConstraint *constraint;

    constraint = RgHeapAlloc(InstanceOfRgHeap(), 0xD0, D_00A565D8, 352);
    if (constraint == 0) {
        assert_prog(D_00A56610, D_00A565D8, 353);
    }
    _InitConstraint(constraint);
    return constraint;
}

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

static void _CombineDestructFunc(MatrixCombine *combine)
{
    if (combine->root != 0) {
        RgHeapFree(InstanceOfRgHeap(), combine->root, D_00A565D8, 453);
    }
    if (combine->top == 0) {
        return;
    }
    RgHeapFree(InstanceOfRgHeap(), combine->top, D_00A565D8, 455);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _InitCombine);

MatrixCombine *CreateRgMatEffCombine(void)
{
    MatrixCombine *combine;

    combine = RgHeapAlloc(InstanceOfRgHeap(), MATRIX_COMBINE_ALLOC_BYTES,
                          D_00A565D8, 475);
    _InitCombine(combine);
    return combine;
}

void RgMatEffCombineSetRoot(MatrixCombine *combine, void *root, void *top)
{
    if (combine->root != 0) {
        RgHeapFree(InstanceOfRgHeap(), combine->root, D_00A565D8, 488);
    }
    combine->root = root;
    if (combine->top != 0) {
        RgHeapFree(InstanceOfRgHeap(), combine->top, D_00A565D8, 491);
    }
    combine->top = top;
}

void RgMatricesEffectorSetActivity(MatrixEffector *effector, int activity)
{
    if (effector == 0) {
        assert_prog(D_00A56610, D_00A565D8, 500);
    }
    _EffectorSetActivity(effector, activity);
}

void DisposeRgMatricesEffector(MatrixEffector *effector)
{
    if (effector == 0) {
        assert_prog(D_00A56610, D_00A565D8, 506);
    }
    _DestructEffector(effector);
    RgHeapFree(InstanceOfRgHeap(), effector, D_00A565D8, 508);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", RgMatricesEffectorAddManipulator);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", RgMatricesEffectorAddDoubleMani);

void RgMatricesEffectorJob(MatrixEffector *effector)
{
    if (effector == 0) {
        assert_prog(D_00A56610, D_00A565D8, 527);
    }
    _EffectorJob(effector);
}

MatrixConstraint *CreateRgMatEffConstraint(void)
{
    return _CreateConstraint();
}

void RgMatEffConstraintSetRoot(MatrixConstraint *constraint, void *root,
                               void *joint)
{
    if (constraint == 0) {
        assert_prog(D_00A56610, D_00A565D8, 538);
    }
    _ConstraintSetRoot(constraint, root, joint);
}

void RgMatEffConstraintSetTop(MatrixConstraint *constraint, void *top)
{
    if (constraint == 0) {
        assert_prog(D_00A56610, D_00A565D8, 544);
    }
    _ConstraintSetTop(constraint, top);
}

void RgMatEffConstraintSetTarget(MatrixConstraint *constraint, RgVector target)
{
    if (constraint == 0) {
        assert_prog(D_00A56610, D_00A565D8, 550);
    }
    _ConstraintSetPointTo(constraint, target);
}

void RgMatEffConstraintGetTarget(MatrixConstraint *constraint, RgVector target)
{
    if (constraint == 0) {
        assert_prog(D_00A56610, D_00A565D8, 556);
    }
    _ConstraintGetPointTo(constraint, target);
}
