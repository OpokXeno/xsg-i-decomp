/*
 * TU-local declarations of ov12/tu021 (src/ov12/rg_charmgr.c).
 */

#ifndef SRC_OV12_RG_CHARMGR_H
#define SRC_OV12_RG_CHARMGR_H

typedef struct RgCharMgr RgCharMgr;

RgCharMgr *InstanceOfRgCharMgrClear(void);

extern RgCharMgr *InstanceOfRgCharMgr(void);

extern void _DestructCharMgr(RgCharMgr *manager);

#endif /* SRC_OV12_RG_CHARMGR_H */
