#include "common.h"
#include "main/open_close_main.h"
#include "open_close_main.h"
#include "main/end_print.h"
#include "main/xgl_2.h"

/* The transition primitives in this TU set red, blue, then green. */
#define E_WIDGET_SET_RGB(widget_value, red_value, green_value, blue_value) do { \
    (widget_value)->color.red = (red_value); \
    (widget_value)->color.blue = (blue_value); \
    (widget_value)->color.green = (green_value); \
} while (0)

typedef struct OpenCloseDrawPrimitive {
    short x;
    short y;
    int work;
    short width;
    short height;
    signed char color[4];
} OpenCloseDrawPrimitive;

/* The controller's first 16 bytes are the rectangle passed to the drawing
 * routines; the phase/subtype/frame bytes follow at +0x10..+0x12. */
typedef struct OpenCloseController {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int work;
    unsigned short width;
    unsigned short height;
    signed char phase;
    unsigned char subtype;
    unsigned char frame;
    unsigned char unmodeled_13;
    OpenCloseDrawPrimitive primitive[3];
} OpenCloseController;

typedef struct OpenCloseCallbacks {
    void (*entry[4])(OpenCloseController *);
} OpenCloseCallbacks;

typedef struct EWidgetColor {
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
} EWidgetColor;

/*
 * Partial view of the object eWindowSet initialises. eWindowMain (still
 * INCLUDE_ASM in this TU) reads x/y/width/height back as the low 16 bits of
 * these words and work as a full word, copying all five into a render packet
 * at +0x1c..+0x27; it also dispatches on mode through a dozen-plus state
 * constants and gates that same packet's use on visible. flag is cleared here
 * but not read by any function in this TU.
 */
typedef struct EWindow {
    int x;               /* +0x00 */
    int y;                /* +0x04 */
    int work;             /* +0x08 */
    int width;             /* +0x0c */
    int height;             /* +0x10 */
    EWidgetColor color;   /* +0x14..0x17 */
    signed char mode;        /* +0x18 */
    signed char flag;         /* +0x19 */
    signed char visible;       /* +0x1a */
} EWindow;

typedef struct EWindowRenderPacket {
    short x;
    short y;
    int work;
    short width;
    short height;
    EWidgetColor color;
} EWindowRenderPacket;

/* Main updates the render descriptor that follows each setter's partial
 * object layout. State and visibility bytes are read without sign extension. */
typedef struct EWindowMainData {
    int x;
    int y;
    int work;
    int width;
    int height;
    EWidgetColor color;
    unsigned char mode;
    unsigned char flag;
    unsigned char visible;
    EWindowRenderPacket primitive;
    OpenCloseController controller;
} EWindowMainData;

static void OpenSubType00(OpenCloseController *);
static void OpenSubType01(OpenCloseController *);
static void CloseSubType00(OpenCloseController *);
extern void endSpriteSet(void *, int);

/* Four original function pointers: open subtype 00/01, close 00/00. */
const OpenCloseCallbacks openCloseHandlers = {
    { OpenSubType00, OpenSubType01, CloseSubType00, CloseSubType00 }
};

const char OpenCloseMenu02Path[24] = "data\\endou\\menu02.xtx";

const char OpenCloseMenu01Path[24] = "data\\endou\\menu01.xtx";

const char OpenCloseCfwinPath[32] = "data\\endou\\cfwin.xtx";

