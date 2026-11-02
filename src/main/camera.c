#include "common.h"
#include "shared.h"
#include "main/jni.h"
#include "camera.h"
#include "act_2.h"
#include "main/string_utf_get_hash.h"
#include "main/xgl_2.h"

typedef struct TCamera TCamera;

/* Native argument records use the four-byte VM slots described by camera.h.
 * Each field below has the Java descriptor's type and the original load offset. */
typedef struct CameraConstantArgs {
    CameraWork *camera;
    SceneObject peer;
    float x, y, z;
} CameraConstantArgs;

typedef struct CameraVector3Args {
    CameraWork *camera;
    float x, y, z;
} CameraVector3Args;

typedef struct CameraScalarArgs {
    CameraWork *camera;
    float value;
} CameraScalarArgs;

typedef struct CameraCFAngleArgs {
    unsigned char unmodeled_00[4];
    int camera_index;
    float degrees[3];
    float angle_w;
} CameraCFAngleArgs;

typedef struct CameraCFAnglePersArgs {
    unsigned char unmodeled_00[4];
    int camera_index;
    float degrees[3];
    float angle_w;
    float perspective;
} CameraCFAnglePersArgs;

typedef struct CameraCFHokanArgs {
    unsigned char unmodeled_00[4];
    int camera_index;
    float interpolation[2];
} CameraCFHokanArgs;

typedef struct CameraCFLockArgs {
    unsigned char unmodeled_00[4];
    int camera_index;
    int lock_mode;
    float lock_position[3];
} CameraCFLockArgs;

typedef struct CameraSetModeArgs {
    CameraWork *camera;
    unsigned char mode;
    unsigned char unmodeled_05[3];
} CameraSetModeArgs;

typedef struct CameraCFPedestalArgs {
    unsigned char unmodeled_00[4];
    unsigned int selector;
    float offset[3];
    float perspective;
    float angles[3];
    float scale;
} CameraCFPedestalArgs;

typedef struct CameraCFPedestalHokanArgs {
    unsigned char unmodeled_00[4];
    int camera_index;
    int interpolate;
} CameraCFPedestalHokanArgs;



/*
 * The game loop's own record, as far as this unit reads it: only the
 * halfword camera_change latches the matched studio camera index into,
 * at +0xc8 (sh at 0x002fb5e0). Nothing before it is read or written here.
 */
typedef struct {
    unsigned char unmodeled_00[0xc0];
    unsigned char cameraMode; /* +0xc0 */
    unsigned char hokanFlags; /* +0xc1 */
    unsigned char unmodeled_c2[6];
    short activeCameraIndex; /* +0xc8 */
} CameraLoopState;

/* Runtime Java fields occupy one EE VM value word. The descriptor
 * supplies its byte offset; its declared Java/native kind selects
 * the integer, floating or reference representation below. No fixed
 * Java-object field positions are assumed. */
typedef union CameraFieldValue {
    Actor * actor;
    float floating;
    u32 bits;
    void * native_pointer;
} CameraFieldValue;

extern CameraLoopState GameLoopState;

/* This TU's own .lit4 copy of the fov-scale seed 1.2, stored at CameraWork
 * +0x12b4 (docs above CameraWork in camera.h). */
/* The engine's camera table (main:0x00465e10, size 0x9600 = 8 * 0x12c0):
 * TCAMERA_get (still asm, src/main/tcamera.c) indexes it the same way. */
extern CameraWork tcamera[];

const char cameraPeerFieldName[];

/* TCAMERA setters are defined in tcamera.c. Their callers
 * pass the studio camera in a0 and scalar/xyz floats in f12/f13/f14. */
extern void TCAMERA_setRotate(TCamera *camera, float x, float y, float z);
extern void TCAMERA_setRoll(TCamera *camera, float roll);
extern void TCAMERA_setTranslate(TCamera *camera, float x, float y, float z);
extern void TCAMERA_setFov(TCamera *camera, float fov);
extern void TCAMERA_setView(TCamera *camera, float x, float y, float z);
extern JavaField *lookupClassField(void *class_object, void *name, int flags);
/*
 * getRotateX__/getRotateY__/getRotateZ__ each keep their own .lit4 copy of
 * the radian-to-degree divisor (bits 0x40490fdb, pi); the scaffold still
 * owns the pool and did not merge the three entries.
 */
