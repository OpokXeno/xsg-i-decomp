/*
 * OV12 original TU 68: 0x00a36de8..0x00a380d8 (39 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_error.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_matrices_effector.h"
#include "ov12/rg_vector.h"
#include "ov12/xrg_rand_int.h"

/* RgHeapAlloc's byte counts: the matrix combine and constraint allocations. */
#define MATRIX_COMBINE_ALLOC_BYTES 0x140
#define MATRIX_CONSTRAINT_ALLOC_BYTES 0xD0

/* Capacity RgVector of units is created with. */
#define MATRIX_EFFECTOR_UNIT_CAPACITY 0x30

/* Per-pass weight ramps of the constraint and the combine effectors. */
#define MATRIX_CONSTRAINT_WEIGHT_STEP 0.3f
#define MATRIX_COMBINE_WEIGHT_RISE 0.4f
#define MATRIX_COMBINE_WEIGHT_FALL 0.15f

/* Per-unit record of a double effector: its two manipulators, in one heap block. */
typedef struct ManipulatorPair ManipulatorPair;
struct ManipulatorPair {
    SkeMani *first;
    SkeMani *second;
};

/*
 * MatrixEffector.units points at an RgVector of per-unit records. That container
 * belongs to rg_vector.c, so this file keeps the field as a generic pointer.
 */
static void _InitCombine(MatrixCombine *combine);
static void _EffectorSetActivity(MatrixEffector *effector, int activity);
static void _DestructEffector(MatrixEffector *effector);
static void _EffectorJob(MatrixEffector *effector);
static MatrixConstraint *_CreateConstraint(void);
static void _ConstraintSetRoot(MatrixConstraint *constraint, SkeMani *root,
                               SkeMani *joint);
static void _ConstraintSetTop(MatrixConstraint *constraint, SkeMani *top);
static void _ManiGet(SkeMani *manipulator, RgMatrix matrix);
static void _InitEffector(MatrixEffector *effector);
static void _SingleEffectorJob(MatrixEffector *effector);
static void _DoubleEffectorJob(MatrixEffector *effector);
static void _ConstraintStartFunc(MatrixConstraint *constraint);
static void _InitConstraint(MatrixConstraint *constraint);

const char matricesEffectorSourcePath[] = "../rg_matrices_effector.euc.c";
const char matricesEffectorUnknownTypeMessage[] = "unknown effector type";
const char matricesEffectorAssertEffNotNil[] = "pEff != NIL";
const char matricesEffectorAssertIsSingle[] = "_IsSingleEffector(pEff)";
const char matricesEffectorAssertIsDouble[] = "_IsDoubleEffector(pEff)";
static int s_inSingleIdentifier;
static int s_inDoubleIdentifier;

static void _ManiSet(SkeMani *manipulator, RgMatrix matrix)
{
    if (manipulator == 0) {
        return;
    }
    if (manipulator->set != 0) {
        manipulator->set(manipulator, matrix);
    }
}


/* Reads a manipulator's matrix. The identity matrix is used when it has no getter. */
static void _ManiGet(SkeMani *manipulator, RgMatrix matrix)
{
    if (manipulator != 0) {
        if (manipulator->get != 0) {
            manipulator->get(manipulator, matrix);
            return;
        }
        return XrgUnitMatrix(matrix);
    }

    return XrgUnitMatrix(matrix);
}


static void _InitEffector(MatrixEffector *effector)
{
    void *units;

    effector->identifier = 0;
    units = CreateRgVector(MATRIX_EFFECTOR_UNIT_CAPACITY, matricesEffectorSourcePath, 75);
    effector->start = 0;
    effector->units = units;
    effector->apply = 0;
    effector->destruct = 0;
    effector->set_activity = 0;
    effector->activity = 1;
}

