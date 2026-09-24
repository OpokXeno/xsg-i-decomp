#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"

extern int dbCX;
extern int dbCY;
extern int dbCH;
extern int dbMODE;

extern void xglLightSetDefault(StudioLight *light);

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", GAME_initCamera);

void GAME_initLight(void)
{
    StudioLight *light;

    xglStudioGetLight(&light);
    xglLightSetDefault(light);
}

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", STR_indexReverse);

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", PAD_getKey);

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

extern int dbCZ;

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

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", DB_printf);

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", DB_pathGetShortPath);

#define NULL ((void *)0)

extern char *strrchr(const char *s, int c);

char *DB_pathFindName(char *path)
{
    char *slash = strrchr(path, '/');

    return (slash != NULL) ? (slash + 1) : path;
}
