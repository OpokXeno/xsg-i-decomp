/*
 * TU-local declarations of main/tu093 (src/main/xgl_mc.c).
 */

#ifndef SRC_MAIN_XGL_MC_H
#define SRC_MAIN_XGL_MC_H

int xglMcGetState(void);

extern unsigned char mw[];

extern void xglMcReset(void);

extern unsigned char queue_top;

extern unsigned char queue_end;

extern void xglMcSetMapName(const unsigned char *primary_euc_name,
                            const unsigned char *secondary_euc_name);

void xglMcWriteMapName(char *buffer, int slot);

void xglMcInitial(void);

extern void sceMcInit(void);

#endif /* SRC_MAIN_XGL_MC_H */