/* Frees each unit and then the unit vector. */
static void _DestructSingleEffector(MatrixEffector *effector)
{
    unsigned int count;
    unsigned int index;
    void *unit;

    index = 0;
    count = RgVectorSize(effector->units);
    if (count != 0) {
        do {
            unit = RgVectorIndex(effector->units, index, matricesEffectorSourcePath, 100);
            index++;
            if (unit != 0) {
                RgHeapFree(InstanceOfRgHeap(), unit, matricesEffectorSourcePath, 102);
            }
        } while (index < count);
    }
    DisposeRgVector(effector->units, matricesEffectorSourcePath, 104);
}

/* Frees both manipulators and the pair record of every unit, then the unit vector. */
static void _DestructDoubleEffector(MatrixEffector *effector)
{
    unsigned int count;
    unsigned int index;
    ManipulatorPair *pair;

    index = 0;
    count = RgVectorSize(effector->units);
    if (count != 0) {
        do {
            pair = RgVectorIndex(effector->units, index, matricesEffectorSourcePath, 113);
            index++;
            if (pair->first != 0) {
                RgHeapFree(InstanceOfRgHeap(), pair->first, matricesEffectorSourcePath, 115);
            }
            if (pair->second != 0) {
                RgHeapFree(InstanceOfRgHeap(), pair->second, matricesEffectorSourcePath, 117);
            }
            RgHeapFree(InstanceOfRgHeap(), pair, matricesEffectorSourcePath, 118);
        } while (index < count);
    }
    DisposeRgVector(effector->units, matricesEffectorSourcePath, 120);
}


/* Frees the units for the effector kind, then runs its destruct hook. */
static void _DestructEffector(MatrixEffector *effector)
{
    void *identifier;

    identifier = effector->identifier;
    if (identifier == &s_inDoubleIdentifier) {
        _DestructDoubleEffector(effector);
    } else if (identifier == &s_inSingleIdentifier) {
        _DestructSingleEffector(effector);
    } else {
        RgError(matricesEffectorUnknownTypeMessage, matricesEffectorSourcePath, 130);
    }
    if (effector->destruct != 0) {
        effector->destruct(effector);
    }
}

/* Runs one pass: start hook, then apply to each unit matrix and store it back. */
static void _SingleEffectorJob(MatrixEffector *effector)
{
    unsigned int unit_count;
    unsigned int index;
    void *manipulator;
    void (*apply)();
    RgMatrix matrix;

    if (effector == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 140);
    }
    if (effector->start != 0) {
        effector->start(effector);
    }

    if (effector->apply != 0) {
        apply = effector->apply;
        unit_count = RgVectorSize(effector->units);
        if (effector->identifier != &s_inSingleIdentifier) {
            assert_prog(matricesEffectorAssertIsSingle, matricesEffectorSourcePath, 149);
        }
        for (index = 0; index < unit_count; index++) {
            manipulator = RgVectorIndex(effector->units, index, matricesEffectorSourcePath,
                                       152);
            _ManiGet(manipulator, matrix);
            apply(effector, matrix);
            _ManiSet(manipulator, matrix);
        }
    }
}

/* Runs one pass: start hook, then apply to each pair of unit matrices and store both back. */
static void _DoubleEffectorJob(MatrixEffector *effector)
{
    unsigned int unit_count;
    unsigned int index;
    ManipulatorPair *pair;
    void (*apply)();
    RgMatrix first_matrix;
    RgMatrix second_matrix;

    if (effector == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 162);
    }
    if (effector->start != 0) {
        effector->start(effector);
    }

    if (effector->apply != 0) {
        apply = effector->apply;
        unit_count = RgVectorSize(effector->units);
        for (index = 0; index < unit_count; index++) {
            pair = RgVectorIndex(effector->units, index, matricesEffectorSourcePath, 173);
            _ManiGet(pair->first, first_matrix);
            _ManiGet(pair->second, second_matrix);
            apply(effector, first_matrix, second_matrix);
            _ManiSet(pair->first, first_matrix);
            _ManiSet(pair->second, second_matrix);
        }
    }
}

