#include "common.h"

struct LadderAttributeTable {
    int attributes[16];
};

struct ArrivalAttributeTable {
    int attributes[4];
};

const struct LadderAttributeTable D_004CAC20 = {{
    0x00100000, 0x00110000, 0x00120000, 0x00130000,
    0x00140000, 0x00150000, 0x00160000, 0x00170000,
    0x00180000, 0x00190000, 0x001A0000, 0x001B0000,
    0x001C0000, 0x001D0000, 0x001E0000, 0x001F0000
}};
const struct ArrivalAttributeTable D_004CAC60 = {{
    0x00200000, 0x00210000, 0x00220000, 0x00230000
}};
short PhCunt = 0;

short Get_Ladder(int attribute)
{
    struct LadderAttributeTable table = D_004CAC20;
    short index = 0;

    do {
        if (attribute == table.attributes[index]) {
            return index;
        }

        index++;
    } while (index < 16);

    return -1;
}

short Get_Arrival(int attribute)
{
    struct ArrivalAttributeTable table = D_004CAC60;
    short index = 0;

    do {
        if (attribute == table.attributes[index]) {
            return index;
        }

        index++;
    } while (index < 4);

    return -1;
}

INCLUDE_ASM("asm/main/nonmatchings/get_ladder", Get_LadderID_Limit);

extern int Get_Attr_NU(const float *position, int mapIndex, int attrMask);

int Get_ArrivalID_First(const float *start, float *position,
                        float xStep, float zStep)
{
    short index = 0;
    int attribute;

    position[0] = start[0];
    position[1] = start[1];
    position[2] = start[2];
    position[3] = start[3];

    for (;;) {
        if (index >= 256) {
            break;
        }

        attribute = Get_Attr_NU(position, 0, 0);

        if (Get_Arrival(attribute) != -1) {
            break;
        }

        position[0] += xStep;
        position[2] += zStep;
        index++;
    }

    return index != 256;
}

INCLUDE_ASM("asm/main/nonmatchings/get_ladder", Fly_to_Ladder);

INCLUDE_ASM("asm/main/nonmatchings/get_ladder", Get_Off_Ladder);

INCLUDE_ASM("asm/main/nonmatchings/get_ladder", Ready_Ladder);

INCLUDE_ASM("asm/main/nonmatchings/get_ladder", On_Ladder);

INCLUDE_ASM("asm/main/nonmatchings/get_ladder", Ladder_Main);
