#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"

INCLUDE_ASM("asm/main/nonmatchings/game_radar", GameRadarDraw);

INCLUDE_ASM("asm/main/nonmatchings/game_radar", DrawRadarPing);

INCLUDE_ASM("asm/main/nonmatchings/game_radar", DrawRadarInfo);

INCLUDE_ASM("asm/main/nonmatchings/game_radar", DrawActorPoint);

typedef struct RadarActor {
    u32 flags;
    u8 unmodeled_04[0x82];
    short inUseId;
    u8 unmodeled_88[0x874];
    struct RadarActor *parent;
} RadarActor;

int CheckActorExist(RadarActor *actor)
{
    if (actor->inUseId <= 0) {
        return 0;
    }
    if (actor->flags & 8) {
        return 0;
    }
    return actor->parent == 0;
}

extern u8 *image_004DC554;
extern u8 rate;
extern const char D_004C0618[];

void GameRadarInit(void)
{
    image_004DC554 = (u8 *) (((int) WorkEnd + 0xF) & ~0xF);
    WorkEnd = image_004DC554 + xglCdReadFile(D_004C0618, image_004DC554, 0, 0);
    rate = 0;
}
