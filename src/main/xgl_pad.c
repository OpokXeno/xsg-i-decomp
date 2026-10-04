#include "common.h"
#include "xgl_pad.h"

static unsigned char PadDmaBuffer[0x200];

void xglPadInitial(void) {
    unsigned int byte_index;
    int map_remaining;
    int dead_zone_remaining;
    unsigned char axis_threshold;
    int button;
    unsigned char *first_record_bytes;
    unsigned char *second_record_bytes;

    scePadInit(0);
    scePadPortOpen(0, 0, PadDmaBuffer);
    scePadPortOpen(1, 0, &PadDmaBuffer[0x100]);

    first_record_bytes = (unsigned char *)&PadData[0];
    second_record_bytes = (unsigned char *)&PadData[1];
    /* Keep the original paired byte walk across the two 0x68-byte records. */
    byte_index = 0;
    do {
        *first_record_bytes = 0;
        *second_record_bytes = 0;
        ++first_record_bytes;
        ++second_record_bytes;
        ++byte_index;
    } while (byte_index < sizeof(XglPadRecord));

    xglPadSetRepeat(0, PAD_REPEAT_MASK, PAD_REPEAT_DELAY, PAD_REPEAT_INTERVAL);
    xglPadSetRepeat(1, PAD_REPEAT_MASK, PAD_REPEAT_DELAY, PAD_REPEAT_INTERVAL);

    button = 1;
    map_remaining = sizeof(PadData[0].button_map) - 1;
    do {
        PadData[0].button_map[sizeof(PadData[0].button_map) - 1 - map_remaining] = button;
        PadData[1].button_map[sizeof(PadData[1].button_map) - 1 - map_remaining] = button;
        button <<= 1;
        --map_remaining;
    } while (map_remaining >= 0);

    PadData[0].horizontal_dead_zone = PAD_DEFAULT_DEAD_ZONE;
    PadData[0].vertical_dead_zone = PAD_DEFAULT_DEAD_ZONE;
    PadData[1].horizontal_dead_zone = PAD_DEFAULT_DEAD_ZONE;
    PadData[1].vertical_dead_zone = PAD_DEFAULT_DEAD_ZONE;
    axis_threshold = PAD_AXIS_THRESHOLD;
    dead_zone_remaining = sizeof(PadData[0].axis_dead_zone) - 1;
    do {
        PadData[0].axis_dead_zone[sizeof(PadData[0].axis_dead_zone) - 1 - dead_zone_remaining] = axis_threshold;
        PadData[1].axis_dead_zone[sizeof(PadData[1].axis_dead_zone) - 1 - dead_zone_remaining] = axis_threshold;
        --dead_zone_remaining;
    } while (dead_zone_remaining >= 0);
}

void xglPadSetRepeat(int port, unsigned short mask, int delay, int interval)
{
    unsigned short button_mask = mask;
    XglPadRecord *pad;

    if (port == -1) {
        xglPadSetRepeat(0, button_mask, delay, interval);
        xglPadSetRepeat(1, button_mask, delay, interval);
        return;
    }

    pad = &PadData[port];
    pad->repeat_delay = delay;
    pad->repeat_interval = interval;
    pad->repeat_mask = button_mask;
    pad->repeat_wait = 0;
    pad->repeat_count = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_pad", xglPadRead);
