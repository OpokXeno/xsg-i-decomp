#ifndef INCLUDE_OV12_RG_CHAR_H
#define INCLUDE_OV12_RG_CHAR_H

#include "shared.h"

typedef void (*RgCharControlFunc)(RgChar *pChar);

typedef void (*RgCharDispFunc)(RgChar *pChar);

typedef void (*RgCharDestructFunc)(RgChar *pChar);

typedef void (*RgCharPassTimeFunc)(RgChar *pChar, float deltaTime);

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

void RgCharDispMethod(RgChar *pChar, RgCharDispFunc dispMethod);

void RgCharDestructMethod(RgChar *pChar, RgCharDestructFunc destructMethod);

void RgCharPassTimeMethod(RgChar *pChar, RgCharPassTimeFunc passTimeMethod);

#endif /* INCLUDE_OV12_RG_CHAR_H */
