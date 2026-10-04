#include "common.h"
#include "umn_procurator.h"

INCLUDE_ASM("asm/nonmatchings/ov02/task_umn_top_win", taskUmnTopWin);

INCLUDE_ASM("asm/nonmatchings/ov02/task_umn_top_win", UmnTopMenu);

short *UmnManzaiText = 0;
unsigned int UmnManzaiFlag = 0;
UmnManzaiWinTask UmnManzaiWin[2] = {{0}, {0}};
