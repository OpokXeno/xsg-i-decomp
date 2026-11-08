#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"

/* JsClassLight.light_type: 0 selects the ambient colour, 1..3 the directional lights. */
#define JS_LIGHT_AMBIENT 0
#define JS_LIGHT_COUNT 3

/* JsClassLight.color_mode: the script passed integer components (0..255). */
#define JS_LIGHT_COLOR_INTEGER 3

typedef union {
    int integer;
    float real;
} JsLightColor;

/*
 * The wrapper object stores the color mode at +0x0c, the light selector at
 * +0x28, and three values at +0x2c. The values are either integer color
 * components or floating-point components, depending on color_mode; the same
 * three floats also carry a light's direction components when the object is
 * used with JS_classLight_setDirection2.
 */
typedef struct {
    unsigned char unmodeled_00[12];
    int color_mode;
    unsigned char unmodeled_10[24];
    int light_type;
    JsLightColor color[3];
} JsClassLight;

extern char jsClassLightGetPeerLightClassName[];

extern void initLight2(void);

char jsClassLightGetPeerLightClassName[8] = "light";

/* Returns the light class name for identifier -1, or 0 for any other identifier. */
void *JS_classLight_getPeer(int identifier)
{
    if (identifier == -1)
        return jsClassLightGetPeerLightClassName;
    return 0;
}

/*
 * Sets the color of the light the peer selects (0 for ambient, 1..3 for directional), converting
 * integer components from 0..255 when color_mode says so, then calls initLight2. Returns 1 for an
 * unknown light_type, else 0.
 */
int JS_classLight_setColor(void *peer)
{
    StudioLight *studio;
    Vector4 *destination;
    int light_type;
    JsClassLight *light;

    light = peer;
    xglStudioGetLight(&studio);
    light_type = light->light_type;
    if (light_type == JS_LIGHT_AMBIENT)
        destination = &studio->ambient_color;
    else if ((unsigned int)(light_type - 1) < JS_LIGHT_COUNT)
        destination = &studio->lights[light_type - 1].color;
    else
        return 1;

    if (light->color_mode == JS_LIGHT_COLOR_INTEGER) {
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

/*
 * Sets the direction of the directional light the peer selects from its three float components,
 * then calls initLight2. Returns 1 for an unknown light_type, else 0.
 */
int JS_classLight_setDirection2(void *peer)
{
    StudioLight *studio;
    Vector4 *destination;
    int light_type;
    JsClassLight *light;

    light = peer;
    xglStudioGetLight(&studio);
    light_type = light->light_type;
    if ((unsigned int)(light_type - 1) < JS_LIGHT_COUNT)
        destination = &studio->lights[light_type - 1].direction;
    else
        return 1;

    destination->x = light->color[0].real;
    destination->y = light->color[1].real;
    destination->z = light->color[2].real;
    initLight2();
    return 0;
}
