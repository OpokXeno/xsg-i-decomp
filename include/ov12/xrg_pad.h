#ifndef INCLUDE_OV12_XRG_PAD_H
#define INCLUDE_OV12_XRG_PAD_H

/* Functions and data used by other translation units. */
void XrgPadSetID(int pad_id);
int XrgPadIsUp(void);
int XrgPadIsDown(void);
int XrgPadIsLeft(void);
int XrgPadIsRight(void);
int XrgPadIsMaru(void);
int XrgPadIsBatu(void);
int XrgPadIsSankaku(void);
int XrgPadIsShikaku(void);
int XrgPadIsL12(void);
int XrgPadIsR12(void);
int XrgPadIsStart(void);
int XrgPadIsSelect(void);
int XrgPadIsSelectLevelLR(void);
int XrgPadIsLR(void);

#endif /* INCLUDE_OV12_XRG_PAD_H */
