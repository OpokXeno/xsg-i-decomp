#ifndef INCLUDE_MAIN_XGL_PAD_H
#define INCLUDE_MAIN_XGL_PAD_H

/*
 * Controller record, 0x68 bytes per pad. The pressed halfword at +0x2A is also
 * established by main/tu236's getEdge and main/tu144's slider input. The menu
 * (window_tex_load.c) reads the direction pressed/repeat halfwords at
 * +0x32/+0x34 and writes the actuator bytes at +0x50; the twsys_init.c menu
 * reads the same two halfwords.
 */
typedef struct XglPadRecord {
    unsigned char packet[0x20];
    unsigned char mode;
    unsigned char actuator_count;
    short mode_id;
    unsigned char unmodeled_24[2];
    unsigned char connection_state;
    unsigned char setup_state;
    union {
        struct {
            unsigned short buttons;
            unsigned short pressed;
            unsigned short repeated;
            unsigned short released;
        };
        /* +0x28: the four button halfwords read as one doubleword
           (script.c sceneObserver's talk-cancel test). */
        unsigned long long button_state;
    };
    unsigned short directions;
    unsigned short direction_pressed;
    unsigned short direction_repeated;
    unsigned short direction_released;
    signed char position[4];
    short horizontal_pressure;
    short vertical_pressure;
    signed char pressure[8];
    signed char repeat_delay;       /* +0x48 */
    signed char repeat_interval;    /* +0x49 */
    unsigned short repeat_mask;    /* +0x4a */
    signed char repeat_wait;        /* +0x4c */
    signed char repeat_count;       /* +0x4d */
    unsigned char horizontal_dead_zone; /* +0x4e */
    unsigned char vertical_dead_zone;   /* +0x4f */
    unsigned char actuator[6];      /* +0x50 */
    unsigned char actuator_pending; /* +0x56 */
    unsigned char unmodeled_57;     /* +0x57 */
    unsigned char button_map[8];    /* +0x58 */
    unsigned char axis_dead_zone[4];/* +0x60 */
    signed char axis[4];            /* +0x64 */
} XglPadRecord;

/* Functions and data used by other translation units. */
void xglPadRead(void);

#endif /* INCLUDE_MAIN_XGL_PAD_H */
