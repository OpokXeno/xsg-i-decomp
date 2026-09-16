#include "common.h"
#include "shared.h"

extern int dbCX;
extern int dbCY;
extern int dbCH;
extern int dbMODE;

extern void xglStudioGetLight(void *light_out);
extern void xglLightSetDefault(void *light);

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", GAME_initCamera);

void GAME_initLight(void)
{
    void *light;

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

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", DB_params);

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", DB_println);

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", DB_printf);

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", DB_pathGetShortPath);

INCLUDE_ASM("asm/main/nonmatchings/game_init_camera", DB_pathFindName);