void Java_xeno_Camera_getRotateX__(JavaEnvironment *environment,
                                   CameraGetterArgs *arguments, float *result)
{
    (void)environment;
    *result = xglStudioGetCamera2(arguments->camera->camera_id)->rotation.vector.x
              / 3.1415927f * 180.0f;
}

void Java_xeno_Camera_getRotateY__(JavaEnvironment *environment,
                                   CameraGetterArgs *arguments, float *result)
{
    (void)environment;
    *result = xglStudioGetCamera2(arguments->camera->camera_id)->rotation.vector.y
              / 3.1415927f * 180.0f;
}

void Java_xeno_Camera_getRotateZ__(JavaEnvironment *environment,
                                   CameraGetterArgs *arguments, float *result)
{
    (void)environment;
    *result = xglStudioGetCamera2(arguments->camera->camera_id)->rotation.vector.z
              / 3.1415927f * 180.0f;
}

void Java_xeno_Camera_getTranslateX__(JavaEnvironment *environment,
                                      CameraGetterArgs *arguments, float *result)
{
    (void)environment;
    *result = xglStudioGetCamera2(arguments->camera->camera_id)->position.vector.x;
}

void Java_xeno_Camera_getTranslateY__(JavaEnvironment *environment,
                                      CameraGetterArgs *arguments, float *result)
{
    (void)environment;
    *result = xglStudioGetCamera2(arguments->camera->camera_id)->position.vector.y;
}

void Java_xeno_Camera_getTranslateZ__(JavaEnvironment *environment,
                                      CameraGetterArgs *arguments, float *result)
{
    (void)environment;
    *result = xglStudioGetCamera2(arguments->camera->camera_id)->position.vector.z;
}

void Java_xeno_Camera_fovSPL__aFI(JavaEnvironment *environment,
                                  CameraSplineRequest *request, void *result)
{
    CameraWork *camera = request->camera;
    unsigned int weight_mode = request->weight_mode;
    const JavaFloatArray *sample_array = request->samples;

    (void)environment;
    (void)result;
    (void)xglStudioGetCamera2(camera->camera_id);
    camera->mode[CAMERA_CHANNEL_FOV] = CAMERA_MODE_SPLINE;
    camera->frame[CAMERA_CHANNEL_FOV] = 0;
    SPL_init(&camera->fov.spline, weight_mode,
             sample_array->elements, sample_array->length >> 1, 1);
    camera->fov.spline.first_key = -1;
}

static void camera_change(int camera_id)
{
    StudioCamera *studio_camera;
    int index;

    for (index = 0; index < 8; index++) {
        studio_camera = xglStudioGetCamera2(index);
        if (camera_id == index) {
            studio_camera->active = 1;
            GameLoopState.activeCameraIndex = (short)index;
        } else {
            studio_camera->active = 0;
        }
    }
}

void Java_xeno_Camera_change__(JavaEnvironment *environment, CameraGetterArgs *arguments)
{
    (void)environment;
    camera_change(arguments->camera->camera_id);
}

static void CAMERA_rotateSPL(int selection, JavaEnvironment *environment,
                             CameraSplineRequest *request, void *result)
{
    CameraWork *camera = request->camera;
    unsigned int weight_mode = request->weight_mode;
    const JavaFloatArray *sample_array;

    (void)environment;
    (void)result;
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
        SPL_init(&camera->view.spline, weight_mode,
                 sample_array->elements, sample_array->length >> 2, 3);
        camera->view.spline.first_key = -1;
        break;
    case 1:
        SPL_init(&camera->view.spline, weight_mode,
                 sample_array->elements, sample_array->length >> 2, 3);
        camera->view.spline.first_key = request->first_key;
        camera->view.spline.last_key = request->last_key;
        break;
    }
}

void Java_xeno_Camera_rotateSPL__aFI(JavaEnvironment *environment,
                                     CameraSplineRequest *arguments,
                                     void *result)
{
    /* The VM supplies a result slot as the helper's fourth ABI argument, even
     * though CAMERA_rotateSPL does not read it. */
    CAMERA_rotateSPL(0, environment, arguments, result);
}

