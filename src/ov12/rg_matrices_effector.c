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

static void _InitCombine(MatrixCombine *combine);
static void _EffectorSetActivity(MatrixEffector *effector, int activity);
static void _DestructEffector(MatrixEffector *effector);
static void _EffectorJob(MatrixEffector *effector);
static MatrixConstraint *_CreateConstraint(void);
static void _ConstraintSetRoot(MatrixConstraint *constraint, void *root,
                               void *joint);
static void _ConstraintSetTop(MatrixConstraint *constraint, void *top);

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

/*
 * A double effector's per-unit record: the two manipulators
 * RgMatricesEffectorAddDoubleMani (0x00a37ea8) pairs into one heap block,
 * _EffectorAddDoubleUnit (0x00a373f0) allocates and fills, and
 * _DestructDoubleEffector (0x00a36f90) frees back apart.
 */
typedef struct ManipulatorPair ManipulatorPair;
struct ManipulatorPair {
    void *first;
    void *second;
};

/*
 * The manipulator's setter callback at offset four, the counterpart of the
 * getter _ManiGet (0x00a36e18) reads at offset zero.  Both are called with
 * the manipulator forwarded unmodified in $a0, so the callback can identify
 * which instance to write back through; the manipulator's own shape belongs
 * to another TU (see the MatrixCombine.root/top comment above), so it stays
 * the named-offset fallback of docs/style.md rule 2.
 */
#define MANIPULATOR_SET(manipulator) \
    (*(void (**)(void *, RgMatrix))((unsigned char *)(manipulator) + 4))

