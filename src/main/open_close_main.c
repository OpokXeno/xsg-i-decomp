#include "common.h"
#include "main/open_close_main.h"
#include "open_close_main.h"
#include "main/end_print.h"
#include "main/xgl_2.h"

/* OpenCloseController.phase: 0 starts the transition, 1 runs it, -1 when done. */
#define OPEN_CLOSE_PHASE_START 0
#define OPEN_CLOSE_PHASE_RUNNING 1
#define OPEN_CLOSE_PHASE_DONE (-1)

/* Frames of the grow/shrink transition (frame counts up to this); the cross
 * transition (subtype 01) runs for OPEN_CLOSE_FADE_FRAMES. */
#define OPEN_CLOSE_FRAMES 11
#define OPEN_CLOSE_FADE_FRAMES 7.0
/* Alpha of the first transition segment. */
#define OPEN_CLOSE_ALPHA 0x40
/* Segments (translucent rectangles) drawn per transition frame. */
#define OPEN_CLOSE_SEGMENTS 3

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
 * Partial view of the object eWindowSet initialises. eWindowMain reads
 * x/y/width/height back as the low 16 bits of these words and work as a full
 * word, copying all five into a render packet at +0x1c..+0x27; it also
 * dispatches on mode and gates that same packet's use on visible. flag is
 * cleared here but not read by any function in this TU.
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

/*
 * Draws one frame of the open transition (subtype 00): OPEN_CLOSE_SEGMENTS
 * rectangles around the controller's rectangle, with the outer edges scaled
 * by cos. Advances the frame, or sets the phase to done on the last frame.
 */
static void OpenSubType00(OpenCloseController *controller)
{
    int frames_remaining = OPEN_CLOSE_FRAMES - controller->frame;
    int segment_parameter = frames_remaining * 5 + 10;
    int segment;

    for (segment = 0; segment < OPEN_CLOSE_SEGMENTS; segment++) {
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

        /* Horizontal edges: the outer position moves with cos. */
        cosine = xglCos(angle);
        origin_x = controller->x;
        half_width = (short)controller->width / 2;
        left_x = (int)((float)(origin_x + half_width) -
                       (float)half_width * cosine);

        cosine = xglCos(angle);
        half_width = (short)controller->width / 2;
        right_x = (int)((float)(controller->x + half_width) +
                        (float)half_width * cosine);

        /* The edges are then offset by a sine wave that depends on the segment. */
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
        /* Store the segment rectangle and its grey colour, then draw it. */
        primitive->width = right_x - left_x;
        primitive->work = controller->work;
        primitive->x = left_x;
        primitive->y = top_y;
        primitive->color[0] = -128;
        primitive->color[2] = -128;
        primitive->color[1] = -128;
        primitive->height = bottom_y - top_y;
        if (segment == 0)
            primitive->color[3] = OPEN_CLOSE_ALPHA;
        else
            primitive->color[3] = segment_parameter;

        endPrintExtFunc(primitive->work, END_PRINT_CMD_FRAME, primitive);
    }

    if (frames_remaining == 0)
        controller->phase = OPEN_CLOSE_PHASE_DONE;
    else
        controller->frame++;
}

/*
 * Draws one frame of the close transition (subtype 00). It uses the same
 * layout as OpenSubType00 with sin and cos swapped. Advances the frame, or
 * sets the phase to done on the last frame.
 */
static void CloseSubType00(OpenCloseController *controller)
{
    int frames_remaining = OPEN_CLOSE_FRAMES - controller->frame;
    int segment_parameter = frames_remaining * 5 + 10;
    int segment;

    for (segment = 0; segment < OPEN_CLOSE_SEGMENTS; segment++) {
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
            primitive->color[3] = OPEN_CLOSE_ALPHA;
        else
            primitive->color[3] = segment_parameter;

        endPrintExtFunc(primitive->work, END_PRINT_CMD_FRAME, primitive);
    }

    if (frames_remaining == 0)
        controller->phase = OPEN_CLOSE_PHASE_DONE;
    else
        controller->frame++;
}