void Java_xeno_Camera_rotateSPL__aFIII(JavaEnvironment *environment,
                                       CameraSplineRequest *arguments,
                                       void *result)
{
    /* Keep forwarding the VM's unused result slot in the original ABI slot. */
    CAMERA_rotateSPL(1, environment, arguments, result);
}

void Java_xeno_Camera_setActive__Z(JavaEnvironment *environment,
                                   CameraSetActiveArgs *arguments, void *result)
{
    (void)environment;
    (void)result;
    xglStudioGetCamera2(arguments->camera->camera_id)->active = arguments->active;
}

void Java_xeno_Camera_create__I(JavaEnvironment *environment,
                                CameraCreateArgs *arguments, CameraWork **result)
{
    int camera_id = arguments->camera_id;
    CameraWork *camera = &tcamera[camera_id];
    JavaClass *camera_class = classJava_xeno_Camera;

    (void)environment;
    /* Seed the native camera's field-of-view scale and Java class reference. */
    camera->fov_scale = 1.2f;
    camera->object_header.classPointer = camera_class->classPointer;
    camera->camera_id = camera_id;
    xglStudioGetCamera2(camera_id);
    *result = camera;
}

void Java_xeno_Camera_transCNS__Ljava_lang_Object_FFF(JavaEnvironment *environment,
                                                       VMSlot *arguments,
                                                       void *result)
{
    CameraWork *camera;
    SceneObject peer;
    int camera_id;
    float value[3];
    JavaField *peer_field;
    /* JavaField supplies the runtime offset of the word-aligned actor pointer. */
    Actor *actor;

    (void)environment;
    (void)result;
    camera = arguments[0].ref;
    camera_id = camera->camera_id;
    peer = arguments[1].ref;
    value[0] = arguments[2].f;
    value[1] = arguments[3].f;
    value[2] = arguments[4].f;
    xglStudioGetCamera2(camera_id);
    camera->mode[CAMERA_CHANNEL_TRANSLATE] = CAMERA_MODE_CONSTANT;
    if (JNI_isInstanceOf(peer, classJava_xeno_Chr) == 1) {
        peer_field = lookupClassField(classJava_xeno_Chr,
                                      loadConstString(cameraPeerFieldName, -1), 0);
        actor = *(Actor **)(peer + peer_field->offset);
        camera->translate.constant.reference_position = &actor->position;
    } else if (JNI_isInstanceOf(peer, classJava_xeno_Unit) == 1) {
        peer_field = lookupClassField(classJava_xeno_Unit,
                                      loadConstString(cameraPeerFieldName, -1), 0);
        actor = *(Actor **)(peer + peer_field->offset);
        camera->translate.constant.reference_position = &actor->position;
    }
    camera->translate.constant.reference_mode = 1;
    {
        float component;
        component = value[0];
        camera->translate.constant.offset.x = component;
    }
    {
        float component;
        component = value[1];
        camera->translate.constant.offset.y = component;
    }
    {
        float component;
        component = value[2];
        camera->translate.constant.offset.z = component;
    }
}

static void CAMERA_transSPL(int selection, JavaEnvironment *environment,
                            CameraSplineRequest *request, void *result)
{
    CameraWork *camera = request->camera;
    unsigned int weight_mode = request->weight_mode;
    const JavaFloatArray *sample_array;

    (void)environment;
    (void)result;
    sample_array = request->samples;
    (void)xglStudioGetCamera2(camera->camera_id);
    camera->mode[CAMERA_CHANNEL_TRANSLATE] = CAMERA_MODE_SPLINE;
    camera->frame[CAMERA_CHANNEL_TRANSLATE] = 0;
    switch (selection) {
    case 0:
        SPL_init(&camera->translate.spline, weight_mode,
                 sample_array->elements, sample_array->length >> 2, 3);
        camera->translate.spline.first_key =
            -1;
        break;
    case 1:
        SPL_init(&camera->translate.spline, weight_mode + 2,
                 sample_array->elements, sample_array->length >> 2, 3);
        camera->translate.spline.first_key =
            request->first_key;
        camera->translate.spline.last_key =
            request->last_key;
        break;
    }
}

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

