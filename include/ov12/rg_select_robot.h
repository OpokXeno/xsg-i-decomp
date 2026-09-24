#ifndef INCLUDE_OV12_RG_SELECT_ROBOT_H
#define INCLUDE_OV12_RG_SELECT_ROBOT_H

/*
 * _InitSelRob stores the preview-actor heap arena's base pointer at
 * +0x164; _DestructSelRob frees it back from the same field.
 */
struct RgSelectRobot {
    unsigned char unmodeled_00[0x138];
    float screenPos;
    unsigned char unmodeled_13c[0x28];
    void *heapBuffer;
};

#endif /* INCLUDE_OV12_RG_SELECT_ROBOT_H */
