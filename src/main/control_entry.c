#include "common.h"

extern int main_param_argc;
extern int main_param_argv;

extern void BootDisplay(void);
extern void xglThreadInitial(void);
extern void xglThreadRotate(void);

/*
 * InitializeSystem is file-local (LOCAL in the original symbol table) and is
 * still asm, so it is declared static before its use in main below.
 */
static void InitializeSystem(void);

INCLUDE_ASM("asm/main/nonmatchings/control_entry", ControlEntry);

INCLUDE_ASM("asm/main/nonmatchings/control_entry", InitializeSystem);

/*
 * The compiler emits the call to __main (running static/global constructors)
 * at entry to any function literally named main; that call precedes the
 * statements below and needs no explicit source-level call.
 */
int main(int argc, int argv)
{
    main_param_argc = argc;
    main_param_argv = argv;
    BootDisplay();
    InitializeSystem();
    xglThreadInitial();
    xglThreadRotate();
    return 0;
}
