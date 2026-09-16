#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"

/*
 * The wrapper object stores the color mode at +0x0c, the light selector at
 * +0x28, and three values at +0x2c. The values are either integer color
 * components or floating-point components, depending on color_mode; the same
 * three floats also carry a light's direction components when the object is
 * used with JS_classLight_setDirection2.
 */
typedef union {
    int integer;
    float real;
} JsLightColor;

typedef struct {
    unsigned char unmodeled_00[12];
    int color_mode;
    unsigned char unmodeled_10[24];
    int light_type;
    JsLightColor color[3];
} JsClassLight;

extern char D_004DA5A8[];
extern void initLight2(void);

void *JS_classLight_getPeer(int identifier)
{
    if (identifier == -1)
        return D_004DA5A8;
    return 0;
}

int JS_classLight_setColor(void *peer)
{
    StudioLight *studio;
    Vector4 *destination;
    int light_type;
    JsClassLight *light;

    light = peer;
    xglStudioGetLight(&studio);
    light_type = light->light_type;
    if (light_type == 0)
        destination = &studio->ambient_color;
    else if ((unsigned int)(light_type - 1) < 3)
        destination = &studio->lights[light_type - 1].color;
    else
        return 1;

    if (light->color_mode == 3) {
        destination->x = (float)light->color[0].integer / 255.0f;
        destination->y = (float)light->color[1].integer / 255.0f;
        destination->z = (float)light->color[2].integer / 255.0f;
    } else {
        destination->x = light->color[0].real;
        destination->y = light->color[1].real;
        destination->z = light->color[2].real;
    }
    initLight2();
    return 0;
}

int JS_classLight_setDirection2(void *peer)
{
    StudioLight *studio;
    Vector4 *destination;
    int light_type;
    JsClassLight *light;

    light = peer;
    xglStudioGetLight(&studio);
    light_type = light->light_type;
    if ((unsigned int)(light_type - 1) < 3)
        destination = &studio->lights[light_type - 1].direction;
    else
        return 1;

    destination->x = light->color[0].real;
    destination->y = light->color[1].real;
    destination->z = light->color[2].real;
    initLight2();
    return 0;
}
