#ifndef INCLUDE_MAIN_INIT_UWAMONO_SYS_H
#define INCLUDE_MAIN_INIT_UWAMONO_SYS_H

#include "shared.h"

/* MapUnitRecord.flags */
enum {
    MAP_UNIT_NO_DRAW = 0x4,             /* the model is not drawn */
    MAP_UNIT_ACTIVE = 0x10000,          /* the slot is live: updated and collidable */
    MAP_UNIT_LANDED = 0x80000,          /* a falling unit has come to rest (CheckUwamonoHeight) */
    MAP_UNIT_BROKEN = 0x20000,          /* a breakable that has been destroyed */
    MAP_UNIT_SELECTABLE = 0x40000,      /* currently a valid shoot/aim target */
    MAP_UNIT_HIDDEN = 0x100000,         /* excluded from collision (the unit it stands in for is inactive) */
    MAP_UNIT_SAVE_SYMBOL = 0x20000000,  /* save point symbol */
    MAP_UNIT_SHOP_SYMBOL = 0x40000000   /* shop symbol */
};

/* MapUnitRecord.serial / resource id ranges. */
#define UWAMONO_SERIAL_RESOURCE_BASE 0x1000  /* below: map parts index; above: resource id */
#define UWAMONO_ID_SPECIAL_FIRST 0x7000      /* ids 0x7000..0x7013 take no resource load */
#define UWAMONO_ID_COLLISION UWAMONO_ID_SPECIAL_FIRST  /* collision box unit */
#define UWAMONO_ID_RESIDENT_FIRST 0x7001     /* first of the 14 resident muobj resources */
#define UWAMONO_RESIDENT_COUNT 14
#define UWAMONO_ID_SYMBOL_FIRST 0x7005       /* treasure, save and shop objects */
#define UWAMONO_ID_SYMBOL_COUNT 3U
#define UWAMONO_ID_SOUND 0x7012              /* sound emitter unit */
#define UWAMONO_ID_SPECIAL_COUNT 0x14U

/* Entries of KowareKind_t, indexed by a breakable part's kindIndex. */
#define KOWARE_KIND_COUNT 127

/* UwamonoMapUnitState.kindIndex / mapType: what an InitUwamono unit is. */
enum {
    UWAMONO_TYPE_DOOR = 1,
    UWAMONO_TYPE_KOWARE = 2,            /* breakable map part */
    UWAMONO_TYPE_TRAP = 3,
    UWAMONO_TYPE_ITEM_BOX = 4,
    UWAMONO_TYPE_ITEM_SYMBOL = 5,
    UWAMONO_TYPE_SAVE_SYMBOL = 6,
    UWAMONO_TYPE_SHOP_SYMBOL = 7,
    UWAMONO_TYPE_EVS_SYMBOL = 8,
    UWAMONO_TYPE_RET_SYMBOL = 9,
    UWAMONO_TYPE_SPECIAL_SYMBOL = 10,
    UWAMONO_TYPE_DRILL = 20
};

/* Collision shape of a unit (state byte, crossType, KowareKind shape). */
enum {
    UWAMONO_SHAPE_CIRCLE = 1,
    UWAMONO_SHAPE_BOX = 2
};

int CrossPointUwamono(const Vector4 *position, const Vector4 *previous,
                      float radius, Vector4 *result);

int HitCheckMapUnitPos(const Vector4 *position);

/* Functions and data used by other translation units. */
int HitCheckMapUnitPosSize(const Vector4 *position, float radius);
void UnlockMapUnit(void);
void InitUwamonoSys(void);
void SetHideObject(void *argument);
int HitCheckMapUnit(void *actor);

#endif /* INCLUDE_MAIN_INIT_UWAMONO_SYS_H */
