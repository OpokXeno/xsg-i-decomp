/*
 * OV10 original TU 7: 0x00a21348..0x00a21768 (3 functions)
 */
#include "common.h"
#include "shared.h"

extern char *strcat(char *destination, const char *source);
extern char *strcpy(char *destination, const char *source);
extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);

/* "data\carddata\tuto\" */
extern char datapath[0x14];
/* Tutorial texture filenames, one per tutorial index. */
extern char *tutoxtx_tbl[];
/* ".xtx" */
extern const char D_00A4E498[];
/* The tutorial texture buffer xglCdReadFile loads the file into. */
extern void *tuxtx;

void CardTutorialFileLoad(int tutorialIndex)
{
    char path[0x80];

    memset(path, 0, sizeof(path));
    strcpy(path, datapath);
    strcat(path, tutoxtx_tbl[tutorialIndex]);
    strcat(path, D_00A4E498);
    xglCdReadFile(path, tuxtx, 0, 1);
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_tutorial", CardTutorialInit);

INCLUDE_ASM("asm/nonmatchings/ov10/card_tutorial", CardTutorialProc);
