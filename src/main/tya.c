#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/main/nonmatchings/tya", getfbp);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaBmpOutput);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaSiPicOutput);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureMain);

static void tyaCaptureNull(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureActor);

/*
 * The engine's actor record (`actor`, 64-entry array at main 0x0043c1e0,
 * 0xa70-byte stride; fuller evidence in src/main/near_dir.h,
 * src/main/enemy_2.h, src/main/set_motion.h and src/main/db_light_write.h,
 * which names the same two fields for the same reason). tyaCaptureShadow
 * only reads +0x00 flags (bits 0x8 and 0x20) and +0x86, the in-use id
 * ACT_create writes and ACT_update skips a slot on when it is zero; the
 * offsets between them are not evidenced by this TU and stay unmodeled.
 */
#define ACTOR_COUNT 64
#define ACTOR_IN_USE_ID_OFFSET 0x86

typedef struct {
    u32 flags;
    u8 unmodeled_04[ACTOR_IN_USE_ID_OFFSET - 0x04];
    short inUseId;
    u8 unmodeled_88[0xA70 - (ACTOR_IN_USE_ID_OFFSET + 2)];
} ActorHead;

extern ActorHead actor[ACTOR_COUNT];

/* The scene's active capture layer: compared against a per-array base
 * (0x200 for the actor array here, 0x300 for MapUnit below) plus the slot
 * index, so only the one actor or map unit the current capture pass wants
 * is drawn. */
extern u16 layer;

/* Defined in src/main/act_3.c (main/tu265), not yet published in
 * include/main/act_3.h. */
void ACT_DrawShadowBegin(void);
void ACT_DrawShadow(ActorHead *unit);
void ACT_DrawShadowEnd(void);

static void tyaCaptureShadow(void)
{
    int i;
    ActorHead *unit;

    unit = actor;
    i = 0;
    ACT_DrawShadowBegin();
    for (; i < ACTOR_COUNT; i++, unit++) {
        if (unit->inUseId != 0 && !(unit->flags & 8) &&
            layer == i + 0x200 && (unit->flags & 0x20)) {
            ACT_DrawShadow(unit);
        }
    }
    ACT_DrawShadowEnd();
}

/*
 * MapUnit[] entries are game-wide unit records (src/main/init_drill.c,
 * src/main/init_uwamono_sys.c and src/main/map_create_unit_peer.h keep
 * their own views of the same array); only the field this TU touches is
 * named. tyaCaptureUnit reads +0xA4 serial, the same field
 * src/main/init_uwamono_sys.c's UwamonoCommonUnit and
 * src/main/init_drill.c's DrillMapUnit name.
 */
#define MAP_UNIT_COUNT 64
#define MAP_UNIT_SERIAL_OFFSET 0xA4

typedef struct {
    u8 unmodeled_00[MAP_UNIT_SERIAL_OFFSET];
    short serial;
    u8 unmodeled_a6[0x300 - (MAP_UNIT_SERIAL_OFFSET + 2)];
} MapUnitHead;

extern MapUnitHead MapUnit[MAP_UNIT_COUNT];

/* Defined in src/main/map_2.c (main/tu269), not yet published in
 * include/main/map_2.h. */
void MAP_drawUnitAt(MapUnitHead *unit);

static void tyaCaptureUnit(void)
{
    int i;
    MapUnitHead *unit;

    for (i = 0, unit = MapUnit; i < MAP_UNIT_COUNT; i++, unit++) {
        if (unit->serial >= 0 && layer == i + 0x300) {
            MAP_drawUnitAt(unit);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureUnit2);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureStart);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureEnd);
