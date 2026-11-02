#ifndef INCLUDE_MAIN_XGL_MC_H
#define INCLUDE_MAIN_XGL_MC_H

extern unsigned char SaveData[];

extern void xglMcSetMapName(const unsigned char *primary_euc_name,
                            const unsigned char *secondary_euc_name);

void xglMcWriteMapName(char *buffer, int slot);

/* Functions and data used by other translation units. */
void xglMcInitial(void);
int xglMcMain(void);
char *xglMcSetFullPath(int card, int selector);

#endif /* INCLUDE_MAIN_XGL_MC_H */