static void _ManiSet(void *manipulator, RgMatrix matrix)
{
    void (*setter)(void *manipulator, RgMatrix matrix);

    if (manipulator == 0) {
        return;
    }
    setter = MANIPULATOR_SET(manipulator);
    if (setter != 0) {
        setter(manipulator, matrix);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _ManiGet);

extern void *CreateRgVector(int capacity, const char *source_file, int line);

static void _InitEffector(MatrixEffector *effector)
{
    void *units;

    effector->identifier = (void *) 0;
    units = CreateRgVector(0x30, D_00A565D8, 75);
    effector->start = (void *) 0;
    effector->units = units;
    effector->apply = (void *) 0;
    effector->destruct = (void *) 0;
    effector->set_activity = (void *) 0;
    effector->activity = 1;
}

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

static void _DestructDoubleEffector(MatrixEffector *effector)
{
    unsigned int count;
    unsigned int index;
    ManipulatorPair *pair;

    index = 0;
    count = RgVectorSize(effector->units);
    if (count != 0) {
        do {
            pair = RgVectorIndex(effector->units, index, D_00A565D8, 113);
            index++;
            if (pair->first != 0) {
                RgHeapFree(InstanceOfRgHeap(), pair->first, D_00A565D8, 115);
            }
            if (pair->second != 0) {
                RgHeapFree(InstanceOfRgHeap(), pair->second, D_00A565D8, 117);
            }
            RgHeapFree(InstanceOfRgHeap(), pair, D_00A565D8, 118);
        } while (index < count);
    }
    DisposeRgVector(effector->units, D_00A565D8, 120);
}

extern void RgError(const char *message, const char *source_file, int line,
                    ...);
extern const char D_00A565F8[];
extern int s_inSingleIdentifier;
extern int s_inDoubleIdentifier;

static void _DestructEffector(MatrixEffector *effector)
{
    void *identifier;

    identifier = effector->identifier;
    if (identifier == &s_inDoubleIdentifier) {
        _DestructDoubleEffector(effector);
    } else if (identifier == &s_inSingleIdentifier) {
        _DestructSingleEffector(effector);
    } else {
        RgError(D_00A565F8, D_00A565D8, 130);
    }
    if (effector->destruct != 0) {
        effector->destruct(effector);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _SingleEffectorJob);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_matrices_effector", _DoubleEffectorJob);

static void _EffectorSetActivity(MatrixEffector *effector, int activity)
{
    void (*set_activity)(MatrixEffector *effector, int activity);

    if (effector == 0) {
        assert_prog(D_00A56610, D_00A565D8, 187);
    }
    set_activity = effector->set_activity;
    effector->activity = activity;
    if (set_activity != 0) {
        set_activity(effector, activity);
    }
}

static void _EffectorAddUnit(MatrixEffector *effector, void *unit)
{
    if (unit != 0) {
        RgVectorPush(effector->units, unit);
    }
}

/*
 * One manipulator: the get/set record _ManiGet (0x00a36e18) reads at offset
 * zero and _ManiSet (0x00a36de8) at offset four.  Its shape belongs to the TU
 * that builds one, so the type is named here and never completed.
 */
typedef struct MatrixManipulator MatrixManipulator;

/*
 * The heap block is the ManipulatorPair above - two manipulators in a row,
 * which _DestructDoubleEffector (0x00a36f90) reads back as `first` and
 * `second` and frees apart - so its size is that record's.  The two slots are
 * written here through the manipulator pointer the words actually hold,
 * because the original reads the effector's `units` pointer at 0x00a3742c
 * ahead of both stores, an order the compiler only reaches while the words
 * being written and `units` are of different types.
 */
static void _EffectorAddDoubleUnit(MatrixEffector *effector,
                                   MatrixManipulator *first,
                                   MatrixManipulator *second)
{
    MatrixManipulator **pair;

    pair = RgHeapAlloc(InstanceOfRgHeap(), sizeof(ManipulatorPair), D_00A565D8,
                       202);
    pair[0] = first;
    pair[1] = second;
    RgVectorPush(effector->units, pair);
}

static void _EffectorJob(MatrixEffector *effector)
{
    void (*job)(MatrixEffector *effector);

    if (effector == 0) {
        assert_prog(D_00A56610, D_00A565D8, 214);
    }
    job = effector->job;
    if (job != 0) {
        job(effector);
    }
}

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

/*
 * The constraint's two stored 4x4 matrices _ConstraintFunc multiplies the
 * target matrix by, in sequence.  MatrixConstraint's layout stays
 * unrecovered (see the point-to vector above), so these follow the same
 * named-offset fallback of docs/style.md rule 2; each RgMatrix is 0x40
 * bytes, so the two lie back-to-back at +0x40 and +0x80.
 */
#define MATRIX_CONSTRAINT_MATRIX_1(constraint) \
    ((float *)((unsigned char *)(constraint) + 0x40))
#define MATRIX_CONSTRAINT_MATRIX_2(constraint) \
    ((float *)((unsigned char *)(constraint) + 0x80))

static void _ConstraintFunc(MatrixConstraint *constraint, RgMatrix matrix)
{
    XrgMulMatrix(matrix, MATRIX_CONSTRAINT_MATRIX_1(constraint), matrix);
    XrgMulMatrix(matrix, MATRIX_CONSTRAINT_MATRIX_2(constraint), matrix);
}

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

extern void XrgUnitVector(RgVector vector);
static void _ConstraintStartFunc(MatrixConstraint *constraint);

/*
 * The constraint's blend weight, ramped in _ConstraintStartFunc (0x00a37528)
 * by +-0.3 per pass and clamped to [0.0, 1.0], exactly like
 * MatrixCombine.weight; MatrixConstraint's layout stays unrecovered (see the
 * point-to vector above), so this keeps the same named-offset fallback.
 */
#define MATRIX_CONSTRAINT_WEIGHT(constraint) \
    (*(float *)((unsigned char *)(constraint) + 0xC0))

static void _InitConstraint(MatrixConstraint *constraint) {
    MatrixEffector *effector;

    effector = (MatrixEffector *) constraint;
    _InitSingleEffector(effector);
    effector->start = (void (*)(MatrixEffector *)) _ConstraintStartFunc;
    effector->apply = _ConstraintFunc;
    effector->destruct = (void (*)(MatrixEffector *)) _ConstraintDestructFunc;
    MATRIX_CONSTRAINT_MANIPULATOR_1(constraint) = 0;
    MATRIX_CONSTRAINT_MANIPULATOR_2(constraint) = 0;
    MATRIX_CONSTRAINT_MANIPULATOR_3(constraint) = 0;
    MATRIX_CONSTRAINT_WEIGHT(constraint) = 0.0f;
    XrgUnitVector(MATRIX_CONSTRAINT_POINT_TO(constraint));
}

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

static void _ConstraintSetRoot(MatrixConstraint *constraint, void *root, void *joint)
{
    if (MATRIX_CONSTRAINT_MANIPULATOR_1(constraint) != 0) {
        RgHeapFree(InstanceOfRgHeap(), MATRIX_CONSTRAINT_MANIPULATOR_1(constraint),
                  D_00A565D8, 363);
    }
    if (MATRIX_CONSTRAINT_MANIPULATOR_2(constraint) != 0) {
        RgHeapFree(InstanceOfRgHeap(), MATRIX_CONSTRAINT_MANIPULATOR_2(constraint),
                  D_00A565D8, 365);
    }
    MATRIX_CONSTRAINT_MANIPULATOR_1(constraint) = root;
    MATRIX_CONSTRAINT_MANIPULATOR_2(constraint) = joint;
}

static void _ConstraintSetTop(MatrixConstraint *constraint, void *top)
{
    if (MATRIX_CONSTRAINT_MANIPULATOR_3(constraint) != 0) {
        RgHeapFree(InstanceOfRgHeap(), MATRIX_CONSTRAINT_MANIPULATOR_3(constraint),
                  D_00A565D8, 375);
    }
    MATRIX_CONSTRAINT_MANIPULATOR_3(constraint) = top;
}

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

static void _InitCombine(MatrixCombine *combine)
{
    if (combine == (void *) 0) {
        assert_prog(D_00A56610, D_00A565D8, 462);
    }
    _InitDoubleEffector(&combine->effector);
    combine->effector.start = (void (*)(MatrixEffector *)) _CombineStartFunc;
    combine->effector.apply = _CombineFunc;
    combine->root = (void *) 0;
    combine->effector.destruct = (void (*)(MatrixEffector *)) _CombineDestructFunc;
    combine->top = (void *) 0;
}

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

extern const char D_00A56620[];

void RgMatricesEffectorAddManipulator(MatrixEffector *effector, void *unit)
{
    if (effector == 0) {
        assert_prog(D_00A56610, D_00A565D8, 513);
    }
    if (effector->identifier != &s_inSingleIdentifier) {
        assert_prog(D_00A56620, D_00A565D8, 514);
    }
    _EffectorAddUnit(effector, unit);
}

extern const char D_00A56638[];

void RgMatricesEffectorAddDoubleMani(MatrixEffector *effector, void *first, void *second)
{
    if (effector == 0) {
        assert_prog(D_00A56610, D_00A565D8, 520);
    }
    if (effector->identifier != &s_inDoubleIdentifier) {
        assert_prog(D_00A56638, D_00A565D8, 521);
    }
    _EffectorAddDoubleUnit(effector, first, second);
}

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
