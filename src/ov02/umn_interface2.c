/*
 * OV02 original TU 6: 0x00a02d68..0x00a03450 (5 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov02/umn_interface2", tskUmnBgCubeMain);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_interface2", UmnBgCubeInit);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_interface2", DrawUmnInterface2);

/*
 * State reached through the global UmnInterface2 pointer; only the two
 * frame counters UmnInterface2Main touches are evidenced.
 */
typedef struct UmnInterface2State {
    unsigned char unmodeled_00[7];
    unsigned char intensity_a; /* decremented by 8 each frame until zero */
    unsigned char unmodeled_08[3];
    unsigned char intensity_b; /* decremented by 2 each frame until zero */
} UmnInterface2State;

/* The font queue calls draw(context, arg) with the arg registered below;
 * the context record (read at +0x00 and +0x30) is not yet recovered. */
static void DrawUmnInterface2(void *context, void *arg);
extern void xglFontPrintExtFunc(unsigned int flags, void (*draw)(void *context, void *arg), void *arg);
extern UmnInterface2State *UmnInterface2;

void UmnInterface2Main(void) {
    if (UmnInterface2->intensity_a != 0) {
        UmnInterface2->intensity_a -= 8;
    }
    if (UmnInterface2->intensity_b != 0) {
        UmnInterface2->intensity_b -= 2;
    }
    if (UmnInterface2->intensity_a != 0 || UmnInterface2->intensity_b != 0) {
        xglFontPrintExtFunc(0x03FFFFF0, DrawUmnInterface2, UmnInterface2);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov02/umn_interface2", UmnInterface2Init);