static void OpenSubType00(OpenCloseController *controller)
{
    int frames_remaining = 11 - controller->frame;
    int segment_parameter = frames_remaining * 5 + 10;
    int segment;

    for (segment = 0; segment < 3; segment++) {
        float angle = (float)frames_remaining * 0.1308997f;
        float cosine;
        float sine;
        float wave;
        int half_width;
        int half_height;
        int left_x;
        int origin_x;
        int right_x;
        int top_y;
        int bottom_y;
        OpenCloseDrawPrimitive *primitive = &controller->primitive[segment];

        cosine = xglCos(angle);
        origin_x = controller->x;
        half_width = (short)controller->width / 2;
        left_x = (int)((float)(origin_x + half_width) -
                       (float)half_width * cosine);

        cosine = xglCos(angle);
        half_width = (short)controller->width / 2;
        right_x = (int)((float)(controller->x + half_width) +
                        (float)half_width * cosine);

        angle = (float)frames_remaining * 0.2617994f;
        sine = xglSin(angle);
        wave = (float)segment * 0.4f;
        half_width = (short)controller->width / 2;
        left_x = (int)((float)left_x - (float)half_width * wave * sine);

        sine = xglSin(angle);
        half_width = (short)controller->width / 2;
        right_x = (int)((float)right_x + (float)half_width * wave * sine);

        angle = (float)frames_remaining * 0.1308997f;
        cosine = xglCos(angle);
        half_height = (short)controller->height / 2;
        top_y = (int)((float)(controller->y + half_height) -
                      (float)half_height * cosine);

        cosine = xglCos(angle);
        half_height = (short)controller->height / 2;
        bottom_y = (int)((float)(controller->y + half_height) +
                         (float)half_height * cosine);

        angle = (float)frames_remaining * 0.2617994f;
        sine = xglSin(angle);
        half_height = (short)controller->height / 2;
        top_y = (int)((float)top_y - (float)half_height * wave * sine);

        sine = xglSin(angle);
        half_height = (short)controller->height / 2;
        bottom_y = (int)((float)bottom_y + (float)half_height * wave * sine);

        angle = (float)frames_remaining * 0.1308997f;
        primitive->width = right_x - left_x;
        primitive->work = controller->work;
        primitive->x = left_x;
        primitive->y = top_y;
        primitive->color[0] = -128;
        primitive->color[2] = -128;
        primitive->color[1] = -128;
        primitive->height = bottom_y - top_y;
        if (segment == 0)
            primitive->color[3] = 0x40;
        else
            primitive->color[3] = segment_parameter;

        endPrintExtFunc(primitive->work, 9, primitive);
    }

    if (frames_remaining == 0)
        controller->phase = -1;
    else
        controller->frame++;
}

static void CloseSubType00(OpenCloseController *controller)
{
    int frames_remaining = 11 - controller->frame;
    int segment_parameter = frames_remaining * 5 + 10;
    int segment;

    for (segment = 0; segment < 3; segment++) {
        float angle = (float)frames_remaining * 0.1308997f;
        float cosine;
        float sine;
        float wave;
        int half_width;
        int half_height;
        int left_x;
        int origin_x;
        int right_x;
        int top_y;
        int bottom_y;
        OpenCloseDrawPrimitive *primitive = &controller->primitive[segment];

        sine = xglSin(angle);
        origin_x = controller->x;
        half_width = (short)controller->width / 2;
        left_x = (int)((float)(origin_x + half_width) -
                       (float)half_width * sine);

        sine = xglSin(angle);
        half_width = (short)controller->width / 2;
        right_x = (int)((float)(controller->x + half_width) +
                        (float)half_width * sine);

        angle = (float)frames_remaining * 0.2617994f;
        cosine = xglCos(angle);
        wave = (float)segment * 0.4f;
        half_width = (short)controller->width / 2;
        left_x = (int)((float)left_x -
                       (float)half_width * wave * cosine);

        cosine = xglCos(angle);
        half_width = (short)controller->width / 2;
        right_x = (int)((float)right_x +
                        (float)half_width * wave * cosine);

        angle = (float)frames_remaining * 0.1308997f;
        sine = xglSin(angle);
        half_height = (short)controller->height / 2;
        top_y = (int)((float)(controller->y + half_height) -
                      (float)half_height * sine);

        sine = xglSin(angle);
        half_height = (short)controller->height / 2;
        bottom_y = (int)((float)(controller->y + half_height) +
                         (float)half_height * sine);

        angle = (float)frames_remaining * 0.2617994f;
        cosine = xglCos(angle);
        half_height = (short)controller->height / 2;
        top_y = (int)((float)top_y -
                      (float)half_height * wave * cosine);

        cosine = xglCos(angle);
        half_height = (short)controller->height / 2;
        bottom_y = (int)((float)bottom_y +
                         (float)half_height * wave * cosine);

        angle = (float)frames_remaining * 0.1308997f;
        primitive->width = right_x - left_x;
        primitive->work = controller->work;
        primitive->x = left_x;
        primitive->y = top_y;
        primitive->color[0] = -128;
        primitive->color[2] = -128;
        primitive->color[1] = -128;
        primitive->height = bottom_y - top_y;
        if (segment == 0)
            primitive->color[3] = 0x40;
        else
            primitive->color[3] = segment_parameter;

        endPrintExtFunc(primitive->work, 9, primitive);
    }

    if (frames_remaining == 0)
        controller->phase = -1;
    else
        controller->frame++;
}

