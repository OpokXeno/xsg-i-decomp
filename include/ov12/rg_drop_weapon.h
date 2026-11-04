#ifndef INCLUDE_OV12_RG_DROP_WEAPON_H
#define INCLUDE_OV12_RG_DROP_WEAPON_H

#include "shared.h"
#include "ov12/rg_char.h"
#include "ov12/xrg_actor.h"

/* The RgChar type tag CreateRgDropWeapon passes to RgCharAlloc. */
#define RG_CHAR_TYPE_DROP_WEAPON 4

/*
 * A dropped weapon pickup: an RgChar that draws an actor and fades it out
 * (transparency from 1.0 toward 0) until the actor disappears.
 */
typedef struct RgDropWeapon {
    RgChar rgChar;                     /* 0x00 */
    int hidden;                        /* 0x1C: set once the actor is gone; the character frees itself next tick */
    XrgActor *actor;                   /* 0x20 */
    float transparency;                /* 0x24: fade factor passed to XrgActorSetTransparent */
    unsigned char unmodeled_28[0x30 - 0x28];
} RgDropWeapon;

/* Functions and data used by other translation units. */
RgDropWeapon *CreateRgDropWeapon(XrgActor *actor);

#endif /* INCLUDE_OV12_RG_DROP_WEAPON_H */
