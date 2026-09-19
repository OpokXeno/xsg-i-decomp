/*
 * TU-local declarations of ov12/tu002 (src/ov12/rg_char.c).
 */

#ifndef SRC_OV12_RG_CHAR_H
#define SRC_OV12_RG_CHAR_H

#include "shared.h"
#include "rg_charmgr.h"

void RgCalcLocalForChar(RgMatrix dest_matrix, RgVector position,
                        RgVector facing, RgVector up_ref);

extern void XrgNegateVector(RgVector destination, RgVector source);

extern void XrgCalcMatrixYtoZ(RgMatrix destination, RgVector first,
                              RgVector second);

extern void XrgCopyVectorXYZ(RgVector destination, RgVector source);

typedef struct RgChar RgChar;

typedef void (*RgCharControlFunc)(RgChar *pChar);
typedef void (*RgCharDispFunc)(RgChar *pChar);
typedef void (*RgCharDestructFunc)(RgChar *pChar);
typedef void (*RgCharPassTimeFunc)(RgChar *pChar, float deltaTime);

/*
 * The type value RgCharFree stores at offset 0x00 for a manager-owned
 * character instead of destroying it immediately. RgCharMgrGC reads offset
 * 0x00 back against this value to find characters its garbage collection may
 * destroy.
 */
#define RG_CHAR_TYPE_DELETED (-1)

/*
 * The observed access view: type at 0x00, the type value RgCharFree saves to
 * 0x04 when it marks a manager-owned character deleted, the four
 * per-instance method callbacks at 0x08-0x14, and the owning character
 * manager at 0x18. InitRgChar (this TU's own unrecovered function) writes
 * 0x00, 0x08, 0x0C, 0x10, 0x14 and 0x18; offset 0x04 is written only by
 * RgCharFree. No size or member beyond 0x18 is claimed.
 */
struct RgChar {
    int type;
    int freedType;
    RgCharControlFunc controlMethod;
    RgCharDispFunc dispMethod;
    RgCharDestructFunc destructMethod;
    RgCharPassTimeFunc passTimeMethod;
    RgCharMgr *mgr;
};

RgChar *RgCharAlloc(unsigned int size, int type);
void RgCharFree(RgChar *pChar);
int RgCharGetType(RgChar *pChar);
void RgCharSetType(RgChar *pChar, int type);
void RgCharControlMethod(RgChar *pChar, RgCharControlFunc controlMethod);
void RgCharDispMethod(RgChar *pChar, RgCharDispFunc dispMethod);
void RgCharDestructMethod(RgChar *pChar, RgCharDestructFunc destructMethod);
void RgCharPassTimeMethod(RgChar *pChar, RgCharPassTimeFunc passTimeMethod);
void RgCharControl(RgChar *pChar);
void RgCharPassTime(RgChar *pChar, float deltaTime);
void RgCharDisp(RgChar *pChar);

#endif /* SRC_OV12_RG_CHAR_H */
