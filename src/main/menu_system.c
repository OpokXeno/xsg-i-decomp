#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/menu_system", MenuSystemPasMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_system", MenuSystemInfoMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_system", MenuSystemMenuMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_system", MenuSystemInitSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_system", subMenuSystemMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_system", subMenuSystemInit);

INCLUDE_ASM("asm/main/nonmatchings/menu_system", MenuSystem);

extern void endPrintInit(void);
extern void subMenuSystemInit(int workEnd);
extern int MainMenuWorkEnd;

void MenuSystem2Init(void) {
    endPrintInit();
    subMenuSystemInit(MainMenuWorkEnd);
}

INCLUDE_ASM("asm/main/nonmatchings/menu_system", MenuSystem2);