void Java_xeno_Camera_viewCNS__Ljava_lang_Object_FFF(JavaEnvironment *environment,
                                                      VMSlot *arguments,
                                                      void *result)
{
    CameraWork *camera;
    SceneObject peer;
    int camera_id;
    float value[3];
    JavaField *peer_field;
    /* JavaField supplies the runtime offset of the word-aligned actor pointer. */
    Actor *actor;

    (void)environment;
    (void)result;
    camera = arguments[0].ref;
    camera_id = camera->camera_id;
    peer = arguments[1].ref;
    value[0] = arguments[2].f;
    value[1] = arguments[3].f;
    value[2] = arguments[4].f;
    xglStudioGetCamera2(camera_id);
    camera->mode[CAMERA_CHANNEL_VIEW] = CAMERA_MODE_CONSTANT;
    if (JNI_isInstanceOf(peer, classJava_xeno_Chr) == 1) {
        peer_field = lookupClassField(classJava_xeno_Chr,
                                      loadConstString(cameraPeerFieldName, -1), 0);
        actor = *(Actor **)(peer + peer_field->offset);
        camera->view.constant.reference_position = &actor->position;
    } else if (JNI_isInstanceOf(peer, classJava_xeno_Unit) == 1) {
        peer_field = lookupClassField(classJava_xeno_Unit,
                                      loadConstString(cameraPeerFieldName, -1), 0);
        actor = *(Actor **)(peer + peer_field->offset);
        camera->view.constant.reference_position = &actor->position;
    }
    camera->view.constant.reference_mode = 1;
    {
        float component;
        component = value[0];
        camera->view.constant.offset.x = component;
    }
    {
        float component;
        component = value[1];
        camera->view.constant.offset.y = component;
    }
    {
        float component;
        component = value[2];
        camera->view.constant.offset.z = component;
    }
}

static void CAMERA_viewSPL(int selection, JavaEnvironment *environment,
                           CameraSplineRequest *request, void *result)
{
    CameraWork *camera = request->camera;
    unsigned int weight_mode = request->weight_mode;
    const JavaFloatArray *sample_array;

    (void)environment;
    (void)result;
    sample_array = request->samples;
    (void)xglStudioGetCamera2(camera->camera_id);
    camera->mode[CAMERA_CHANNEL_VIEW] = CAMERA_MODE_SPLINE;
    camera->frame[CAMERA_CHANNEL_VIEW] = 0;
    switch (selection) {
    case 0:
        SPL_init(&camera->view.spline, weight_mode,
                 sample_array->elements, sample_array->length >> 2, 3);
        camera->view.spline.first_key =
            -1;
        break;
    case 1:
        SPL_init(&camera->view.spline, weight_mode,
                 sample_array->elements, sample_array->length >> 2, 3);
        camera->view.spline.first_key =
            request->first_key;
        camera->view.spline.last_key =
            request->last_key;
        break;
    }
}

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

void Java_xeno_Camera_rollSPL__aFI(JavaEnvironment *environment,
                                   CameraSplineRequest *request, void *result)
{
    CameraWork *camera = request->camera;
    unsigned int weight_mode = request->weight_mode;
    const JavaFloatArray *sample_array = request->samples;

    (void)environment;
    (void)result;
    (void)xglStudioGetCamera2(camera->camera_id);
    camera->mode[CAMERA_CHANNEL_ROLL] = CAMERA_MODE_SPLINE;
    camera->frame[CAMERA_CHANNEL_ROLL] = 0;
    SPL_init(&camera->roll.spline, weight_mode,
             sample_array->elements, sample_array->length >> 1, 1);
    camera->roll.spline.first_key = -1;
}

void Java_xeno_Camera_setRotate__FFF(JavaEnvironment *environment, VMSlot *arguments)
{
    CameraWork *camera = arguments[0].ref;
    StudioCamera *studio_camera;

    (void)environment;
    studio_camera = xglStudioGetCamera2(camera->camera_id);
    camera->mode[CAMERA_CHANNEL_VIEW] = CAMERA_MODE_IDLE;
    {
        float x;
        float y;
        float z;
        x = arguments[1].f;
        y = arguments[2].f;
        z = arguments[3].f;
        TCAMERA_setRotate((void *)studio_camera, x, y, z);
    }
}

