#include "common.h"
#include "shared.h"
#include "tslider_create.h"

INCLUDE_ASM("asm/main/nonmatchings/tslider_create", TSLIDER_create);

INCLUDE_ASM("asm/main/nonmatchings/tslider_create", TSLIDER_init);

INCLUDE_ASM("asm/main/nonmatchings/tslider_create", TSLIDER_updateDefault);

INCLUDE_ASM("asm/main/nonmatchings/tslider_create", TSLIDER_drawDefault);

INCLUDE_ASM("asm/main/nonmatchings/tslider_create", TMENU_addQuery2);

INCLUDE_ASM("asm/main/nonmatchings/tslider_create", TWIN_initCF);

INCLUDE_ASM("asm/main/nonmatchings/tslider_create", TWIN_initScene);

extern const char D_004C1D18[]; /* "/[waitkey(0);close()]" */

void *createItemGetWin(const char *text)
{
    TwinWindow *window;
    int length;

    window = TWIN_create2(-1);
    length = strlen(text);
    window->kind = 2;
    window->param = length + 2;
    TWIN_initCF(window);
    MSG_print2(window, text, length);
    MSG_print2(window, D_004C1D18, -1);
    return window->body;
}
