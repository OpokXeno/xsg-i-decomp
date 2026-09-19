/*
 * OV12 original TU 2: 0x00a01ff8..0x00a024f8 (15 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_char.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(void *heap, void *pointer, const char *source_file,
                       int line);
extern RgCharMgr *InstanceOfRgCharMgr(void);
extern void RgCharMgrEntry(RgCharMgr *manager, RgChar *pChar);
extern void InitRgChar(RgChar *pChar, int type);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a51808 contains the source filename "../rg_char.euc.c".
 * ov12:0x00a51820 contains the assertion expression "pChar != NIL".
 */
extern const char D_00A51808[];
extern const char D_00A51820[];

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

    pChar = RgHeapAlloc(InstanceOfRgHeap(), size, D_00A51808, 49);
    InitRgChar(pChar, type);
    pChar->mgr = InstanceOfRgCharMgr();
    RgCharMgrEntry(InstanceOfRgCharMgr(), pChar);
    return pChar;
}

void RgCharFree(RgChar *pChar)
{
    if (pChar == 0) {
        assert_prog(D_00A51820, D_00A51808, 59);
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
    RgHeapFree(InstanceOfRgHeap(), pChar, D_00A51808, 76);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_char", InitRgChar);

int RgCharGetType(RgChar *pChar)
{
    if (pChar == 0) {
        assert_prog(D_00A51820, D_00A51808, 104);
    }
    return pChar->type;
}

void RgCharSetType(RgChar *pChar, int type)
{
    if (pChar == 0) {
        assert_prog(D_00A51820, D_00A51808, 114);
    }
    pChar->type = type;
}

void RgCharControlMethod(RgChar *pChar, RgCharControlFunc controlMethod)
{
    if (pChar == 0) {
        assert_prog(D_00A51820, D_00A51808, 125);
    }
    pChar->controlMethod = controlMethod;
}

void RgCharDispMethod(RgChar *pChar, RgCharDispFunc dispMethod)
{
    if (pChar == 0) {
        assert_prog(D_00A51820, D_00A51808, 133);
    }
    pChar->dispMethod = dispMethod;
}

void RgCharDestructMethod(RgChar *pChar, RgCharDestructFunc destructMethod)
{
    if (pChar == 0) {
        assert_prog(D_00A51820, D_00A51808, 141);
    }
    pChar->destructMethod = destructMethod;
}

void RgCharPassTimeMethod(RgChar *pChar, RgCharPassTimeFunc passTimeMethod)
{
    if (pChar == 0) {
        assert_prog(D_00A51820, D_00A51808, 149);
    }
    pChar->passTimeMethod = passTimeMethod;
}

void RgCharControl(RgChar *pChar)
{
    if (pChar == 0) {
        assert_prog(D_00A51820, D_00A51808, 158);
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
        assert_prog(D_00A51820, D_00A51808, 168);
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
        assert_prog(D_00A51820, D_00A51808, 184);
    }
    if (pChar->type != RG_CHAR_TYPE_DELETED) {
        if (pChar->dispMethod != 0) {
            pChar->dispMethod(pChar);
        }
    }
}
