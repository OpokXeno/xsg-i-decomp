#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"

static int dbCX = 0;
static int dbCY = 0;
static int dbCZ = 0x00FFFFFF;
static int dbCH = 24;
static int dbMODE = 0;

extern void xglLightSetDefault(StudioLight *light);

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", GAME_initCamera);

void GAME_initLight(void)
{
    StudioLight *light;

    xglStudioGetLight(&light);
    xglLightSetDefault(light);
}

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", STR_indexReverse);

extern unsigned int strlen(const char *string);
extern char *strchr(const char *string, int character);
extern char *strncpy(char *destination, const char *source, unsigned int count);
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
extern PADControllerState PadData[];

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

void DB_reset(int x, int y)
{
    dbCX = x << 1;
    dbCY = y << 1;
    dbCH = 24;
}

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

extern int xglFontGetFlags(void);

void DB_params(const char *text)
{
    if ((xglFontGetFlags() & 3) == 3) {
        xglFontPrint(dbCX, dbCY, dbCZ, text);
    }
}

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (8 - __builtin_args_info(2)) * 8)
#define va_end(ap) ((void)0)

extern int vsprintf(char *buffer, const char *format, va_list args);

void DB_println(const char *format, ...)
{
    va_list args;
    char buffer[0x100];

    if ((xglFontGetFlags() & 3) == 3) {
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);
        if (dbMODE == 0) {
            xglFontPrint(dbCX, dbCY, dbCZ, buffer);
        }
        dbCY += dbCH;
    }
}

int DB_printf(const char *format, ...)
{
    va_list args;
    char buffer[0x100];

    if ((xglFontGetFlags() & 3) != 3) {
        return 0;
    }
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    xglFontPrint(dbCX, dbCY, dbCZ, buffer);
    dbCY += dbCH;
    return 0;
}

void DB_pathGetShortPath(char *destination, const char *path, int max_width)
{
    extern char *strrchr(const char *string, int character);
    char prefix[0x80];
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

#define NULL ((void *)0)

extern char *strrchr(const char *s, int c);

char *DB_pathFindName(char *path)
{
    char *slash = strrchr(path, '/');

    return (slash != NULL) ? (slash + 1) : path;
}
