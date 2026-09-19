/*
 * OV12 original TU 28: 0x00a1e618..0x00a1e880 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_char.h"
#include "rg_drop_weapon.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern const char D_00A53CF0[]; /* "pDrop != NIL" */
extern const char D_00A53D00[]; /* "../rg_drop_weapon.euc.c" */

extern void XrgActorDraw(XrgActor *actor);
extern void DisposeXrgActor(XrgActor *actor);

/* Defined later in this TU (still INCLUDE_ASM); called by CreateRgDropWeapon. */
extern void _InitDrop(RgDropWeapon *pDrop, XrgActor *actor);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_drop_weapon", _PassTime_00A1E618);

static void _Disp(RgDropWeapon *pDrop)
{
    XrgActor *actor;

    if (pDrop == 0) {
        assert_prog(D_00A53CF0, D_00A53D00, 61);
    }
    if (pDrop->hidden == 0) {
        actor = pDrop->actor;
        if (actor != 0) {
            /*
             * The original calls XrgActorDraw with jal and returns through
             * the shared epilogue instead of a sibling jump. Under this TU's
             * compiler the call stays out of tail position only inside a
             * loop construct, which is the shape a do/while (0) statement
             * macro gives it; a nested if, a combined condition, early
             * returns, a switch or a void (RgChar *) signature all compile
             * to `j XrgActorDraw`.
             */
            do {
                XrgActorDraw(actor);
            } while (0);
        }
    }
}

static void _Destruct(RgDropWeapon *pDrop)
{
    if (pDrop == 0) {
        assert_prog(D_00A53CF0, D_00A53D00, 75);
    }
    if (pDrop->actor != 0) {
        DisposeXrgActor(pDrop->actor);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_drop_weapon", _InitDrop);

RgDropWeapon *CreateRgDropWeapon(XrgActor *actor)
{
    RgDropWeapon *pDrop;

    pDrop = (RgDropWeapon *)RgCharAlloc(sizeof(RgDropWeapon), RG_CHAR_TYPE_DROP_WEAPON);
    _InitDrop(pDrop, actor);
    return pDrop;
}
