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

extern void subMenuSystemMain(void);
extern void endPrintExtFunc(int kind, int id, void *data);

/*
 * The system-menu work block MenuSystem2 polls (window_tex_load's MenuWork
 * data, main 0x0036c180); only the completion byte at +0x11 is modelled
 * here.
 */
typedef struct MenuSystemWork {
    unsigned char unmodeled_00[0x11];
    unsigned char state;            /* +0x11 */
} MenuSystemWork;

extern MenuSystemWork MenuWork;

/*
 * Drives the system-options screen for one frame and reports whether it is
 * still running: MenuWork.state is 0xFF once the screen driven by
 * subMenuSystemMain has finished.
 */
int MenuSystem2(void)
{
    subMenuSystemMain();
    endPrintExtFunc(0, 100, 0);
    return MenuWork.state != 0xFF;
}
