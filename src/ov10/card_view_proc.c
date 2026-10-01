/*
 * OV10 original TU 5: 0x00a1c1b8..0x00a1e5d8 (5 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov10/card_view_proc", CardViewProc);

INCLUDE_ASM("asm/nonmatchings/ov10/card_view_proc", COPCardDispSub);

int COPSerachPackR(unsigned char *pack_occupied, int current_position)
{
    int found_position = -1;
    int scan_position;

    for (scan_position = current_position + 1; scan_position < 20; scan_position += 1) {
        if (pack_occupied[scan_position] != 0) {
            found_position = scan_position;
            break;
        }
    }

    if (found_position < 0) {
        for (scan_position = 0; scan_position < current_position - 1; scan_position += 1) {
            if (pack_occupied[scan_position] != 0) {
                found_position = scan_position;
                break;
            }
        }
        if (found_position < 0) {
            found_position = current_position;
        }
    }

    return found_position;
}

int COPSerachPackL(unsigned char *pack_occupied, int current_position)
{
    int found_position = -1;
    int scan_position;

    for (scan_position = current_position - 1; scan_position >= 0; scan_position -= 1) {
        if (pack_occupied[scan_position] != 0) {
            found_position = scan_position;
            break;
        }
    }

    if (found_position < 0) {
        for (scan_position = 19; scan_position > current_position; scan_position -= 1) {
            if (pack_occupied[scan_position] != 0) {
                found_position = scan_position;
                break;
            }
        }
        if (found_position < 0) {
            found_position = current_position;
        }
    }

    return found_position;
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_view_proc", CardOpenProc);