void Java_xeno_Camera_setRoll__F(JavaEnvironment *environment, VMSlot *arguments)
{
    CameraWork *camera = arguments[0].ref;
    StudioCamera *studio_camera;

    (void)environment;
    studio_camera = xglStudioGetCamera2(camera->camera_id);
    camera->mode[CAMERA_CHANNEL_ROLL] = CAMERA_MODE_IDLE;
    {
        float value;
        value = arguments[1].f;
        TCAMERA_setRoll((void *)studio_camera, value);
    }
}

void Java_xeno_Camera_setTranslate__FFF(JavaEnvironment *environment, VMSlot *arguments)
{
    CameraWork *camera = arguments[0].ref;
    StudioCamera *studio_camera;

    (void)environment;
    studio_camera = xglStudioGetCamera2(camera->camera_id);
    camera->mode[CAMERA_CHANNEL_TRANSLATE] = CAMERA_MODE_IDLE;
    {
        float x;
        float y;
        float z;
        x = arguments[1].f;
        y = arguments[2].f;
        z = arguments[3].f;
        TCAMERA_setTranslate((void *)studio_camera, x, y, z);
    }
}

void Java_xeno_Camera_setFov__F(JavaEnvironment *environment, VMSlot *arguments)
{
    CameraWork *camera = arguments[0].ref;
    StudioCamera *studio_camera;

    (void)environment;
    studio_camera = xglStudioGetCamera2(camera->camera_id);
    camera->mode[CAMERA_CHANNEL_FOV] = CAMERA_MODE_IDLE;
    {
        float value;
        value = arguments[1].f;
        TCAMERA_setFov((void *)studio_camera, value);
    }
}

void Java_xeno_Camera_getFov__(JavaEnvironment *environment,
                               CameraGetterArgs *arguments, float *result)
{
    (void)environment;
    *result = xglStudioGetCamera2(arguments->camera->camera_id)->fov
              / 3.1415927f * 180.0f;
}

void Java_xeno_Camera_setView__FFF(JavaEnvironment *environment, VMSlot *arguments)
{
    CameraWork *camera = arguments[0].ref;
    StudioCamera *studio_camera;

    (void)environment;
    studio_camera = xglStudioGetCamera2(camera->camera_id);
    camera->mode[CAMERA_CHANNEL_VIEW] = CAMERA_MODE_IDLE;
    {
        float x;
        float y;
        float z;
        x = arguments[1].f;
        y = arguments[2].f;
        z = arguments[3].f;
        TCAMERA_setView((void *)studio_camera, x, y, z);
    }
}

void Java_xeno_Camera_start__ILjava_lang_Object_(JavaEnvironment *environment,
                                                 CameraStartArgs *arguments,
                                                 void *result)
{
    CameraFieldValue *field_value;
    CameraWork *camera = arguments->camera;
    int state = arguments->state;
    SceneObject peer = arguments->peer;
    JavaField *peer_field;

    (void)environment;
    (void)result;
    xglStudioGetCamera2(camera->camera_id)->state = state;
    if (state == 3) {
        if (peer != 0) {
            if (JNI_isInstanceOf(peer, classJava_xeno_Chr) != 0) {
                peer_field = lookupClassField(classJava_xeno_Chr,
                                              loadConstString(cameraPeerFieldName, -1), 0);
                /* `peer` is a field of the Java class xeno.Chr, so its
                 * position inside the object is the offset the JVM hands
                 * back at run time, exactly as CHR_sclX (src/main/chr.c)
                 * reads its own peer field. CameraWork names the
                 * native camera's destination peer slot. */
                field_value = (void *)(peer + peer_field->offset);
                camera->peer =
                    field_value->actor;
            }
        } else {
            camera->peer = 0;
        }
    }
}