/*
 * Draws one frame of the cross transition (subtype 01): a rectangle that
 * grows vertically from the centre of the controller's rectangle. Sets the
 * phase to done once the frame reaches OPEN_CLOSE_FADE_FRAMES.
 */
static void OpenSubType01(OpenCloseController *controller)
{
    int draw_count;
    int dark_channel = -128;
    OpenCloseDrawPrimitive *primitive = controller->primitive;
    double total_frames = OPEN_CLOSE_FADE_FRAMES;
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
        primitive->color[3] = OPEN_CLOSE_ALPHA;
        primitive->color[0] = dark_channel;
        primitive->color[2] = dark_channel;
        primitive->color[1] = dark_channel;
        endPrintExtFunc(primitive->work, END_PRINT_CMD_FRAME, primitive);
        ++primitive;
    } while (draw_count >= 0);

    if (controller->frame == (int)final_duration)
        controller->phase = OPEN_CLOSE_PHASE_DONE;
    else
        controller->frame++;
}

/* Starts the transition on its first call, then runs the handler for the subtype. */
void OpenCloseMain(OpenCloseController *controller)
{
    OpenCloseCallbacks callbacks;

    if (controller->phase != OPEN_CLOSE_PHASE_START) {
        if (controller->phase <= 0)
            return;
        if (controller->phase != OPEN_CLOSE_PHASE_RUNNING)
            return;
    } else {
        controller->frame = 0;
        controller->phase = OPEN_CLOSE_PHASE_RUNNING;
    }

    callbacks = openCloseHandlers;
    callbacks.entry[(controller->subtype >> 4) * 2 +
                    (controller->subtype & 15)](controller);
}

/* Runs one step of the window state machine and draws the window while it is visible. */
void eWindowMain(EWindowMainData *window)
{
    unsigned char mode = window->mode;

    switch (mode) {
    case E_MODE_INIT:
        window->mode = E_MODE_IDLE;
        window->visible = 0;
        break;
    case E_MODE_IDLE:
        break;
    case E_MODE_SHOW_START:
        window->primitive.x = (unsigned short)window->x;
        window->primitive.y = (unsigned short)window->y;
        window->primitive.work = window->work;
        window->primitive.width = (unsigned short)window->width;
        window->primitive.height = (unsigned short)window->height;
        window->primitive.color.red = -128;
        window->primitive.color.blue = -128;
        window->primitive.color.green = -128;
        window->primitive.color.alpha = OPEN_CLOSE_ALPHA;
        window->visible = 1;
        window->mode = mode;
        /* fall through */
    case E_MODE_SHOW_STEP:
        window->mode = E_MODE_RUN_START;
        break;
    /* Open: run the open transition, then switch to showing. */
    case E_MODE_OPEN_START:
        window->controller.x = (unsigned short)window->x;
        window->controller.y = (unsigned short)window->y;
        window->controller.work = window->work;
        window->controller.width = (unsigned short)window->width;
        window->controller.height = (unsigned short)window->height;
        window->mode = E_MODE_OPEN_STEP;
        window->controller.phase = OPEN_CLOSE_PHASE_START;
        window->controller.subtype = 0;
        /* fall through */
    case E_MODE_OPEN_STEP:
        OpenCloseMain(&window->controller);
        if (window->controller.phase < 0)
            window->mode = E_MODE_SHOW_START;
        break;
    /* Running: copy the current window geometry into the render packet each frame. */
    case E_MODE_RUN_START:
        window->mode = E_MODE_RUN_STEP;
        window->visible = 1;
        /* fall through */
    case E_MODE_RUN_STEP:
        window->primitive.x = (unsigned short)window->x;
        window->primitive.y = (unsigned short)window->y;
        window->primitive.work = window->work;
        window->primitive.width = (unsigned short)window->width;
        window->primitive.height = (unsigned short)window->height;
        window->primitive.color.red = -128;
        window->primitive.color.blue = -128;
        window->primitive.color.green = -128;
        window->primitive.color.alpha = OPEN_CLOSE_ALPHA;
        break;
    case 0xa0:
        window->mode = -95;
        break;
    case 0xa1:
        break;
    }
    if (window->visible)
        endPrintExtFunc(window->primitive.work, END_PRINT_CMD_WINDOW,&window->primitive);
}

