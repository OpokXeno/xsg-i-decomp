#include "common.h"
#include "shared.h"
#include "main/jni.h"
#include "main/nml_model_set.h"
#include "main/string_utf_get_hash.h"
#include "main/xgl_light.h"

/* Original .lit4 pool at 0x004D83EC: IEEE-754 binary32 0x40490fdb. */
#define PI_F (3.141592741f)

#define HALF_TURN_RADIANS PI_F

typedef struct JThread JThread;

typedef struct LightVectorCall {
    SceneObject object;
    float x;
    float y;
    float z;
} LightVectorCall;

extern const char lightIdFieldName[];
extern const char lightPeerFieldName[];

extern JavaField *lookupClassField(void *class_object, void *name, int flags);
extern void *xglStudioGetLight2(void);
extern void xglLightIntensityAmbient(void *light, void *intensity);
extern void xglLightIntensityParallel(void *light, int index, void *intensity);
extern void xglLightDirection(void *light, unsigned int index,
                              const Vector4 *direction);

void Java_xeno_Light_setColor__FFF(JThread *thread, LightVectorCall *arguments)
{
    SceneObject object = arguments->object;
    JavaField *id_field;
    JavaField *peer_field;
    int light_id;
    void *light;
    Vector4 intensity;

    id_field = lookupClassField(classJava_xeno_Light,
                                loadConstString(lightIdFieldName, -1), 0);
    light_id = *(int *)(object + id_field->offset);
    peer_field = lookupClassField(classJava_xeno_Light,
                                  loadConstString(lightPeerFieldName, -1), 0);
    light = *(void **)(object + peer_field->offset);
    if (light == 0) {
        light = xglStudioGetLight2();
    }

    intensity.x = arguments->x;
    intensity.y = arguments->y;
    intensity.z = arguments->z;
    intensity.w = 1.0f;
    if (light_id == 0) {
        xglLightIntensityAmbient(light, &intensity);
    } else {
        xglLightIntensityParallel(light, light_id - 1, &intensity);
    }
}

void Java_xeno_Light_setDirection__FFF(JThread *thread,
                                       LightVectorCall *arguments)
{
    SceneObject object = arguments->object;
    JavaField *id_field;
    JavaField *peer_field;
    int light_id;
    void *light;
    Vector4 angles;

    id_field = lookupClassField(classJava_xeno_Light,
                                loadConstString(lightIdFieldName, -1), 0);
    light_id = *(int *)(object + id_field->offset);
    peer_field = lookupClassField(classJava_xeno_Light,
                                  loadConstString(lightPeerFieldName, -1), 0);
    light = *(void **)(object + peer_field->offset);
    if (light == 0) {
        light = xglStudioGetLight2();
    }

    angles.x = (arguments->x / 180.0f) * HALF_TURN_RADIANS;
    angles.y = (arguments->y / 180.0f) * HALF_TURN_RADIANS;
    angles.z = (arguments->z / 180.0f) * HALF_TURN_RADIANS;
    angles.w = 1.0f;
    xglLightAngle(light, light_id - 1, &angles);
}

void Java_xeno_Light_setDirection2__FFF(JThread *thread,
                                         LightVectorCall *arguments)
{
    SceneObject object = arguments->object;
    JavaField *id_field;
    JavaField *peer_field;
    int light_id;
    unsigned int parallel_index;
    void *light;
    Vector4 direction;

    id_field = lookupClassField(classJava_xeno_Light,
                                loadConstString(lightIdFieldName, -1), 0);
    light_id = *(int *)(object + id_field->offset);
    peer_field = lookupClassField(classJava_xeno_Light,
                                  loadConstString(lightPeerFieldName, -1), 0);
    light = *(void **)(object + peer_field->offset);
    if (light == 0) {
        light = xglStudioGetLight2();
    }

    parallel_index = light_id - 1;
    direction.x = arguments->x;
    direction.y = arguments->y;
    direction.z = arguments->z;
    direction.w = 1.0f;
    if (parallel_index < 3U) {
        xglLightDirection(light, parallel_index, &direction);
    }
}

void Java_xeno_Light_setGlobalPointLightCol__IFFF(
    void *thread, void *argument_block)
{
    union LightJavaArgument {
        void *reference;
        int integer;
        float real;
    } *arguments = argument_block;
    Vector4 color;

    color.x = arguments[2].real;
    color.y = arguments[3].real;
    color.z = arguments[4].real;
    nmlModelSetGlobalPointLightCol(
        arguments[1].integer, &color);
}

void Java_xeno_Light_setGlobalPointLightPos__IFFF(
    void *thread, void *argument_block)
{
    union LightJavaArgument {
        void *reference;
        int integer;
        float real;
    } *arguments = argument_block;
    Vector4 position;

    position.x = arguments[2].real;
    position.y = arguments[3].real;
    position.z = arguments[4].real;
    nmlModelSetGlobalPointLightPos(
        arguments[1].integer, &position);
}

void Java_xeno_Light_setGlobalPointLightReset__(void)
{
    nmlModelSetGlobalPointLightReset();
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char lightIdFieldName[8] = "id";

const char lightPeerFieldName[8] = "peer";