void Java_xeno_Camera_setCFAngle__IFFFF(JavaEnvironment *environment,
                                        CameraCFAngleArgs *arguments)
{
    CfCameraDefinition *definition;
    float angles[4];
    float full_turn;
    int index;
    int last;

    (void)environment;
    if (arguments->camera_index < 0) {
        index = 0;
        last = 33;
    } else {
        index = arguments->camera_index;
        last = index + 1;
    }
    definition = &CfCameraDefine[index];
    full_turn = 6.2831855f;
    angles[0] = arguments->degrees[0] / 180.0f * 3.1415927f;
    angles[1] = arguments->degrees[1] / 180.0f * 3.1415927f;
    angles[2] = arguments->degrees[2] / 180.0f * 3.1415927f;
    angles[3] = arguments->angle_w;
    while (angles[0] < 0.0f) {
        angles[0] += full_turn;
    }
    while (angles[0] > full_turn) {
        angles[0] -= full_turn;
    }
    while (angles[1] < 0.0f) {
        angles[1] += full_turn;
    }
    while (angles[1] > full_turn) {
        angles[1] -= full_turn;
    }
    while (angles[2] < 0.0f) {
        angles[2] += full_turn;
    }
    while (angles[2] > full_turn) {
        angles[2] -= full_turn;
    }
    for (; index < last; index++) {
        float *source = angles;

        definition->pedestal_mode = 0;
        __asm__ __volatile__("lq $2, 0(%1)\n"
                             "sq $2, 0(%0)\n"
                             :
                             : "r" (definition->angles.values), "r" (source)
                             : "$2", "memory");
        definition->current_yaw = definition->angles.values[1];
        definition++;
    }
    GameLoopState.hokanFlags |= 0x40;
}

void Java_xeno_Camera_setCFAnglePers__IFFFFF(JavaEnvironment *environment,
                                             CameraCFAnglePersArgs *arguments)
{
    CfCameraDefinition *definition;
    float angles[4];
    float perspective;
    float full_turn;
    int index;
    int last;

    (void)environment;
    if (arguments->camera_index < 0) {
        index = 0;
        last = 33;
    } else {
        index = arguments->camera_index;
        last = index + 1;
    }
    definition = &CfCameraDefine[index];
    full_turn = 6.2831855f;
    angles[0] = arguments->degrees[0] / 180.0f * 3.1415927f;
    angles[1] = arguments->degrees[1] / 180.0f * 3.1415927f;
    angles[2] = arguments->degrees[2] / 180.0f * 3.1415927f;
    angles[3] = arguments->angle_w;
    perspective = arguments->perspective / 180.0f * 3.1415927f;
    while (angles[0] < 0.0f) {
        angles[0] += full_turn;
    }
    while (angles[0] > full_turn) {
        angles[0] -= full_turn;
    }
    while (angles[1] < 0.0f) {
        angles[1] += full_turn;
    }
    while (angles[1] > full_turn) {
        angles[1] -= full_turn;
    }
    while (angles[2] < 0.0f) {
        angles[2] += full_turn;
    }
    while (angles[2] > full_turn) {
        angles[2] -= full_turn;
    }
    for (; index < last; index++) {
        float *source = angles;

        definition->pedestal_mode = 0;
        __asm__ __volatile__("lq $2, 0(%1)\n"
                             "sq $2, 0(%0)\n"
                             :
                             : "r" (definition->angles.values), "r" (source)
                             : "$2", "memory");
        definition->perspective = perspective;
        definition->current_yaw = definition->angles.values[1];
        definition++;
    }
    GameLoopState.hokanFlags |= 0x40;
}

void Java_xeno_Camera_setCFHokan__IFF(JavaEnvironment *environment,
                                      CameraCFHokanArgs *arguments)
{
    CfCameraDefinition *definition;
    float yaw_frames;
    float pitch_frames;
    int index;
    int last;

    (void)environment;
    if (arguments->camera_index < 0) {
        index = 0;
        last = 33;
    } else {
        index = arguments->camera_index;
        last = index + 1;
    }
    yaw_frames = arguments->interpolation[0];
    pitch_frames = arguments->interpolation[1];
    definition = &CfCameraDefine[index];
    for (; index < last; index++) {
        definition->interpolation[0] = yaw_frames;
        definition->interpolation[1] = pitch_frames;
        definition++;
    }
    GameLoopState.hokanFlags |= 0x40;
}

