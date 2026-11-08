#include "common.h"
#include "shared.h"
#include "xgl_pad.h"
#include "main/xgl_pad.h"

/* Controller setup state machine (XglPadRecord.setup_state); a pad that is
 * disconnected restarts at PAD_SETUP_PROBE. */
#define PAD_SETUP_PROBE 0
#define PAD_SETUP_QUERY_ACTUATORS_MODE 0x40
#define PAD_SETUP_SET_MAIN_MODE 0x41
#define PAD_SETUP_WAIT_MAIN_MODE 0x42
#define PAD_SETUP_ACTUATOR_ALIGN 0x70
#define PAD_SETUP_WAIT_ALIGN 0x71
#define PAD_SETUP_PRESSURE_INFO 0x80
#define PAD_SETUP_PRESSURE_ENTER 0x81
#define PAD_SETUP_WAIT_PRESSURE 0x82
#define PAD_SETUP_READY 0xff

/* scePadGetState results and the scePadInfoMode terminal ids. */
#define PAD_STATE_STABLE 6
#define PAD_STATE_FINDCTP1 2
#define PAD_STATE_EXECCMD 5
#define PAD_TERM_ID_DIGITAL 4
#define PAD_TERM_ID_DUALSHOCK 7

/* High nibble of packet[1]: the controller type. */
#define PAD_TYPE_MASK 0xf0
#define PAD_TYPE_MOUSE 0x10
#define PAD_TYPE_NEGCON 0x20
#define PAD_TYPE_DIGITAL 0x40
#define PAD_TYPE_ANALOG_JOYSTICK 0x50
#define PAD_TYPE_DUALSHOCK 0x70
#define PAD_PACKET_ID_PRESSURE 0x79

#define PAD_PORT_COUNT 2
#define PAD_DMA_BUFFER_SIZE 0x100
#define PAD_ANALOG_CENTER 128
#define PAD_FULL_PRESSURE 255

static unsigned char act_align_0[6] = { 0, 1, 255, 255, 255, 255 };

static unsigned char PadDmaBuffer[2 * PAD_DMA_BUFFER_SIZE];

/* Opens both controller ports and clears the pad records, then sets the default repeat, button map and dead zones. */
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
    scePadPortOpen(1, 0, &PadDmaBuffer[PAD_DMA_BUFFER_SIZE]);

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

/* Sets the repeat mask, delay and interval of port (-1 sets both ports) and restarts the repeat counters. */
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

