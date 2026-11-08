#include "common.h"
#include "shared.h"
#include "main/jni.h"
#include "main/spline.h"
#include "spline.h"
#include "main/spl.h"

#define SPLINE_OBJECT_SIZE 0x494      /* xeno.util.Spline native instance */
#define SPLINE_HEAP_CATEGORY 0xE      /* xmalloc category used for the Spline object */

/* Java float[] object: class pointer, byte length, then the elements. */
struct SplineFloatArray {
    void *class_pointer;
    unsigned int byte_length;
    float *values;
};

union SplineArgumentValue {
    Spline *spline;
    struct SplineFloatArray *array;
    int integer;
};

struct SplineGetValueArgs {
    Spline *spline;
    int frame;
};

/* Vector4f object returned to Java: class pointer, then z, y, x and w at +0x04..+0x10. */
struct SplineValueVector4f {
    DispatchTable *class_ref;
    float z;
    float y;
    float x;
    float w;
    u8 unmodeled_14[4];
};

union SplineResultValue {
    unsigned int word;
    struct SplineValueVector4f *object;
};

extern void SPL_init2(Spline *spline, int order, float *control_points,
                      int point_count, int component_count);

/* Allocates a Spline object, stores the Spline class pointer in it and returns it. */
void Java_xeno_util_Spline_create__(void *environment, void *arguments,
                                    void **result)
{
    JavaClass *spline_class;
    void *class_pointer;
    Spline *spline;

    spline_class = classJava_xeno_util_Spline;
    spline = xmalloc(SPLINE_OBJECT_SIZE, SPLINE_HEAP_CATEGORY);
    class_pointer = spline_class->classPointer;
    spline->class_pointer = class_pointer;
    *result = spline;
}

/*
 * Initializes the spline from its arguments (the control points, order, weight mode and duration),
 * with three components per point.
 */
void Java_xeno_util_Spline_setCtrlVertex__aFIII(
    void *environment, union SplineArgumentValue *arguments)
{
    Spline *spline = arguments[0].spline;
    int weight_mode = arguments[3].integer;
    struct SplineFloatArray *control_points = arguments[1].array;
    int order = arguments[2].integer;
    int point_count;
    float *values;

    spline->weight_mode = weight_mode;
    point_count = control_points->byte_length / sizeof(float);
    values = control_points->values;
    spline->duration = arguments[4].integer;
    SPL_init2(spline, order, values, point_count, 3);
}

/* Evaluates the spline at frame and returns the point as a shared Vector4f with w set to 1. */
void Java_xeno_util_Spline_getValue__I(
    void *environment, struct SplineGetValueArgs *arguments,
    union SplineResultValue *result)
{
    static struct SplineValueVector4f value;
    float coordinates[3];
    JavaClass *vector_class;
    struct SplineValueVector4f *vector;

    SPL_getValueXYZ(coordinates, arguments->spline, (float)arguments->frame);
    vector = &value;
    vector_class = classJava_xeno_util_Vector4f;
    vector->x = coordinates[0];
    vector->y = coordinates[1];
    vector->z = coordinates[2];
    vector->w = 1.0f;
    vector->class_ref = vector_class->classPointer;
    result->object = vector;
}
