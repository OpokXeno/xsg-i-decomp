#include "common.h"

extern unsigned short MenuTopCommandLock;

int MenuTopCommandCheck(int commandId)
{
    unsigned int commandBit = 1 << (commandId - 1);
    return (commandBit & ~MenuTopCommandLock) != 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", TopStatusWinMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", TopInfoMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", TopMenuWinMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", TopFaceExWinMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", TopMenu);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", tskMenuTai);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", tskMenuTaiPointa);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", MenuBackModelSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", MenuStatusDisp);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", MenuInfoWindow);

INCLUDE_ASM("asm/main/nonmatchings/menu_top_command_check", MenuSelectWindow);

void MenuPasWindow(void) {

}
