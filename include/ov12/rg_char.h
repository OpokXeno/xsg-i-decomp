#ifndef INCLUDE_OV12_RG_CHAR_H
#define INCLUDE_OV12_RG_CHAR_H

#include "shared.h"

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
 * Common header of every character: type at 0x00, the type value RgCharFree
 * saves to 0x04 when it marks a manager-owned character deleted, the four
 * per-instance method callbacks at 0x08-0x14, and the owning character
 * manager at 0x18. Derived characters (RgBgObj) start their own fields
 * right after it.
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

/* Functions and data used by other translation units. */
RgChar *RgCharAlloc(unsigned int size, int type);
void RgCharFree(RgChar *pChar);
void InitRgChar(RgChar *pChar, int type);
int RgCharGetType(RgChar *pChar);
void RgCharSetType(RgChar *pChar, int type);
void RgCharControlMethod(RgChar *pChar, RgCharControlFunc controlMethod);
void RgCharDispMethod(RgChar *pChar, RgCharDispFunc dispMethod);
void RgCharDestructMethod(RgChar *pChar, RgCharDestructFunc destructMethod);
void RgCharPassTimeMethod(RgChar *pChar, RgCharPassTimeFunc passTimeMethod);
void RgCharControl(RgChar *pChar);
void RgCharPassTime(RgChar *pChar, float deltaTime);
void RgCharDisp(RgChar *pChar);
void RgCalcLocalForChar(RgMatrix dest_matrix, RgVector position,
                        RgVector facing, RgVector up_ref);

#endif /* INCLUDE_OV12_RG_CHAR_H */
