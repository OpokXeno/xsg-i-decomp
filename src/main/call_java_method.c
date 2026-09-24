#include "common.h"
#include "shared.h"
#include "call_java_method.h"

INCLUDE_ASM("asm/main/nonmatchings/call_java_method", Call_JavaMethod);

/* Call_JavaMethod (defined above in this TU, still assembler) takes actor
 * plus the method id, the same two arguments its own body proves for
 * EventCheck_Line_Touch below (it dereferences actor at +0x80 for the
 * enepc-table index and takes the method id in $a1). */
extern void Call_JavaMethod(void *actor, int method_id);

/* Check_InsideID (0x002d67e8, still assembler in this TU) takes actor plus
 * the two UnduDataGetHeader results, as Check_Locater passes them; this
 * handler forwards those same three arguments untouched. */
extern int Check_InsideID(void *actor, LayoutHeader *first, LayoutHeader *second);

/* PadData's pressed-button halfword (include/shared.h PadPrefix.half_2a);
 * bit 0x20 gates every EventCheck_*_Button handler below, the same bit
 * src/ov01/debug_entry.c tests as PadData.half_2a & 0x20. */
extern PadPrefix PadData;

void EventCheck_Line_Button(void *actor, LayoutHeader *first, LayoutHeader *second) {
    if ((PadData.half_2a & 0x20) && (Check_InsideID(actor, first, second) != 0)) {
        Call_JavaMethod(actor, 1);
    }
}

/* Call_JavaMethod is defined above in this TU (still assembler); both
 * arguments are proven by its own body: it dereferences actor at +0x80 for
 * the enepc-table index and takes the method id in $a1 ($a2, reused there as
 * a local constant, is never supplied by any caller in this TU). */
extern void Call_JavaMethod(void *actor, int method_id);

/* Check_InsideID (0x002d67e8, still assembler in this TU) takes actor plus
 * the two UnduDataGetHeader results, as Check_Locater passes them. The touch
 * handlers receive those same three arguments and forward them untouched
 * (whole-program argument liveness: all three are read on entry). */
extern int Check_InsideID(void *actor, LayoutHeader *first, LayoutHeader *second);

void EventCheck_Line_Touch(void *actor, LayoutHeader *first, LayoutHeader *second) {
    if (Check_InsideID(actor, first, second) != 0) {
        Call_JavaMethod(actor, 2);
    }
}

void EventCheck_Circle_Button(void *actor, LayoutHeader *first, LayoutHeader *second) {
    if ((PadData.half_2a & 0x20) && (Check_InsideID(actor, first, second) != 0)) {
        Call_JavaMethod(actor, 3);
    }
}

/*
 * This TU's own reading of the engine's actor record: only the +0x00..+0x70
 * common head (the six Vector4 members position/previous_position/velocity/
 * acceleration/rotation/scale after flags/update/draw/quadword_alignment_gap)
 * plus the one field EventCheck_Square_Button_Center below reads past it.
 * src/main/chr.h, src/main/act_2.h, src/main/enemy_2.h, src/main/near_dir.h
 * and src/main/set_motion.h each restate the same common head independently
 * as their own TU-local view, under their own tag.
 */
typedef struct EventLocatorActor {
    u32 flags;
    void (*update)(struct EventLocatorActor *actor);
    void (*draw)(struct EventLocatorActor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_70[0x9e8 - 0x70];
    float interaction_radius; /* +0x9e8: EventCheck_Square_Button_Center's
                                  Check_InsideFan radius argument, alongside
                                  rotation.y as the facing angle and the
                                  literal 50.0/60.0 degree bounds. */
} EventLocatorActor;

/* Check_InsideFan (0x002d8240, defined in a different TU, still assembler)
 * is called here with &actor->position, the incoming first locator header
 * (passed through unmodified in $a1), -1, and four floats: actor's
 * rotation.y, the literal 50.0/60.0 degree bounds and actor's +0x9e8 field. */
extern int Check_InsideFan(Vector4 *origin, LayoutHeader *target, int flag,
                            float facing, float minDegrees, float maxDegrees,
                            float radius);

void EventCheck_Square_Button_Center(EventLocatorActor *actor, LayoutHeader *first, LayoutHeader *second) {
    if ((PadData.half_2a & 0x20)
        && (Check_InsideFan(&actor->position, first, -1, actor->rotation.y,
                             50.0f, 60.0f, actor->interaction_radius) != 0)
        && (Check_InsideID(actor, first, second) != 0)) {
        Call_JavaMethod(actor, 4);
    }
}

void EventCheck_Square_Button(void *actor, LayoutHeader *first, LayoutHeader *second) {
    if ((PadData.half_2a & 0x20) && (Check_InsideID(actor, first, second) != 0)) {
        Call_JavaMethod(actor, 5);
    }
}

void EventCheck_Square_Touch(void *actor, LayoutHeader *first, LayoutHeader *second) {
    if (Check_InsideID(actor, first, second) != 0) {
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