/* Stores the activity flag and calls the effector's set_activity hook. */
static void _EffectorSetActivity(MatrixEffector *effector, int activity)
{
    void (*set_activity)(MatrixEffector *effector, int activity);

    if (effector == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 187);
    }
    set_activity = effector->set_activity;
    effector->activity = activity;
    if (set_activity != 0) {
        set_activity(effector, activity);
    }
}

static void _EffectorAddUnit(MatrixEffector *effector, SkeMani *unit)
{
    if (unit != 0) {
        RgVectorPush(effector->units, unit);
    }
}

/* Allocates a pair record for two manipulators and appends it to the units. */
static void _EffectorAddDoubleUnit(MatrixEffector *effector, SkeMani *first,
                                   SkeMani *second)
{
    ManipulatorPair *pair;

    pair = RgHeapAlloc(InstanceOfRgHeap(), sizeof(ManipulatorPair), matricesEffectorSourcePath,
                       202);
    pair->first = first;
    pair->second = second;
    RgVectorPush(effector->units, pair);
}

static void _EffectorJob(MatrixEffector *effector)
{
    void (*job)(MatrixEffector *effector);

    if (effector == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 214);
    }
    job = effector->job;
    if (job != 0) {
        job(effector);
    }
}

static void _InitSingleEffector(MatrixEffector *effector)
{
    _InitEffector(effector);
    effector->identifier = &s_inSingleIdentifier;
    effector->job = _SingleEffectorJob;
}


static void _InitDoubleEffector(MatrixEffector *effector)
{
    _InitEffector(effector);
    effector->identifier = &s_inDoubleIdentifier;
    effector->job = _DoubleEffectorJob;
}

/*
 * Moves the constraint weight up while the effector is active and down otherwise,
 * clamped to [0, 1]. Builds the root and constrained frames from the three manipulators.
 * Identity frames are used while any manipulator is missing.
 */
static void _ConstraintStartFunc(MatrixConstraint *constraint)
{
    RgMatrix root_frame;
    RgMatrix top_frame;
    RgMatrix root_matrix;
    RgMatrix joint_matrix;
    RgMatrix top_matrix;
    RgVector direction;
    RgVector blended_axis;
    float bound;

    if (constraint->joint_manipulator == 0 ||
        constraint->root_manipulator == 0 ||
        constraint->top_manipulator == 0) {
        XrgUnitMatrix(constraint->root_inverse);
        XrgUnitMatrix(constraint->constrained_frame);
        return;
    }

    if (constraint->effector.activity != 0) {
        constraint->weight += MATRIX_CONSTRAINT_WEIGHT_STEP;
    } else {
        constraint->weight -= MATRIX_CONSTRAINT_WEIGHT_STEP;
    }
    bound = 0.0f;
    if (bound > constraint->weight) {
        constraint->weight = 0.0f;
    } else {
        constraint->weight =
            constraint->weight > 1.0f
                ? 1.0f
                : constraint->weight;
    }

    _ManiGet(constraint->root_manipulator, root_matrix);
    _ManiGet(constraint->joint_manipulator, joint_matrix);
    _ManiGet(constraint->top_manipulator, top_matrix);

    /* Root frame, aimed from the root toward the top manipulator. */
    XrgSubVector(direction, &top_matrix[12], &root_matrix[12]);
    XrgCalcMatrixXtoZ(root_frame, direction, &root_matrix[8]);
    XrgCopyVector(&root_frame[12], &joint_matrix[12]);
    root_frame[15] = 1.0f;
    XrgInvMatrix(constraint->root_inverse, root_frame);

    /* Blends the target-direction frame with the root frame by the weight. */
    XrgSubVector(direction, constraint->point_to,
                 &root_matrix[12]);
    XrgCalcMatrixXtoZ(top_frame, direction, &root_matrix[8]);
    XrgLinearIntpVector(direction, top_frame, root_frame, constraint->weight);
    XrgCopyVector(blended_axis, &root_frame[4]);
    XrgCalcMatrixXtoY(constraint->constrained_frame, direction,
                      blended_axis);
    XrgCopyVector(&constraint->constrained_frame[12], &joint_matrix[12]);
}

