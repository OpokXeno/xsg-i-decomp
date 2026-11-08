#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"
#include "game_init_camera.h"
#include "main/game_init_camera.h"
#include "main/xgl_font.h"
#include "main/xgl_light.h"
#include "camera.h"

#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (8 - __builtin_args_info(2)) * 8)
#define va_end(ap) ((void)0)
#define NULL ((void *)0)

typedef struct PADControllerState {
    u8 unmodeled_00[0x28];
    u16 held;
    u16 pressed;
    u16 repeat;
    u16 other_buttons;
    u8 unmodeled_30[0x38];
} PADControllerState;

typedef struct PADKeyResult {
    u16 held;
    u16 pressed;
    u16 repeat;
    u16 other_buttons;
    u8 flags[8];
} PADKeyResult;

typedef char *va_list;

extern PADControllerState PadData[];

extern unsigned int strlen(const char *string);
extern char *strchr(const char *string, int character);
extern char *strncpy(char *destination, const char *source, unsigned int count);
extern int vsprintf(char *buffer, const char *format, va_list args);
extern char *strrchr(const char *s, int c);

static int dbCX = 0;

static int dbCY = 0;

/* Debug text printer: depth (the 24-bit maximum), default line height, the
 * buffer a formatted line is built in, and the font flags that mean debug
 * text is on. */
#define DB_DEPTH 0x00FFFFFF
#define DB_LINE_HEIGHT 24
#define DB_LINE_BUFFER_SIZE 0x100
#define DB_PATH_PREFIX_SIZE 0x80
#define DB_FONT_FLAGS_ON 3

static int dbCZ = DB_DEPTH;

static int dbCH = DB_LINE_HEIGHT;

static int dbMODE = 0;

/* Resets the tcamera table, sets camera 0's offsets and constraints, and activates only studio camera 0. */
void GAME_initCamera(void)
{
    StudioCamera *studio_camera;
    float translation_z;
    float translation_y;
    float view_height;

    TCAMERA_init();
    translation_z = 4.0f;
    translation_y = 2.0f;
    view_height = 1.4f;
    tcamera[0].view_offset.y = view_height;
    tcamera[0].translation_offset.z = translation_z;
    tcamera[0].translation_offset.y = translation_y;
    tcamera[0].channel_mode[0] = CAMERA_MODE_CONSTANT;
    tcamera[0].channel_mode[1] = CAMERA_MODE_CONSTANT;
    tcamera[0].translation_constraint = &actor;
    tcamera[0].view_constraint = &actor;
    tcamera[0].view_offset.x = 0.0f;
    tcamera[0].view_offset.z = 0.0f;
    tcamera[0].translation_offset.x = 0.0f;

    xglStudioGetCamera(&studio_camera, 0);
    studio_camera->active = 1;
    xglStudioGetCamera(&studio_camera, 1);
    studio_camera->active = 0;
    xglStudioGetCamera(&studio_camera, 2);
    studio_camera->active = 0;
    xglStudioGetCamera(&studio_camera, 3);
    studio_camera->active = 0;
}

void GAME_initLight(void)
{
    StudioLight *light;

    xglStudioGetLight(&light);
    xglLightSetDefault(light);
}

/* Returns the last occurrence of `character` in the first `length` bytes of `string`, or 0. */
char *STR_indexReverse(char *string, int length, int character)
{
    int index = (int)((unsigned int)length - 1U);

    while (index >= 0) {
        char *cursor = &string[index];
        const long current_character = *cursor;

        --index;
        if (current_character == character)
            return cursor;
    }
    return 0;
}

/* Copies pad `pad_index`'s button words masked by `button_mask` into `result`, and clears its flags. */
void PAD_getKey(PADKeyResult *result, int pad_index, unsigned int button_mask)
{
    PADControllerState *pad = &PadData[pad_index];

    result->held = pad->held & button_mask;
    result->pressed = pad->pressed & button_mask;
    result->repeat = pad->repeat & button_mask;
    result->other_buttons = pad->other_buttons & button_mask;
    result->flags[0] = 0;
    result->flags[1] = 0;
    result->flags[4] = 0;
    result->flags[5] = 0;
    result->flags[2] = 0;
    result->flags[3] = 0;
    result->flags[6] = 0;
    result->flags[7] = 0;
}

void DB_cmode(int mode)
{
    dbMODE = mode;
}

/* Sets the debug text cursor to (x * 2, y * 2) and restores the default line height. */
void DB_reset(int x, int y)
{
    dbCX = x << 1;
    dbCY = y << 1;
    dbCH = DB_LINE_HEIGHT;
}

/* Sets the debug text cursor to (x * 2, y * 2) with a line height of line_height * 2. */
void DB_reset2(int x, int y, int line_height)
{
    dbCX = x << 1;
    dbCY = y << 1;
    dbCH = line_height << 1;
}

void DB_incPos(int x, int y)
{
    dbCX += x;
    dbCY += y;
}

/* Prints `text` at the debug cursor while debug text is enabled. */
void DB_params(const char *text)
{
    if ((xglFontGetFlags() & DB_FONT_FLAGS_ON) == DB_FONT_FLAGS_ON) {
        xglFontPrint(dbCX, dbCY, dbCZ, text);
    }
}

/*
 * Formats a line and prints it at the debug cursor while debug text is enabled, then
 * moves the cursor down one line.  The text is drawn only in mode 0.
 */
void DB_println(const char *format, ...)
{
    va_list args;
    char buffer[DB_LINE_BUFFER_SIZE];

    if ((xglFontGetFlags() & DB_FONT_FLAGS_ON) == DB_FONT_FLAGS_ON) {
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);
        if (dbMODE == 0) {
            xglFontPrint(dbCX, dbCY, dbCZ, buffer);
        }
        dbCY += dbCH;
    }
}

/* Formats a line, prints it at the debug cursor, moves the cursor down one line, and returns 0. */
int DB_printf(const char *format, ...)
{
    va_list args;
    char buffer[DB_LINE_BUFFER_SIZE];

    if ((xglFontGetFlags() & DB_FONT_FLAGS_ON) != DB_FONT_FLAGS_ON) {
        return 0;
    }
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    xglFontPrint(dbCX, dbCY, dbCZ, buffer);
    dbCY += dbCH;
    return 0;
}

/*
 * Writes `path` to `destination`.  A path longer than `max_width` is shortened to its
 * first directory, "...", and its last component.
 */
void DB_pathGetShortPath(char *destination, const char *path, int max_width)
{
    extern char *strrchr(const char *string, int character);
    char prefix[DB_PATH_PREFIX_SIZE];
    char *first_slash;
    const char *last_slash;
    int prefix_length;

    if (max_width < (int)strlen(path)) {
        prefix[0] = '\0';
        first_slash = strchr(path, '/');
        if (first_slash != 0) {
            prefix_length = first_slash - path + 1;
            strncpy(prefix, path, prefix_length);
            prefix[prefix_length] = '\0';
        }
        last_slash = strrchr(path, '/');
        sprintf(destination, "%s...%s\n", prefix,
                last_slash != 0 ? last_slash : path);
    } else {
        sprintf(destination, "%s\n", path);
    }
}

/* Returns the part of `path` after its last slash, or `path` itself when it has no slash. */
char *DB_pathFindName(char *path)
{
    char *slash = strrchr(path, '/');

    return (slash != NULL) ? (slash + 1) : path;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* JP: 遠藤
 * EN: Endo */
const char GameEndouTestTitleText[8] = "\031\000\261\363\306\243";