void Java_xeno_Camera_setCFLock__IIFFF(JavaEnvironment *environment,
                                       CameraCFLockArgs *arguments)
{
    CfCameraDefinition *definition;
    int lock_mode;
    float x;
    float y;
    float z;
    int index;
    int last;

    (void)environment;
    if (arguments->camera_index < 0) {
        index = 0;
        last = 33;
    } else {
        index = arguments->camera_index;
        last = index + 1;
    }
    lock_mode = arguments->lock_mode;
    x = arguments->lock_position[0];
    y = arguments->lock_position[1];
    z = arguments->lock_position[2];
    definition = &CfCameraDefine[index];
    for (; index < last; index++) {
        definition->lock_mode = lock_mode;
        definition->lock_position[0] = x;
        definition->lock_position[1] = y;
        definition->lock_position[2] = z;
        definition++;
    }
    GameLoopState.hokanFlags |= 0x40;
}

void Java_xeno_Camera_setCFOffset__IFFFIFIF(JThread *thread,
                                            VMSlot *arguments, VMSlot *result)
{
    CfCameraDefinition *definition;
    CameraVector3 offset;
    short first_mode;
    float first_value;
    short second_mode;
    float second_value;
    int index;
    int last;

    (void)thread;
    (void)result;
    if (arguments[1].i < 0) {
        index = 0;
        last = 33;
    } else {
        index = arguments[1].i;
        last = index + 1;
    }
    offset.x = arguments[2].f;
    offset.y = arguments[3].f;
    offset.z = arguments[4].f;
    first_mode = arguments[5].i;
    first_value = arguments[6].f;
    second_mode = arguments[7].i;
    second_value = arguments[8].f;
    definition = &CfCameraDefine[index];
    for (; index < last; index++) {
        definition->pedestal_mode = 0;
        definition->offset[0] = offset.x;
        definition->offset[1] = offset.y;
        definition->offset[2] = offset.z;
        definition->parameters.offset.first_mode = first_mode;
        definition->parameters.offset.first_value = first_value;
        definition->parameters.offset.second_mode = second_mode;
        definition->parameters.offset.second_value = second_value;
        definition++;
    }
    GameLoopState.hokanFlags |= 0x40;
}

void Java_xeno_Camera_setMode__I(JavaEnvironment *environment,
                                 CameraSetModeArgs *arguments)
{
    (void)environment;
    GameLoopState.cameraMode = arguments->mode;
    GameLoopState.hokanFlags |= 0x40;
    camera_change(0);
}

void Java_xeno_Camera_getMode__(JavaEnvironment *environment, void *arguments, int *result)
{
    (void)environment;
    (void)arguments;
    *result = GameLoopState.cameraMode;
}

void Java_xeno_Camera_setCFPedestal__IFFFFFFFF(JavaEnvironment *environment,
                                               CameraCFPedestalArgs *arguments)
{
    CfCameraDefinition *definition;
    unsigned int selector = arguments->selector;
    int mode;
    float scale;
    int index;
    int last;

    (void)environment;
    if ((selector & 0xff) == 0xff) {
        index = 0;
        last = 33;
        mode = (selector + 1) >> 28;
    } else {
        mode = selector >> 28;
        index = selector;
        last = index + 1;
    }
    definition = &CfCameraDefine[index];
    for (; index < last; index++) {
        definition->pedestal_mode = 1;
        if (mode < 3) {
            if (mode >= 0) {
                definition->offset[0] = arguments->offset[0];
                definition->offset[1] = arguments->offset[1];
                definition->offset[2] = arguments->offset[2];
            }
        }
        if (mode == 0 || mode == 4) {
            definition->perspective = arguments->perspective / 180.0f * 3.1415927f;
        }
        scale = arguments->scale;
        definition->parameters.pedestal[3] = scale;
        if (scale == 2.0f) {
            definition->parameters.pedestal[0] = arguments->angles[0] / 180.0f * 3.1415927f;
            definition->parameters.pedestal[1] = arguments->angles[1] / 180.0f * 3.1415927f;
            definition->parameters.pedestal[2] = arguments->angles[2] / 180.0f * 3.1415927f;
        } else {
            definition->parameters.pedestal[0] = arguments->angles[0];
            definition->parameters.pedestal[1] = arguments->angles[1];
            definition->parameters.pedestal[2] = arguments->angles[2];
        }
        definition++;
    }
    GameLoopState.hokanFlags |= 0x40;
}

