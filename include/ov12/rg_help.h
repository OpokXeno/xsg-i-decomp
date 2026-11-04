#ifndef INCLUDE_OV12_RG_HELP_H
#define INCLUDE_OV12_RG_HELP_H

/* The help screen object; its layout is private to src/ov12/rg_help.c. */
typedef struct RgHelp RgHelp;

/* Functions and data used by other translation units. */
RgHelp *CreateRgHelp(void);
void DisposeRgHelp(RgHelp *pHelp);
int RgHelpIsEnd(RgHelp *pHelp);
void RgHelpPassTime(RgHelp *pHelp, float deltaTime);
void RgHelpDisp(RgHelp *pHelp);

#endif /* INCLUDE_OV12_RG_HELP_H */
