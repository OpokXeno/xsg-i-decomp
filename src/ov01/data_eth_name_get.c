/*
 * OV01 original TU 11: 0x00a2c5a8..0x00a2c9b8 (13 functions)
 */
#include "common.h"

extern int printf(const char *format, ...);
extern const char D_00A4DFA8[];

/*
 * Sibling of the dataItmNameGet/dataWepNameGet/dataBltNameGet/dataAccNameGet
 * family this TU also defines (forward-declared, with this same
 * `const char **` return, in src/ov01/battle_init.c): each entry's first
 * member is the display name string this family returns the address of.
 * ethNameTbl is 0x3B4 bytes for a stride of 0xC, i.e. 79 entries.
 */
typedef struct EthName {
    const char *name;
    unsigned char unmodeled_04[8];
} EthName;

extern EthName ethNameTbl[79];

const char **dataEthNameGet(int ethId)
{
    if (ethId <= 0)
    {
        printf(D_00A4DFA8, ethId);
        return 0;
    }
    return &ethNameTbl[ethId - 1].name;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataItmNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataSklNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataAccNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataWepNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataBltNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataNrmNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataSpcNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataEnemyNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataStatNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataPlayerNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataEngNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_eth_name_get", dataFrmNameGet);