void Java_xeno_Camera_setCFPedestalHokan__II(JavaEnvironment *environment,
                                             CameraCFPedestalHokanArgs *arguments)
{
    CfCameraDefinition *definition;
    int index;
    int last;
    int interpolate;

    (void)environment;
    if (arguments->camera_index < 0) {
        index = 0;
        last = 33;
    } else {
        index = arguments->camera_index;
        last = index + 1;
    }
    interpolate = arguments->interpolate;
    definition = &CfCameraDefine[index];
    for (; index < last; index++) {
        if (interpolate != 0) {
            definition->pedestal_interpolation |= 1;
        } else {
            definition->pedestal_interpolation &= 0xfe;
        }
        definition++;
    }
}

void Java_xeno_Camera_changeID__III(JavaEnvironment *environment, CameraChangeIDArgs *arguments)
{
    (void)environment;
    GameCameraChangeID(arguments->id, arguments->id2, arguments->id3);
}

void Java_xeno_Camera_setFog__IFFFFIIII(JThread *thread,
                                        VMSlot *arguments, VMSlot *result)
{
    CfCameraDefinition *definition;
    float fog_parameters[4];
    float fog_color[4];
    int index;
    int last;

    (void)thread;
    (void)result;
    if (arguments[1].i < 0) {
        index = 0;
        last = 33;
    } else {
        index = arguments[1].i;
        last = index + 1;
    }
    fog_parameters[0] = arguments[2].f;
    fog_parameters[1] = arguments[3].f;
    fog_parameters[2] = arguments[4].f;
    fog_parameters[3] = arguments[5].f;
    fog_color[0] = I2F(arguments[6].i);
    fog_color[1] = I2F(arguments[7].i);
    fog_color[2] = I2F(arguments[8].i);
    fog_color[3] = I2F(arguments[9].i);
    definition = &CfCameraDefine[index];
    for (; index < last; index++) {
        float *source_parameters = fog_parameters;
        float *source_color = fog_color;

        __asm__ __volatile__("lq $2, 0(%1)\n"
                             "sq $2, 0(%0)\n"
                             :
                             : "r" (definition->fog_parameters), "r" (source_parameters)
                             : "$2", "memory");
        __asm__ __volatile__("lq $2, 0(%1)\n"
                             "sq $2, 0(%0)\n"
                             :
                             : "r" (&definition->fog_color), "r" (source_color)
                             : "$2", "memory");
        definition++;
    }
}

void Java_xeno_Camera_resetFog__I(JThread *thread, VMSlot *arguments, VMSlot *result)
{
    CfCameraDefinition *definition;
    int index;
    int last;

    (void)thread;
    (void)result;
    if (arguments[1].i < 0) {
        index = 0;
        last = 33;
    } else {
        index = arguments[1].i;
        last = index + 1;
    }
    for (definition = &CfCameraDefine[index]; index < last; index++, definition++) {
        __asm__ __volatile__("sq $0, 0(%0)\n" : : "r" (definition->fog_parameters) : "memory");
        __asm__ __volatile__("sq $0, 0(%0)\n" : : "r" (&definition->fog_color) : "memory");
    }
}

void Java_xeno_Camera_setClipRange__FF(JavaEnvironment *environment,
                                       CameraSetClipRangeArgs *arguments,
                                       void *result)
{
    StudioCamera *studio_camera = xglStudioGetCamera2(arguments->camera->camera_id);

    (void)environment;
    (void)result;
    studio_camera->nearClip = arguments->nearClip;
    studio_camera->farClip = arguments->farClip;
}

/* Keep the field name data after Java_xeno_Camera_start's external use. */
/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char cameraPeerFieldName[8] = "peer";