/* Polls both pads once per frame: advances each controller's setup state, decodes its packet and updates the pressed, released and repeated bits. */
void xglPadRead(void)
{
    int port;
    unsigned char button_bits;
    unsigned char button_down;
    unsigned char direction_down;
    unsigned char direction_bits;
    unsigned char *map;
    int map_remaining;
    signed char vertical;
    signed char horizontal;

    for (port = 0; port < PAD_PORT_COUNT; ++port) {
        XglPadRecord *pad;
        int buttons;
        int directions;
        /* One value serves both the 0x60 pressed-button word and the 0x50
           first dead zone, keeping the original register reuse. */
        unsigned short shared_value;
        /* One int holds the request-state reply of the mode setup and the
           repeated-button word, keeping the original register reuse. */
        int result;

        pad = &PadData[port];
        buttons = 0;
        pad->connection_state = scePadGetState(port, 0);
        if (!pad->connection_state)
            pad->setup_state = PAD_SETUP_PROBE;
        /* Run the setup state; PAD_SETUP_READY reads the packet and drives the actuators. */
        switch (pad->setup_state) {
        case PAD_SETUP_PROBE: {
            int mode;

            pad->mode = 0;
            pad->actuator_count = 0;
            if (pad->connection_state == PAD_STATE_STABLE || pad->connection_state == PAD_STATE_FINDCTP1) {
                mode = scePadInfoMode(port, 0, 1, 0);
                if (mode) {
                    pad->mode_id = scePadInfoMode(port, 0, 2, 0);
                    if (mode == PAD_TERM_ID_DIGITAL)
                        pad->setup_state = PAD_SETUP_QUERY_ACTUATORS_MODE;
                    else if (mode == PAD_TERM_ID_DUALSHOCK)
                        pad->setup_state = PAD_SETUP_ACTUATOR_ALIGN;
                    else
                        pad->setup_state = PAD_SETUP_READY;
                }
            }
            break;
        }
        case PAD_SETUP_QUERY_ACTUATORS_MODE:
            if (scePadInfoMode(port, 0, PAD_TERM_ID_DIGITAL, -1))
                pad->setup_state = PAD_SETUP_SET_MAIN_MODE;
            else
                pad->setup_state = PAD_SETUP_READY;
            break;
        case PAD_SETUP_SET_MAIN_MODE:
            if (scePadSetMainMode(port, 0, 1, 3) == 1)
                pad->setup_state = PAD_SETUP_WAIT_MAIN_MODE;
            break;
        case PAD_SETUP_WAIT_MAIN_MODE:
            result = scePadGetReqState(port, 0);
            switch (result) {
            case 0: pad->setup_state = PAD_SETUP_PROBE; break;
            case 1: pad->setup_state = PAD_SETUP_SET_MAIN_MODE; break;
            }
            break;
        case PAD_SETUP_ACTUATOR_ALIGN:
            pad->actuator_count = scePadInfoAct(port, 0, -1, 0);
            if (pad->actuator_count) {
                if (scePadSetActAlign(port, 0, act_align_0))
                    pad->setup_state = PAD_SETUP_WAIT_ALIGN;
            } else {
                pad->setup_state = PAD_SETUP_READY;
            }
            break;
        case PAD_SETUP_WAIT_ALIGN:
            if (pad->connection_state != PAD_STATE_EXECCMD)
                pad->setup_state = PAD_SETUP_PRESSURE_INFO;
            break;
        case PAD_SETUP_PRESSURE_INFO:
            if (scePadInfoPressMode(port, 0) != 1)
                pad->setup_state = PAD_SETUP_READY;
            else
                pad->setup_state = PAD_SETUP_PRESSURE_ENTER;
            break;
        case PAD_SETUP_PRESSURE_ENTER:
            if (scePadEnterPressMode(port, 0) == 1)
                pad->setup_state = PAD_SETUP_WAIT_PRESSURE;
            break;
        case PAD_SETUP_WAIT_PRESSURE:
            result = scePadGetReqState(port, 0);
            switch (result) {
            case 0: pad->setup_state = PAD_SETUP_READY; break;
            case 1: pad->setup_state = PAD_SETUP_PRESSURE_ENTER; break;
            }
            break;
        case PAD_SETUP_READY: {
            scePadRead(port, 0, pad->packet);
            pad->mode = pad->packet[1];
            if (pad->actuator_count) {
                if (pad->actuator_pending) {
                    pad->actuator[0] = 0;
                    pad->actuator[1] = 0;
                    pad->actuator[2] = 0;
                    pad->actuator[3] = 0;
                    pad->actuator[4] = 0;
                    pad->actuator[5] = 0;
                }
                scePadSetActDirect(port, 0, pad->actuator);
                pad->actuator[0] = 0;
                pad->actuator[1] = 0;
                pad->actuator[2] = 0;
                pad->actuator[3] = 0;
                pad->actuator[4] = 0;
                pad->actuator[5] = 0;
            }
            if (pad->packet[0] == 0) {
            /* Decode the packet by controller type: sticks, buttons and pressures. */
            switch (pad->packet[1] & PAD_TYPE_MASK) {
            case PAD_TYPE_MOUSE: {
                int stick_x;
                int stick_y;

                stick_x = pad->packet[4];
                stick_y = pad->packet[5];
                pad->position[0] = pad->position[0] + stick_x;
                pad->position[1] = pad->position[1] + stick_y;
                pad->position[2] = stick_x;
                pad->position[3] = stick_y;
                buttons = (pad->packet[3] << 3) ^ 0xfff8;
                break;
            }
            case PAD_TYPE_NEGCON: {
                int stick_x;
                int stick_y;
                int right_x;
                int right_y;

                buttons = ~(pad->packet[3] + (pad->packet[2] << 8)) & 0xffff;
                stick_x = pad->packet[4];
                right_x = pad->packet[6];
                pad->position[0] = stick_x;
                stick_y = pad->packet[5];
                right_y = pad->packet[7];
                pad->position[1] = stick_y;
                pad->position[2] = right_x;
                pad->position[3] = right_y;
                break;
            }
            case 0x60: {
                shared_value = ~(pad->packet[3] + (pad->packet[2] << 8));
                pad->position[0] = pad->packet[4] + pad->position[2];
                pad->position[1] = pad->packet[6] + pad->position[3];
                if ((shared_value & 0x6008) == 0x6008) {
                    pad->position[2] = -96 - pad->position[0];
                    pad->position[3] = 120 - pad->position[1];
                }
                buttons = shared_value;
                break;
            }
            case 0xe0:
                pad->position[0] = pad->packet[4];
                pad->position[1] = pad->packet[6];
                buttons = ~(pad->packet[3] + (pad->packet[2] << 8)) & 0xffff;
                break;
            case 0x30:
            case PAD_TYPE_DIGITAL:
                buttons = ~(pad->packet[3] + (pad->packet[2] << 8)) & 0xffff;
                break;
            case PAD_TYPE_ANALOG_JOYSTICK:
            case PAD_TYPE_DUALSHOCK: {
                unsigned char first_vertical_position;
                unsigned char second_axis_threshold;
                unsigned char third_axis_threshold;
                unsigned char fourth_axis_threshold;
                int axis;

                shared_value = pad->axis_dead_zone[0];
                axis = pad->packet[4] + PAD_ANALOG_CENTER;
                pad->position[1] = pad->packet[5] + PAD_ANALOG_CENTER;
                pad->position[2] = pad->packet[6] + PAD_ANALOG_CENTER;
                pad->position[3] = pad->packet[7] + PAD_ANALOG_CENTER;
                pad->horizontal_pressure = pad->packet[8] - pad->packet[9];
                pad->vertical_pressure = pad->packet[11] - pad->packet[10];
                pad->pressure[0] = pad->packet[12];
                pad->pressure[1] = pad->packet[13];
                pad->pressure[2] = pad->packet[14];
                pad->pressure[3] = pad->packet[15];
                pad->pressure[4] = pad->packet[16];
                pad->pressure[5] = pad->packet[17];
                pad->pressure[6] = pad->packet[18];
                pad->pressure[7] = pad->packet[19];
                pad->position[0] = axis;
                pad->axis[0] = axis;
                if (-shared_value < (signed char)axis &&
                    (signed char)axis < shared_value)
                    pad->axis[0] = 0;
                first_vertical_position = pad->position[1];
                second_axis_threshold = pad->axis_dead_zone[1];
                pad->axis[1] = first_vertical_position;
                if (-second_axis_threshold < (signed char)first_vertical_position &&
                    (signed char)first_vertical_position < second_axis_threshold)
                    pad->axis[1] = 0;
                third_axis_threshold = pad->axis_dead_zone[2];
                pad->axis[2] = pad->position[2];
                if (-third_axis_threshold < pad->position[2] &&
                    pad->position[2] < third_axis_threshold)
                    pad->axis[2] = 0;
                fourth_axis_threshold = pad->axis_dead_zone[3];
                pad->axis[3] = pad->position[3];
                if (-fourth_axis_threshold < pad->position[3] &&
                    pad->position[3] < fourth_axis_threshold)
                    pad->axis[3] = 0;
                buttons = ~(pad->packet[3] + (pad->packet[2] << 8)) & 0xffff;
                break;
            }
            case 0x80:
                continue;
            }
            }
            break;
        }
        }
        /* Packets without pressure data get their pressure values from the digital buttons. */
        if (pad->packet[1] != PAD_PACKET_ID_PRESSURE) {
            int maximum_pressure;
            int digital_horizontal;
            int digital_vertical;
            int triangle;
            int circle;
            int cross;
            int square;
            int l1;
            int r1;
            int l2;
            int r2;
            int first_buttons;
            int up;
            int right;
            int down;
            int left;
            int second_buttons;

            maximum_pressure = PAD_FULL_PRESSURE;
            first_buttons = pad->packet[2];
            second_buttons = pad->packet[3];
            right = first_buttons & 0x20;
            down = first_buttons & 0x40;
            left = first_buttons & 0x80;
            up = first_buttons & 0x10;
            digital_horizontal = maximum_pressure;
            digital_vertical = maximum_pressure;
            if (right) digital_horizontal -= maximum_pressure;
            if (!left) digital_horizontal -= maximum_pressure;
            r2 = second_buttons & 2;
            triangle = second_buttons & 0x10;
            circle = second_buttons & 0x20;
            cross = second_buttons & 0x40;
            square = second_buttons & 0x80;
            l1 = second_buttons & 4;
            r1 = second_buttons & 8;
            l2 = second_buttons & 1;
            triangle = triangle != 0;
            circle = circle != 0;
            cross = cross != 0;
            square = square != 0;
            l1 = l1 != 0;
            r1 = r1 != 0;
            r2 = r2 != 0;
            if (down) digital_vertical -= maximum_pressure;
            if (!up) digital_vertical -= maximum_pressure;
            pad->horizontal_pressure = digital_horizontal;
            pad->vertical_pressure = digital_vertical;
            pad->pressure[0] = triangle - 1;
            pad->pressure[1] = circle - 1;
            pad->pressure[2] = cross - 1;
            pad->pressure[3] = square - 1;
            pad->pressure[4] = l1 - 1;
            pad->pressure[5] = r1 - 1;
            pad->pressure[6] = l2 - 1;
            pad->pressure[7] = r2 - 1;
        }
        /* Stick positions past the dead zones set direction bits; the button bits go through button_map. */
        directions = buttons;
        horizontal = pad->position[2];
        if (horizontal < -pad->horizontal_dead_zone)
            directions = (buttons | PAD_LEFT) & 0xffff;
        if (horizontal > pad->horizontal_dead_zone)
            directions = (directions | PAD_RIGHT) & 0xffff;
        vertical = pad->position[3];
        if (vertical < -pad->vertical_dead_zone)
            directions = (directions | PAD_UP) & 0xffff;
        if (vertical > pad->vertical_dead_zone)
            directions = (directions | PAD_DOWN) & 0xffff;
        button_bits = buttons & 0xff;
        buttons &= 0xff00;
        direction_bits = directions & 0xff;
        directions &= 0xff00;
        for (map_remaining = 0; map_remaining < (int)sizeof(pad->button_map); ++map_remaining) {
            button_down = button_bits & 1;
            if (button_down)
                buttons = (buttons | pad->button_map[map_remaining]) & 0xffff;
            button_bits >>= 1;
            direction_down = direction_bits & 1;
            if (direction_down)
                directions = (directions | pad->button_map[map_remaining]) & 0xffff;
            direction_bits >>= 1;
        }
        /* Compare with the previous frame: changed bits give pressed and released. */
        {
            int changed = buttons ^ pad->buttons;
            int direction_changed = directions ^ pad->directions;
            int pressed = buttons & changed;
            int direction_pressed = directions & direction_changed;

            pad->released = changed & ~buttons;
            pad->repeated = pad->pressed = pressed;
            pad->buttons = buttons;
            pad->direction_released = direction_changed & ~directions;
            pad->direction_repeated = pad->direction_pressed = direction_pressed;
            pad->directions = directions;
        }
        /* Auto-repeat: held directions and buttons repeat after repeat_delay, every repeat_interval frames. */
        {
            int repeat = directions & pad->repeat_mask;
            unsigned char repeat_count;

            if (repeat && pad->repeat_wait > pad->repeat_delay) {
                repeat_count = pad->repeat_count;
                if (pad->repeat_count > pad->repeat_interval) {
                    pad->direction_repeated = repeat;
                    result = buttons & pad->repeat_mask;
                    pad->repeated = result;
                    repeat_count = 0;
                    pad->repeat_count = 0;
                }
                pad->repeat_count = repeat_count + 1;
            } else if (repeat) {
                ++pad->repeat_wait;
            } else {
                pad->repeat_wait = 0;
            }
        }
    }
}