/* Resets the window to hidden, at the origin, 16x16, with the default grey colour. */
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

/* Advances the cursor state machine one frame and draws its sprite while active. */
void eCursolMain(ECursolMainData *cursol)
{
    switch (cursol->state) {
    case E_MODE_INIT:
        cursol->active = 0;
        cursol->state = E_MODE_INIT;
        break;
    case E_MODE_IDLE:
    case 0x10:
    case 0x30:
    case 0x40:
    case 0x60:
        break;
    case E_MODE_SHOW_START:
        cursol->active = 1;
        cursol->state = E_MODE_SHOW_STEP;
        /* fall through */
    case E_MODE_SHOW_STEP:
        cursol->sprite.x = cursol->x;
        cursol->sprite.y = cursol->y;
        cursol->sprite.style = cursol->width;
        cursol->state = E_MODE_RUN_START;
        break;
    case E_MODE_OPEN_START:
        cursol->active = 1;
        cursol->timer = -4;
        cursol->state = E_MODE_OPEN_STEP;
        /* fall through */
    case E_MODE_OPEN_STEP:
        cursol->sprite.x = cursol->x;
        cursol->sprite.y = cursol->y - (signed char)cursol->timer + 8;
        cursol->sprite.width = 24 - (signed char)cursol->timer;
        cursol->sprite.height = (signed char)cursol->timer * 2;
        cursol->sprite.style = cursol->width;
        if ((signed char)cursol->timer == 8)
            cursol->state = E_MODE_RUN_START;
        else
            cursol->timer = cursol->timer + 1;
        break;
    case E_MODE_RUN_START:
        cursol->active = 1;
        cursol->state = E_MODE_RUN_STEP;
        cursol->timer = 0;
        /* fall through */
    case E_MODE_RUN_STEP:
        cursol->sprite.x = cursol->x;
        cursol->sprite.y = cursol->y;
        cursol->sprite.style = cursol->width;
        break;
    case E_MODE_HIDE_START:
        cursol->active = 0;
        cursol->state = E_MODE_HIDE_STEP;
        break;
    case E_MODE_HIDE_STEP:
        break;
    }
    /* While active, advance the frame counter and draw the sprite with a cycling frame. */
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
        endPrintExtFunc(cursol->sprite.work, END_PRINT_CMD_SPRITE,&cursol->sprite);
    }
}

/*
 * Sets the cursor state to mode. A show or run mode is refused while the
 * cursor is already showing or running; modes below 32 are always accepted.
 */
void eCursolModeChange(ECursolMainData *cursol, unsigned int mode)
{
    switch (mode) {
    case 0 ... 31:
        /* Modes below 32 are always accepted. */
        break;
    case E_MODE_SHOW_START:
    case E_MODE_SHOW_STEP:
        if (cursol->state == E_MODE_RUN_START) return;
        if (cursol->state == E_MODE_RUN_STEP) return;
        if (cursol->state == E_MODE_SHOW_START) return;
        if (cursol->state == E_MODE_SHOW_STEP) return;
        break;
    case E_MODE_RUN_START:
    case E_MODE_RUN_STEP:
        if (cursol->state == E_MODE_RUN_START) return;
        if (cursol->state == E_MODE_RUN_STEP) return;
        break;
    }
    cursol->state = mode;
}

/* Resets the cursor to the origin, the default grey colour and state 0, inactive. */
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
