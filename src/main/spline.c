#include "common.h"
#include "shared.h"
#include "spline.h"

void Java_xeno_util_Spline_create__(void *environment, void *arguments,
                                    void **result)
{
    SplineClassEntry *spline_class;
    void *class_pointer;
    Spline *spline;

    spline_class = classJava_xeno_util_Spline;
    spline = xmalloc(0x494, 0xE);
    class_pointer = spline_class->class_pointer;
    spline->class_pointer = class_pointer;
    *result = spline;
}

INCLUDE_ASM("asm/main/nonmatchings/spline", Java_xeno_util_Spline_setCtrlVertex__aFIII);

INCLUDE_ASM("asm/main/nonmatchings/spline", Java_xeno_util_Spline_getValue__I);
