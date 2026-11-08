#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_drop_weapon.h"

/* Transparency a dropped weapon loses each tick. */
#define RG_DROP_WEAPON_FADE_STEP 0.15f

/* Defined here: the assert strings. */
const char dropWeaponAssertDropNotNil[] = "pDrop != NIL";
const char dropWeaponSourcePath[] = "../rg_drop_weapon.euc.c";

static void _InitDrop(RgDropWeapon *pDrop, XrgActor *actor);
static void _PassTime(RgChar *pChar, float deltaTime);

/* Fades the dropped weapon actor by one step per tick. A drop without an actor is marked hidden, and a hidden drop is freed. */
static void _PassTime(RgChar *pChar, float deltaTime)
{
    RgDropWeapon *pDrop;
    XrgActor *actor;
    int hidden;
    float transparency;
    float clampedTransparency;

    pDrop = (RgDropWeapon *)pChar;
    if (pDrop == 0) {
        assert_prog(dropWeaponAssertDropNotNil, dropWeaponSourcePath, 34);
    }

    hidden = pDrop->hidden;
    actor = pDrop->actor;
    if (hidden == 0) {
        if (actor == 0) {
            pDrop->hidden = 1;
            return;
        }
        {
            transparency = pDrop->transparency - RG_DROP_WEAPON_FADE_STEP;
            pDrop->transparency = transparency;
            clampedTransparency = 0.0f;
            if (!(transparency < 0.0f)) {
                if (!(transparency < 0.0f)) {
                    clampedTransparency = 1.0f;
                    if (!(transparency > 1.0f)) {
                        clampedTransparency = transparency;
                    }
                }
                XrgActorSetTransparent(actor, clampedTransparency);
                return;
            }
            XrgActorSetDraw(actor, 0);
            return;
        }
    } else {
        do {
            RgCharFree(pChar);
        } while (0);
    }
    return;
}

/* Draws the actor of a drop weapon unless the drop is hidden. */
static void _Disp(RgDropWeapon *pDrop)
{
    XrgActor *actor;

    if (pDrop == 0) {
        assert_prog(dropWeaponAssertDropNotNil, dropWeaponSourcePath, 61);
    }
    if (pDrop->hidden == 0) {
        actor = pDrop->actor;
        if (actor != 0) {
            /*
             * The original calls XrgActorDraw with jal and returns through
             * the epilogue, not a sibling jump. The compiler keeps the call
             * out of tail position only inside a loop, so the do/while (0)
             * stays.
             */
            do {
                XrgActorDraw(actor);
            } while (0);
        }
    }
}

static void _Destruct(RgChar *pChar)
{
    RgDropWeapon *pDrop = (RgDropWeapon *)pChar;

    if (pDrop == 0) {
        assert_prog(dropWeaponAssertDropNotNil, dropWeaponSourcePath, 75);
    }
    if (pDrop->actor != 0) {
        DisposeXrgActor(pDrop->actor);
    }
}

/* Installs the destruct, pass-time and display methods, and sets the actor, hidden flag and transparency. */
static void _InitDrop(RgDropWeapon *pDrop, XrgActor *actor)
{
    if (pDrop == 0) {
        assert_prog(dropWeaponAssertDropNotNil, dropWeaponSourcePath, 86);
    }
    RgCharDestructMethod(&pDrop->rgChar, _Destruct);
    RgCharPassTimeMethod(&pDrop->rgChar, _PassTime);
    RgCharDispMethod(&pDrop->rgChar, (void (*)(RgChar *)) _Disp);
    pDrop->hidden = 0;
    pDrop->actor = actor;
    pDrop->transparency = 1.0f;
}

RgDropWeapon *CreateRgDropWeapon(XrgActor *actor)
{
    RgDropWeapon *pDrop;

    pDrop = (RgDropWeapon *)RgCharAlloc(sizeof(RgDropWeapon), RG_CHAR_TYPE_DROP_WEAPON);
    _InitDrop(pDrop, actor);
    return pDrop;
}