static void _ConstraintFunc(MatrixConstraint *constraint, RgMatrix matrix)
{
    XrgMulMatrix(matrix, constraint->root_inverse, matrix);
    XrgMulMatrix(matrix, constraint->constrained_frame, matrix);
}

/* Frees the top, root and joint manipulators that the constraint holds. */
static void _ConstraintDestructFunc(MatrixConstraint *constraint)
{
    if (constraint->top_manipulator != 0) {
        RgHeapFree(InstanceOfRgHeap(), constraint->top_manipulator,
                  matricesEffectorSourcePath, 326);
    }
    if (constraint->root_manipulator != 0) {
        RgHeapFree(InstanceOfRgHeap(), constraint->root_manipulator,
                  matricesEffectorSourcePath, 328);
    }
    if (constraint->joint_manipulator == 0) {
        return;
    }
    RgHeapFree(InstanceOfRgHeap(), constraint->joint_manipulator,
              matricesEffectorSourcePath, 330);
}

static void _InitConstraint(MatrixConstraint *constraint) {
    MatrixEffector *effector;

    effector = &constraint->effector;
    _InitSingleEffector(effector);
    effector->start = (void (*)(MatrixEffector *)) _ConstraintStartFunc;
    effector->apply = _ConstraintFunc;
    effector->destruct = (void (*)(MatrixEffector *)) _ConstraintDestructFunc;
    constraint->root_manipulator = 0;
    constraint->joint_manipulator = 0;
    constraint->top_manipulator = 0;
    constraint->weight = 0.0f;
    XrgUnitVector(constraint->point_to);
}

static MatrixConstraint *_CreateConstraint(void)
{
    MatrixConstraint *constraint;

    constraint = RgHeapAlloc(InstanceOfRgHeap(), MATRIX_CONSTRAINT_ALLOC_BYTES, matricesEffectorSourcePath, 352);
    if (constraint == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 353);
    }
    _InitConstraint(constraint);
    return constraint;
}

static void _ConstraintSetRoot(MatrixConstraint *constraint, SkeMani *root, SkeMani *joint)
{
    if (constraint->root_manipulator != 0) {
        RgHeapFree(InstanceOfRgHeap(), constraint->root_manipulator,
                  matricesEffectorSourcePath, 363);
    }
    if (constraint->joint_manipulator != 0) {
        RgHeapFree(InstanceOfRgHeap(), constraint->joint_manipulator,
                  matricesEffectorSourcePath, 365);
    }
    constraint->root_manipulator = root;
    constraint->joint_manipulator = joint;
}

static void _ConstraintSetTop(MatrixConstraint *constraint, SkeMani *top)
{
    if (constraint->top_manipulator != 0) {
        RgHeapFree(InstanceOfRgHeap(), constraint->top_manipulator,
                  matricesEffectorSourcePath, 375);
    }
    constraint->top_manipulator = top;
}

static void _ConstraintGetPointTo(MatrixConstraint *constraint, RgVector output)
{
    XrgCopyVector(output, constraint->point_to);
}

static void _ConstraintSetPointTo(MatrixConstraint *constraint, RgVector input)
{
    XrgCopyVector(constraint->point_to, input);
}

/*
 * Refreshes the root and top matrices and their inverses. The combine weight rises
 * while the effector is active and falls otherwise, clamped to [0, 1].
 */
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
        float step = MATRIX_COMBINE_WEIGHT_RISE;
        float boundary;

        if (combine->effector.activity == 0)
            step = -MATRIX_COMBINE_WEIGHT_FALL;

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

/*
 * Blends the first matrix toward root * top^-1 * second by the combine weight, when
 * the weight is positive.
 */
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