static void OpenSubType01(OpenCloseController *controller)
{
    int draw_count;
    int dark_channel = -128;
    OpenCloseDrawPrimitive *primitive = controller->primitive;
    double total_frames = 7.0;
    double final_duration;

    draw_count = 0;
    do {
        int half_height;
        double center_y;
        double displacement;
        int top;
        int bottom;
        int draw_height;

        --draw_count;
        primitive->x = controller->x;
        primitive->width = controller->width;
        half_height = (short)controller->height / 2;
        final_duration = total_frames;
        center_y = controller->y + half_height;
        displacement = half_height *
                       ((double)(int)controller->frame / final_duration);
        top = (int)(center_y - displacement);
        bottom = (int)(center_y + displacement);
        draw_height = bottom - top;
        primitive->y = top;
        primitive->work = controller->work;
        primitive->height = draw_height;
        primitive->color[3] = 0x40;
        primitive->color[0] = dark_channel;
        primitive->color[2] = dark_channel;
        primitive->color[1] = dark_channel;
        endPrintExtFunc(primitive->work, 9, primitive);
        ++primitive;
    } while (draw_count >= 0);

    if (controller->frame == (int)final_duration)
        controller->phase = -1;
    else
        controller->frame++;
}

void OpenCloseMain(OpenCloseController *controller)
{
    OpenCloseCallbacks callbacks;

    if (controller->phase != 0) {
        if (controller->phase <= 0)
            return;
        if (controller->phase != 1)
            return;
    } else {
        controller->frame = 0;
        controller->phase = 1;
    }

    callbacks = openCloseHandlers;
    callbacks.entry[(controller->subtype >> 4) * 2 +
                    (controller->subtype & 15)](controller);
}

void eWindowMain(EWindowMainData *window)
{
    unsigned char mode = window->mode;

    switch (mode) {
    case 0:
        window->mode = 1;
        window->visible = 0;
        break;
    case 1:
        break;
    case 0x20:
        window->primitive.x = (unsigned short)window->x;
        window->primitive.y = (unsigned short)window->y;
        window->primitive.work = window->work;
        window->primitive.width = (unsigned short)window->width;
        window->primitive.height = (unsigned short)window->height;
        window->primitive.color.red = -128;
        window->primitive.color.blue = -128;
        window->primitive.color.green = -128;
        window->primitive.color.alpha = 64;
        window->visible = 1;
        window->mode = mode;
        /* fall through */
    case 0x21:
        window->mode = 0x50;
        break;
    case 0x22:
        window->controller.x = (unsigned short)window->x;
        window->controller.y = (unsigned short)window->y;
        window->controller.work = window->work;
        window->controller.width = (unsigned short)window->width;
        window->controller.height = (unsigned short)window->height;
        window->mode = 0x23;
        window->controller.phase = 0;
        window->controller.subtype = 0;
        /* fall through */
    case 0x23:
        OpenCloseMain(&window->controller);
        if (window->controller.phase < 0)
            window->mode = 0x20;
        break;
    case 0x50:
        window->mode = 0x51;
        window->visible = 1;
        /* fall through */
    case 0x51:
        window->primitive.x = (unsigned short)window->x;
        window->primitive.y = (unsigned short)window->y;
        window->primitive.work = window->work;
        window->primitive.width = (unsigned short)window->width;
        window->primitive.height = (unsigned short)window->height;
        window->primitive.color.red = -128;
        window->primitive.color.blue = -128;
        window->primitive.color.green = -128;
        window->primitive.color.alpha = 64;
        break;
    case 0xa0:
        window->mode = -95;
        break;
    case 0xa1:
        break;
    }
    if (window->visible)
        endPrintExtFunc(window->primitive.work, 1, &window->primitive);
}

