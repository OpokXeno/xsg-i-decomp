#include "common.h"

#include "shared.h"
#include "main/jni.h"

#include "spline.h"

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

struct SplineValueVector4f {
    SceneObjectClassRef *class_ref;
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

extern void SPL_getValueXYZ(float *destination, void *spline, float frame);

void Java_xeno_util_Spline_create__(void *environment, void *arguments,
                                    void **result)
{
    SceneClass *spline_class;
    void *class_pointer;
    Spline *spline;

    spline_class = classJava_xeno_util_Spline;
    spline = xmalloc(0x494, 0xE);
    class_pointer = spline_class->instance_class_ref;
    spline->class_pointer = class_pointer;
    *result = spline;
}

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

void Java_xeno_util_Spline_getValue__I(
    void *environment, struct SplineGetValueArgs *arguments,
    union SplineResultValue *result)
{
    static struct SplineValueVector4f value;
    float coordinates[3];
    SceneClass *vector_class;
    struct SplineValueVector4f *vector;

    SPL_getValueXYZ(coordinates, arguments->spline, (float)arguments->frame);
    vector = &value;
    vector_class = classJava_xeno_util_Vector4f;
    vector->x = coordinates[0];
    vector->y = coordinates[1];
    vector->z = coordinates[2];
    vector->w = 1.0f;
    vector->class_ref = vector_class->instance_class_ref;
    result->object = vector;
}
