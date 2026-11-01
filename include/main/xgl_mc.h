#ifndef INCLUDE_MAIN_XGL_MC_H
#define INCLUDE_MAIN_XGL_MC_H

extern unsigned char SaveData[];

extern void xglMcSetMapName(const unsigned char *primary_euc_name,
                            const unsigned char *secondary_euc_name);

void xglMcWriteMapName(char *buffer, int slot);

#endif /* INCLUDE_MAIN_XGL_MC_H */
