#include "common.h"
#include "call_java_method.h"

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", Call_JavaMethod);

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", EventCheck_Line_Button);

/* Call_JavaMethod is defined above in this TU (still assembler); both
 * arguments are proven by its own body: it dereferences actor at +0x80 for
 * the enepc-table index and takes the method id in $a1 ($a2, reused there as
 * a local constant, is never supplied by any caller in this TU). */
extern void Call_JavaMethod(void *actor, int method_id);

/* Check_InsideID (0x002d67e8, still assembler in this TU) is defined with
 * three parameters (Check_Locater passes actor plus the two UnduDataGetHeader
 * results), but these two call sites pass only actor: an old-style,
 * unspecified-argument declaration reproduces exactly that call without
 * overclaiming the arity used elsewhere in the TU. */
extern int Check_InsideID();

void EventCheck_Line_Touch(void *actor) {
    if (Check_InsideID(actor) != 0) {
        Call_JavaMethod(actor, 2);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", EventCheck_Circle_Button);

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", EventCheck_Square_Button_Center);

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", EventCheck_Square_Button);

void EventCheck_Square_Touch(void *actor) {
    if (Check_InsideID(actor) != 0) {
        Call_JavaMethod(actor, 6);
    }
}

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
