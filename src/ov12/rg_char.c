/*
 * OV12 original TU 2: 0x00a01ff8..0x00a024f8 (15 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_char.h"
#include "ov12/rg_charmgr.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_rand_int.h"


/*
 * Defined here: the source file name and assertion text the scaffold data
 * window (ov12:0x00a51808, 0x00a51820) provides.
 */
const char charSourcePath[] = "../rg_char.euc.c";
const char charAssertCharNotNil[] = "pChar != NIL";

static void _nonControlMethod(RgChar *pChar)
{
}

static void _nonDispMethod(RgChar *pChar)
{
}

void RgCalcLocalForChar(RgMatrix dest_matrix, RgVector position,
                        RgVector facing, RgVector up_ref)
{
    RgVector neg_facing;

    XrgCopyVector(neg_facing, facing);
    XrgNegateVector(neg_facing, neg_facing);
    XrgCalcMatrixYtoZ(dest_matrix, neg_facing, up_ref);
    XrgCopyVectorXYZ(&dest_matrix[12], position);
}

RgChar *RgCharAlloc(unsigned int size, int type)
{
    RgChar *pChar;

    pChar = RgHeapAlloc(InstanceOfRgHeap(), size, charSourcePath, 49);
    InitRgChar(pChar, type);
    pChar->mgr = InstanceOfRgCharMgr();
    RgCharMgrEntry(InstanceOfRgCharMgr(), pChar);
    return pChar;
}

void RgCharFree(RgChar *pChar)
{
    if (pChar == 0) {
        assert_prog(charAssertCharNotNil, charSourcePath, 59);
    }

    if (pChar->mgr != 0) {
        /*
         * A manager-owned character is not freed here: mark it deleted and
         * let the manager's own garbage collection destroy and release it.
         */
        pChar->freedType = pChar->type;
        pChar->type = RG_CHAR_TYPE_DELETED;
        return;
    }

    if (pChar->destructMethod != 0) {
        pChar->destructMethod(pChar);
    }
    RgHeapFree(InstanceOfRgHeap(), pChar, charSourcePath, 76);
}

void InitRgChar(RgChar *pChar, int type)
{
    if (pChar == 0) {
        assert_prog(charAssertCharNotNil, charSourcePath, 86);
    }
    pChar->type = type;
    pChar->destructMethod = 0;
    pChar->controlMethod = _nonControlMethod;
    pChar->dispMethod = _nonDispMethod;
    pChar->passTimeMethod = 0;
    pChar->mgr = 0;
}

int RgCharGetType(RgChar *pChar)
{
    if (pChar == 0) {
        assert_prog(charAssertCharNotNil, charSourcePath, 104);
    }
    return pChar->type;
}

void RgCharSetType(RgChar *pChar, int type)
{
    if (pChar == 0) {
        assert_prog(charAssertCharNotNil, charSourcePath, 114);
    }
    pChar->type = type;
}

void RgCharControlMethod(RgChar *pChar, RgCharControlFunc controlMethod)
{
    if (pChar == 0) {
        assert_prog(charAssertCharNotNil, charSourcePath, 125);
    }
    pChar->controlMethod = controlMethod;
}

void RgCharDispMethod(RgChar *pChar, RgCharDispFunc dispMethod)
{
    if (pChar == 0) {
        assert_prog(charAssertCharNotNil, charSourcePath, 133);
    }
    pChar->dispMethod = dispMethod;
}

void RgCharDestructMethod(RgChar *pChar, RgCharDestructFunc destructMethod)
{
    if (pChar == 0) {
        assert_prog(charAssertCharNotNil, charSourcePath, 141);
    }
    pChar->destructMethod = destructMethod;
}

void RgCharPassTimeMethod(RgChar *pChar, RgCharPassTimeFunc passTimeMethod)
{
    if (pChar == 0) {
        assert_prog(charAssertCharNotNil, charSourcePath, 149);
    }
    pChar->passTimeMethod = passTimeMethod;
}

void RgCharControl(RgChar *pChar)
{
    if (pChar == 0) {
        assert_prog(charAssertCharNotNil, charSourcePath, 158);
    }
    if (pChar->type != RG_CHAR_TYPE_DELETED) {
        if (pChar->controlMethod != 0) {
            pChar->controlMethod(pChar);
        }
    }
}

void RgCharPassTime(RgChar *pChar, float deltaTime)
{
    if (pChar == 0) {
        assert_prog(charAssertCharNotNil, charSourcePath, 168);
    }
    if (pChar->type != RG_CHAR_TYPE_DELETED) {
        if (pChar->passTimeMethod != 0) {
            pChar->passTimeMethod(pChar, deltaTime);
        }
    }
}

void RgCharDisp(RgChar *pChar)
{
    if (pChar == 0) {
        assert_prog(charAssertCharNotNil, charSourcePath, 184);
    }
    if (pChar->type != RG_CHAR_TYPE_DELETED) {
        if (pChar->dispMethod != 0) {
            pChar->dispMethod(pChar);
        }
    }
}