void eWindowSet(EWindow *window)
{
    window->color.alpha = -128;
    window->height = 0x10;
    window->flag = 0;
    window->x = 0;
    window->y = 0;
    window->work = 0;
    E_WIDGET_SET_RGB(window, -128, -128, -128);
    window->width = 0x10;
    window->mode = 0;
    window->visible = 0;
}

void eCursolMain(ECursolMainData *cursol)
{
    switch (cursol->state) {
    case 0:
        cursol->active = 0;
        cursol->state = 0;
        break;
    case 1:
    case 0x10:
    case 0x30:
    case 0x40:
    case 0x60:
        break;
    case 0x20:
        cursol->active = 1;
        cursol->state = 0x21;
        /* fall through */
    case 0x21:
        cursol->sprite.x = cursol->x;
        cursol->sprite.y = cursol->y;
        cursol->sprite.style = cursol->width;
        cursol->state = 0x50;
        break;
    case 0x22:
        cursol->active = 1;
        cursol->timer = -4;
        cursol->state = 0x23;
        /* fall through */
    case 0x23:
        cursol->sprite.x = cursol->x;
        cursol->sprite.y = cursol->y - (signed char)cursol->timer + 8;
        cursol->sprite.width = 24 - (signed char)cursol->timer;
        cursol->sprite.height = (signed char)cursol->timer * 2;
        cursol->sprite.style = cursol->width;
        if ((signed char)cursol->timer == 8)
            cursol->state = 0x50;
        else
            cursol->timer = cursol->timer + 1;
        break;
    case 0x50:
        cursol->active = 1;
        cursol->state = 0x51;
        cursol->timer = 0;
        /* fall through */
    case 0x51:
        cursol->sprite.x = cursol->x;
        cursol->sprite.y = cursol->y;
        cursol->sprite.style = cursol->width;
        break;
    case 0x70:
        cursol->active = 0;
        cursol->state = 0x71;
        break;
    case 0x71:
        break;
    }
    if (cursol->active) {
        int frame = cursol->timer + 1;
        unsigned char style = cursol->width;
        int phase;

        cursol->sprite.color[0] = cursol->color[0];
        cursol->sprite.color[1] = cursol->color[1];
        cursol->sprite.color[2] = cursol->color[2];
        cursol->sprite.color[3] = cursol->color[3];
        cursol->timer = frame;
        cursol->sprite.flags = 0;
        if (style == 0) {
            phase = (unsigned long long)frame >> 1;
            cursol->sprite.frame = phase % 8;
        } else {
            phase = (unsigned long long)frame >> 2;
            cursol->sprite.frame = phase % 4;
        }
        cursol->sprite.work = cursol->work;
        endSpriteSet(&cursol->sprite, 1);
        endPrintExtFunc(cursol->sprite.work, 2, &cursol->sprite);
    }
}

void eCursolModeChange(ECursolMainData *cursol, unsigned int mode)
{
    if (mode < 34) {
        if (mode < 32)
            goto set_state;
        goto protected_early_states;
    }
    if (mode >= 82)
        goto set_state;
    if (mode >= 80)
        goto protected_late_states;
    goto set_state;

protected_early_states:
    if (cursol->state == 'P') return;
    if (cursol->state == 'Q') return;
    if (cursol->state == ' ') return;
    if (cursol->state == '!') return;
    goto set_state;

protected_late_states:
    if (cursol->state == 'P') return;
    if (cursol->state == 'Q') return;
set_state:
    cursol->state = mode;
}

void eCursolSet(ECursolMainData *cursol, int width)
{
    cursol->x = 0;
    cursol->y = 0;
    cursol->work = 0;
    cursol->color[3] = -128;
    cursol->color[2] = -128;
    cursol->color[1] = -128;
    cursol->state = 0;
    cursol->color[0] = -128;
    cursol->timer = 0;
    cursol->active = 0;
    cursol->width = width;
}
