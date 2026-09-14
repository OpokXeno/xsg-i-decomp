#include "common.h"
#include "call_java_method.h"

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", Call_JavaMethod);

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", EventCheck_Line_Button);

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", EventCheck_Line_Touch);

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", EventCheck_Circle_Button);

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", EventCheck_Square_Button_Center);

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", EventCheck_Square_Button);

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", EventCheck_Square_Touch);

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", Check_Locater);

char Get_LocaterType(short locator_index)
{
    int offset;
    LayoutHeader *locator_header;
    float angle;
    short type;

    offset = locator_index * 2;
    locator_header = UnduDataGetHeader(0x207, offset);
    UnduDataGetHeader(0x207, offset + 1);
    angle = locator_header->components[3];
    type = 0;
    while (type < 8) {
        if (LocaterAngle[type * 2] < angle) {
            if (angle < LocaterAngle[type * 2 + 1]) {
                break;
            }
        }
        type += 1;
    }
    return type == 8 ? -1 : (char)type;
}

char Get_LocaterType_Angle(float angle)
{
    short type;

    type = 0;
    while (type < 8) {
        if (LocaterAngle[type * 2] < angle) {
            if (angle < LocaterAngle[type * 2 + 1]) {
                break;
            }
        }
        type += 1;
    }
    return type == 8 ? -1 : (char)type;
}

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", Check_InsideID);
