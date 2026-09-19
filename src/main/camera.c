#include "common.h"
#include "shared.h"
#include "camera.h"

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_getRotateX__);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_getRotateY__);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_getRotateZ__);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_getTranslateX__);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_getTranslateY__);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_getTranslateZ__);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_fovSPL__aFI);

INCLUDE_ASM("asm/main/nonmatchings/camera", camera_change);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_change__);

static void CAMERA_rotateSPL(int selection, JavaEnvironment *environment,
                             CameraSplineRequest *request)
{
    CameraWork *camera = request->camera;
    unsigned int weight_mode = request->weight_mode;
    unsigned char *camera_bytes = (unsigned char *)camera;
    const JavaFloatArray *sample_array;

    (void)environment;
    sample_array = request->samples;
    (void)xglStudioGetCamera2(camera->camera_id);
    camera->mode[CAMERA_CHANNEL_VIEW] = CAMERA_MODE_ROTATE_SPLINE;
    camera->frame[CAMERA_CHANNEL_VIEW] = 0;

    /* The view channel's spline payload is at camera + 0x4d0: the four channel
     * payloads are 0x4a0 apart (TCAMERA_update passes +0x30, +0x4d0, +0x970 and
     * +0xe10 to the channel handlers) and nothing evidences the internal extent
     * of a payload, so the offset cannot become a CameraWork member without
     * inventing the span in between.
     * A rotation key is four floats -- one weight plus x, y and z, SplinePoint
     * in src/main/spl.h -- so the array's float count >> 2 is its key count. */
    switch (selection) {
    case 0:
        SPL_init((CameraSpline *)(camera_bytes + 0x4d0), weight_mode,
                 sample_array->elements, sample_array->length >> 2, 3);
        ((CameraSpline *)(camera_bytes + 0x4d0))->first_key =
            (unsigned short)-1;
        break;
    case 1:
        SPL_init((CameraSpline *)(camera_bytes + 0x4d0), weight_mode,
                 sample_array->elements, sample_array->length >> 2, 3);
        ((CameraSpline *)(camera_bytes + 0x4d0))->first_key =
            request->first_key;
        ((CameraSpline *)(camera_bytes + 0x4d0))->last_key =
            request->last_key;
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_rotateSPL__aFI);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_rotateSPL__aFIII);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setActive__Z);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_create__I);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_transCNS__Ljava_lang_Object_FFF);

INCLUDE_ASM("asm/main/nonmatchings/camera", CAMERA_transSPL);

void Java_xeno_Camera_transSPL__aFI(JavaEnvironment *environment,
                                    CameraSplineRequest *request,
                                    void *result)
{
    CAMERA_transSPL(0, environment, request, result);
}

void Java_xeno_Camera_transSPL__aFIII(JavaEnvironment *environment,
                                      CameraSplineRequest *request,
                                      void *result)
{
    CAMERA_transSPL(1, environment, request, result);
}

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_viewCNS__Ljava_lang_Object_FFF);

INCLUDE_ASM("asm/main/nonmatchings/camera", CAMERA_viewSPL);

void Java_xeno_Camera_viewSPL__aFI(JavaEnvironment *environment,
                                   CameraSplineRequest *request,
                                   void *result)
{
    CAMERA_viewSPL(0, environment, request, result);
}

void Java_xeno_Camera_viewSPL__aFIII(JavaEnvironment *environment,
                                     CameraSplineRequest *request,
                                     void *result)
{
    CAMERA_viewSPL(1, environment, request, result);
}

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_rollSPL__aFI);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setRotate__FFF);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setRoll__F);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setTranslate__FFF);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setFov__F);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_getFov__);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setView__FFF);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_start__ILjava_lang_Object_);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setCFAngle__IFFFF);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setCFAnglePers__IFFFFF);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setCFHokan__IFF);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setCFLock__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setCFOffset__IFFFIFIF);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setMode__I);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_getMode__);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setCFPedestal__IFFFFFFFF);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setCFPedestalHokan__II);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_changeID__III);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setFog__IFFFFIIII);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_resetFog__I);

INCLUDE_ASM("asm/main/nonmatchings/camera", Java_xeno_Camera_setClipRange__FF);
