/*
 * TU-local declarations of ov12/tu028 (src/ov12/rg_drop_weapon.c).
 */

#ifndef SRC_OV12_RG_DROP_WEAPON_H
#define SRC_OV12_RG_DROP_WEAPON_H

#include "rg_char.h"

/*
 * The actor handle _Disp/_Destruct/CreateRgDropWeapon pass to the Xrg actor
 * API (XrgActorDraw, DisposeXrgActor) is a pointer; XrgActor itself is
 * defined TU-locally by ov12/xrg_actor (src/ov12/xrg_actor.h), so this TU
 * only uses the opaque pointer, as rg_weapon.h and rg_shotmot_control.c do.
 */
typedef struct XrgActor XrgActor;

/*
 * The RgChar type tag CreateRgDropWeapon passes to RgCharAlloc for a
 * dropped-weapon pickup.
 */
#define RG_CHAR_TYPE_DROP_WEAPON 4

/*
 * A dropped weapon extends RgChar: CreateRgDropWeapon allocates it with
 * RgCharAlloc(sizeof(RgDropWeapon), RG_CHAR_TYPE_DROP_WEAPON), so its own
 * fields begin right after RgChar's 0x1C bytes. _InitDrop (this TU's own
 * unrecovered function) clears hidden to 0, stores the actor argument at
 * 0x20 and a float at 0x24 (not evidenced by this allocation's own
 * functions, so it stays unmodeled).
 */
/*
 * _InitDrop sets the float at 0x24 to 1.0; _PassTime_00A1E618 (this TU's
 * own unrecovered function) fades it toward 0 each tick and passes it to
 * XrgActorSetTransparent, so it is a transparency fade factor.
 */
typedef struct RgDropWeapon RgDropWeapon;

struct RgDropWeapon {
    RgChar rgChar;                     /* 0x00 */
    int hidden;                        /* 0x1C: _Disp only draws the actor while this is zero */
    XrgActor *actor;                   /* 0x20 */
    float transparency;                /* 0x24: fade factor passed to XrgActorSetTransparent */
    unsigned char unmodeled_28[0x30 - 0x28];
};

#endif /* SRC_OV12_RG_DROP_WEAPON_H */
