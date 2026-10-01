#include "common.h"
#include "shared.h"

typedef struct JThread JThread;

typedef struct LightVectorCall {
    SceneObject object;
    float x;
    float y;
    float z;
} LightVectorCall;

extern void *classJava_xeno_Light;
extern const char D_004DC180[];
extern const char D_004DC188[];
extern float D_004D83EC;

extern SceneString *loadConstString(const char *bytes, int length);
extern JavaField *lookupClassField(void *class_object, void *name, int flags);

extern void *xglStudioGetLight2(void);
extern void xglLightIntensityAmbient(void *light, void *intensity);
extern void xglLightIntensityParallel(void *light, int index, void *intensity);
extern void xglLightAngle(void *light, int index, const Vector4 *angles);
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
                                loadConstString(D_004DC180, -1), 0);
    light_id = *(int *)(object + id_field->offset);
    peer_field = lookupClassField(classJava_xeno_Light,
                                  loadConstString(D_004DC188, -1), 0);
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
                                loadConstString(D_004DC180, -1), 0);
    light_id = *(int *)(object + id_field->offset);
    peer_field = lookupClassField(classJava_xeno_Light,
                                  loadConstString(D_004DC188, -1), 0);
    light = *(void **)(object + peer_field->offset);
    if (light == 0) {
        light = xglStudioGetLight2();
    }

    angles.x = (arguments->x / 180.0f) * D_004D83EC;
    angles.y = (arguments->y / 180.0f) * D_004D83EC;
    angles.z = (arguments->z / 180.0f) * D_004D83EC;
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
                                loadConstString(D_004DC180, -1), 0);
    light_id = *(int *)(object + id_field->offset);
    peer_field = lookupClassField(classJava_xeno_Light,
                                  loadConstString(D_004DC188, -1), 0);
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
    void nmlModelSetGlobalPointLightCol(int light_id,
                                        const float (*color)[3]);
    float color[3];

    color[0] = arguments[2].real;
    color[1] = arguments[3].real;
    color[2] = arguments[4].real;
    nmlModelSetGlobalPointLightCol(
        arguments[1].integer, (const float (*)[3])&color);
}

void Java_xeno_Light_setGlobalPointLightPos__IFFF(
    void *thread, void *argument_block)
{
    union LightJavaArgument {
        void *reference;
        int integer;
        float real;
    } *arguments = argument_block;
    void nmlModelSetGlobalPointLightPos(int light_id,
                                        const float (*position)[3]);
    float position[3];

    position[0] = arguments[2].real;
    position[1] = arguments[3].real;
    position[2] = arguments[4].real;
    nmlModelSetGlobalPointLightPos(
        arguments[1].integer, (const float (*)[3])&position);
}

/*
 * nmlModelSetGlobalPointLightReset is src/main/nml_model_set.c's own
 * INCLUDE_ASM function (main/tu106, still unresolved there); this TU only
 * tail-calls it, so it stays a local extern declaration until that TU
 * converts it.
 */
void nmlModelSetGlobalPointLightReset(void);

void Java_xeno_Light_setGlobalPointLightReset__(void)
{
    nmlModelSetGlobalPointLightReset();
}
