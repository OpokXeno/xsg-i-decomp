#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherDataGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherJoutoCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherCharPointCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherWhoCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherSetCheck);

/*
 * The retail ELF names these ov01 data-table accessors (still assembler in
 * their own TU, src/ov01/data_unit_org_get.c) dataEthGet (0x00a1a488) and
 * dataTecGet (0x00a1a378); until that TU recovers them, the synthetic
 * linker script only binds their scaffold func_<VA> spelling.
 */
typedef struct EtherStatus {
    unsigned char unmodeled_00[6];
    short tecId; /* +6: technique dataTecGet resolves for this ether */
} EtherStatus;

typedef struct TecStatus {
    unsigned char unmodeled_00[4];
    signed char type; /* +4 */
} TecStatus;

extern EtherStatus *func_A1A488(unsigned short etherId); /* dataEthGet */
extern TecStatus *func_A1A378(short tecId);               /* dataTecGet */

signed char MenuEtherTypeGet(int etherId) {
    return func_A1A378(func_A1A488(etherId)->tecId)->type;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherUseCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherCapSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherPasMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherInfoMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherMenuMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherExMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherL1R1Main);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherStatusMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherStatusMain2);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherListMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherListMain2);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherListMake00);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherListMake01);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherListMake02);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherListMake03);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherEquip);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEther);