/* Frees the root and top manipulators that the combine holds. */
static void _CombineDestructFunc(MatrixCombine *combine)
{
    if (combine->root != 0) {
        RgHeapFree(InstanceOfRgHeap(), combine->root, matricesEffectorSourcePath, 453);
    }
    if (combine->top == 0) {
        return;
    }
    RgHeapFree(InstanceOfRgHeap(), combine->top, matricesEffectorSourcePath, 455);
}

static void _InitCombine(MatrixCombine *combine)
{
    if (combine == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 462);
    }
    _InitDoubleEffector(&combine->effector);
    combine->effector.start = (void (*)(MatrixEffector *)) _CombineStartFunc;
    combine->effector.apply = _CombineFunc;
    combine->root = 0;
    combine->effector.destruct = (void (*)(MatrixEffector *)) _CombineDestructFunc;
    combine->top = 0;
}

MatrixCombine *CreateRgMatEffCombine(void)
{
    MatrixCombine *combine;

    combine = RgHeapAlloc(InstanceOfRgHeap(), MATRIX_COMBINE_ALLOC_BYTES,
                          matricesEffectorSourcePath, 475);
    _InitCombine(combine);
    return combine;
}

void RgMatEffCombineSetRoot(MatrixCombine *combine, SkeMani *root, SkeMani *top)
{
    if (combine->root != 0) {
        RgHeapFree(InstanceOfRgHeap(), combine->root, matricesEffectorSourcePath, 488);
    }
    combine->root = root;
    if (combine->top != 0) {
        RgHeapFree(InstanceOfRgHeap(), combine->top, matricesEffectorSourcePath, 491);
    }
    combine->top = top;
}

void RgMatricesEffectorSetActivity(MatrixEffector *effector, int activity)
{
    if (effector == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 500);
    }
    _EffectorSetActivity(effector, activity);
}

void DisposeRgMatricesEffector(MatrixEffector *effector)
{
    if (effector == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 506);
    }
    _DestructEffector(effector);
    RgHeapFree(InstanceOfRgHeap(), effector, matricesEffectorSourcePath, 508);
}


void RgMatricesEffectorAddManipulator(MatrixEffector *effector, SkeMani *unit)
{
    if (effector == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 513);
    }
    if (effector->identifier != &s_inSingleIdentifier) {
        assert_prog(matricesEffectorAssertIsSingle, matricesEffectorSourcePath, 514);
    }
    _EffectorAddUnit(effector, unit);
}


void RgMatricesEffectorAddDoubleMani(MatrixEffector *effector, SkeMani *first,
                                     SkeMani *second)
{
    if (effector == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 520);
    }
    if (effector->identifier != &s_inDoubleIdentifier) {
        assert_prog(matricesEffectorAssertIsDouble, matricesEffectorSourcePath, 521);
    }
    _EffectorAddDoubleUnit(effector, first, second);
}

void RgMatricesEffectorJob(MatrixEffector *effector)
{
    if (effector == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 527);
    }
    _EffectorJob(effector);
}

MatrixConstraint *CreateRgMatEffConstraint(void)
{
    return _CreateConstraint();
}

void RgMatEffConstraintSetRoot(MatrixConstraint *constraint, SkeMani *root,
                               SkeMani *joint)
{
    if (constraint == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 538);
    }
    _ConstraintSetRoot(constraint, root, joint);
}

void RgMatEffConstraintSetTop(MatrixConstraint *constraint, SkeMani *top)
{
    if (constraint == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 544);
    }
    _ConstraintSetTop(constraint, top);
}

void RgMatEffConstraintSetTarget(MatrixConstraint *constraint, RgVector target)
{
    if (constraint == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 550);
    }
    _ConstraintSetPointTo(constraint, target);
}

void RgMatEffConstraintGetTarget(MatrixConstraint *constraint, RgVector target)
{
    if (constraint == 0) {
        assert_prog(matricesEffectorAssertEffNotNil, matricesEffectorSourcePath, 556);
    }
    _ConstraintGetPointTo(constraint, target);
}
