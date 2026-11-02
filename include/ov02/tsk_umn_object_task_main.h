#ifndef INCLUDE_OV02_TSK_UMN_OBJECT_TASK_MAIN_H
#define INCLUDE_OV02_TSK_UMN_OBJECT_TASK_MAIN_H

typedef unsigned int UmnWorkQuadword __attribute__((mode(TI)));

typedef struct UmnWorkData {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02;
    unsigned char stateOrCategory;
    unsigned char unmodeled_04[0x0C];
    union {
        UmnWorkQuadword section_state[7];
        struct {
            signed char repeat_delay;
            unsigned char unmodeled_11;
            signed char direction;
            unsigned char unmodeled_13[0x0d];
            unsigned char slots[4];
            unsigned char unmodeled_24[0x0c];
            unsigned char count;
            unsigned char cursor;
        } top;
    };
} UmnWorkData;

#endif /* INCLUDE_OV02_TSK_UMN_OBJECT_TASK_MAIN_H */
