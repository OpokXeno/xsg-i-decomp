#ifndef INCLUDE_OV12_RG_CHARMGR_H
#define INCLUDE_OV12_RG_CHARMGR_H

#include "shared.h"

/* Capacity of the manager's character table. */
#define RG_CHAR_MAX 256

/*
 * The character manager: up to RG_CHAR_MAX registered character pointers and
 * how many of them are in use (InstanceOfRgCharMgr allocates exactly
 * sizeof(RgCharMgr) bytes for it).
 */
struct RgCharMgr {
    RgChar *chars[RG_CHAR_MAX]; /* +0x000 */
    unsigned int count;         /* +0x400 */
};

/* Functions and data used by other translation units. */
RgChar *RgCharMgrEntry(RgCharMgr *manager, RgChar *pChar);
void RgCharMgrFree(RgCharMgr *manager, RgChar *pChar);
RgChar *RgCharMgrSearch(RgCharMgr *manager, int type);
int RgCharMgrIsFullOfBuffer(RgCharMgr *manager, int count);
void RgCharMgrGC(RgCharMgr *manager);
void RgCharMgrControl(RgCharMgr *manager);
void RgCharMgrPassTime(RgCharMgr *manager, float deltaTime);
void RgCharMgrDisp(RgCharMgr *manager);
void InitRgCharMgr(RgCharMgr *manager);
RgCharMgr *InstanceOfRgCharMgr(void);
RgCharMgr *InstanceOfRgCharMgrClear(void);
void RgCharMgrDump(RgCharMgr *manager);

#endif /* INCLUDE_OV12_RG_CHARMGR_H */
